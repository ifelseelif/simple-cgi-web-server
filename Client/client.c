//
// Created by Артём on 07.06.2021.
//

#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include "client.h"

struct arg_struct {
    char *source;
    char *save_name;
};

#define max_urls 10

int is_parallel = 0;
FILE *files[max_urls];
pthread_t pthreads[max_urls];

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

static void *donwload_file(char *save_name, char *source) {

    char *filename = save_name;
    remove(filename);
    FILE *file = NULL;
    file = fopen(filename, "wb");

    if (file == NULL) {
        printf("File could not opened");
    }

    CURL *curl;
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, source);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_perform(curl); /* ignores error */
    curl_easy_cleanup(curl);

    fclose(file);
}

static void add_transfer(CURLM *cm, int j, char *source, char *file) {
    CURL *eh = curl_easy_init();

    remove(file);
    files[j] = fopen(file, "wb");

    curl_easy_setopt(eh, CURLOPT_URL, source);
    curl_easy_setopt(eh, CURLOPT_WRITEDATA, files[j]);
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_data);
    curl_multi_add_handle(cm, eh);
}

void execute_client(int argc, char *argv[]) {
    CURLM *cm;
    CURLMsg *msg;
    int count_pthreads;
    int still_alive = 1;
    int msgs_left = -1;

    if (strcmp(argv[2], "parallel") == 0) {
        is_parallel = 1;
        curl_global_init(CURL_GLOBAL_ALL);
        cm = curl_multi_init();
        count_pthreads = argc - 2;
        if (count_pthreads <= 0) {
            printf("Not enough arguments");
            return;
        }
        curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, (long) count_pthreads);
    }

    int j = 0;
    char filename[1000];
    int has_name = 0;
    for (int i = 3; i < argc; i++) {
        char *in = strchr(argv[i], '~');
        if (in != NULL) {
            strcpy(filename, in + 1);
            has_name = 1;
            continue;
        } else {
            struct arg_struct args;
            if (has_name == 1) {
                args.save_name = filename;
            } else {
                args.save_name = strrchr(argv[i], '/') + 1;
            }
            args.source = argv[i];
            has_name = 0;
            if (is_parallel == 1) {
                //pthread_create(&pthreads[j], NULL, donwload_file, (void *) &args);
                add_transfer(cm, j, args.source, args.save_name);
                j = j + 1;
            } else {
                donwload_file(args.save_name, args.source);
            }
        }
    }

    if (is_parallel == 1) {
        do {
            curl_multi_perform(cm, &still_alive);
            while ((msg = curl_multi_info_read(cm, &msgs_left))) {
                if (msg->msg == CURLMSG_DONE) {
                    char *url;
                    CURL *e = msg->easy_handle;
                    fprintf(stdout, "R: %d - %s <%s>\n",
                            msg->data.result, curl_easy_strerror(msg->data.result), url);
                    curl_multi_remove_handle(cm, e);
                    curl_easy_cleanup(e);
                } else {
                    fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
                }
            }
            if (still_alive)
                curl_multi_wait(cm, NULL, 0, 3000, NULL);

        } while (still_alive);

        curl_multi_cleanup(cm);
        curl_global_cleanup();

        for (int i = 0; i < j; i++) {
            fclose(files[i]);
        }
    }
}

