#define LGFX_AUTODETECT
#include <LGFX_AUTODETECT.hpp>
#include <Arduino.h>
#include <lvgl.h>
#include <SD.h>
#include "FS.h"
#include "LittleFS.h"
#include "lcd.h"

static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 480;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][screenWidth * 10];

lv_obj_t *img = NULL;
lv_obj_t *page0 = NULL;

myLGFX gfx;

// LVGL 显示刷新回调
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  
  if (gfx.getStartCount() == 0) {
    gfx.startWrite();
  }
  gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1,
                   (lgfx::swap565_t *)&color_p->full);
  lv_disp_flush_ready(disp);
}

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
LV_IMG_DECLARE(test_screen);
// 创建 UI 页面 page0，显示图片
void create_page0() {
  
  page0 = lv_obj_create(lv_scr_act());
  lv_obj_set_size(page0, screenWidth, screenHeight);
  lv_obj_center(page0);

  img = lv_img_create(page0);
  lv_img_set_src(img, "L:/res/img/test_screen.png");  // LittleFS 使用 L:/ 前缀
  //lv_img_set_src(img, &test_screen);  // 使用 LVGL 内嵌的图片资源
  lv_obj_center(img);
}

void setup() {
  Serial.begin(115200);
  //while (!Serial);  // USB CDC 等待连接（ESP32-S3）

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  } else {
    Serial.println("LittleFS mounted.");
  }

  gfx.begin();
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf[0], buf[1], screenWidth * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);

  lv_fs_littlefs_init();       // 初始化 LittleFS 文件系统给 LVGL 用
  const char* imgPath = "/res/img/test_screen.png";
if (LittleFS.exists(imgPath)) {
  File f = LittleFS.open(imgPath, "r");
  Serial.printf("File %s exist, size=%d\n", imgPath, (int)f.size());
  f.close();
} else {
  Serial.printf("File %s not found!\n", imgPath);
}
  create_page0();              // 创建页面
}

void loop() {
  lv_timer_handler();
  delay(5);
    //delay(3000);
    //lv_img_set_src(img, "L:/res/img/test_screen_truecolor.bin");
    //Serial.println("Set image to indexed 8-bit RGB565");
    //delay(3000);
    //lv_img_set_src(img, "L:/res/img/test_screen.jpg"); 
    //Serial.println("Set image to JPG");
    //delay(3000);
    //lv_img_set_src(img, "L:/res/img/test_screen.png");
    //Serial.println("Set image to PNG");
    //delay(3000);
    //lv_img_set_src(img, &test_screen);
    //Serial.println("Set image to LVGL embedded resource");

}
