//
// Created by Артём on 09.06.2021.
//

#include "http_response_errors.h"

static const char * not_found_response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Length: 111\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "\r\n"
        "<html>\n"
        "<head>\n"
        "    <title>404 - Not Found</title>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>File not found :(</h1>\n"
        "</body>\n"
        "</html>\n";

static const char * forbidden_response =
        "HTTP/1.1 403 Forbidden\r\n"
        "Content-Length: 114\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "\r\n"
        "<html>\n"
        "<head>\n"
        "    <title>403 - Forbidden</title>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>Access forbidden >:(</h1>\n"
        "</body>\n"
        "</html>\n";