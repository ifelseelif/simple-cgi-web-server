#include <stdio.h>
#include <string.h>
#include "Server/server.h"
#include "Client/client.h"

int main(int argc, char *argv[]) {
    printf("Добрый день\n");
    printf("Идет проверка аргументов\n");

    if(argc < 2){
        printf("Недостаточно аргументов");
        return 0;
    }

    if(strcmp(argv[1],"server") == 0){
        printf("Запускаюсь, как сервер\n");
        execute_server();
    }else{
        printf("Запускаюсь, как клиент\n");
        execute_client();
    }

    printf("До свидания\n");
    return 0;
}
