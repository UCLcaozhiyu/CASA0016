#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 显示屏宽度和高度定义
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 红外传感器引脚定义
const int vs1838_1 = 2; // 第一个VS1838B信号引脚
const int vs1838_2 = 4; // 第二个VS1838B信号引脚
const int vs1838_3 = 7; // 第三个VS1838B信号引脚

// 参数定义
int peopleCount = 0;                     // 房间内人数计数
unsigned long lastTriggerTime = 0;       // 上一次触发时间
const unsigned long debounceTime = 50;   // 去抖延时（毫秒）
const unsigned long resetDelay = 2000;   // 队列超时自动重置时间（毫秒）

int triggerQueue[3] = {0, 0, 0};         // 触发队列

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
  display.clearDisplay();
  display.display();

  // 初始化引脚模式
  pinMode(vs1838_1, INPUT);
  pinMode(vs1838_2, INPUT);
  pinMode(vs1838_3, INPUT);

  // 初始化显示人数
  updateDisplay();
}

void loop() {
  unsigned long currentTime = millis();

  // 读取传感器状态
  bool state1 = digitalRead(vs1838_1) == LOW;
  bool state2 = digitalRead(vs1838_2) == LOW;
  bool state3 = digitalRead(vs1838_3) == LOW;

  // 防止同时触发误判
  if (state1 && state2 && state3) {
    Serial.println("检测到同时触发，忽略更新");
    return;
  }

  // 检测单个传感器触发并更新队列
  if (state1) processTrigger(1, currentTime);
  if (state2) processTrigger(2, currentTime);
  if (state3) processTrigger(3, currentTime);

  // 检查触发队列是否符合进出条件
  if (triggerQueue[0] == 1 && triggerQueue[1] == 2 && triggerQueue[2] == 3) {
    handleEntry();
  } else if (triggerQueue[0] == 3 && triggerQueue[1] == 2 && triggerQueue[2] == 1) {
    handleExit();
  }

  // 超时重置队列
  if (triggerQueue[0] != 0 && currentTime - lastTriggerTime > resetDelay) {
    clearQueue();
    Serial.println("队列超时重置");
  }
}

// 处理传感器触发逻辑
void processTrigger(int sensorID, unsigned long currentTime) {
  // 去抖动处理
  if (currentTime - lastTriggerTime < debounceTime) return;

  lastTriggerTime = currentTime;

  // 更新队列
  if (triggerQueue[2] != sensorID) {
    for (int i = 0; i < 2; i++) {
      triggerQueue[i] = triggerQueue[i + 1];
    }
    triggerQueue[2] = sensorID;

    Serial.print("队列更新: ");
    Serial.print(triggerQueue[0]);
    Serial.print(", ");
    Serial.print(triggerQueue[1]);
    Serial.print(", ");
    Serial.println(triggerQueue[2]);
  }
}

// 处理进入房间
void handleEntry() {
  peopleCount++;
  Serial.println("检测到进入房间");
  clearQueue();
  updateDisplay();
}

// 处理离开房间
void handleExit() {
  if (peopleCount > 0) {
    peopleCount--;
    Serial.println("检测到离开房间");
    updateDisplay();
  } else {
    Serial.println("检测到离开信号，但房间人数为0，忽略操作");
  }
  clearQueue();
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
