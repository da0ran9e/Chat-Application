#include "../../include/server/feature.h"

int main(int argc, char const *argv[]) {
    // if (argc != 2) {
    //     printf("Usage: %s PortNumber\n", argv[0]);
    //     exit(1);
    // }

    // run_client(argv[1], atoi(argv[2]));
    Parameters p;
    p.Param1 = 99;
    printf("%d\n", handle_features(2,0, 0, p));

    return 0;
}