
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define connect_ssid "devil"
#define connect_psw "12345678"
#define MAX_SIZE 1024 
#define WIDTH 128
#define HEIGHT 64
//String weather_position_1="zhengzhou";
//String weather_position_2="anyang";
//String weather_position_3="xian";
//String weather_position_4="qujing";
//String weather_position_5="dali";

String weather_position;
String weather_position_array[5]={"zhengzhou","anyang","xian","qujing","dali"};
int location_count;

const char *host = "api.seniverse.com";
String request_json;

char buff[1024];
Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire);
struct Weather
{
    String weather;
    int temperature;
} weather_data;
void setup()
{
    int count = 0;
    location_count=0;
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(1.5);
    Serial.begin(115200);
    WiFi.begin(connect_ssid, connect_psw);
    Serial.println("connect to ");
    Serial.print(connect_ssid);
    Serial.print("   ....");
    WiFi.waitForConnectResult(5000);
    while (!WiFi.isConnected())
    {
        Serial.println("wait to connect WIFI...");
        if (count++ > 7)
        {
            Serial.println("Please check you passward");
        }
        delay(1000);
    }
}
void loop()
{
  
    weather_position=weather_position_array[location_count++%5];
    request_json="/v3/weather/now.json?key=yourkey="+weather_position+"&language=en&unit=c";   //这里的yourkey指的是心知天气的密钥
    if (WiFi.isConnected())
    {
        update_weather();   
    }
    else
    {
        Serial.println("WIFI can't connect! So cann't update");
    }

    delay(3000);
}
void update_weather()
{
    WiFiClient client;
    String request_info = String("GET ") + request_json + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
    if (client.connect(host, 80))
    {

        client.flush(); //先清理接受缓冲区
   
        //说明连接上  发送请求
        client.print(request_info);
        Serial.println("send request:");
        Serial.print(request_info);

        //接下来跳过http响应头部
        client.find("\r\n\r\n");
        //开始进行读取数据
        receive_json(buff, MAX_SIZE,client);
        //数据读取完毕   进行json文件解析
        parse_json(buff);

        Serial.print("weather:  ");
        Serial.println(weather_data.weather);
        Serial.print("temperature:   ");
        Serial.println(weather_data.temperature);
        //解析完毕
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("location:");
        display.println(weather_position);
        display.setCursor(0,20);
        display.print("weature:   ");
        display.println(weather_data.weather);
        display.setCursor(0,40);
        display.print("temperature:    ");
        display.println(weather_data.temperature);
        
        display.display();
        
    }
    else
    {
        Serial.println("client connect fail");
    }
}
void receive_json(char *buff, int max_size,WiFiClient& client)
{
    memset(buff, 0, max_size);
    client.readBytes(buff, max_size);
    buff[MAX_SIZE - 1] = 0;
}
void parse_json(char *buff)
{

    StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, buff, MAX_SIZE);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    JsonObject results_0 = doc["results"][0];

    JsonObject results_0_now = results_0["now"];
    const char *results_0_now_text = results_0_now["text"];               
    const char *results_0_now_code = results_0_now["code"];               
    const char *results_0_now_temperature = results_0_now["temperature"]; 
    weather_data.weather = String(results_0_now_text);
    weather_data.temperature = atoi(results_0_now_temperature);
}
