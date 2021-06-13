//
// Created by ifelseelif on 13.06.2021.
//

#include "config_worker.h"
#include "server.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/stat.h>

char *get_value(const char *key);

int open_config_file(const char *filename, value *arr);

value *vals = NULL;
int count = 0;

char *find_path(char *start_dir, char *filename) {
    char path[5000];
    struct dirent *fi;
    if (strcmp(start_dir, "/")) strcat(start_dir, "/");
    DIR *di = opendir(start_dir);
    struct stat stt;
    if (!di) {
        return NULL;
    }
    while ((fi = readdir(di))) {
        if (!strcmp(fi->d_name, ".") || !strcmp(fi->d_name, ".."))continue;
        if (!strcmp(fi->d_name, filename)) {
            memset(&path[0], 0, sizeof(path));
            strcpy(path, start_dir);
            strcpy(path, filename);
            char *result = path;
            return result;
        }
        strcpy(path, start_dir);
        strcat(path, fi->d_name);
        lstat(path, &stt);
        if (S_ISDIR(stt.st_mode)) {
            char *res = find_path(path, filename);
            if (res == NULL) {
                continue;
            } else {
                return res;
            }
        }
    }
    closedir(di);
    return NULL;
}

int start_work_with_config_file(char *workdir, char *config_file) {
    char *path_file = find_path(workdir, config_file);
    if (path_file == NULL) {
        error("Can't find server config file");
    }

    vals = (value *) malloc(sizeof(value) * MAX_INI_RECORDS);
    if (vals == NULL)
        error("Can't create space for config");

    if (!open_config_file(path_file, vals)) {
        free(vals);
        error("Can't open config file");
    }

    // вывод всех пар ключ = значение
    int i;
    for(i = 0; i < count; i++)
        printf("%s = %s\n", vals[i].key, vals[i].val );
    return 0;
}

char *get_value(const char *key) {
    int i;
    for (i = 0; i < count; i++) {
        if (!strcmp(vals[i].key, key))
            return vals[i].val;
    }
    return NULL;
}

int open_config_file(const char *filename, value *arr) {
    value val;
    FILE *fp;
    int cnt;
    if (!(fp = fopen(filename, "r")))
        return 0;
    memset((void *) &val, 0, sizeof(value));

    for (cnt = 0; fscanf(fp, "%[^=]=%[^\n]%*1c", val.key, val.val) > 1; cnt++){
        memcpy((void *) &arr[cnt], (const void *) &val, sizeof(value));
        count++;
    }
    fclose(fp);
    return 1;
}

