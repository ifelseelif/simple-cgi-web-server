#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main (void) {
    int num;
    time_t t;
    srand (time ( &t) ) ;

    num = rand() % 10;

    printf ("Content-type: text/html \n");
    printf("Pragma: no-cache\n");
    printf ( "\n");
    printf("<!DOCTYPE  html>");
    printf("<html lang='en'>");
    printf("<head>");
    printf("<title>Look This Amazing</title>");
    printf("<meta charset='utf-8'>");
    printf("</head>");
    printf("<body>");
    printf("<h1>Look This Amazing!</h1>");
    printf("</body>");
    printf("</html>");

}