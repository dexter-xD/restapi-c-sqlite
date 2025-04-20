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
