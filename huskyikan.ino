#include <Wire.h>
#include "HUSKYLENS.h"
#include <Servo.h>

HUSKYLENS huskylens;
Servo myServo;

bool servoMoved = false;
unsigned long lastServoMoveTime = 0;
const unsigned long servoDelay = 10000;

void printResult(HUSKYLENSResult result);
void moveServo();

void setup() {
    Serial.begin(115200);
    Wire.begin();
    while (!huskylens.begin(Wire)) {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1. Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
        Serial.println(F("2. Please recheck the connection."));
        delay(100);
    }
    myServo.attach(4);
    myServo.write(0);
}

void loop() {
    if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if (!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if (!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
    else {
        Serial.println(F("###########"));
        while (huskylens.available()) {
            HUSKYLENSResult result = huskylens.read();
            printResult(result);
        }
    }

    unsigned long currentTime = millis();

    if (servoMoved && (currentTime - lastServoMoveTime >= servoDelay)) {
        servoMoved = false;
    }
}

void printResult(HUSKYLENSResult result) {
    if (result.command == COMMAND_RETURN_BLOCK) {
        if (result.ID == 1) {
            Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID);
            if (!servoMoved) {
                moveServo();
                servoMoved = true;
                lastServoMoveTime = millis();
            }
        }
    } else if (result.command == COMMAND_RETURN_ARROW) {
        Serial.println(String() + F("Arrow:xOrigin=") + result.xOrigin + F(",yOrigin=") + result.yOrigin + F(",xTarget=") + result.xTarget + F(",yTarget=") + result.yTarget + F(",ID=") + result.ID);
    } else {
        Serial.println("Object unknown!");
    }
}

void moveServo() {
    int startPos = 0;
    int targetPos = 90;
    int increment = 1;
    
    for (int pos = startPos; pos <= targetPos; pos += increment) {
        myServo.write(pos);
        delay(15);
    }
    delay(100);
    for (int pos = targetPos; pos >= startPos; pos -= increment) {
        myServo.write(pos);
        delay(15);
    }
}
