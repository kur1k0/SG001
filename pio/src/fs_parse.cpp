#include "fs_parse.h"

void *fs_open_cb(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode) {
  String fullPath = String("/") + path;
  File file = (mode == LV_FS_MODE_WR) ? LittleFS.open(fullPath, "w") : LittleFS.open(fullPath, "r");
  if (!file) return nullptr;
  File *fp = new File(file);
  return fp;
}

lv_fs_res_t fs_close_cb(lv_fs_drv_t *drv, void *file_p) {
    File *file = static_cast<File *>(file_p);
    file->close();
    delete file;
    return LV_FS_RES_OK;
}

lv_fs_res_t fs_read_cb(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br) {
  File *file = static_cast<File *>(file_p);
  Serial.printf("[LVGL FS] read available=%d size=%d pos=%d\n", file->available(), file->size(), file->position());
  *br = file->read((uint8_t *)buf, btr);
  return LV_FS_RES_OK;
}

lv_fs_res_t fs_seek_cb(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence) {
  File *file = static_cast<File *>(file_p);
  return file->seek(pos, (whence == LV_FS_SEEK_SET) ? SeekSet : 
                          (whence == LV_FS_SEEK_CUR) ? SeekCur : SeekEnd)
         ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

lv_fs_res_t fs_tell_cb(lv_fs_drv_t *drv, void *file_p, uint32_t *pos) {
  File *file = static_cast<File *>(file_p);
  *pos = file->position();
  return LV_FS_RES_OK;
}

void lv_fs_littlefs_init() {
  static lv_fs_drv_t fs_drv;
  lv_fs_drv_init(&fs_drv);
  fs_drv.letter = 'L';
  fs_drv.open_cb  = fs_open_cb;
  fs_drv.close_cb = fs_close_cb;
  fs_drv.read_cb  = fs_read_cb;
  fs_drv.seek_cb  = fs_seek_cb;
  fs_drv.tell_cb  = fs_tell_cb;
  lv_fs_drv_register(&fs_drv);
}
