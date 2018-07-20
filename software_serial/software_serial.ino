#include<SoftwareSerial.h>

SoftwareSerial se_read(12,13); //write only
SoftwareSerial se_write(10,11); //read only

struct ProjectData { //arduino sent a struct(is data type that have many data type in it) to 'node MCU'//
  int32_t is_button_pressed;// 32 is the number of bits == 4 byte
  float temperature;
  int32_t light_intensity;
  }
  cur_project_data={1, 25.34, 69};

struct ServerData { //arduidno pull it form server
  int32_t is_lamp_off;
  int32_t is_door_locking;
  }
  server_data;

const char GET_SERVER_DATA = 1; //char use less byte than int, const use for the data that we don't need to chance their value
const char GET_SERVER_DATA_RESULT = 2; //get data from server and get any result.
const char UPDATE_PROJECT_DATA = 3; //update data to Aduino, Arduino work hard.
const char SEND_PROJECT_DATA_TO_NODEMCU = 4;// send only.

void send_to_nodemcu(char code, void *data  /* it can be every type of data */ , char data_size){
  char *b=(char*)data;// It cut data && Get location
  char sent_size = 0;
  while (se_write.write(code)==0){ // If se_write.write can't read, it value is 0
    delay(1);
    }
  while(sent_size<data_size){ // 
    sent_size+=se_write.write(b,data_size);// se_write is command 
    delay(1);
    }
}




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  se_read.begin(38400);
  se_write.begin(38400);
  pinMode(LED_BUILTIN, OUTPUT);
  while (!se_read.isListening()){
    se_read.listen();
    }
  Serial.println((int)sizeof(ServerData));
  Serial.println("ARDUINO READY!");  
}

uint32_t last_sent_time = 0;
char expected_data_size = 0;
char cur_data_header = 0;
char buffer[256];
uint32_t cur_buffer_length = -1;

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t cur_time = millis();
  //send to Node-MCU
  if (cur_time - last_sent_time > 500){
    //always update
    //send_to_nodemcu(UPDATE_PROJECT_DATA, &project_data)
    send_to_nodemcu(GET_SERVER_DATA, &server_data, sizeof(ServerData));
    last_sent_time = cur_time;
    }
  //read from sensor....
  //send to nodemcu
  //read data from server pass by nodemcu
  while(se_read.available()){
    char ch = se_read.read();
    Serial.print("RECV: ");
    Serial.println((byte)ch);
    if (cur_buffer_length == -1){
      cur_data_header = ch;
      switch (cur_data_header){
        case GET_SERVER_DATA_RESULT:
          //unknown header
            expected_data_size = sizeof(ServerData);
            cur_buffer_length = 0;
            break;
            }
        }
     else if(cur_buffer_length < expected_data_size){
        buffer[cur_buffer_length++] = ch;
        if (cur_buffer_length == expected_data_size){
          switch(cur_data_header){
            case GET_SERVER_DATA_RESULT:{
              ServerData *data = (ServerData*)buffer;
              //use data to control sensor
              Serial.print("temp status: ");
              Serial.println(data->temp);
              Serial.print("light: ");
              Serial.println(data->light_lux);
              Serial.print("sound status: ");
              Serial.println(data->sound);
              }
            } break;
          }
      }
    }
    }

}
