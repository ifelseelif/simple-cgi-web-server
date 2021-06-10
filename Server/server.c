//
// Created by Артём on 07.06.2021.
//
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h>
#include "server.h"
#include "http_response_errors.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <ctype.h>

const char *SERVER_PORT = "8085";

atomic_bool stop_socket_thread = false;

#define BUFSIZE 1024

extern char **environ; /* the environment */

/*
 * error - wrapper for perror used for bad syscalls
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

/*
 * cerror - returns an error message to the client
 */
void cerror(int client) {
    send(client, not_found_response, strlen(not_found_response), 0);
}

void *start_server(void *data) {

    /* variables for connection management */
    int parentfd;          /* parent socket */
    int portno;            /* port to listen on */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */

    /* variables for connection I/O */
    char buf[BUFSIZE];     /* message buffer */
    char method[BUFSIZE];  /* request method */
    char uri[BUFSIZE];     /* request uri */
    char version[BUFSIZE]; /* request method */
    char filename[BUFSIZE];/* path derived from uri */
    char filetype[BUFSIZE];/* path derived from uri */
    char cgiargs[BUFSIZE]; /* cgi argument list */
    char *p;               /* temporary pointer */
    int is_static;         /* static request? */
    struct stat sbuf;      /* file status */
    int fd;                /* static content filedes */
    int pid;               /* process id from fork */
    int wait_status;       /* status from wait */

    portno = atoi(SERVER_PORT);

    /* open socket descriptor */
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0) {
        error("ERROR opening socket");
        return 0;
    }
    printf("[OK]: Socket started successfully \n");

    /* bind port to socket */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short) portno);
    if (bind(parentfd, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0)
        error("ERROR on binding");
    printf("[OK]: Successfully binded \n");

    /* get us ready to accept connection requests */
    if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */
        error("ERROR on listen");


    printf("[INFO]: Try to open 'http://127.0.0.1:%s' in your browser\n", SERVER_PORT);
    /*
     * main loop: wait for a connection request, parse HTTP,
     * serve requested content, close connection.
     */
    while (1) {
        if (stop_socket_thread) {
            printf("Stop server");
            return 0;
        }

        int client_socket_fd;
        int client_address_len;

        client_address_len = sizeof(clientaddr);

        /* open the child socket descriptor*/
        if ((client_socket_fd = accept(parentfd, (struct sockaddr *) &clientaddr, &client_address_len)) < 0)
            error("ERROR on fdopen");

        /* get the HTTP request line */
        read(client_socket_fd, buf, BUFSIZE);
        //printf("%s", buf);
        sscanf(buf, "%s %s %s\n", method, uri, version);
        printf("%s %s", uri);


        /* tiny only supports the GET method */
        if (strcasecmp(method, "GET")) {
            cerror(client_socket_fd);
            close(client_socket_fd);
            continue;
        }

        /* parse the uri [crufty] */
        if (!strstr(uri, "cgi-bin")) { /* static content */
            cerror("Start from cgi-bin?name_cgi_script");
            continue;
        } else { /* dynamic content */
            is_static = 0;
            p = index(uri, '?');
            if (p) {
                strcpy(cgiargs, p + 1);
                *p = '\0';
            } else {
                strcpy(cgiargs, "");
            }
            strcpy(filename, ".");
            strcat(filename, uri);
            printf("%s", filename);
        }

        /* make sure the file exists */
        if (stat(filename, &sbuf) < 0) {
            cerror(client_socket_fd);
            close(client_socket_fd);
            continue;
        }

        /* a real server would set other CGI environ vars as well*/
        setenv("QUERY_STRING", cgiargs, 1);
        setenv("REQUEST_METHOD", method, 1);

        /* print first part of response header */
        sprintf(buf, "HTTP/1.1 200 OK\n");

        send(client_socket_fd, buf, strlen(buf), 0);
        sprintf(buf, "Content-Type: text/html; charset=utf-8\r\n");
        send(client_socket_fd, buf, strlen(buf), 0);
        sprintf(buf, "Server: Tiny Web Server\n");
        send(client_socket_fd, buf, strlen(buf), 0);

        /* create and run the child CGI process so that all child
           output to stdout and stderr goes back to the client via the
           childfd socket descriptor */

        pid = fork();
        if (pid < 0) {
            perror("ERROR in fork");
            exit(1);
        } else if (pid > 0) { /* parent process */
            wait(&wait_status);
        } else { /* child  process*/
            dup2(client_socket_fd, 1); /* map socket to stdout */
            dup2(client_socket_fd, 2); /* map socket to stderr */

            char *arg[] = {"/bin/sh", filename, (char *) 0};
            if (execve("/bin/sh", arg, environ) < 0) {
                perror("ERROR in execve");
            }
            close(0); /* close stdin */

        }


        /* clean up */
        close(client_socket_fd);
    }
}

void execute_server() {
    void *thread_data = NULL;
    pthread_t thread;
    pthread_create(&thread, NULL, start_server, thread_data);

    while (1) {
        if (getchar() == (int) 'c') {
            stop_socket_thread = true;
            break;
        }
    }

    pthread_join(thread, NULL);
}
