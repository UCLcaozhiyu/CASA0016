// 红外LED和接收模块的引脚定义
const int ledPin = 3;        // 红外LED连接到Pin 3 (PWM输出)
const int receiverPin = 2;   // VS1838B的信号引脚连接到Pin 2

// 定义计数变量
int beamNormalCount = -1;     // 光束正常计数
bool lastState = HIGH;       // 保存上一次的信号状态，默认未中断（HIGH）

void setup() {
  // 初始化引脚模式
  pinMode(ledPin, OUTPUT);      // 红外LED作为输出
  pinMode(receiverPin, INPUT);  // VS1838B信号引脚作为输入

  // 初始化串口，用于调试
  Serial.begin(9600);
  Serial.println("光束正常计数系统启动...");
}

void loop() {
  // 发射端：生成38kHz调制信号
  tone(ledPin, 38000); // 在ledPin上输出38kHz的PWM信号

  // 接收端：读取VS1838B信号引脚状态
  int currentState = digitalRead(receiverPin);

  // 检查状态是否从中断变为正常
  if (currentState == LOW && lastState == HIGH) {
    beamNormalCount++; // 光束正常计数增加
    Serial.print("光束正常计数：");
    Serial.println(beamNormalCount);
  }

  // 更新状态
  lastState = currentState;

  // 稳定输出，避免抖动影响
  delay(300);
}
