
/**
 * @file gfxpLog.hpp
 * Contains definitions of macros for console output messages
*/

#ifndef GFXP_LOG_H
#define GFXP_LOG_H

#include <stdio.h>

#define GREEN_TEXT_COLOR        "\033[0;32m"
#define YELLOW_TEXT_COLOR       "\033[0;33m"
#define RED_TEXT_COLOR          "\033[0;31m"
#define WHITE_TEXT_COLOR        "\033[0;37m"

#define LOG_INFO(msg, ...)       fprintf(stdout, GREEN_TEXT_COLOR  "[ INFO ] " msg WHITE_TEXT_COLOR "\n", ##__VA_ARGS__);
#define LOG_WARN(msg, ...)       fprintf(stderr, YELLOW_TEXT_COLOR "[ WARNING ] " msg WHITE_TEXT_COLOR "\n", ##__VA_ARGS__);
#define LOG_ERROR(msg, ...)      fprintf(stderr, RED_TEXT_COLOR "[ ERROR ] " msg WHITE_TEXT_COLOR "\n", ##__VA_ARGS__);

#endif // GFXP_LOG_H
