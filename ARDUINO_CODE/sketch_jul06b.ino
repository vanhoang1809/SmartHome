#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ESP32Servo.h>

Servo myservo;

const char *ssid = "Hanh";
const char *password = "0905128744";

#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883 //port của hivemq
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_TOPIC_PUBLIC_SENSORS "Esp32/sensors"
#define MQTT_TOPIC_PUBLIC_FAN "Esp32/fan"
#define MQTT_TOPIC_SUBSCRIBE "Esp32/control"

//FAN AND CONTROL HC05
#define fan_living 21
#define fan_bed 19
#define fan_kitchen 18
#define fan_toilet 5
#define control_hc 17

//LED AND DOOR SERVO
#define led_living 27
#define led_bed 26
#define led_kitchen 25
#define led_toilet 33
///SERVO
#define door_living 32
//DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
//SENSORS
#define HC05Pin 23   
#define MQ2Pin  22
/********TIME*********/
unsigned long previousMillis = 0;
const long interval = 300;
unsigned long previousMillis2 = 0;
const long interval2 = 7000;
//Khai bao message nhận được
String message = "";

String SendWebJsonSensors = "";
String SendWebJsonFan = "";
//Khai bao du lieu cam bien
float humidity = 0;
float temperature = 0;
int gas_sensor=0;
int infrared_sensor=0;

//Tinh trang LED LIVING
int current_ledState_living = LOW;
int last_ledState_living = LOW;

//Tinh trang LED BED ROOM
int current_ledState_bed = LOW;
int last_ledState_bed = LOW;

//Tinh trang LED KITCHEN
int current_ledState_kitchen = LOW;
int last_ledState_kitchen = LOW;

//Tinh trang LED TOILET
int current_ledState_toilet = LOW;
int last_ledState_toilet = LOW;

//Tinh trang LED TOILET
int current_servo_living = LOW;
int last_servo_living = LOW;

/***********FAN AND CONTROL HC05*********/
//Tinh trang FAN LIVING
int current_fanState_living = LOW;
int last_fanState_living = LOW;

//Tinh trang FAN BED ROOM
int current_fanState_bed = LOW;
int last_fanState_bed = LOW;

//Tinh trang FAN KITCHEN
int current_fanState_kitchen = LOW;
int last_fanState_kitchen = LOW;

//Tinh trang FAN TOILET
int current_fanState_toilet = LOW;
int last_fanState_toilet = LOW;

//Tinh trang FAN HC05
int current_HC05 = LOW;
int last_HC05 = LOW;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi()
{
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void connect_to_broker()
{
    while (!client.connected())
    {
        Serial.print("Đang kết nối MQTT...");
        String clientId = "nodemqtt32";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("Đã kết nối!!!");
            client.subscribe(MQTT_TOPIC_SUBSCRIBE);
        }
        else
        {
            Serial.print("Đã lỗi, rc=");
            Serial.print(client.state());
            Serial.println(" Đợi kết nối sau 5 giây");
            delay(5000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.println("------------Nhận tin nhắn từ topic----------");
    Serial.print("topic: ");
    Serial.println(topic);
    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }
    Serial.print("message: ");
    Serial.println(message);
    /**********LED AND SERVO*********/
    if (message == "led1On")
    {
        current_ledState_living = HIGH;
    }
    else if (message == "led1Off")
    {
        current_ledState_living = LOW;
    }
    else if (message == "led2On")
    {
        current_ledState_bed = HIGH;
    }
    else if (message == "led2Off")
    {
        current_ledState_bed = LOW;
    }
    else if (message == "led3On")
    {
        current_ledState_kitchen = HIGH;
    }
    else if (message == "led3Off")
    {
        current_ledState_kitchen = LOW;
    }
    else if (message == "led4On")
    {
        current_ledState_toilet = HIGH;
    }
    else if (message == "led4Off")
    {
        current_ledState_toilet = LOW;
    }
    else if (message == "servo1On")
    {
        current_servo_living = HIGH;
    }
    else if (message == "servo1Off")
    {
        current_servo_living = LOW;
    }
    /***********FAN AND CONTROL HC05**********/
    else if (message == "fan1On")
    {
        current_fanState_living = HIGH;
    }
    else if (message == "fan1Off")
    {
        current_fanState_living = LOW;
    }
    else if (message == "fan2On")
    {
        current_fanState_bed = HIGH;
    }
    else if (message == "fan2Off")
    {
        current_fanState_bed = LOW;
    }
    else if (message == "fan3On")
    {
        current_fanState_kitchen = HIGH;
    }
    else if (message == "fan3Off")
    {
        current_fanState_kitchen = LOW;
    }
    else if (message == "fan4On")
    {
        current_fanState_toilet = HIGH;
    }
    else if (message == "fan4Off")
    {
        current_fanState_toilet = LOW;
    }
    else if (message == "hcOn")
    {
        current_HC05 = HIGH;
    }
    else if (message == "hcOff")
    {
        current_HC05 = LOW;
    }
    message = "";
}

void setup()
{
    Serial.begin(115200);
    //Serial.setTimeout(500);
    setup_wifi();
    //Thiết lập địa chỉ broker và port của hivemq
    client.setServer(MQTT_SERVER, MQTT_PORT);
    /*Đăng ký hàm setcallback trong thư viện pubsubclient sẽ được gọi 
  khi thư viện nhận được giá trị mới thay đổi trong broker cho 
  dữ liệu mà topic được subscribe*/
    client.setCallback(callback);
    connect_to_broker();
    dht.begin();
    /************INPUT**************/
    pinMode(HC05Pin, INPUT);
    pinMode(MQ2Pin, INPUT);
    /********OUTPUT LED*************/
    pinMode(led_living, OUTPUT);
    pinMode(led_bed, OUTPUT);
    pinMode(led_kitchen, OUTPUT);
    pinMode(led_toilet, OUTPUT);
    myservo.setPeriodHertz(50);
    myservo.attach(door_living);
    /***********OUTPUT FAN*******/
    pinMode(fan_living, OUTPUT);
    pinMode(fan_bed, OUTPUT);
    pinMode(fan_kitchen, OUTPUT);
    pinMode(fan_toilet, OUTPUT);
    pinMode(control_hc, OUTPUT);
}
void sendDataMQTT(String humidity, String temperature,String gas_sensor,String infrared_sensor, String current_fanState_living, String current_ledState_living, String current_servo_living, String current_HC05)
{
    SendWebJsonSensors = "";
    SendWebJsonSensors = "{\"humidity\":\"" + String(humidity) + "\"," +
                         "\"temperature\":\"" + String(temperature) + "\"," +
                         "\"infrared_sensor\":\"" + String(infrared_sensor) + "\"," +
                          "\"gas_sensor\":\"" + String(gas_sensor) + "\"," +
                         "\"current_fanState_living\":\"" + String(current_fanState_living) + "\"," +
                         "\"current_ledState_living\":\"" + String(current_ledState_living) + "\"," +
                         "\"current_HC05\":\"" + String(current_HC05) + "\"," +
                         "\"current_servo_living\":\"" + String(current_servo_living) + "\"}";
    Serial.print("SendDataMQTT:");
    Serial.println(SendWebJsonSensors);

    //Gửi dữ liệu lên broker
    client.publish(MQTT_TOPIC_PUBLIC_SENSORS, SendWebJsonSensors.c_str());
}
void sendDataMQTTFAN(String current_fanState_kitchen, String current_fanState_toilet,
                     String current_ledState_kitchen, String current_ledState_toilet, String current_fanState_bed, String current_ledState_bed)
{
    SendWebJsonFan = "";
    SendWebJsonFan = "{\"current_fanState_kitchen\":\"" + String(current_fanState_kitchen) + "\"," +
                     "\"current_fanState_toilet\":\"" + String(current_fanState_toilet) + "\"," +
                     "\"current_ledState_kitchen\":\"" + String(current_ledState_kitchen) + "\"," +
                     "\"current_fanState_bed\":\"" + String(current_fanState_bed) + "\"," +
                     "\"current_ledState_bed\":\"" + String(current_ledState_bed) + "\"," +
                     "\"current_ledState_toilet\":\"" + String(current_ledState_toilet) + "\"}";
    Serial.print("SendDataMQTT_KIT:");
    Serial.println(SendWebJsonFan);

    //Gửi dữ liệu lên broker
    client.publish(MQTT_TOPIC_PUBLIC_FAN, SendWebJsonFan.c_str());
}
void loop()
{
    client.loop();
    //Kiem tra trang thai ket noi den broker
    if (!client.connected())
    {
        connect_to_broker();
    }
    if (last_ledState_living != current_ledState_living || last_ledState_bed != current_ledState_bed || last_ledState_kitchen != current_ledState_kitchen || last_ledState_toilet != current_ledState_toilet || last_servo_living != current_servo_living || last_fanState_living != current_fanState_living || last_fanState_kitchen != current_fanState_kitchen || last_fanState_bed != current_fanState_bed || last_fanState_toilet != current_fanState_toilet || last_HC05 != current_HC05)
    {
        last_ledState_living = current_ledState_living;
        last_ledState_bed = current_ledState_bed;
        last_ledState_kitchen = current_ledState_kitchen;
        last_ledState_toilet = current_ledState_toilet;
        last_servo_living = current_servo_living;
        /********FAN**********/
        last_fanState_living = current_fanState_living;
        last_fanState_bed = current_fanState_bed;
        last_fanState_kitchen = current_fanState_kitchen;
        last_fanState_toilet = current_fanState_toilet;
        last_HC05 = current_HC05;
        /*******DIGITAL**************/
        digitalWrite(led_kitchen, current_ledState_kitchen);
        digitalWrite(led_toilet, current_ledState_toilet);
        digitalWrite(led_bed, current_ledState_bed);
        digitalWrite(led_living, current_ledState_living);
        /*************FAN***********/
        digitalWrite(fan_living, current_fanState_living);
        digitalWrite(fan_kitchen, current_fanState_bed);
        digitalWrite(fan_bed, current_fanState_kitchen);
        digitalWrite(fan_toilet, current_fanState_toilet);
        digitalWrite(control_hc, current_HC05);
        if (current_servo_living == 1)
        {
            myservo.write(0);
        }
        else
        {
            myservo.write(90);
        }
        Serial.print("Tình trạng led 1: ");
        Serial.println(current_ledState_living);
        Serial.print("Tình trạng led 2: ");
        Serial.println(current_ledState_bed);
        Serial.print("Tình trạng led 3: ");
        Serial.println(current_ledState_kitchen);
        Serial.print("Tình trạng led 4: ");
        Serial.println(current_ledState_toilet);
        Serial.print("Tình trạng servo 1: ");
        Serial.println(current_servo_living);
        Serial.print("Tình trạng fan 1: ");
        Serial.println(current_fanState_living);
        Serial.print("Tình trạng fan 2: ");
        Serial.println(current_fanState_bed);
        Serial.print("Tình trạng fan 3: ");
        Serial.println(current_fanState_kitchen);
        Serial.print("Tình trạng fan 4: ");
        Serial.println(current_fanState_toilet);
        Serial.print("Tình trạng HC05: ");
        Serial.println(current_HC05);
    }
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        
        humidity = dht.readHumidity();
        temperature = dht.readTemperature();
        gas_sensor=digitalRead(MQ2Pin);
        infrared_sensor =digitalRead(HC05Pin);
        //Kiểm tra việc đọc giá trị sensor
        if (isnan(humidity) || isnan(temperature))
        {
            Serial.println(("Lỗi đọc giá trị của sensor DHT11!"));
            return;
        }
        else
        {
            sendDataMQTT(String(humidity), String(temperature),String(gas_sensor), String(infrared_sensor), String(current_fanState_living), String(current_ledState_living), String(current_servo_living), String(current_HC05));
        }

        //   sendDataMQTTFAN(String(current_fanState_kitchen), String(current_fanState_bed), String(current_fanState_living), String(current_fanState_toilet), String(current_HC05));
        previousMillis = currentMillis;
    }
    unsigned long currentMillis2 = millis();
    if (currentMillis2 - previousMillis2 >= interval2)
    {
        //Kiểm tra việc đọc giá trị sensor

        sendDataMQTTFAN(String(current_fanState_kitchen), String(current_ledState_kitchen), String(current_fanState_toilet), String(current_ledState_toilet), String(current_fanState_bed), String(current_ledState_bed));
        previousMillis2 = currentMillis2;
    }
}
