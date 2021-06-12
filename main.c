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

    if(strcmp(argv[1],"server") == 0){
        printf("Start as server\n");
        execute_server();
    }else{
        printf("Start as client\n");
        execute_client();
    }

    printf("Bye\n");
    return 0;
}
