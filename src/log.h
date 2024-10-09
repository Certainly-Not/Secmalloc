#ifndef LOG_H
#define LOG_H

#define LOG_INFO        0b001
#define LOG_WARNING     0b010
#define LOG_ERROR       0b100

#define LOG_MESSAGE_INFO    "\x1b[37m[INFO] "
#define LOG_MESSAGE_WARNING "\x1b[33m[WARNING] "
#define LOG_MESSAGE_ERROR   "\x1b[31m[ERROR] "
#define LOG_COLOR_RESET     "\033[0m"
#define ENV_NAME            "LOG_LVL"

void my_log(uint8_t level, const char *fmt, ...);
#endif