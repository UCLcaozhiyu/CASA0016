const int ledPin = 18; // GPIO18 用于输出 38kHz 信号

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("开始输出 38kHz 信号");
}

void loop() {
  // 手动生成 38kHz 信号，占空比约 50%
  digitalWrite(ledPin, HIGH);
  delayMicroseconds(13); // 高电平持续时间
  digitalWrite(ledPin, LOW);
  delayMicroseconds(13); // 低电平持续时间
}
