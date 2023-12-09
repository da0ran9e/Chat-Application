# Chat-Application
💬🪼a message machine
## Folder structure:
```c
project_root/
|-- server/
|   |-- server.c
|   |-- authentication.c
|   |-- connections.c
|   |-- database.c
|   |-- handle_client_message.c
|   |-- handle_database_message.c
|   |-- relations.c
|   |-- rooms.c
|   |-- conversation.c
|
|-- client/
|   |-- client.c
|   |-- authorize.c
|   |-- feature.c
|   |-- chat.c
|   |-- notification.c
|   |-- handle_server_message.c
|
|-- shared/
|   |-- common.h         // Header file with common declarations
|   |-- constants.h      // Header file with constants
|   |-- utility.c        // Utility functions used by both server and client
|   |-- protocol.h       // Define the communication protocol
|
|-- database/
|   |-- database.c         // Database-related functions
|   |-- database.h         // Header file with function declarations
|   |-- db_connection.c    // Database connection setup
|   |-- db_connection.h    // Header file for database connection
|   |-- sql_queries.c      // SQL queries used in the application
|   |-- sql_queries.h      // Header file for SQL queries
|   |-- Makefile            // Makefile for building the database module
|
|-- Makefile             // Makefile for building the project
```