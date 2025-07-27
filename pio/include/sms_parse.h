#ifndef SMS_PARSE_H
#define SMS_PARSE_H

#include <FS.h>
#include <stdio.h>
#include <string.h>
#include "LittleFS.h"

#define MAX_SMS_LINE 256

typedef struct {
    char sender[32];
    char timestamp[16]; // 原始字符串，不转 time_t（方便显示）
    char content[MAX_SMS_LINE];
} sms_parsed_t;

bool sms_parse_file(const char *path, sms_parsed_t *out_sms);

#endif
