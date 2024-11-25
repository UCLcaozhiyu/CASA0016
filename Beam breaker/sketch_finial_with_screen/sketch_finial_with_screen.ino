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

// 计数和状态变量
int peopleCount = 0;          // 房间内人数计数
bool vs1838_1Detected = false; // 标记VS1838_1检测状态
bool vs1838_2Detected = false; // 标记VS1838_2检测状态
bool inProcess = false;       // 用于防止重复检测的状态锁

// 定时相关变量
unsigned long lastTriggerTime = 0; // 上一次触发时间
const unsigned long debounceTime = 50; // 去抖延时，单位毫秒
const unsigned long resetDelay = 3000; // 触发状态固定3秒后自动重置

void setup() {
  // 初始化串口
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
  pinMode(ledPin, OUTPUT);       // 红外LED作为输出
  pinMode(vs1838_1, INPUT);      // VS1838_1信号引脚作为输入
  pinMode(vs1838_2, INPUT);      // VS1838_2信号引脚作为输入

  // 发射端：生成38kHz调制信号
  tone(ledPin, 38000); // 在ledPin上输出38kHz的PWM信号

  delay(2000); // 冷却时间，等待系统稳定

  // 初始化显示人数
  updateDisplay();
}

void loop() {
  unsigned long currentTime = millis();

  // 读取传感器状态
  bool vs1838_1State = digitalRead(vs1838_1) == LOW; // 检测到信号为 LOW
  bool vs1838_2State = digitalRead(vs1838_2) == LOW; // 检测到信号为 LOW

  // 如果两边同时触发，忽略这次检测
  if (vs1838_1State && vs1838_2State) {
    Serial.println("两侧传感器同时触发，忽略检测");
    vs1838_1Detected = false;
    vs1838_2Detected = false;
    lastTriggerTime = currentTime; // 重置计时器，避免误判
    delay(10);
    return;
  }

  // 检测进入逻辑：VS1838_1先触发，后跟VS1838_2
  if (!inProcess && vs1838_1State) {
    delay(debounceTime); // 去抖
    if (digitalRead(vs1838_1) == LOW) { // 确认稳定
      vs1838_1Detected = true;
      Serial.println("VS1838_1触发");
    }
  }

  if (vs1838_1Detected && vs1838_2State) {
    delay(debounceTime); // 去抖
    if (digitalRead(vs1838_2) == LOW) { // 确认稳定
      vs1838_2Detected = true;
      peopleCount++;
      Serial.print("有人进入房间，当前人数：");
      Serial.println(peopleCount);

      // 更新显示
      updateDisplay();

      // 重置状态
      vs1838_1Detected = false;
      vs1838_2Detected = false;
      inProcess = true; // 锁定逻辑
      lastTriggerTime = currentTime; // 记录触发时间
    }
  }

  // 检测离开逻辑：VS1838_2先触发，后跟VS1838_1
  if (!inProcess && vs1838_2State) {
    delay(debounceTime); // 去抖
    if (digitalRead(vs1838_2) == LOW) { // 确认稳定
      vs1838_2Detected = true;
      Serial.println("VS1838_2触发");
    }
  }

  if (vs1838_2Detected && vs1838_1State) {
    delay(debounceTime); // 去抖
    if (digitalRead(vs1838_1) == LOW) { // 确认稳定
      vs1838_1Detected = true;
      if (peopleCount > 0) {
        peopleCount--;
        Serial.print("有人离开房间，当前人数：");
        Serial.println(peopleCount);

        // 更新显示
        updateDisplay();
      } else {
        Serial.println("检测到离开信号,但房间内人数为0,忽略操作。");
      }

      // 重置状态
      vs1838_1Detected = false;
      vs1838_2Detected = false;
      inProcess = true; // 锁定逻辑
      lastTriggerTime = currentTime; // 记录触发时间
    }
  }

  // 固定3秒后重置触发状态
  if (currentTime - lastTriggerTime > resetDelay) {
    vs1838_1Detected = false;
    vs1838_2Detected = false;
    inProcess = false;
    lastTriggerTime = currentTime; // 防止重复触发
    Serial.println("状态超时重置");
  }

  // 小延时，避免占用过多CPU时间
  delay(10);
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
