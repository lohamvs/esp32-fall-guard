#include <WiFiManager.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <PubSubClient.h>  
#include <Wire.h>
#include <ArduinoJson.h>

long lastTemp = 0;
const char* mqtt_server = "192.168.0.16";

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
    reconnect();
  }
  
  client.loop();
  
  long now = millis();

  if (now - lastTemp > 1000) {
    lastTemp = now;
    
    StaticJsonDocument<192> doc;
    
    doc["accx"] = a.acceleration.x;
    doc["accy"] = a.acceleration.y;
    doc["accz"] = a.acceleration.z;
    doc["gyrox"] = g.gyro.x;
    doc["gyroy"] = g.gyro.y;
    doc["gyroz"] = g.gyro.z;
    doc["temp"] = temp.temperature;

    String output;
    serializeJson(doc, output);
    
    client.publish("event", (char*)output.c_str());
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
    WiFiManager wifiManager;

    // Automatically connect or set up a portal
    if (!wifiManager.autoConnect("ESP32_AP")) {
        Serial.println("Failed to connect. Restarting...");
        ESP.restart();
    }

    // Wi-Fi is connected
    Serial.println("WiFi connected");
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
      client.publish("event", "Giroscópio acelerômetro conectado.");
      
      // Envia a mensagem ao servidor MQTT
      client.subscribe("event");
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
      delay(10);
    }
  }
  Serial.println("MPU6050 encontrado!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Intervalo do acelerômetro: +- ");
  Serial.print(mpu.getAccelerometerRange());
  Serial.println(" 8G");
  
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Intervalo do giroscópio: +- ");
  Serial.print(mpu.getGyroRange());
  Serial.println(" deg/s");
  
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filtro de largura de banda: ");
  Serial.print(mpu.getFilterBandwidth());
  Serial.println(" Hz");
  
  delay(100);
}
