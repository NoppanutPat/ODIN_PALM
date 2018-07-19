#include <Servo.h>
#define ldr 17
#define servo 5
Servo myservo;
int ldr_value=0; //variable to store LDR values
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ldr, INPUT);
  myservo.attach(servo);
  myservo.write(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  ldr_value = analogRead(ldr); //read the LDR value
  Serial.print("ldr_value: ");
  Serial.println(ldr_value); //prints the LDR value to serial monitor
  myservo.write(0);
  if (ldr_value>300) {
    myservo.write(90);
    delay(1000);
    }
  else{
    myservo.write(0);
    delay(1000);
    }
  
}
