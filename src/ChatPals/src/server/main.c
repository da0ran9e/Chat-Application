#include "../../include/server/feature.h"

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        printf("Usage: %s PortNumber\n", argv[0]);
        exit(1);
    }


    int port = atoi(argv[1]);

    _s_init(port);

    return 0;
}