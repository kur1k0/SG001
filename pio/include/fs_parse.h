#ifndef FS_PARSE_H
#define FS_PARSE_H

#include <lvgl.h>
#include <LittleFS.h>
#include <Arduino.h>
#include <FS.h>
#include <stdint.h>
#include <string.h>

void *fs_open_cb(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode);
lv_fs_res_t fs_close_cb(lv_fs_drv_t *drv, void *file_p);
lv_fs_res_t fs_read_cb(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
lv_fs_res_t fs_seek_cb(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence);
lv_fs_res_t fs_tell_cb(lv_fs_drv_t *drv, void *file_p, uint32_t *pos);
void lv_fs_littlefs_init();

#endif // FS_PARSE_H
