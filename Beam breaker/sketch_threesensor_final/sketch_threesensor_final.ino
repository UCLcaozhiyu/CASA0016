#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 显示屏宽度和高度定义
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// 红外传感器和LED引脚定义
const int ledPin = 3;         // 红外LED连接到Pin 3 (PWM输出)
const int vs1838_1 = 2;       // 第一个VS1838B的信号引脚连接到Pin 2
const int vs1838_2 = 4;       // 第二个VS1838B的信号引脚连接到Pin 4
const int vs1838_3 = 7;       // 第三个VS1838B的信号引脚连接到Pin 7

// 计数和状态变量
int peopleCount = 0;          // 房间内人数计数
bool vs1838_1Detected = false; // 标记VS1838_1检测状态
bool vs1838_2Detected = false; // 标记VS1838_2检测状态
bool vs1838_3Detected = false; // 标记VS1838_3检测状态
bool vs1838_2Lock = false;     // VS1838_2锁定状态
bool vs1838_3Lock = false;     // VS1838_3锁定状态
bool inProcess = false;        // 用于防止重复检测的状态锁

// 定时相关变量
unsigned long lastTriggerTime = 0; // 上一次触发时间
const unsigned long debounceTime = 50; // 去抖延时，单位毫秒
const unsigned long lockTime = 500;    // 锁定时间，单位毫秒
unsigned long vs1838_2LockTime = 0;    // VS1838_2锁定时间戳
unsigned long vs1838_3LockTime = 0;    // VS1838_3锁定时间戳
const unsigned long resetDelay = 3000; // 触发状态固定3秒后自动重置

void setup() {
  Serial.begin(9600);
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

  // 初始化引脚模式
  pinMode(ledPin, OUTPUT);
  pinMode(vs1838_1, INPUT);
  pinMode(vs1838_2, INPUT);
  pinMode(vs1838_3, INPUT);
  tone(ledPin, 38000);
  delay(2000);

  updateDisplay();
}

void loop() {
  unsigned long currentTime = millis();

  // 如果锁定时间已过，解锁传感器
  if (vs1838_2Lock && currentTime - vs1838_2LockTime > lockTime) vs1838_2Lock = false;
  if (vs1838_3Lock && currentTime - vs1838_3LockTime > lockTime) vs1838_3Lock = false;

  // 读取传感器状态，受锁定影响
  bool vs1838_1State = digitalRead(vs1838_1) == LOW;
  bool vs1838_2State = !vs1838_2Lock && digitalRead(vs1838_2) == LOW;
  bool vs1838_3State = !vs1838_3Lock && digitalRead(vs1838_3) == LOW;

  // 如果三传感器同时触发，忽略检测
   if ((vs1838_1State && vs1838_2State) || (vs1838_1State && vs1838_3State) || (vs1838_2State && vs1838_3State)) {
    Serial.println("多个传感器同时触发，忽略检测");
    resetStates();
    return;
  }

  // 检测进入逻辑
  if (!inProcess && vs1838_1State) {
    delay(debounceTime);
    if (digitalRead(vs1838_1) == LOW) {
      vs1838_1Detected = true;
      Serial.println("VS1838_1触发");
    }
  }

  if (vs1838_1Detected && vs1838_2State) {
    delay(debounceTime);
    if (digitalRead(vs1838_2) == LOW) {
      vs1838_2Detected = true;
      Serial.println("VS1838_2触发");
    }
  }

  if (vs1838_2Detected && vs1838_3State) {
    delay(debounceTime);
    if (digitalRead(vs1838_3) == LOW) {
      peopleCount++;
      Serial.print("有人进入房间，当前人数：");
      Serial.println(peopleCount);
      updateDisplay();
      vs1838_2Lock = true;
      vs1838_3Lock = true;
      vs1838_2LockTime = currentTime;
      vs1838_3LockTime = currentTime;
      resetStates();
      return;
    }
  }

  // 检测离开逻辑
  if (!inProcess && vs1838_3State) {
    delay(debounceTime);
    if (digitalRead(vs1838_3) == LOW) {
      vs1838_3Detected = true;
      Serial.println("VS1838_3触发");
    }
  }

  if (vs1838_3Detected && vs1838_2State) {
    delay(debounceTime);
    if (digitalRead(vs1838_2) == LOW) {
      vs1838_2Detected = true;
      Serial.println("VS1838_2触发");
    }
  }

  if (vs1838_2Detected && vs1838_1State) {
    delay(debounceTime);
    if (digitalRead(vs1838_1) == LOW) {
      if (peopleCount > 0) {
        peopleCount--;
        Serial.print("有人离开房间，当前人数：");
        Serial.println(peopleCount);
        updateDisplay();
        vs1838_1Detected = false;
        vs1838_2Lock = true;
        vs1838_2LockTime = currentTime;
        resetStates();
        return;
      }
    }
  }

  // 固定3秒后重置触发状态
  if (currentTime - lastTriggerTime > resetDelay) {
    resetStates();
    Serial.println("状态超时重置");
  }
  delay(10);
}

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

void resetStates() {
  vs1838_1Detected = false;
  vs1838_2Detected = false;
  vs1838_3Detected = false;
  inProcess = false;
  lastTriggerTime = millis();
}
