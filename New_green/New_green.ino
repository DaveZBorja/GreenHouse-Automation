#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>
//#include <CapacitiveSoilMoistureSensor.h>

// Pin definitions

#define DHTPIN 9   // GPIO16 for DHT11 (D3)
#define DHTTYPE DHT11
#define RELAY1 14  // GPIO14 for Relay1 (D5)
#define RELAY2 12  // GPIO12 for Relay2 (D6)
#define RELAY3 13  // GPIO13 for Relay3 (D7)
#define RELAY4 15  // GPIO15 for Relay4 (D8)
#define SOIL_MOISTURE_PIN A0  // Analog pin for soil moisture sensor
#define MOTOR1_PWM_PIN 10     // PWM pin for motor 1
#define MOTOR2_PWM_PIN 2    // PWM pin for motor 2

// Sensor and server objects
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;
ESP8266WebServer server(80);
//CapacitiveSoilMoistureSensor soilMoistureSensor(SOIL_MOISTURE_PIN);

// Thresholds
float tempThreshold = 30.0;
float humidityThreshold = 70.0;
int motor1SpeedThreshold = 128;
int motor2SpeedThreshold = 128;
int soilMoistureThreshold = 50;



void handleData() {
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN); // Read soil moisture value (0-1023)
  float soilMoisturePercentage = map(soilMoistureValue, 1023, 0, 0, 100); // Convert to percentage

  String data = String(dht.readTemperature()) + "," + String(dht.readHumidity()) + "," + String(bmp.readTemperature()) + "," + String(bmp.readPressure() / 100.0F) + "," + String(soilMoisturePercentage) + "," + String(analogRead(MOTOR1_PWM_PIN) / 1023.0 * 100) + "," + String(analogRead(MOTOR2_PWM_PIN) / 1023.0 * 100);
  server.send(200, "text/plain", data);

    // Trigger relays based on thresholds
  if (dht.readTemperature() > tempThreshold) digitalWrite(RELAY1, HIGH);
  else digitalWrite(RELAY1, LOW);

  if (dht.readHumidity() > humidityThreshold) digitalWrite(RELAY2, HIGH);
  else digitalWrite(RELAY2, LOW);

  if (soilMoisturePercentage > soilMoistureThreshold) digitalWrite(RELAY3, HIGH);
  else digitalWrite(RELAY3, LOW);
}

void handleSetThresholds() {
  if (server.hasArg("temp") && server.hasArg("humidity") && server.hasArg("motor1Speed")&& server.hasArg("motor2Speed") && server.hasArg("soilMoisture")) {
 
  tempThreshold = server.arg("temp").toFloat();
  humidityThreshold = server.arg("humidity").toFloat();
  motor1SpeedThreshold = server.arg("motor1Speed").toInt();
  motor2SpeedThreshold = server.arg("motor2Speed").toInt();
  soilMoistureThreshold = server.arg("soilMoisture").toInt();

  Serial.print("Setting tempThreshold to: ");
  Serial.println(tempThreshold);
  Serial.print("Setting humidityThreshold to: ");
  Serial.println(humidityThreshold);
  Serial.print("Setting motor1SpeedThreshold to: ");
  Serial.println(motor1SpeedThreshold);
  Serial.print("Setting motor2SpeedThreshold to: ");
  Serial.println(motor2SpeedThreshold);

  EEPROM.put(0, tempThreshold);
  EEPROM.put(4, humidityThreshold);
  EEPROM.put(8, motor1SpeedThreshold);
  EEPROM.put(12, motor2SpeedThreshold);
  EEPROM.put(16, soilMoistureThreshold);
  EEPROM.commit();
    server.send(200, "text/plain", "Thresholds Set");
  } else {
    server.send(400, "text/plain", "Invalid Input");
  }
}

void handleToggleRelay() {
  if (server.hasArg("relay")) {
    int relay = server.arg("relay").toInt();
    int relayPin = 14;
    switch (relay) {
      case 1: relayPin = RELAY1; break;
      case 2: relayPin = RELAY2; break;
      case 3: relayPin = RELAY3; break;
      case 4: relayPin = RELAY4; break;
      default: server.send(400, "text/plain", "Invalid relay number"); return;
    }
    int currentState = digitalRead(relayPin);
    digitalWrite(relayPin, !currentState);  // Toggle the state
    server.send(200, "text/plain", "Relay " + String(relay) + " toggled");
  } else {
    server.send(400, "text/plain", "No relay specified");
  }
}

void handleSetMotorSpeed() {
  int motor = server.arg("motor").toInt();
  int speed = server.arg("speed").toInt();
  analogWrite(motor == 1 ? MOTOR1_PWM_PIN : MOTOR2_PWM_PIN, speed / 100.0 * 255);
  server.send(200, "text/plain", "Motor " + String(motor) + " speed set to " + String(speed) + "% successfully!");
}
void loadThresholds() {
  EEPROM.get(0, tempThreshold);
  EEPROM.get(4, humidityThreshold);
  EEPROM.get(8, motor1SpeedThreshold);
  EEPROM.get(12, motor2SpeedThreshold);
  EEPROM.get(16, soilMoistureThreshold);
}


void setup() {
  Serial.begin(115200);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);
  digitalWrite(RELAY3, LOW);
  digitalWrite(RELAY4, LOW);

  pinMode(MOTOR1_PWM_PIN, OUTPUT);
  pinMode(MOTOR2_PWM_PIN, OUTPUT);

  dht.begin();
  bmp.begin(0x76);

  WiFi.softAP("ESP8266");
  //WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  //WiFi.broadcastAP(true);

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/set", handleSetThresholds);
  server.on("/toggleRelay", handleToggleRelay);
  server.on("/setMotorSpeed", handleSetMotorSpeed);
  server.begin();

  EEPROM.begin(512);
  loadThresholds();

}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = R"=====(
    <html>
    <head>
    <meta charset='UTF-8' /><meta name='viewport' content='width=device-width, initial-scale=1.0' />

      <title>Incubator Dashboard</title>
      <style>
        body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 0; background-color: #AfAfAf; }
        .container { width: 80%; margin: 0 auto; padding: 20px; background-color: #003e29; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
        .header { background-color: #4CAF50; color: white; padding: 10px; border-radius: 10px 10px 0 0; }
        //header img { width: 50px; vertical-align: middle; }
        .header { background-color: #4CAF50; color: white; padding: 10px; border-radius: 10px 10px 0 0; display: flex; align-items: center; justify-content: center; }
        //.header img { width: 50px; margin-right: 15px; }
        .header h1 { margin: 0; }
        .section { margin: 20px 0; padding: 10px; border: 1px solid #ddd; border-radius: 5px; background-color: #fafafa; }
        .section h2 { margin: 0; }
        .sensor-data, .relay-control { text-align: left; }
        .sensor-data div, .relay-control div { margin-bottom: 10px; }
        .btn { padding: 10px 20px; font-size: 16px; border: none; border-radius: 5px; cursor: pointer; }
        .btn-primary { background-color: #4CAF50; color: white; }
        .btn-secondary { background-color: #f44336; color: white; }
       // .img-icon { width: 30px; vertical-align: left; }
        .footer { margin-top: 20px; padding: 10px; background-color: #333; color: white; font-size: 14px; }
        .footer p { margin: 0; } 
      </style>
      <script>
        function fetchData() {
          var xhr = new XMLHttpRequest();
          xhr.onreadystatechange = function() {
            if (xhr.readyState == 4 && xhr.status == 200) {
              var data = xhr.responseText.split(',');
              document.getElementById('temp').innerHTML = 'Temperature: ' + data[0] + ' °C';
              document.getElementById('humidity').innerHTML = 'Humidity: ' + data[1] + ' %';
              document.getElementById('bmpTemp').innerHTML = 'BMP Temp: ' + data[2] + ' °C';
              document.getElementById('bmpPressure').innerHTML = 'Pressure: ' + data[3] + ' hPa';
              document.getElementById('soilMoisture').innerHTML = 'Soil Moisture: ' + data[4] + ' %';
              document.getElementById('motor1Speed').innerHTML = 'Motor 1 Speed: ' + data[5] + ' %';
              document.getElementById('motor2Speed').innerHTML = 'Motor 2 Speed: ' + data[6] + ' %';
            }
          };
          xhr.open('GET', '/data', true);
          xhr.send();
        }

        function setThresholds() {
          var temp = document.getElementById('tempThreshold').value;
          var humidity = document.getElementById('humidityThreshold').value;
          var motor1Speed = document.getElementById('motor1SpeedThreshold').value;
          var motor2Speed = document.getElementById('motor2SpeedThreshold').value;
          var soilMoisture = document.getElementById('soilMoistureThreshold').value;
          var xhr = new XMLHttpRequest();
          xhr.open('GET', '/set?temp=' + temp + '&humidity=' + humidity + '&motor1Speed=' + motor1Speed + '&motor2Speed=' + motor2Speed + '&soilMoisture=' + soilMoisture, true);
          xhr.onreadystatechange = function() {
            if (xhr.readyState == 4 && xhr.status == 200) {
              console.log('Thresholds updated successfully!');
            }
          };
          xhr.send();
        }
        function toggleRelay(relay) {
          var xhr = new XMLHttpRequest();
          xhr.open('GET', '/toggleRelay?relay=' + relay, true);
          xhr.send();
        }

        function setMotorSpeed(motor, speed) {
          var xhr = new XMLHttpRequest();
          xhr.open('GET', '/setMotorSpeed?motor=' + motor + '&speed=' + speed, true);
          xhr.send();
        }
        setInterval(fetchData, 2000);
      </script>
    </head>
    <body onload="fetchData()">
      <div class="container">
      
        <div class="header">

          <h1>Green House</h1>
        </div>
        <div class="section sensor-data">
          <h2>Sensor Data</h2>
          <div id="temp"></div>
          <div id="humidity"></div>
          <div id="bmpTemp"></div>
          <div id="bmpPressure"></div>
          <div id="soilMoisture"></div>
          <div id="motor1Speed"></div>
          <div id="motor2Speed"></div>
        </div>
        
        <div class="section relay-control">
          <h2>Relay Control</h2>
          <button class="btn btn-primary" onclick="toggleRelay(1)">Toggle Relay 1</button>
          <button class="btn btn-primary" onclick="toggleRelay(2)">Toggle Relay 2</button>
          <button class="btn btn-primary" onclick="toggleRelay(3)">Toggle Relay 3</button>
          <button class="btn btn-primary" onclick="toggleRelay(4)">Toggle Relay 4</button>
        </div>
        <div class="section threshold-control">
          <h2>Threshold Control</h2>
          <form>
            <label for="tempThreshold">Temperature Threshold:</label>
            <input type="number" id="tempThreshold" value=")">
            <br>
            <br>
            <label for="humidityThreshold">Humidity Threshold:</label>
            <input type="number" id="humidityThreshold" value=")">
            <br>
            <br>
            <label for="motor1SpeedThreshold">Motor 1 Speed Threshold:</label>
            <input type="number" id="motor1SpeedThreshold" value=")">
            <br>
            <br>
            <label for="motor2SpeedThreshold">Motor 2 Speed Threshold:</label>
            <input type="number" id="motor2SpeedThreshold" value=")">
            <br>
            <br>
            <label for="soilMoistureThreshold">Soil Moisture Threshold:</label>
            <input type="number" id="soilMoistureThreshold" value=")">
            <br>
            <br>
            <button class="btn btn-primary" onclick="setThresholds()">Set Thresholds</button>
          </form>
        </div>
        <div class="footer">
          <p>&copy; 2023 Incubator Dashboard</p>
        </div>
      </div>
    </body>
  </html>
)=====";

  server.send(200, "text/html", html);
}
