#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 显示屏宽度和高度定义
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 红外传感器和LED引脚定义
const int ledPin = 3;          // 红外LED连接到Pin 3 (PWM输出)
const int vs1838_1 = 2;        // 第一个VS1838B信号引脚
const int vs1838_2 = 4;        // 第二个VS1838B信号引脚
const int vs1838_3 = 7;        // 第三个VS1838B信号引脚

// 计数和状态变量
int peopleCount = 0;           // 房间内人数计数
bool inProcess = false;        // 状态锁，防止重复检测
unsigned long lastTriggerTime = 0;  // 上一次触发时间
const unsigned long debounceTime = 50; // 去抖延时，单位毫秒
const unsigned long resetDelay = 3000; // 状态固定3秒后自动重置

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
  pinMode(ledPin, OUTPUT);
  pinMode(vs1838_1, INPUT);
  pinMode(vs1838_2, INPUT);
  pinMode(vs1838_3, INPUT);

  // 发射端：生成38kHz调制信号
  tone(ledPin, 38000);

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
  if (currentTime - lastTriggerTime > resetDelay) {
    clearQueue();
    Serial.println("状态超时重置");
  }

  delay(10);
}

// 更新队列
void updateQueue(int sensorID) {
  lastTriggerTime = millis();
  // 若队列未满，添加触发传感器ID
  if (triggerQueue[2] == 0) {
    for (int i = 0; i < 3; i++) {
      if (triggerQueue[i] == 0) {
        triggerQueue[i] = sensorID;
        Serial.print("传感器");
        Serial.print(sensorID);
        Serial.println("触发");
        break;
      }
    }
  }
}

// 清空触发队列
void clearQueue() {
  for (int i = 0; i < 3; i++) {
    triggerQueue[i] = 0;
  }
  inProcess = false;
}

// 更新OLED显示
void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(3);
  display.println("People:");
  display.setTextSize(3);
  display.setCursor(0, 30);
  display.print(peopleCount);
  display.display();
}
