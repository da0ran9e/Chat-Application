# Chat-Application
💬🪼a message machine
## Folder structure:
```c
project_root/
|-- include/
|   |-- shared/
|       |-- common.h         // Shared header file with common declarations
|       |-- constants.h      // Shared header file with constants
|       |-- utility.h        // Shared header file for utility functions
|       |-- protocol.h       // Shared header file to define the communication protocol
|-- build/
|   // Add build artifacts or compiled files here (gitignore this directory)
|-- bin/
|   // Add executable files here after building
|-- src/
|   |-- server/
|   |   |-- server.c
|   |   |-- authentication.c
|   |   |-- connections.c
|   |   |-- database.c
|   |   |-- handle_client_message.c
|   |   |-- handle_database_message.c
|   |   |-- relations.c
|   |   |-- rooms.c
|   |   |-- conversation.c
|   |-- client/
|   |   |-- client.c
|   |   |-- authorize.c
|   |   |-- feature.c
|   |   |-- chat.c
|   |   |-- notification.c
|   |   |-- handle_server_message.c
|   |-- shared/
|       |-- utility.c        // Implement utility functions used by both server and client
|-- database/
|   |-- database.c           // Implement database-related functions
|   |-- db_connection.c      // Implement database connection setup using libpq
|   |-- sql_queries.c        // Implement SQL queries used in the application
|-- tests/                   // Include your test files here
|-- docs/
|   |-- README.md            // Project documentation and instructions
|-- Makefile                 // Makefile for building the project

```