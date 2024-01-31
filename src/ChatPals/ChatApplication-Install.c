#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Welcome!\n\n\n________________________ChatPals________________________\n\n\n\nWhat part of application that you want to install on this machine?\n1.Server\n2.Client\n3.GUI demotration\n(Enter a number [1-3])");
    int choose, result;
    scanf("%d", &choose);
    switch (choose)
    {
    case 1:
        result = system("./server_build.sh");
        break;
    case 2:
        result = system("./client_build.sh");
        break;
    case 3:
        result = system("./gui_demo_build.sh");
        break;
    default:
        break;
    }
    
    if (result == 0) {
        printf("Build script executed successfully.\n");
    } else {
        printf("Error executing build script. Exit code: %d\n", result);
    }

    return 0;
}
