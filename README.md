# C REST API - Todo Application

A lightweight RESTful API for managing todo items, built entirely in C. This project demonstrates how to build web services at a low level using standard C libraries.

```
┌────────────┐     ┌─────────────┐     ┌─────────────┐
│            │     │             │     │             │
│   Client   │◄────►  HTTP API   │◄────►   SQLite    │
│            │     │             │     │  Database   │
└────────────┘     └─────────────┘     └─────────────┘
                         │
                         │
                    ┌────▼────┐
                    │         │
                    │  JSON   │
                    │         │
                    └─────────┘
```

## Features

- Complete CRUD operations (Create, Read, Update, Delete)
- RESTful design following HTTP method conventions
- SQLite database for data persistence
- JSON request/response format
- Modular, maintainable C code structure
- Extensive testing capabilities
- Memory-safe implementations with proper cleanup
- Error handling with descriptive messages

## REST API Theory

### What is REST?

REST (Representational State Transfer) is an architectural style for designing networked applications. Unlike SOAP or RPC which focus on actions, REST emphasizes resources and states.

Key principles of REST:
- **Statelessness**: Each request contains all information needed to process it
- **Client-Server Architecture**: Separation of concerns between UI/client and data storage
- **Uniform Interface**: Standard methods to interact with resources
- **Resource-Based**: Everything is a resource identified by a unique URI
- **Representation**: Resources can be represented in different formats (JSON, XML, etc.)

### HTTP Methods in REST

This API implements the standard HTTP methods for CRUD operations:

| HTTP Method | CRUD Operation | Description |
|-------------|---------------|-------------|
| GET | Read | Retrieve resource(s) |
| POST | Create | Create a new resource |
| PUT | Update | Update an existing resource |
| DELETE | Delete | Remove a resource |

## Dependencies

- **libmicrohttpd**: Small C library that makes it easy to run an HTTP server
- **libsqlite3**: C library for SQLite, a self-contained, serverless database engine
- **libjansson**: C library for encoding, decoding, and manipulating JSON data
- **libcurl**: Client-side URL transfer library (used for testing)
- **CMake**: Cross-platform build system generator

## Setup & Installation

### Install Dependencies

For Debian/Ubuntu:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libcurl4-openssl-dev libsqlite3-dev libmicrohttpd-dev libjansson-dev
```

### Build

```bash
mkdir -p build
cd build
cmake ..
make
```

## Running the API

Start the server:
```bash
./build/src/todo_api
```

The server will listen on port 8080 by default.

## Example API Calls

### Create a Todo
```bash
curl -X POST http://localhost:8080/todos -H "Content-Type: application/json" -d '{"title":"Buy groceries","description":"Get milk, bread, and eggs"}'
```

### List All Todos
```bash
curl http://localhost:8080/todos
```

### Get a Specific Todo
```bash
curl http://localhost:8080/todos/1
```

### Update a Todo
```bash
curl -X PUT http://localhost:8080/todos/1 -H "Content-Type: application/json" -d '{"title":"Buy groceries","description":"Get milk, bread, eggs, and cheese","completed":true}'
```

### Delete a Todo
```bash
curl -X DELETE http://localhost:8080/todos/1
```

## Management Script

For easier development, use the management script:
```bash
./manage.sh build   # Build the project
./manage.sh run     # Run the server
./manage.sh test    # Run unit tests
./manage.sh api-test # Test API endpoints
```

## Project Architecture

### Directory Structure

```
rest-api/
├── CMakeLists.txt              # Main CMake configuration
├── manage.sh                   # Management script
├── src/                        # Source code
│   ├── CMakeLists.txt          # Source CMake configuration
│   ├── main.c                  # Entry point
│   ├── core/                   # Core functionality
│   │   ├── todo.h              # Todo structure definition
│   │   └── todo.c              # Todo operations
│   ├── db/                     # Database operations
│   │   ├── database.h          # Database interface
│   │   └── database.c          # SQLite implementation
│   └── http/                   # HTTP handling
│       ├── server.h            # Server interface
│       ├── server.c            # Server implementation
│       ├── handlers.h          # Request handlers interface
│       └── handlers.c          # Request handlers implementation
├── tests/                      # Unit tests
│   ├── CMakeLists.txt          # Test CMake configuration
│   └── test_todo.c             # Todo unit tests
└── scripts/                    # Helper scripts
    └── test_api.sh             # API test script
```

### Internal Components

#### Todo Data Structure (core/todo.h, core/todo.c)

The central data model representing a task with:
- Unique identifier
- Title and description
- Completion status
- Timestamps for creation and updates

Operations include creating, retrieving, updating, and deleting todos.

#### Database Management (db/database.h, db/database.c)

Manages all interactions with SQLite:
- Initializes the database and creates tables
- Executes SQL statements for CRUD operations
- Provides a callback mechanism for processing query results

SQLite was chosen for its simplicity, zero-configuration, and self-contained nature.

#### HTTP Server (http/server.h, http/server.c)

Built with libmicrohttpd to:
- Start and stop the HTTP server
- Route requests to appropriate handlers
- Parse request URLs, methods, and bodies
- Send formatted responses with correct status codes

#### Request Handlers (http/handlers.h, http/handlers.c)

Implements the business logic for each API endpoint:
- Parsing JSON requests using jansson
- Performing operations on the todo structure
- Generating JSON responses
- Error handling with appropriate HTTP status codes

## Implementation Details

### Memory Management

The API takes care to properly manage memory:
- Dynamic allocations are tracked and freed
- String buffers are properly sized and null-terminated
- JSON objects are reference-counted and properly released

### Error Handling

Comprehensive error handling includes:
- Database errors with detailed messages
- JSON parsing errors
- HTTP request validation
- Resource not found errors
- Server initialization failures

### Concurrency

libmicrohttpd handles concurrency with a thread-per-connection model, allowing the API to serve multiple clients simultaneously. The SQLite database is configured for thread-safety.

### Testing Strategy

The project implements multiple testing layers:
1. **Unit Tests**: Test individual functions in isolation
2. **Integration Tests**: Test the interaction between components
3. **API Tests**: Test HTTP endpoints end-to-end

## Performance Considerations

- **Connection Pooling**: The server uses connection pooling to reduce overhead
- **Prepared Statements**: SQL statements are prepared to improve database performance
- **Minimal Copying**: Data is processed with minimal copying between memory regions
- **Efficient JSON Parsing**: Uses jansson's stream parsing for large payloads

## Security Considerations

- **Input Validation**: All client inputs are validated before processing
- **SQL Injection Prevention**: Uses prepared statements to prevent SQL injection
- **Memory Safety**: Careful buffer management to prevent overflows
- **Error Information**: Limited error details exposed to clients

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

When contributing, please:
1. Follow the existing code style
2. Add tests for new functionality
3. Ensure all tests pass
4. Update documentation as needed

## Support

If you find this project helpful, consider buying me a coffee:

[![Buy Me A Coffee](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://buymeacoffee.com/trish07)

## License

This project is licensed under the MIT License - see the LICENSE file for details. 
