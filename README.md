# CASA0016
Light Beam Breaker Based Activity Count Detection System

---

#### **Introduction**
This repository contains the code, design files, and documentation for the "Light Beam Breaker Based Activity Count Detection System," a project developed for the CASA0016 coursework. The goal was to design a cost-effective and energy-efficient system capable of detecting and counting individuals entering or exiting a monitored space with real-time feedback.

---

#### **Project Features**
- **Accurate People Counting**: Achieves over 70% accuracy in controlled environments.
- **Cost-Effective Design**: Utilizes inexpensive components like the VS1838B infrared receiver and EK8460U IR LED module.
- **Real-Time Feedback**: Displays the count on an OLED screen using the Adafruit SSD1306 library.
- **Robust Logic**: Mitigates false detections through state-locking mechanisms and error handling.
- **Scalable Architecture**: Supports future enhancements such as wireless communication and integration with IoT systems.

---

#### **Hardware Used**
- **Transmitter**: ESP32 DROOM 32D, EK8460U IR LED module.
- **Receiver**: Arduino Uno R3, VS1838B IR receiver, SSD1306 OLED display.
- **Additional Components**: Custom 3D-printed parts, fan-shaped matrix sensor configuration for optimized detection.

---

#### **Software Highlights**
- **Sequential Logic**: Determines the direction of movement based on the activation sequence of sensors.
- **Error Handling**: Handles edge cases like simultaneous sensor triggers and target reversal.
- **User-Friendly Display**: Real-time count updates with Adafruit_SSD1306.
- **Expandable Code**: Supports adding more sensors for increased precision and adaptability.

---

#### **System Workflow**
1. **Detection**: IR sensors detect objects interrupting the light beam.
2. **Direction Determination**: Sequential logic identifies entry or exit movements.
3. **Count Update**: Displays updated count on the OLED screen.
4. **Reset Mechanism**: Resets sensor states after successful detection or timeout.

---

#### **Challenges and Solutions**
- **Simultaneous Triggers**: Addressed by adding a third sensor and refining the state-locking mechanism.
- **Detection Range**: Enhanced with a 940nm wavelength IR beam for minimal interference.
- **Power Efficiency**: Incorporated 38kHz modulation for reduced power consumption.

---

#### **Results**
- **Accuracy**: Achieved 78% accuracy within a one-meter range during controlled testing.
- **Limitations**: Accuracy declines beyond 2.5 meters due to signal attenuation. Edge cases like side-by-side passage remain challenging.

---

#### **Future Work**
- **Cost Optimization**: Replace ESP32 with a 555 timer for simpler applications.
- **Improved Detection**: Add lenses or LED arrays for better signal intensity.
- **Enhanced Logic**: Develop concurrent systems to detect multiple targets simultaneously.
- **Battery Optimization**: Address voltage drop issues in long-duration usage.

---

#### **Getting Started**
1. Clone the repository:
   ```bash
   git clone https://github.com/UCLcaozhiyu/CASA0016.git
   ```
2. Follow the hardware setup guide in the documentation folder.
3. Upload the code to your microcontroller using Arduino IDE.
