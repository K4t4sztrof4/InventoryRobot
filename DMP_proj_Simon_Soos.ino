#include <SoftwareSerial.h>
#include <IRremote.h>

#define in1 4
#define in2 5
#define in3 2  //jobb
#define in4 3  //jobb
#define stopNr 3
int stopCounter = 0;
const unsigned long eventInterval = 1000;
#define left 7
#define center 8
#define right 9
int fullSpeed = 128, turningSpeed = 96;
int flag = false;

const int RECV_PIN = 6;
IRrecv irrecv(RECV_PIN);
decode_results results;

SoftwareSerial bluetooth(11, 10);  // RX, TX

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(left, INPUT);
  pinMode(center, INPUT);
  pinMode(right, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);


  irrecv.enableIRIn();
  irrecv.blink13(true);

  bluetooth.begin(9600);
}

void StartMotor(int m1, int m2, int forward, int speed) {
  if (speed == 0)  // stop
  {
    digitalWrite(m1, 0);
    digitalWrite(m2, 0);
  } else {
    if (forward) {
      digitalWrite(m2, 0);
      analogWrite(m1, speed);  // use PWM
    } else {
      digitalWrite(m1, 0);
      analogWrite(m2, speed);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly

  irrecv.decode(&results);
  //Serial.println(results.value);

  if (results.value != 0) {
    //Serial.println("Hello");
    if (digitalRead(left) && !digitalRead(center) && digitalRead(right)) {
      //Serial.println("forward");
      StartMotor(in1, in2, 1, 256);
      StartMotor(in3, in4, 1, 256);
    } else if (!digitalRead(left) && digitalRead(right)) {
      //Serial.println("go right");
      StartMotor(in1, in2, 1, 128);
      StartMotor(in3, in4, 1, 0);
    } else if (digitalRead(left) && !digitalRead(right)) {
      //Serial.println("go left");
      StartMotor(in1, in2, 1, 0);
      StartMotor(in3, in4, 1, 128);
    } else if (!digitalRead(left) && !digitalRead(center) && !digitalRead(right)) {
      //Serial.println("stop point");
      flag = true;
      if (flag == true && (stopCounter < stopNr)) {
        bluetooth.write("x");
        bluetooth.setTimeout(2000);
        flag = false;
      }
      unsigned long prevTime = millis();
      unsigned long currTime = millis();
      while ((currTime - prevTime) < eventInterval) {
        currTime = millis();
        StartMotor(in1, in2, 1, 0);
        StartMotor(in3, in4, 1, 0);
      }
      stopCounter++;
      prevTime = millis();
      currTime = millis();

      if (stopCounter < stopNr) {
        while ((currTime - prevTime) < (eventInterval / 10)) {
          currTime = millis();
          StartMotor(in1, in2, 1, 256);
          StartMotor(in3, in4, 1, 256);
        }
      }


    } else if (digitalRead(left) && digitalRead(center) && digitalRead(right)) {
      //Serial.print("forward a bit then error");
      StartMotor(in1, in2, 1, 0);
      StartMotor(in3, in4, 1, 0);
    }
  }
}