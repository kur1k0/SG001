#define LGFX_AUTODETECT
#include <LGFX_AUTODETECT.hpp>
#include <Arduino.h>
#include <lvgl.h>
#include <SD.h>
#include "FS.h"
#include "LittleFS.h"
#include "lcd.h"
#include "sms_parse.h"
#include "fs_parse.h"
#include <stdio.h>
#include <string.h>

#define MAX_SMS_LINE 256

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



LV_IMG_DECLARE(test_screen);
// 创建 UI 页面 page0，显示图片
void create_page0() {
    // 页面容器
    page0 = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(page0);
    lv_obj_set_size(page0, screenWidth, screenHeight);
    lv_obj_set_scrollbar_mode(page0, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(page0, LV_OBJ_FLAG_SCROLLABLE);

    // 背景图片
    img = lv_img_create(page0);
    lv_img_set_src(img, "L:/res/img/test_screen.sjpg");
    lv_obj_clear_flag(img, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(img, 0, 0);
    lv_obj_set_style_border_width(img, 0, 0);
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
  sms_parsed_t sms;
if (sms_parse_file("/res/sms/sms000.txt", &sms)) {
    printf("发件人: %s\n", sms.sender);
    printf("时间戳: %s\n", sms.timestamp);
    printf("正文: %s\n", sms.content);
}else {
    printf("解析短信文件失败或文件内容不足。\n");
}

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
