#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <EspSoftwareSerial.h>
#include <math.h>

SoftwareSerial se_read(D5, D6); // write only
SoftwareSerial se_write(D0, D1); // read only
String const url = "http://ecourse.cpe.ku.ac.th:1515/api/";

struct ProjectData {
  
  int32_t alarm_status;
  int32_t air_status;
  int32_t light_status;
  float temp;
  float humid;
    
} cur_project_data{0,0,0,0,0};
  
struct ServerData {
  
  int32_t press_alarm;
  int32_t on_off_air;
  int32_t outside_light;
  int32_t inside_light;
  int32_t door;
  int32_t press_alarm_auto_status;
  int32_t on_off_air_auto_status;
  int32_t outside_light_auto_status;
  int32_t inside_light_auto_status;
  int32_t door_auto_status;
  
} server_data{0,0,0,0,0,0,0,0,0,0};

const char GET_SERVER_DATA = 1;
const char GET_SERVER_DATA_RESULT = 2;
const char UPDATE_PROJECT_DATA = 3;

// wifi configuration
const char SSID[] = "KUWIN_KING_2.4GHz";
const char PASSWORD[] = "1234567890";

// for nodemcu communication
uint32_t last_sent_time = 0;
char expected_data_size = 0;
char cur_data_header = 0;
char buffer[256];
int8_t cur_buffer_length = -1;

void send_to_arduino(char code, void *data, char data_size) {
  char *b = (char*)data;
  int sent_size = 0;
  while (se_write.write(code) == 0) {
    delay(1);
  }
  while (sent_size < data_size) {
    sent_size += se_write.write(b, data_size);
    delay(1);
  }
}

void wifi_initialization() {
  Serial.println("WIFI INITIALIZING.");

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    yield();
    delay(10);
  }

  Serial.println("WIFI INITIALIZED.");
}

void serial_initialization() {
  Serial.begin(115200);
  se_read.begin(38400);
  se_write.begin(38400);

  while (!se_read.isListening()) {
    se_read.listen();
  }

  Serial.println();
  Serial.println("SERIAL INITIALIZED.");
}

String set_builder(const char *key, int32_t value) {
  String str = url;
  str = str + key;
  str = str + "/set?value=";
  str = str + value;
  Serial.println(str);
  return str;
}

String get_builder(const char *key) {
  String str = url;
  str = str + key;
  str = str + "/view/";
  return str;
}

void update_data_to_server_callback(String const &str) {
  Serial.println("update_data_to_server_callback FINISHED!");
}

bool GET(const char *url, void (*callback)(String const &str,int32_t &value), int32_t &value) {
  HTTPClient main_client;
  main_client.begin(url);
  if (main_client.GET() == HTTP_CODE_OK) {
    Serial.println("GET REQUEST RESPONSE BEGIN");
    if (callback != 0) {
      callback(main_client.getString(),value);
    }
    return true;
  }
  Serial.println("GET REQUEST RESPONSE BEGIN");
  return false;
}
bool GET(const char *url, void (*callback)(String const &str,float &value), float &value) {
  HTTPClient main_client;
  main_client.begin(url);
  if (main_client.GET() == HTTP_CODE_OK) {
    Serial.println("GET REQUEST RESPONSE BEGIN");
    if (callback != 0) {
      callback(main_client.getString(),value);
    }
    return true;
  }
  Serial.println("GET REQUEST RESPONSE BEGIN");
  return false;
}

bool POST(const char *url, void (*callback)(String const &str)) {
  HTTPClient main_client;
  main_client.begin(url);
  if (main_client.GET() == HTTP_CODE_OK) {
    Serial.println("POST REQUEST RESPONSE BEGIN");
    if (callback != 0) {
      callback(main_client.getString());
    }
    return true;
  }
  Serial.println("POST REQUEST RESPONSE BEGIN");
  return false;
}

int get_request_int(String const &str) {
  int32_t tmp = str.toInt();
  return tmp;
}

float get_request_float(String const &str) {
  float tmp = str.toFloat();
  return tmp;
}
void get_request(String const &str, int32_t &value){
  value = get_request_int(str);
}
void get_request(String const &str, float &value){
  value = get_request_float(str);  
}

void get_request_raw_callback(String const &str) {
  Serial.println("GET REQUEST RESPONSE BEGIN");
  Serial.println("========================");
  Serial.println(str.c_str());
  Serial.println("========================");
  Serial.println("GET REQUEST RESPONSE END");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  serial_initialization();
  wifi_initialization();

  Serial.print("sizeof(ServerData): ");
  Serial.println((int)sizeof(ServerData));
  Serial.print("ESP READY!");
}
void loop() {
  /* 1) Server polling data from server every 1500 ms
     2) Arduino always get local data
  */

  uint32_t cur_time = millis();
  if (cur_time - last_sent_time > 1000) {
    GET(get_builder("odinpalm-press_alarm").c_str(), get_request, server_data.press_alarm);
    Serial.print("press alarm status : ");
    Serial.println(server_data.press_alarm);

    GET(get_builder("odinpalm-on_off_air").c_str(), get_request, server_data.on_off_air);
    Serial.print("on/off air status : ");
    Serial.println(server_data.on_off_air);

    GET(get_builder("odinpalm-outside_light").c_str(), get_request,server_data.outside_light); 
    Serial.print("outside light status : ");
    Serial.println(server_data.outside_light);

    GET(get_builder("odinpalm-inside_light").c_str(), get_request,server_data.inside_light); 
    Serial.print("inside_light status : ");
    Serial.println(server_data.inside_light);

    GET(get_builder("odinpalm-inside_light").c_str(), get_request,server_data.inside_light); 
    Serial.print("inside_light status : ");
    Serial.println(server_data.inside_light);

    GET(get_builder("odinpalm-door").c_str(), get_request,server_data.door); 
    Serial.print("door status : ");
    Serial.println(server_data.door);

    GET(get_builder("odinpalm-press_alarm_auto_status").c_str(), get_request,server_data.press_alarm_auto_status); 
    Serial.print("press_alarm_auto_status : ");
    Serial.println(server_data.press_alarm_auto_status);

    GET(get_builder("odinpalm-on_off_air_auto_status").c_str(), get_request,server_data.on_off_air_auto_status); 
    Serial.print("on_off_air_auto_status : ");
    Serial.println(server_data.on_off_air_auto_status);

    GET(get_builder("odinpalm-outside_light_auto_status").c_str(), get_request,server_data.outside_light_auto_status); 
    Serial.print("outside_light_auto_status : ");
    Serial.println(server_data.outside_light_auto_status);

    GET(get_builder("odinpalm-inside_light_auto_status").c_str(), get_request,server_data.inside_light_auto_status); 
    Serial.print("inside_light_auto_status : ");
    Serial.println(server_data.inside_light_auto_status);

    GET(get_builder("odinpalm-door_auto_status").c_str(), get_request,server_data.door_auto_status); 
    Serial.print("door_auto_status : ");
    Serial.println(server_data.door_auto_status);
    last_sent_time = cur_time;
  }

  while (se_read.available()) {
    char ch = se_read.read();
    //Serial.print("RECV: ");0
    //Serial.println((byte)ch);
    if (cur_buffer_length == -1) {
      cur_data_header = ch;
      switch (cur_data_header) {
        case UPDATE_PROJECT_DATA:
          expected_data_size = sizeof(ProjectData);
          cur_buffer_length = 0;
          break;
        case GET_SERVER_DATA:
          expected_data_size = sizeof(ServerData);
          cur_buffer_length = 0;
          break;
      }
    } else if (cur_buffer_length < expected_data_size) {
      buffer[cur_buffer_length++] = ch;
      if (cur_buffer_length == expected_data_size) {
        switch (cur_data_header) {
          case UPDATE_PROJECT_DATA: {
              ProjectData *project_data = (ProjectData*)buffer;

              int32_t alarm_status = project_data->alarm_status;
              Serial.println(alarm_status);
              POST(set_builder("odinpalm-alarm_status", alarm_status).c_str(), update_data_to_server_callback);

              int32_t air_status = project_data->air_status;
              Serial.println(air_status);
              POST(set_builder("odinpalm-air_status", air_status).c_str(), update_data_to_server_callback);

              int32_t light_status = project_data->light_status;
              Serial.println(light_status);
              POST(set_builder("odinpalm-light_status", light_status).c_str(), update_data_to_server_callback);

              float temp = project_data->temp;
              Serial.println(temp);
              POST(set_builder("odinpalm-temp", temp).c_str(), update_data_to_server_callback);

              float humid = project_data->humid;
              Serial.println(humid);
              POST(set_builder("odinpalm-humid", humid).c_str(), update_data_to_server_callback);

            }
            break;
          case GET_SERVER_DATA:
            Serial.println("Send data to arduino");
            send_to_arduino(GET_SERVER_DATA_RESULT, &server_data, sizeof(ServerData));
            break;
        }
        cur_buffer_length = -1;
      }
    }
  }
}
