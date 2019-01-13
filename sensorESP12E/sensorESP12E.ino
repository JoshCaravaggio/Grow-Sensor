#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiUdp.h>


// Structure of time series data to be sent to the server
// Configured for a single DHT11 Sensnor and two DS18B20 sensors
// (Soil temperature could be replace with water temperature for hydroponic systems)
struct dataReading{  
  
  float airTemperature;
  float airHumidity;
  float soilTemperature1;  
  float soilTemperature2;  
  
};



const char* firebaseHost = "firebase-host.firebaseio.com";           //firebase domain
const char* firebaseAuth = "firebase-db-api-key";    //firebase database API key
const char* ssid = "wifi-ssid";                                                //Wifi ssid
const char* password = "wifi-password";                                      //WiFi password



const int DHTPin = 2;                                // Decleration of DHT pin
const int DS18Pin1 = 0;                              // Decleration of first DS18B20 bus line pin


float airTemperature;                               // Floats used for sensor values
float airHumidity;                                  
float soilTemperature1; 
float soilTemperature2; 
OneWire oneWire1(DS18Pin1);                           // Onewire interface for DS18B20 Dallas Temperature sensor
DallasTemperature DS18_1(&oneWire1);                  // Dallas ttemperature sensor structure;
DHT dht(DHTPin, 11);                                  // DHT11 sensor devleration, assigning it to the DHTPin
WiFiUDP ntpUDP;                                       // Wifi UDP for retrieving UTC timestamp
NTPClient timeClient(ntpUDP);                         // NTP client for collecting timestamp with ntp UDP
int timestamp;                                        // Timestamp integer
int delayMinutes = 1;                                 //  Amount of minutes between each data reading


// Function to create a JSON string out of a dataReading and a collected timestamp
// params: address of dataReading to be JSONified, integer timestamp
// return: JSON string of data 
String dataToJSON(dataReading *record, int timestamp){
  
  String JSONrecord;

  // Adding soil temperatures to JSON string
  JSONrecord += "{soilTemperature1:";     
  JSONrecord += record->soilTemperature1;
  JSONrecord += ",soilTemperature2:";
  JSONrecord += record->soilTemperature2;  
  JSONrecord += ",airTemperature:";


  // Checking the DHT temperature and humidity values for a bad read. If it occurs, we apend
  // the error code -127.00 instead, as firebase DB expects a number 
  if(!isnan(record->airTemperature)){

    JSONrecord += record->airTemperature;
    
    
  }else{
    
    JSONrecord += "-127.00";
    
  }  
  JSONrecord += ",airHumidity:";
  if(!isnan(record->airHumidity) ){

    JSONrecord += record->airHumidity;
    
    
  }else{
    
    JSONrecord += "-127.00";
    
  }
  JSONrecord += ",timestamp:";
  JSONrecord += timestamp;  
  JSONrecord += "}";

  return JSONrecord;
  
  
}

// Fuction to collect environmental data from the DHT and DS18 sensors
// params: address of dataReading to be populated with collected data

void collectData(dataReading *record){

  DS18_1.requestTemperatures();                                         // Requesting temperatures froom the DS18B20 busline

  record->soilTemperature1 = DS18_1.getTempCByIndex(0);                 // Retrieving soil/water temperature from first temperature probe and setting
  record->soilTemperature2 = DS18_1.getTempCByIndex(1);                 // Retrieving soil/water temperature from second temperature probe and setting
  record->airTemperature = dht.readTemperature();                       // Retrieving air temperature from DHT11 and setting
  record->airHumidity = dht.readHumidity();                             // Retrieving relative humidity of air from DHT11 and setting
  
  return;
  
}



// Function to push a datapoint to the firebase database
// params: datapoint object to be pushed to the firebase realtime database
 
void pushDataToDatabase(JsonObject& datapointObject ){
  
    Firebase.begin(firebaseHost,firebaseAuth);          // Begin firebase connection
    Firebase.push("datapoints/",datapointObject);       // Push databaseObject to the datapoints section of realtime DB


   
    if(Firebase.failed()){                              // If firebase connection fails, handle the error appropriately     
      Serial.println(Firebase.error());                 // Change this in for any embedded error handling, soon to add error logging system
      
    }  
  
  
  
}


//Function to test the first DS18 and print reading to serial output

void testDS18_1(){

  DS18_1.requestTemperatures();                    // Requesting temperature from DS18B20 line
  delay(1000);
  soilTemperature1 = DS18_1.getTempCByIndex(0);    // Retrieving first DS18B20 reading   
  Serial.print("Soil Temperature 1: ");
  Serial.print(soilTemperature1);                  // Printing to serial output
  Serial.print("\n");  
  return;
  
}

// Function to test the second DS18 and print reading to serial output

void testDS18_2(){


  DS18_1.requestTemperatures();                    // Requesting temperature from DS18B20 line
  delay(3000);                                     // Delay slightly to allow memory to be updated 
  soilTemperature2 = DS18_1.getTempCByIndex(1);    // Retrieving first DS18B20 reading  
  Serial.print("Soil Temperature 2: ");
  Serial.print(soilTemperature2);                  // Printing to serial output
  Serial.print("\n");  

  return;
  
}

// Function to test the DHT11 moduke and print reading to serial output
void testDHT(){

  airTemperature = dht.readTemperature();           // Rerieving temperature
  Serial.print("Air Temperature: ");
  Serial.println(airTemperature);                   // Printing Temperature
  airHumidity = dht.readHumidity();                 // Retieving relative humidity
  Serial.print("Relative Humidity: ");
  Serial.println(airHumidity);                      // Printing humidity

  return;
  
}


// Function to print out a dataReading to the serial output
// params: dataReading to be printed
void printDataReading(dataReading* record){


  Serial.print("Soil Temperature 1: ");
  Serial.print(record->soilTemperature1);
  Serial.print("\n"); 
  Serial.print("Soil Temperature 2: ");
  Serial.print(record->soilTemperature2);
  Serial.print("\n");   
  Serial.print("Air Temperature: ");
  Serial.println(record->airTemperature);
  Serial.print("Relative Humidity: ");
  Serial.println(record->airHumidity);
  
  return;
  
}




// Setup for wifi and serial output
void setup () {

  WiFi.begin(ssid, password);                   // Starting WiFi connection

  while (WiFi.status() != WL_CONNECTED) {       // Wait for the WiFI connection completion
 
    delay(500);
 
  }
  
  Serial.begin(115200);                         // Begin serial output



}
// Main loop of the program
void loop() {

  // Initiating both sensor reads at the start of the loop and the NTP time client for timestamps, helps to avoid sensors from failing from experience
  dht.begin();
  DS18_1.begin();
  timeClient.begin(); 

  
  dataReading record;                                                          // Single dataReading to store the read data
   
  if (WiFi.status() == WL_CONNECTED) {                                         // Check WiFi connection status, 

  
    collectData(&record);                                                      // Use the collectData function to populate the record with retrieved sensor data
    timeClient.update();                                                       // Updating the NTP time client with the most recent timestamp 
    
    timestamp = timeClient.getEpochTime();                                     // Retrieving the current UTC timestamp for the collected datapoint
    String JSONrecord = dataToJSON(&record, timestamp);                        // Creating a JSON representation of the datapoint with the record and timestamp
    
    //Serial.println(JSONrecord);                                              // Printing out the JSON record (if uncommented, used for debugging)   
     
    StaticJsonBuffer<500> JSONObjectRecord;                                    // Static JSON Object to hold the the data
    JsonObject& datapointObject = JSONObjectRecord.parseObject(JSONrecord);    // Create the address of that JSON object with the parseObject 
    
    pushDataToDatabase(datapointObject);                                       // Pushing the data objectg to the database 



  }
  delay( delayMinutes* 60 * 1000);                                             // Send a request every amount of minutes in delayMinutes

}
