#include "sms_parse.h"

bool sms_parse_file(const char *path, sms_parsed_t *out_sms) {
    Serial.printf("🔍 正在解析短信文件: %s\n", path);

    if (!LittleFS.exists(path)) {
        Serial.printf("❌ 文件未找到: %s\n", path);
        return false;
    }

    File f = LittleFS.open(path, "r");
    if (!f || f.isDirectory()) {
        Serial.println("❌ 无法打开文件或路径是目录");
        return false;
    }
    Serial.printf("✅ 文件已打开, 大小 = %d bytes\n", (int)f.size());

    char line[MAX_SMS_LINE];
    int line_num = 0;

    while (f.available() && line_num < 3) {
        int len = f.readBytesUntil('\n', line, sizeof(line) - 1);
        line[len] = '\0';

        // 去除末尾换行
        char *newline = strpbrk(line, "\r\n");
        if (newline) *newline = '\0';

        Serial.printf("📄 第 %d 行原始: [%s]\n", line_num + 1, line);

        switch (line_num) {
            case 0:
                strncpy(out_sms->sender, line, sizeof(out_sms->sender) - 1);
                out_sms->sender[sizeof(out_sms->sender) - 1] = '\0';
                Serial.printf("✅ 发件人: %s\n", out_sms->sender);
                break;
            case 1:
                strncpy(out_sms->timestamp, line, sizeof(out_sms->timestamp) - 1);
                out_sms->timestamp[sizeof(out_sms->timestamp) - 1] = '\0';
                Serial.printf("✅ 时间戳: %s\n", out_sms->timestamp);
                break;
            case 2:
                strncpy(out_sms->content, line, sizeof(out_sms->content) - 1);
                out_sms->content[sizeof(out_sms->content) - 1] = '\0';
                Serial.printf("✅ 正文: %s\n", out_sms->content);
                break;
        }

        line_num++;
    }

    f.close();

    if (line_num < 3) {
        Serial.printf("⚠️  读取失败，仅读取 %d 行\n", line_num);
        return false;
    }

    Serial.println("🎉 短信解析完成！");
    return true;
}
