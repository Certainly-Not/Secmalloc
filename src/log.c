
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "log.h"

void my_log(uint8_t level, const char *fmt, ...)
{
    va_list ap;
    char *c_buf;
    char buff_info[20] = { 0 };
    char *log_lvl;
    uint8_t choosen_lvl;
    size_t write_sz;
    va_start(ap, fmt);
    log_lvl = getenv(ENV_NAME);
    if(log_lvl == NULL){
        return;
    }
    size_t sz = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if(level == LOG_INFO){
        memcpy(buff_info, LOG_MESSAGE_INFO, sizeof(LOG_MESSAGE_INFO));
    }
    if(level == LOG_WARNING){
        memcpy(buff_info, LOG_MESSAGE_WARNING, sizeof(LOG_MESSAGE_WARNING));
    }
    if(level == LOG_ERROR){
        memcpy(buff_info, LOG_MESSAGE_ERROR, sizeof(LOG_MESSAGE_ERROR));
    }
    choosen_lvl = (uint8_t)atoi(log_lvl);
    if((choosen_lvl & level) == level){
        c_buf = alloca(sz + 2);
        va_start(ap, fmt);
        vsnprintf(c_buf, sz + 2, fmt, ap);
        va_end(ap);
        write_sz = write(2, buff_info, strlen(buff_info));
        write_sz = write(2, c_buf, sz);
        write_sz = write(2, LOG_COLOR_RESET, sizeof(LOG_COLOR_RESET));
        (void)write_sz;
    }
}
