
const int ledPin = 3;         
const int vs1838_1 = 2;       
const int vs1838_2 = 4;       


int peopleCount = 0;          
bool vs1838_1Detected = false; 
bool vs1838_2Detected = false;
bool inProcess = false;       


unsigned long lastTriggerTime = 0; 
const unsigned long debounceTime = 50; 
const unsigned long processDelay = 1000; 

void setup() {
  pinMode(ledPin, OUTPUT);       
  pinMode(vs1838_1, INPUT);      
  pinMode(vs1838_2, INPUT);      

  Serial.begin(9600);
  Serial.println("Room occupancy detection system activated...");

  tone(ledPin, 38000); 

  delay(2000); 
}

void loop() {
  unsigned long currentTime = millis();
  if (!inProcess && digitalRead(vs1838_1) == LOW) {
    delay(debounceTime);
    if (digitalRead(vs1838_1) == LOW) { 
      vs1838_1Detected = true;
      Serial.println("VS1838_1");
    }
  }

  if (vs1838_1Detected && digitalRead(vs1838_2) == LOW) {
    delay(debounceTime);
    if (digitalRead(vs1838_2) == LOW) { 
      vs1838_2Detected = true; 
      peopleCount++;          
      Serial.print("Someone has entered the room. Current number of people：");
      Serial.println(peopleCount);

      vs1838_1Detected = false;
      vs1838_2Detected = false;
      inProcess = true; 
      lastTriggerTime = currentTime; 
    }
  }

  if (!inProcess && digitalRead(vs1838_2) == LOW) {
    delay(debounceTime);
    if (digitalRead(vs1838_2) == LOW) {
      vs1838_2Detected = true; 
      Serial.println("VS1838_2");
    }
  }

  if (vs1838_2Detected && digitalRead(vs1838_1) == LOW) {
    delay(debounceTime);
    if (digitalRead(vs1838_1) == LOW) { 
      vs1838_1Detected = true; 
      if (peopleCount > 0) {
        peopleCount--;        
        Serial.print("Someone has left the room. current number：");
        Serial.println(peopleCount);
      }

      vs1838_1Detected = false;
      vs1838_2Detected = false;
      inProcess = true; 
      lastTriggerTime = currentTime; 
    }
  }

  if (inProcess && (currentTime - lastTriggerTime > processDelay)) {
    inProcess = false; 
    Serial.println("Logic unlocked for new testing");
  }

  delay(10);
} 
