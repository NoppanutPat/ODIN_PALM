#include <SoftwareSerial.h>

SoftwareSerial se_read(12,13); //write only
SoftwareSerial se_write(10,11); //read only
struct ProjectData {
  int32_t is_button_pressed;
  float temperature;
  int32_t light_intensity;
  } cur_project_data = { 8,25.34,69 };

struct ServerData {
  int32_t is_lamp_off;
  int32_t is_door_locking;
} server_data;

//working mode
const char GET_SERVER_DATA = 1;
const char GET_SERVER_DATA_RESULT = 2;
const char UPDATE_PROJECT_DATA = 3;
const char SEND_PROJECT_DATA_TO_NODEMCU = 4;

void send_to_nodemcu (char code, void *data, char data_size) {
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
  Serial.begin(115200);
  se_read.begin(38400);
  se_write.begin(38400);
  pinMode(LED_BUILTIN, OUTPUT);
  while(!se_read.isListening()) {
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
  uint32_t cur_time = millis();
  //send to nodemcu
  if (cur_time - last_sent_time > 500) {
    send_to_nodemcu(GET_SERVER_DATA, &server_data, sizeof(ServerData));
    last_sent_time = cur_time;
    }
  while (se_read.available()) {
    char ch = se_read.read();
    if (cur_buffer_length == -1) {
      cur_data_header = ch;
      switch (cur_data_header) {
        case GET_SERVER_DATA_RESULT:
        expected_data_size = sizeof(ServerData);
        cur_buffet_length = 0;
        break;
      }
    } else if (cur_buffer_length < expected_data_size) {
      buffer[cur_buffer_length++] = ch;
      if (cur_buffer_length == expected_data_size) {
        switch (cur_data_header) {
          case GET_SERVER_DATA_RESULT: {
            ServerDara *data = (ServerData*)buffer;
          }
        }
      }
    }
  }
}

