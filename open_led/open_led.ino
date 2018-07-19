/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#define LED 3
#define S 6
int s;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);
  pinMode(S, INPUT);
  Serial.begin(9600); //port
}

// the loop function runs over and over again forever
int on_off=0,c=0; //on=0 off=1
void loop() {
  s=digitalRead(S);
  if (s==0 && c==0){
    if (on_off==0){
      digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
      Serial.println(on_off);    // แสดงค่า
      on_off=1;                  
      c=1;
      //delay(1000);                       // wait for a second
    }
  
    else{
      digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
      Serial.println(on_off);
      on_off=0;
      c=1;
      //delay(1000);                       // wait for a second
    }
  }
  else if (s==1 && c==1) {
   c=0;
    }
}
