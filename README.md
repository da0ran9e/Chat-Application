# Chat-Application
💬🪼a message machine
## Folder structure:
```c
project_root/
|-- include/
|   |-- shared/
|       |-- common.h         // Shared header file with common declarations (e.g., data structures)
|       |-- constants.h      // Shared header file with constants (e.g., protocol codes, buffer sizes)
|       |-- utility.h        // Shared header file for utility functions (e.g., string manipulation, error handling)
|       |-- protocol.h       // Shared header file to define the communication protocol (e.g., message formats)
|-- build/                   // Add build artifacts or compiled files here (gitignore this directory)
|-- bin/                     // Add executable files here after building
|-- src/
|   |-- server/
|   |   |-- server.c         // Main server application logic
|   |   |-- authentication.c // User authentication functionality
|   |   |-- connections.c    // Handling client connections and disconnections
|   |   |-- database.c       // Interactions with the PostgreSQL database
|   |   |-- handle_client_message.c   // Logic for processing messages from clients
|   |   |-- handle_database_message.c // Logic for processing messages related to the database
|   |   |-- relations.c       // Handling relationships between users
|   |   |-- rooms.c           // Managing chat rooms
|   |   |-- conversation.c    // Managing conversations and message history
|   |-- client/
|   |   |-- client.c          // Main client application logic
|   |   |-- authorize.c       // User authorization logic
|   |   |-- feature.c         // Additional client features
|   |   |-- chat.c            // Handling chat functionality
|   |   |-- notification.c    // Managing notifications
|   |   |-- handle_server_message.c // Logic for processing messages received from the server
|   |-- shared/
|       |-- utility.c        // Implement utility functions used by both server and client
|-- database/
|   |-- database.c           // Implement database-related functions
|   |-- db_connection.c      // Implement database connection setup using libpq
|   |-- sql_queries.c        // Implement SQL queries used in the application
|-- tests/                   // Include your test files here
|-- docs/
|   |-- README.md            // Project documentation and instructions
|-- .gitignore               // Specify files and directories to be ignored by version control
|-- Makefile                 // Makefile for building the project



```
![403408803_319216974361748_2353456239239778420_n](https://github.com/da0ran9e/Chat-Application/assets/98570451/8c01c3c7-2d60-4e42-8175-65ebf2e916fc)

![403406770_1861191954276363_6140285072657846173_n](https://github.com/da0ran9e/Chat-Application/assets/98570451/942cdeb8-a348-4ce7-a18a-85273b3d2eaf)
![403405948_1572914026851187_5014938361630156887_n](https://github.com/da0ran9e/Chat-Application/assets/98570451/1e9e88f4-ebf9-4cac-aebf-48db491c4b09)

