#include "../../include/server/feature.h"

int main(int argc, char const *argv[]) {
    // if (argc != 2) {
    //     printf("Usage: %s PortNumber\n", argv[0]);
    //     exit(1);
    // }
    // gcc -o app main.c feature.c log.c authentication.c connection.c conversation.c relation.c rooms.c ../../database/dbc.c ../../database/database.c ../../database/query.c ../../
    // run_client(argv[1], atoi(argv[2]));
    Parameters p;
    p.Param1 = 99;
    printf("%d\n", handle_features(2,0, 0, p));

    return 0;
}