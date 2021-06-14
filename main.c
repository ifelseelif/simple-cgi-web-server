#include <stdio.h>
#include <string.h>
#include "Server/server.h"
#include "Client/client.h"

int main(int argc, char *argv[]) {
    printf("Goo morning\n");
    printf("Checking arguments...\n");

    if(argc < 2){
        printf("Not enough arguments");
        return 0;
    }

    if(strcmp(argv[1],"server") == 0 && argc ){
        printf("Start as server\n");
        if(argc < 3){
            printf("Not enough arguments for server");
            return 0;
        }

        execute_server(argv[2]);
    }else{
        printf("Start as client\n");
        execute_client(argc, argv);
    }

    printf("Bye\n");
    return 0;
}
