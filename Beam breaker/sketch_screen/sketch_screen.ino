#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 显示屏宽度和高度定义
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup() {
  // 初始化串口
  Serial.begin(9600);
  Serial.println("初始化开始...");
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED init failed"));
while(true);}
  display.clearDisplay();
  display.display();
  Serial.println("初始化完成");
  display.clearDisplay();             // 清除显示缓冲区
  display.setTextSize(1);             // 设置文字大小
  display.setTextColor(SSD1306_WHITE);// 设置文字颜色
  display.setCursor(0, 10);           // 设置光标位置
  display.println("Hello, SSD1306!"); // 显示文字
  display.display();                  // 更新显示
}

void loop() {
  // 空循环
} 