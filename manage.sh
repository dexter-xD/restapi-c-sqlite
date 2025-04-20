#!/bin/bash

# Script to manage the REST API project
# Usage: ./manage.sh [command]
# Commands:
#   build       - Build the project
#   clean       - Clean the build directory
#   run         - Run the server
#   test        - Run unit tests
#   api-test    - Run API tests
#   help        - Display this help message

set -e  # Exit on error

PROJECT_DIR="$(pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
SRC_DIR="${PROJECT_DIR}/src"
TEST_DIR="${PROJECT_DIR}/tests"
SCRIPTS_DIR="${PROJECT_DIR}/scripts"
SERVER_PORT=8080

# Create scripts directory if it doesn't exist
mkdir -p ${SCRIPTS_DIR}

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored message
print_message() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Function to build the project
build() {
    print_message "$BLUE" "Building project..."
    mkdir -p ${BUILD_DIR}
    cd ${BUILD_DIR}
    cmake ..
    make
    print_message "$GREEN" "Build complete!"
}

# Function to clean the build directory
clean() {
    print_message "$BLUE" "Cleaning build directory..."
    if [ -d ${BUILD_DIR} ]; then
        rm -rf ${BUILD_DIR}
        print_message "$GREEN" "Clean complete!"
    else
        print_message "$YELLOW" "Build directory does not exist."
    fi
}

# Function to run the server
run() {
    if [ ! -d ${BUILD_DIR} ]; then
        print_message "$YELLOW" "Build directory not found. Building first..."
        build
    fi

    print_message "$BLUE" "Starting server on port ${SERVER_PORT}..."
    cd ${BUILD_DIR}
    ./src/todo_api
}

# Function to run unit tests
test() {
    if [ ! -d ${BUILD_DIR} ]; then
        print_message "$YELLOW" "Build directory not found. Building first..."
        build
    fi

    print_message "$BLUE" "Running unit tests..."
    cd ${BUILD_DIR}
    ctest --output-on-failure
    
    # Alternatively, run test_todo directly
    # ./tests/test_todo
    
    print_message "$GREEN" "Unit tests complete!"
}

# Function to create API test script
create_api_test_script() {
    cat > ${SCRIPTS_DIR}/test_api.sh << 'EOF'
#!/bin/bash

# API test script for TODO REST API
SERVER_URL="http://localhost:8080"
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to make requests and validate responses
test_request() {
    local method=$1
    local endpoint=$2
    local data=$3
    local expected_status=$4
    local description=$5

    echo -e "${BLUE}Test: ${description}${NC}"
    echo "Request: ${method} ${endpoint}"
    if [ ! -z "$data" ]; then
        echo "Data: ${data}"
    fi
    
    local response
    local status
    
    if [ "$method" == "GET" ]; then
        response=$(curl -s -w "\n%{http_code}" -X GET ${SERVER_URL}${endpoint})
    elif [ "$method" == "POST" ]; then
        response=$(curl -s -w "\n%{http_code}" -X POST -H "Content-Type: application/json" -d "${data}" ${SERVER_URL}${endpoint})
    elif [ "$method" == "PUT" ]; then
        response=$(curl -s -w "\n%{http_code}" -X PUT -H "Content-Type: application/json" -d "${data}" ${SERVER_URL}${endpoint})
    elif [ "$method" == "DELETE" ]; then
        response=$(curl -s -w "\n%{http_code}" -X DELETE ${SERVER_URL}${endpoint})
    fi
    
    status=$(echo "$response" | tail -n1)
    body=$(echo "$response" | sed '$d')
    
    echo "Response: ${body}"
    echo "Status: ${status}"
    
    if [ "$status" -eq "$expected_status" ]; then
        echo -e "${GREEN}✓ Test passed${NC}"
    else
        echo -e "${RED}✗ Test failed: Expected status ${expected_status}, got ${status}${NC}"
    fi
    echo ""
    
    # Return the ID of the created todo if this was a POST to /todos
    if [ "$method" == "POST" ] && [ "$endpoint" == "/todos" ]; then
        id=$(echo "$body" | grep -o '"id":[0-9]\+' | cut -d':' -f2)
        echo "$id"
    fi
}

echo -e "${BLUE}Starting API tests...${NC}"
echo "Server URL: ${SERVER_URL}"
echo ""

# Test 1: List todos (initially should be empty)
test_request "GET" "/todos" "" 200 "List todos (initially)"

# Test 2: Create a todo
todo_id=$(test_request "POST" "/todos" '{"title":"Buy groceries","description":"Get milk, bread, and eggs"}' 200 "Create a todo")

# Test 3: List todos (should contain the new todo)
test_request "GET" "/todos" "" 200 "List todos (after creation)"

# Test 4: Get a specific todo
test_request "GET" "/todos/${todo_id}" "" 200 "Get todo with ID ${todo_id}"

# Test 5: Update a todo
test_request "PUT" "/todos/${todo_id}" '{"title":"Buy groceries","description":"Get milk, bread, eggs, and cheese","completed":true}' 200 "Update todo with ID ${todo_id}"

# Test 6: Get the updated todo
test_request "GET" "/todos/${todo_id}" "" 200 "Get updated todo with ID ${todo_id}"

# Test 7: Delete a todo
test_request "DELETE" "/todos/${todo_id}" "" 200 "Delete todo with ID ${todo_id}"

# Test 8: Verify deletion
test_request "GET" "/todos/${todo_id}" "" 404 "Verify todo with ID ${todo_id} is deleted"

echo -e "${GREEN}API tests completed!${NC}"
EOF

    chmod +x ${SCRIPTS_DIR}/test_api.sh
    print_message "$GREEN" "API test script created at ${SCRIPTS_DIR}/test_api.sh!"
}

# Function to run API tests
api_test() {
    # Create API test script if it doesn't exist
    if [ ! -f ${SCRIPTS_DIR}/test_api.sh ]; then
        create_api_test_script
    fi
    
    print_message "$BLUE" "Running API tests..."
    # Check if server is running
    if ! curl -s http://localhost:${SERVER_PORT} > /dev/null; then
        print_message "$YELLOW" "Server is not running. Please start the server in another terminal with './manage.sh run'"
        exit 1
    fi
    
    # Run the API test script
    ${SCRIPTS_DIR}/test_api.sh
}

# Display help
help() {
    echo "Usage: ./manage.sh [command]"
    echo "Commands:"
    echo "  build       - Build the project"
    echo "  clean       - Clean the build directory"
    echo "  run         - Run the server"
    echo "  test        - Run unit tests"
    echo "  api-test    - Run API tests"
    echo "  help        - Display this help message"
}

# Check command line arguments
if [ $# -eq 0 ]; then
    help
    exit 1
fi

# Parse command
case "$1" in
    build)
        build
        ;;
    clean)
        clean
        ;;
    run)
        run
        ;;
    test)
        test
        ;;
    api-test)
        api_test
        ;;
    help)
        help
        ;;
    *)
        print_message "$RED" "Unknown command: $1"
        help
        exit 1
        ;;
esac

exit 0 