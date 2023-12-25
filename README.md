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
![37![387458988_347561424625399_7360024737408271869_n](https://github.com/da0ran9e/Chat-Application/assets/98570451/0e6b2fcf-d8b7-463b-b972-0540acd92240)
0206![385524984_371032058916203_219531895103340878_n](https://github.com/da0ran9e/Chat-Application/assets/98570451/b719001d-07fa-4e87-90f8-d89dacbcc559)
058_3633465760234901_6052871018547640783_n](https://github.com/da0ran9e/Chat-Application/assets/98570451/2caa7dd3-8fff-493c-873d-71df00a0a08c)
