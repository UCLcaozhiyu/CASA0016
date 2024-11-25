// 引脚定义
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
const unsigned long processDelay = 1000; // 每次进入/离开逻辑的延时

void setup() {
  // 初始化引脚模式
  pinMode(ledPin, OUTPUT);       // 红外LED作为输出
  pinMode(vs1838_1, INPUT);      // VS1838_1信号引脚作为输入
  pinMode(vs1838_2, INPUT);      // VS1838_2信号引脚作为输入

  // 初始化串口，用于调试
  Serial.begin(9600);
  Serial.println("房间人数检测系统启动...");

  // 发射端：生成38kHz调制信号
  tone(ledPin, 38000); // 在ledPin上输出38kHz的PWM信号

  delay(2000); // 冷却时间，等待系统稳定
}

void loop() {
  // 非阻塞逻辑，检查时间差来处理事件
  unsigned long currentTime = millis();

  // 检测进入情况：VS1838_1先触发，再触发VS1838_2
  if (!inProcess && digitalRead(vs1838_1) == LOW) {
    delay(debounceTime); // 去抖
    if (digitalRead(vs1838_1) == LOW) { // 确认状态稳定
      vs1838_1Detected = true; // 标记VS1838_1已触发
      Serial.println("VS1838_1触发");
    }
  }

  if (vs1838_1Detected && digitalRead(vs1838_2) == LOW) {
    delay(debounceTime); // 去抖
    if (digitalRead(vs1838_2) == LOW) { // 确认状态稳定
      vs1838_2Detected = true; // 标记VS1838_2已触发
      peopleCount++;           // 计数加1
      Serial.print("有人进入房间，当前人数：");
      Serial.println(peopleCount);

      // 状态重置和延时锁
      vs1838_1Detected = false;
      vs1838_2Detected = false;
      inProcess = true; // 锁定逻辑
      lastTriggerTime = currentTime; // 记录触发时间
    }
  }

  // 检测离开情况：VS1838_2先触发，再触发VS1838_1
  if (!inProcess && digitalRead(vs1838_2) == LOW) {
    delay(debounceTime); // 去抖
    if (digitalRead(vs1838_2) == LOW) { // 确认状态稳定
      vs1838_2Detected = true; // 标记VS1838_2已触发
      Serial.println("VS1838_2触发");
    }
  }

  if (vs1838_2Detected && digitalRead(vs1838_1) == LOW) {
    delay(debounceTime); // 去抖
    if (digitalRead(vs1838_1) == LOW) { // 确认状态稳定
      vs1838_1Detected = true; // 标记VS1838_1已触发
      if (peopleCount > 0) {
        peopleCount--;         // 计数减1
        Serial.print("有人离开房间，当前人数：");
        Serial.println(peopleCount);
      }

      // 状态重置和延时锁
      vs1838_1Detected = false;
      vs1838_2Detected = false;
      inProcess = true; // 锁定逻辑
      lastTriggerTime = currentTime; // 记录触发时间
    }
  }

  // 解锁逻辑延时
  if (inProcess && (currentTime - lastTriggerTime > processDelay)) {
    inProcess = false; // 解锁逻辑
    Serial.println("逻辑解锁，可以进行新的检测");
  }

  // 小延时，避免占用过多CPU时间
  delay(10);
} 