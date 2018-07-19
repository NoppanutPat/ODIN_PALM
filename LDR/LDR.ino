#define ldr 17
#define LED 5
int ldr_value=0; //variable to store LDR values
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ldr, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  ldr_value = analogRead(ldr); //read the LDR value
  if (ldr_value<=300) {
    digitalWrite(LED, HIGH);
    }
  else{
    digitalWrite(LED, LOW);
    }
  Serial.print("ldr_value: ");
  Serial.println(ldr_value); //prints the LDR value to serial monitor
}
