#include "server.h"
#include "handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <microhttpd.h>

static struct MHD_Daemon* http_daemon = NULL;

#define MAX_POST_DATA_SIZE 16384  // 16KB max

struct ConnectionInfo {
    char* post_data;
    size_t post_data_size;
    int post_data_processed;
};

static void free_connection_info(struct ConnectionInfo* con_info) {
    if (con_info) {
        if (con_info->post_data) {
            free(con_info->post_data);
        }
        free(con_info);
    }
}

static enum MHD_Result handle_request(void* cls,
                        struct MHD_Connection* connection,
                        const char* url,
                        const char* method,
                        const char* version,
                        const char* upload_data,
                        size_t* upload_data_size,
                        void** con_cls) {
    (void)cls;
    (void)version;

    printf("DEBUG: Request [%s] %s\n", method, url);
    printf("DEBUG: upload_data_size = %zu\n", upload_data_size ? *upload_data_size : 0);

    struct ConnectionInfo* con_info = *con_cls;

    if (con_info == NULL) {
        printf("DEBUG: First call - initializing connection info\n");
        con_info = calloc(1, sizeof(struct ConnectionInfo));
        if (!con_info) return MHD_NO;
        
        con_info->post_data = malloc(MAX_POST_DATA_SIZE);
        if (!con_info->post_data) {
            free(con_info);
            return MHD_NO;
        }
        con_info->post_data[0] = '\0';
        con_info->post_data_size = 0;
        con_info->post_data_processed = 0;
        
        *con_cls = con_info;
        return MHD_YES;
    }

    if ((strcmp(method, "POST") == 0 || strcmp(method, "PUT") == 0) && *upload_data_size != 0) {
        printf("DEBUG: Received %zu bytes of POST/PUT data\n", *upload_data_size);
        if (upload_data) {
            printf("DEBUG: Data: '%.*s'\n", (int)*upload_data_size, upload_data);
        }
        
        if (con_info->post_data_size + *upload_data_size <= MAX_POST_DATA_SIZE) {
            memcpy(con_info->post_data + con_info->post_data_size, upload_data, *upload_data_size);
            con_info->post_data_size += *upload_data_size;
            con_info->post_data[con_info->post_data_size] = '\0';  // Null-terminate
            
            printf("DEBUG: Total accumulated data: %zu bytes\n", con_info->post_data_size);
            *upload_data_size = 0;
            return MHD_YES;
        }
        return MHD_NO;
    }

    if ((strcmp(method, "POST") == 0 || strcmp(method, "PUT") == 0) && !con_info->post_data_processed) {
        printf("DEBUG: Finished receiving data - setting processed flag\n");
        con_info->post_data_processed = 1;
        return MHD_YES;
    }

    printf("DEBUG: Processing request with %zu bytes of data\n", con_info->post_data_size);
    if (con_info->post_data_size > 0) {
        printf("DEBUG: Data: '%.*s'\n", (int)con_info->post_data_size, con_info->post_data);
    }

    struct ResponseData response_data = {NULL, 0};
    CURL* curl = curl_easy_init();
    struct MHD_Response* response;
    enum MHD_Result ret;
    int http_status = MHD_HTTP_OK;

    if (strcmp(method, "GET") == 0) {
        if (strcmp(url, "/todos") == 0) {
            handle_list_todos(curl, &response_data);
        } else if (strncmp(url, "/todos/", 7) == 0) {
            int id = atoi(url + 7);
            handle_get_todo(curl, id, &response_data);
        } else {
            http_status = MHD_HTTP_NOT_FOUND;
            response_data.data = strdup("{\"error\": \"Not found\"}");
            response_data.size = strlen(response_data.data);
        }
    } else if (strcmp(method, "POST") == 0) {
        if (strcmp(url, "/todos") == 0) {
            printf("DEBUG: Calling handle_create_todo with %zu bytes\n", con_info->post_data_size);
            handle_create_todo(curl, con_info->post_data, con_info->post_data_size, &response_data);
        } else {
            http_status = MHD_HTTP_NOT_FOUND;
            response_data.data = strdup("{\"error\": \"Not found\"}");
            response_data.size = strlen(response_data.data);
        }
    } else if (strcmp(method, "PUT") == 0) {
        if (strncmp(url, "/todos/", 7) == 0) {
            int id = atoi(url + 7);
            printf("DEBUG: Calling handle_update_todo with %zu bytes\n", con_info->post_data_size);
            handle_update_todo(curl, id, con_info->post_data, con_info->post_data_size, &response_data);
        } else {
            http_status = MHD_HTTP_NOT_FOUND;
            response_data.data = strdup("{\"error\": \"Not found\"}");
            response_data.size = strlen(response_data.data);
        }
    } else if (strcmp(method, "DELETE") == 0) {
        if (strncmp(url, "/todos/", 7) == 0) {
            int id = atoi(url + 7);
            handle_delete_todo(curl, id, &response_data);
        } else {
            http_status = MHD_HTTP_NOT_FOUND;
            response_data.data = strdup("{\"error\": \"Not found\"}");
            response_data.size = strlen(response_data.data);
        }
    } else {
        http_status = MHD_HTTP_METHOD_NOT_ALLOWED;
        response_data.data = strdup("{\"error\": \"Method not allowed\"}");
        response_data.size = strlen(response_data.data);
    }

    if (!response_data.data) {
        response_data.data = strdup("{\"status\": \"OK\"}");
        response_data.size = strlen(response_data.data);
    }

    response = MHD_create_response_from_buffer(response_data.size,
                                             response_data.data,
                                             MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", "application/json");

    ret = MHD_queue_response(connection, http_status, response);
    MHD_destroy_response(response);
    curl_easy_cleanup(curl);

    free_connection_info(con_info);
    *con_cls = NULL;

    return ret;
}

int http_server_init(int port) {
    http_daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
                            port,
                            NULL,
                            NULL,
                            (MHD_AccessHandlerCallback)&handle_request,
                            NULL,
                            MHD_OPTION_END);
    return http_daemon ? 0 : -1;
}

void http_server_process(void) {
}

void http_server_cleanup(void) {
    if (http_daemon) {
        MHD_stop_daemon(http_daemon);
        http_daemon = NULL;
    }
} 