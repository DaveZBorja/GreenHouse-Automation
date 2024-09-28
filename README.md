
# Greenhouse Monitoring and Control System

This project utilizes the ESP8266 microcontroller to monitor and control a greenhouse environment. It reads data from various sensors and allows users to control relays and set thresholds via a web interface.

## Features

- **Sensor Monitoring**:
  - Temperature and humidity using a DHT11 sensor.
  - Atmospheric pressure and temperature using a BMP280 sensor.
  - Soil moisture level using an analog sensor.

- **Relay Control**:
  - Control up to four relays for managing devices like fans, heaters, or lights based on sensor data.

- **Threshold Management**:
  - Set and save thresholds for temperature, humidity, and soil moisture.
  - Adjustable motor speeds for automated watering or ventilation.

- **Web Interface**:
  - A responsive web dashboard to view real-time data and manage settings.

## Components Required

- ESP8266 microcontroller
- DHT11 sensor (for temperature and humidity)
- BMP280 sensor (for atmospheric pressure)
- Soil moisture sensor (analog)
- Relays (x4)
- Motors (optional)
- Jumper wires
- Breadboard (optional)

## Installation

1. **Clone this repository**:
   ```bash
   [https://github.com/DaveZBorja/GreenHouse-Automation.git]
   ```

2. **Install necessary libraries**:
   - Install the following libraries via the Arduino Library Manager:
     - `DHT sensor library`
     - `Adafruit BMP280 Library`
     - `EEPROM library` (included with ESP8266)

3. **Upload the code**:
   - Open the `greenhouse_monitor.ino` file in the Arduino IDE.
   - Make sure to set up the correct board and port in the IDE.
   - Upload the code to your ESP8266.

4. **Connect your sensors and relays**:
   - Follow the pin definitions in the code to connect the sensors and relays to the corresponding GPIO pins.

5. **Access the web interface**:
   - Connect to the ESP8266's WiFi (SSID: `ESP8266`).
   - Open a web browser and navigate to `http://192.168.4.1`.

## Usage

- **View Sensor Data**: The main dashboard displays real-time sensor readings.
- **Control Relays**: Toggle the state of the relays with the provided buttons.
- **Set Thresholds**: Input desired thresholds for temperature, humidity, and soil moisture, and click "Set Thresholds" to save them.

## Code Structure

- **Main Functions**:
  - `setup()`: Initializes the sensors, relays, and web server.
  - `loop()`: Handles client requests and updates the server.
  - `handleData()`: Fetches sensor data and updates relay states based on thresholds.
  - `handleSetThresholds()`: Sets new thresholds based on user input.
  - `handleToggleRelay()`: Toggles the state of the specified relay.
  - `handleSetMotorSpeed()`: Adjusts the speed of connected motors.
  - `loadThresholds()`: Loads previously set thresholds from EEPROM.

## Thresholds

- **Temperature Threshold**: Default is 30°C
- **Humidity Threshold**: Default is 70%
- **Soil Moisture Threshold**: Default is 50%
- **Motor Speed Thresholds**: Default is 128 (on a scale of 0-255)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

- Special thanks to the developers of the libraries used in this project.
- Inspiration drawn from various IoT and automation projects.

---

Feel free to modify and enhance this project for your specific needs! If you encounter issues or have suggestions, please raise them as issues in the repository.
```

