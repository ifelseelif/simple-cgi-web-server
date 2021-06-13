//
// Created by ifelseelif on 13.06.2021.
//

#ifndef LAB3_CONFIG_WORKER_H
#define LAB3_CONFIG_WORKER_H

#define  BUF_KEY  32          // размер буфера для ключа
#define  BUF_VAL  255         // размер буфера для значения
#define  MAX_INI_RECORDS  7   // кол-во записей в ini-файле

typedef struct {
    char key[BUF_KEY];
    char val[BUF_VAL];
} value;

int start_work_with_config_file(char *workdir, char *config_file);

char *get_value(const char *key);

#endif //LAB3_CONFIG_WORKER_H
