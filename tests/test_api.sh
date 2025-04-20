#!/bin/bash
echo "Creating a todo..."
curl -X POST -H "Content-Type: application/json" -d '{
  "title": "Buy groceries",
  "description": "Get milk, bread, and eggs"
}' http://localhost:8080/todos
echo -e "\n\nListing all todos..."
curl http://localhost:8080/todos
echo -e "\n\nGetting todo with ID 1..."
curl http://localhost:8080/todos/1
echo -e "\n\nUpdating todo with ID 1..."
curl -X PUT -H "Content-Type: application/json" -d '{
  "title": "Buy groceries",
  "description": "Get milk, bread, eggs, and cheese",
  "completed": true
}' http://localhost:8080/todos/1
echo -e "\n\nVerifying update..."
curl http://localhost:8080/todos/1
echo -e "\n\nDeleting todo with ID 1..."
curl -X DELETE http://localhost:8080/todos/1
echo -e "\n\nVerifying deletion..."
curl http://localhost:8080/todos
echo -e "\n\nTests completed."