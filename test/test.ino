#include <SoftwareSerial.h>
#define LED 6
#define SW 8

SoftwareSerial se_read(12, 13);  // write only
SoftwareSerial se_write(10, 11); // read only

struct ProjectData
{
    float temperature;
    uint32_t SW;
    uint32_t on_off_led;
    
} project_data = {25.25, 0, 800}; //your value

struct ServerData
{
    uint32_t SW;
    
} server_data = {0}; // your value

const char GET_SERVER_DATA = 1;
const char GET_SERVER_DATA_RESULT = 2;
const char UPDATE_PROJECT_DATA = 3;

void send_to_nodemcu(char code /* Thing that you want to do */, void *data /* Data that you want to action */, char data_size /* Input data size */)
{
    char *b = (char *)data;
    char sent_size = 0;
    while (se_write.write(code) == 0)
    {
        delay(1);
    }

    while (sent_size < data_size)
    {
        sent_size += se_write.write(b, data_size);
        delay(1);
    }
}
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    se_read.begin(38400);
    se_write.begin(38400);
    pinMode(SW, INPUT);
    pinMode(LED, OUTPUT);
    while (!se_read.isListening())
    {
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

void loop()
{
    project_data.SW=digitalRead(SW)
    uint32_t cur_time = millis();
    //send to nodemcu
    if (cur_time - last_sent_time > 500)
    { //always update
        send_to_nodemcu(UPDATE_PROJECT_DATA, &project_data, sizeof(ProjectData)); //update project data on server
        send_to_nodemcu(GET_SERVER_DATA, &server_data, sizeof(ServerData)); //get server data from server
        last_sent_time = cur_time;
    }

    //read from sensor....
    
    //send to nodemcu

    //read data from server pass by nodemcu

    while (se_read.available())
    {
        char ch = se_read.read();
        //Serial.print("RECV: ");
        //Serial.println((byte)ch);
        if (cur_buffer_length == -1)
        {
            cur_data_header = ch;
            switch (cur_data_header)
            {
            case GET_SERVER_DATA_RESULT:
                //unknown header
                expected_data_size = sizeof(ServerData);

                cur_buffer_length = 0;

                break;
            }
        }
        else if (cur_buffer_length < expected_data_size)
        {

            buffer[cur_buffer_length++] = ch;

            if (cur_buffer_length == expected_data_size)
            {

                switch (cur_data_header){

                  case GET_SERVER_DATA_RESULT:
                  {

                      ServerData *data = (ServerData *)buffer; //Get data from server

                      //use data to control sensor
                    
                      if(data->SW == 0){ //  '->' is symbol to sent value in pointer
                       digitalWrite(LED, HIGH);   
                        }
                      else{
                        digitalWrite(LED, LOW);
                        }
                }
                break;
                }

                cur_buffer_length = -1;
            }
        }
    }
}

