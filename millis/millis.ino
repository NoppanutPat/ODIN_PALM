void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}


int last_time=0;

void loop() {
  // put your main code here, to run repeatedly:
  int cur_time = millis();
  Serial.println(cur_time);
  if(cur_time - last_time >2000){
    //Serial.println("a");
    }
  last_time = cur_time;
}
