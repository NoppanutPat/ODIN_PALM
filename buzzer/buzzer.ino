#define buzzer 11

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
}
int a=255;
void loop() {
  // put your main code here, to run repeatedly:
  //analogWrite(buzzer, a); //0-255
  //tone(11, 255, 0);
  delay(1000);
}
