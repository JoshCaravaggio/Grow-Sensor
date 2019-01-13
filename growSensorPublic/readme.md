# ESP 12-E Grow Sensor Embedded Code
#### By Joshua Caravaggio

## Description

This is the embedded code for a cheap grow sensor that can be used to collect environmental data for indoor growing operations. The server side of this system is a Node.js application hosted on Firebase and utilizing their realtime DB, and could be easily expanded to a net of sensors with some modifications (really just a sensor identifier member in the datapoint structure). 

After the server side is perfected and code cleaned up for a public repo that will be uploaded

## Setup

The code is embedded on an ESP-12E MCU running NodeMCU and being programmed with the arduino IDE. The full design fits onto a breadboard quite easily, and the code written here requires:
1x ESP-12E with NodeMCU
2x DS18B20 Dallas Waterproof Temperature Sensors
1x DHT11 Digital Air & Relative Humidity Sensor
1x MB102 Breadboard Power Supply (Or any other way the sensors and board could be powered)

A breadboard schematic will be included soon

## Documentation

#### Dependencies 
[Firebase Arduino](https://github.com/FirebaseExtended/firebase-arduino) for pushing datapoint objects to a Firebase Realtime DB

(https://github.com/arduino-libraries/NTPClient) for retrieving a UTC timestamp
(https://github.com/bblanchon/ArduinoJson "ArduinoJSON ") for turning data into JSON objects to play nice with the Firebase library (Use Version 5.13.2)
(https://github.com/esp8266/Arduino/tree/master/doc/esp8266wifi "ESP8266 Wifi") for setting up wifi connection on ESP 12-E
(https://github.com/adafruit/DHT-sensor-library "DHT") for retrieving digital reads from the DHT11 sensor 
(https://github.com/PaulStoffregen/OneWire "OneWire") for interfacing with Dallas temperature sensors
(https://github.com/milesburton/Arduino-Temperature-Control-Library "Dallas Temperature") used in junction with OneWire for digital reads from DS18B20 bus line

#### Datapoint Structure
The current set up creates a struct with air temperature and humidity reading as well as 2 soil temperature channels (the DS18B20s) which could be easily increased or changed to water temperature for a hydroponic operation.

The datapoint is converted to a JSON object for pushing to firebase, and takes on the form:

{
    "airTemperature" : <float>,
    "airHumidity" : <float>,
    "soilTemperature1" : <float>,
    "soilTemperature2" : <float>,
    "timestamp" : <integer>
}
The program is designed such that even if a read fails, the associated field is populated with -127.00, which is effectively an error code we use such that the JSON still contains the appropriate data types for the DB push.

#### Functions




