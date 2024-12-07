# esp32-fall-guard
A real-time fall detection system using ESP32, MPU6050, and MQTT. Collects accelerometer and gyroscope data, publishes to an MQTT broker, and supports historical analysis and visualization. Extensible for machine learning-based fall prediction, with Docker support for easy deployment and scalability.

# Docker

To start docker with a bind mount volume (-v option), the root path, run:

```
docker run -it -p 1880:1880 -v ./src/node-red:/data --name mynodered nodered/node-red
```
