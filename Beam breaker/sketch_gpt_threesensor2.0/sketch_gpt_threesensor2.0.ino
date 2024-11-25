#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 显示屏宽度和高度定义
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 红外传感器引脚定义
const int vs1838_1 = 2;        // 第一个VS1838B信号引脚
const int vs1838_2 = 4;        // 第二个VS1838B信号引脚
const int vs1838_3 = 7;        // 第三个VS1838B信号引脚

// 计数和状态变量
int peopleCount = 0;               // 房间内人数计数
unsigned long lastTriggerTime = 0; // 上一次触发时间
unsigned long lastDelayTime = 0;   // 检测到同时触发的延迟时间
const unsigned long debounceTime = 50; // 去抖延时，单位毫秒
const unsigned long resetDelay = 1000; // 状态固定3秒后自动重置

// 状态队列，用于记录触发顺序
int triggerQueue[3] = {0, 0, 0};

// 初始化函数
void setup() {
  Serial.begin(9600);
  Serial.println("房间人数检测系统启动...");

  // 初始化OLED显示屏
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED初始化失败"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("初始化完成");
  display.display();
  delay(2000);

  // 显示屏清空
  display.clearDisplay();
  display.display();

  // 初始化引脚模式
  pinMode(vs1838_1, INPUT);
  pinMode(vs1838_2, INPUT);
  pinMode(vs1838_3, INPUT);

  // 初始化 lastTriggerTime
  lastTriggerTime = millis();

  // 初始化显示人数
  updateDisplay();
}

// 主循环
void loop() {
  unsigned long currentTime = millis();

  // 读取传感器状态
  bool state1 = digitalRead(vs1838_1) == LOW;
  bool state2 = digitalRead(vs1838_2) == LOW;
  bool state3 = digitalRead(vs1838_3) == LOW;

  // 检测同时触发
  if (state1 && state2 && state3) {
    //if (currentTime - lastDelayTime > debounceTime) {
      Serial.println("检测到同时触发，忽略本次更新");
      lastDelayTime = currentTime;
  //  }
    return;
  }

  // 更新触发队列
  if (state1) updateQueue(1);
  if (state2) updateQueue(2);
  if (state3) updateQueue(3);

  // 判断触发序列
  if (triggerQueue[0] == 1 && triggerQueue[1] == 2 && triggerQueue[2] == 3) {
    // 检测到进入
    peopleCount++;
    Serial.println("检测到进入房间");
    clearQueue();
    updateDisplay();
  } else if (triggerQueue[0] == 3 && triggerQueue[1] == 2 && triggerQueue[2] == 1) {
    // 检测到离开
    if (peopleCount > 0) {
      peopleCount--;
      Serial.println("检测到离开房间");
      updateDisplay();
    } else {
      Serial.println("检测到离开信号，但房间人数为0，忽略操作");
    }
    clearQueue();
  }

  // 超时重置
  if (triggerQueue[0] != 0 && currentTime - lastTriggerTime > resetDelay) {
    clearQueue();
    Serial.println("状态超时重置");
  }
}

// 更新队列
void updateQueue(int sensorID) {
  lastTriggerTime = millis();

  // 避免重复记录
  if (triggerQueue[2] == sensorID) {
    return; // 如果重复触发最后一个传感器，直接忽略
  }

  // 移动队列内容，形成滑动窗口
  for (int i = 0; i < 2; i++) {
    triggerQueue[i] = triggerQueue[i + 1];
  }
  triggerQueue[2] = sensorID; // 在队列末尾插入新触发的传感器ID

  // 打印调试信息
  Serial.print("队列更新: ");
  Serial.print(triggerQueue[0]);
  Serial.print(", ");
  Serial.print(triggerQueue[1]);
  Serial.print(", ");
  Serial.println(triggerQueue[2]);
}

// 清空触发队列
void clearQueue() {
  for (int i = 0; i < 3; i++) {
    triggerQueue[i] = 0;
  }
  Serial.println("队列已清空");
}

// 更新OLED显示
void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(3);
  display.print("People: ");
  display.println(peopleCount);
  display.display();
}
