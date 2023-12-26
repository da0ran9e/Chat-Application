#include "../../include/server/feature.h"

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        printf("Usage: %s PortNumber\n", argv[0]);
        exit(1);
    }


    int port = atoi(argv[1]);

    _init(port);
    // if (argc != 2) {
    //     printf("Usage: %s PortNumber\n", argv[0]);
    //     exit(1);
    // }
    // gcc -o app main.c features.c log.c authentication.c connection.c conversations.c relation.c rooms.c ../database/db_connection.c ../database/database.c ../database/sql_queries.c ../utility.c
    // run_client(argv[1], atoi(argv[2]));
    // Parameters p;
    // strcpy(p.Param1, "99");
    // printf("%d\n", handle_features(2,0, 0, p));

    return 0;
}
// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         printf("Usage: %s PortNumber\n", argv[0]);
//         exit(1);
//     }

//     int serverSocket;
//     int port = atoi(argv[1]);

//     initializeServer(&serverSocket, port);

//     runServer(serverSocket);

//     close(serverSocket);

//     return 0;
// }