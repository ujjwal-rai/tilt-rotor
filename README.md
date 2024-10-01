# tilt-rotor

This code is designed to control a tilt rotor mechanism using an ESP32 microcontroller. The tilt rotor mechanism consists of a servo motor and a BLDC (brushless DC) motor, with the ESP32 receiving commands via Bluetooth to adjust the motor speed and rotation angle. The serial communication with the ESP32 is handled using Bluetooth, and the mechanism can be controlled through a mobile app or another device by sending commands to the ESP32 via Bluetooth.

Here's a detailed explanation of the code in the context of the tilt rotor mechanism:

### 1. **Libraries and Setup**

```cpp
#include <ESP32Servo.h> 
#include "BluetoothSerial.h" 
```
- The `ESP32Servo` library is used to control the servo motor, allowing you to set its position in degrees (e.g., tilt the rotor).
- The `BluetoothSerial` library enables Bluetooth communication, allowing commands to be sent from a Bluetooth-enabled device (e.g., a phone).

### 2. **Bluetooth Configuration**
```cpp
#if! defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif 
```
This ensures Bluetooth is enabled on the ESP32. If not, it triggers an error and instructs the user to enable Bluetooth in the configuration settings.

```cpp
BluetoothSerial SerialBT; 
```
`SerialBT` is an instance of the `BluetoothSerial` class, which is used to manage Bluetooth communication.

### 3. **Pins and Variables**

```cpp
const int SERVO = 32;  // Servo motor connected to GPIO 32
const int BLDC = 33;   // BLDC motor connected to GPIO 33
const int voltage = 12;  // Voltage for RPM calculation (assumed 12V)
int servoPos = 90;  // Initial position of the servo motor (90° is neutral)
Servo myservo;  // Servo object to control the tilt rotor
Servo ESC;      // Servo object to control the BLDC motor speed (used like an ESC)
```
- **Servo Motor (`SERVO`)**: The tilt angle of the rotor is controlled by the servo motor connected to pin 32.
- **BLDC Motor (`BLDC`)**: The speed of the rotor is controlled by the BLDC motor, which is managed through an Electronic Speed Controller (ESC) connected to pin 33.
- **Voltage**: This is used for calculating RPM based on the voltage supplied to the motor.

### 4. **Motor RPM Function**

```cpp
int motorRPM(long RPM) { 
   return (RPM * 255) / (voltage * 1000); 
} 
```
This function converts the given RPM (rotations per minute) to a value suitable for the ESC control. The result is scaled between 0 and 255 (PWM value), considering the operating voltage (12V). The higher the value, the faster the motor spins.

### 5. **Servo Control Functions**

#### Return to Neutral (90°) Position
```cpp
void servoReturn() { 
   if(servoPos > 90) { 
     for(servoPos; servoPos >= 90; servoPos--) { 
       myservo.write(servoPos); 
       delay(20); 
     } 
   } 
   else if(servoPos < 90) { 
     for(servoPos; servoPos <= 90; servoPos++) { 
       myservo.write(servoPos); 
       delay(20); 
     } 
   } 
}
```
- The `servoReturn()` function returns the servo to the neutral (90°) position by gradually moving the servo back to 90°.
- It checks if the current position is greater or less than 90°, and then adjusts it accordingly.

#### Move Right (Clockwise)
```cpp
void servoRight(int posDegrees) { 
   for(servoPos; servoPos <= posDegrees; servoPos++) { 
     myservo.write(servoPos); 
     delay(20); 
   } 
}
```
- This function moves the servo to the right (clockwise), increasing the position angle up to `posDegrees`.

#### Move Left (Counterclockwise)
```cpp
void servoLeft(int posDegrees) { 
   for(servoPos; servoPos >= posDegrees; servoPos--) { 
     myservo.write(servoPos); 
     delay(20); 
   } 
}
```
- This function moves the servo to the left (counterclockwise), decreasing the position angle down to `posDegrees`.

### 6. **Setup Function**

```cpp
void setup() { 
   Serial.begin(9600);  // Start serial communication for debugging
   myservo.attach(SERVO);  // Attach the servo motor to pin 32
   ESC.attach(BLDC);  // Attach the ESC to pin 33 for the BLDC motor
   SerialBT.begin("ESP32test");  // Start Bluetooth with the device name "ESP32test"
   Serial.println("The device started, now you can pair it with bluetooth!"); 
}
```
- Initializes the servo, ESC, and Bluetooth.
- The Bluetooth device name is set to "ESP32test", allowing pairing with this device.
- This also starts serial communication at 9600 baud for debugging purposes.

### 7. **Main Loop**

```cpp
void loop() { 
   if (SerialBT.available() > 0) { 
     String inputFromSerial = SerialBT.readStringUntil('\n'); 
     Serial.println(inputFromSerial); 
     char firstChar = inputFromSerial[0]; 
     char remainChar[6]; 
     inputFromSerial.substring(2).toCharArray(remainChar, sizeof(remainChar)); 
     long intValue = String(remainChar).toInt(); 
     Serial.println(intValue); 
```
- The `loop()` function continuously checks for incoming Bluetooth commands.
- It reads the data sent via Bluetooth, extracts the first character (`firstChar`) to determine the command, and parses the remaining part as a numeric value (`intValue`).
  
### 8. **Handling Commands**

- **Control Motor Speed**:
  ```cpp
  if(firstChar == 'm') { 
    int raftar = motorRPM(intValue); 
    ESC.write(raftar); 
  }
  ```
  - If the command starts with `'m'`, it interprets the value as RPM for the BLDC motor and adjusts its speed using the ESC.

- **Tilt Left**:
  ```cpp
  else if(firstChar == 'l') { 
    if(intValue < 90) 
      servoLeft(intValue); 
  }
  ```
  - If the command starts with `'l'`, it tilts the rotor to the left (if the angle is less than 90°).

- **Tilt Right**:
  ```cpp
  else if(firstChar == 'r') { 
    if(intValue > 90) 
      servoRight(intValue); 
  }
  ```
  - If the command starts with `'r'`, it tilts the rotor to the right (if the angle is greater than 90°).

- **Return to Neutral Position**:
  ```cpp
  else if(firstChar == 'o') { 
    servoReturn(); 
  }
  ```
  - If the command starts with `'o'`, it returns the rotor to the neutral position (90°).

### Summary

This code allows you to control both the rotation angle of a tilt rotor using a servo motor and the speed of a BLDC motor using an ESC via Bluetooth commands. By sending specific commands through a Bluetooth serial monitor or mobile app, you can adjust the tilt of the rotor (left or right) and control the motor speed for the tilt rotor mechanism.
