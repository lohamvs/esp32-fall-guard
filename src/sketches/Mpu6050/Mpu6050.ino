#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <PubSubClient.h>  
#include <Wire.h>
#include <ArduinoJson.h>

const char* ssid = "ssid";
const char* password =  "password"; 
const char* mqtt_server = "192.168.1.124";
long lastTemp = 0;

Adafruit_MPU6050 mpu;

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);

  while (!Serial)
    delay(10);
  
  configureMpu6050();
    
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
}

void loop()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  if (!client.connected())
  {
    Serial.println("Client not connected");
    reconnect();
  }
  
  client.loop();
  
  long now = millis();

  if (now - lastTemp > 200) {
    lastTemp = now;
    
    StaticJsonDocument<192> doc;
    
    doc["accx"] = a.acceleration.x;
    doc["accy"] = a.acceleration.y;
    doc["accz"] = a.acceleration.z;

    String output;
    serializeJson(doc, output);
    
    client.publish("sensor/data", (char*)output.c_str());
  }
}
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Mensagem recebida[");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup_wifi() {
  delay(500);
  Serial.println();
  Serial.print("Conectado a rede: ");
  Serial.println(ssid);
  Serial.println(password);

  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    if (millis() - startAttemptTime > 10000) { 
      Serial.println("WiFi connection timeout.");
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("Conectado"); 
      
      //Envia a mensagem ao servidor MQTT
      client.subscribe("sensor/data");
    }
    else
    {
      Serial.print("Erro:");
      Serial.print(client.state());
      Serial.println(" reconectando em 5 segundos.");
      delay(5000);
    }
  }
}


void configureMpu6050()
{
  if (!mpu.begin()) {
    Serial.println("Falha ao encontrar MPU6050");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("MPU6050 encontrado!");

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  Serial.print("Intervalo do acelerômetro: +- ");
  Serial.print(mpu.getAccelerometerRange());
  
  mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);
  Serial.print("Filtro de largura de banda: ");
  Serial.print(mpu.getFilterBandwidth());
  Serial.println(" Hz");
  
  delay(100);
}
