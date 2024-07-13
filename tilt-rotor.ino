#include <ESP32Servo.h> 
#include "BluetoothSerial.h" 
#if! defined(CONFIG_BT_ENABLED) ||! defined(CONFIG_BLUEDROID_ENABLED) 
 #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it 
 #endif 
BluetoothSerial SerialBT; 
const int SERVO = 32; 
 const int BLDC = 33; 
 const int voltage = 12; 
 int servoPos = 90;  
Servo myservo; 
 Servo ESC; 
int motorRPM(long RPM) { 
   return (RPM*255)/(voltage*1000); 
 } 
void servoReturn() { 
   if(servoPos>90) 
   { 
     for(servoPos; servoPos >= 90; servoPos--) { 
       myservo.write(servoPos); 
       delay(20); 
     } 
   } 
   else if(servoPos<90) 
   { 
     for(servoPos; servoPos <= 90; servoPos++) { 
       myservo.write(servoPos); 
       delay(20); 
     } 
   } 
 }  
void servoRight(int posDegrees) { 
   for(servoPos; servoPos <= posDegrees; servoPos++) { 
     myservo.write(servoPos); 
     delay(20); 
   } 
 } 
void servoLeft(int posDegrees) { 
   for(servoPos; servoPos >= posDegrees; servoPos--) { 
     myservo.write(servoPos); 
     delay(20); 
   } 
 } 
void setup() { 
   Serial.begin(9600); 
   myservo.attach(SERVO); 
   ESC.attach(BLDC); 
   SerialBT.begin("ESP32test"); 
   Serial.println("The device started, now you can pair it with bluetooth!"); 
 } 
void loop() { 
   if (SerialBT.available() > 0) { 
     String inputFromSerial = SerialBT.readStringUntil('\n'); 
     Serial.println(inputFromSerial); 
     char firstChar = inputFromSerial[0]; 
     char remainChar[6]; 
     inputFromSerial.substring(2).toCharArray(remainChar, sizeof(remainChar)); 
     long intValue = String(remainChar).toInt(); 
     Serial.println(intValue); 
     if(firstChar == 'm') 
     { 
       int raftar = motorRPM(intValue); 
       ESC.write(raftar); 
     } 
     else if(firstChar == 'l') 
     { 
       if(intValue < 90) 
         servoLeft(intValue); 
     } 
     else if(firstChar == 'r') 
     { 
       if(intValue > 90) 
         servoRight(intValue); 
     } 
     else if(firstChar == 'o') 
     { 
       servoReturn(); 
     } 
   } 
 }
