#include <SoftwareSerial.h>
#define SWITCH 8
#define LED 6
#define BUZZER 11
#define ldr 17
#define trigger_pin 7
#define echo_pin 6
long duration, cm,floor_length;
long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}
#define <Servo.h>
#define servo 5
Servo myservo;
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial se_read(12, 13); // write only
SoftwareSerial se_write(10, 11); // read only

struct ProjectData { 
  int32_t alarm_status;
  int32_t air_status;
  int32_t light_status;
  float temp;
  float humid;
  
} project_data = {0,0,0,0,0};

struct ServerData {
  int32_t alarm_status;
  int32_t on_off_air;
  int32_t outside_light;
  int32_t inside_light;
  int32_t door;
  int32_t press_alarm_auto_status;
  int32_t on_off_air_auto_status;
  int32_t outside_light_auto_status;
  int32_t inside_light_auto_status;
  int32_t door_auto_status;
  
} server_data = {0,0,0,0,0,0,0,0,0,0};

const char GET_SERVER_DATA = 1;
const char GET_SERVER_DATA_RESULT = 2;
const char UPDATE_PROJECT_DATA = 3;

void send_to_nodemcu(char code, void *data, char data_size) {
  char *b = (char*)data; 
  char sent_size = 0;
  while (se_write.write(code) == 0) {
    delay(1);
  }
  while (sent_size < data_size) {
    sent_size += se_write.write(b, data_size);
    delay(1);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  se_read.begin(38400);
  se_write.begin(38400);
  pinMode(SWITCH, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(ldr, INPUT);
  pinMode(LED, OUTPUT);
  myservo.attach(servo);
  myservo.write(0);
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  
  while (!se_read.isListening()) {
    se_read.listen();
  }
  Serial.println((int)sizeof(ServerData));
  Serial.println("ARDUINO READY!");
}

uint32_t last_sent_time = 0;
boolean is_data_header = false;
char expected_data_size = 0;
char cur_data_header = 0;
char buffer[256];
int8_t cur_buffer_length = -1;
int32_t b = -1;

void loop() {
  uint32_t cur_time = millis();
  //send to nodemcu
  int a = digitalRead(SWITCH);
  if(a == 0){
    Serial.println("Press");
    //Serial.println(a);
    b*=-1;
    delay(500);
  }
  project_data.SW = b;
  if(server_data.SW == 1){
    digitalWrite(LED,HIGH);
  }
  else{
    digitalWrite(LED,LOW);
  }
  
  if (cur_time - last_sent_time > 500) {//always update
    Serial.println(project_data.SW);
    send_to_nodemcu(GET_SERVER_DATA, &server_data, sizeof(ServerData));

    last_sent_time = cur_time;
  }

  //read from sensor....
  //send to nodemcu
  
  //read data from server pass by nodemcu
  while (se_read.available()) {
    char ch = se_read.read();
    Serial.print("RECV: ");
    Serial.println((byte)ch);
    if (cur_buffer_length == -1) {
      cur_data_header = ch;
      switch (cur_data_header) {
        case GET_SERVER_DATA_RESULT:
        //unknown header
          expected_data_size = sizeof(ServerData);
          cur_buffer_length = 0;
          break;
      }
    } else if (cur_buffer_length < expected_data_size) {
      buffer[cur_buffer_length++] = ch;
      if (cur_buffer_length == expected_data_size) {
        switch (cur_data_header) {
          case GET_SERVER_DATA_RESULT: {
            ServerData *data = (ServerData*)buffer;
            //use data to control sensor
            Serial.print("press_alarm status: ");
            Serial.println(data->press_alarm);

            Serial.print("on_off_air status: ");
            Serial.println(data->on_off_air);

            Serial.print("outside_light status: ");
            Serial.println(data->outside_light);

            Serial.print("inside_light status: ");
            Serial.println(data->inside_light);

            Serial.print("door status: ");
            Serial.println(data->door);

            Serial.print("press_alarm_auto_status status: ");
            Serial.println(data->press_alarm_auto_status);

            Serial.print("on_off_air_auto_status status: ");
            Serial.println(data->on_off_air_auto_status);

            Serial.print("outside_light_auto_status status: ");
            Serial.println(data->outside_light_auto_status);

            Serial.print("inside_light_auto_status status: ");
            Serial.println(data->inside_light_auto_status);

            Serial.print("door_auto_status status: ");
            Serial.println(data->door_auto_status);
            
          //write code here//

          //write code here//

            send_to_nodemcu(UPDATE_PROJECT_DATA, &project_data, sizeof(ProjectData));
            Serial.println("Send data");
          } break;
        }
        cur_buffer_length = -1;
      }
    }
  }
}

