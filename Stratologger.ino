#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>

Adafruit_BMP085 bmp;

File logFile;

const int chipSelect = 10;
const int timeout = 500;

typedef struct sensorData {
    float T;
    int32_t P;
    float A;
} sensorData;

void setup() {
    Serial.begin(57600);
    pinMode(chipSelect, OUTPUT);
    while(!bmp.begin()) {
        clear();
        Serial.println("BMP180 not found, check connections.");
        delay(100);
    }
    while(!SD.begin(chipSelect)) {
        clear();
        Serial.println("SDCard not found, check connections.");
        delay(100);
    }
    logFile = SD.open("log.txt", FILE_WRITE);
    if(!logFile) {
        Serial.println("can't open log.txt file, restart.");
        while(1);
    }
    logFile.println("======== new start ========");
    logFile.print("timeout: ");
    logFile.print(timeout);
    logFile.println(" ms");
    logFile.println("C°   Pa    M");
    logFile.println("------------------");
    logFile.println("Temp|Press|Alt   |");
    logFile.close();
}

void loop() {
    sensorData data = getSensor(bmp);

    if(data.T < -50) {
        clear();
        Serial.println("BMP180 not found, check connections.");
        delay(100);
        return;
    }
    
    logFile = SD.open("log.txt", FILE_WRITE);
    if(!logFile) {
        Serial.println("can't open log.txt file, restart.");
        while(1);
    }

    saveSensor(data, logFile);
    printSensor(data);

    logFile.close();

    delay(timeout);
}

void saveSensor(sensorData data, File log) {
    log.print(data.T, 1);
    log.print("|");
    log.print(data.P);
    log.print("|");
    log.print(data.A, 2);
    log.println("|");
}

void printSensor(sensorData data) {
    clear();
    Serial.print("T: ");
    Serial.print(data.T, 1);
    Serial.println(" *C");
    Serial.print("P: ");
    Serial.print(data.P);
    Serial.println(" Pa");
    Serial.print("A: ");
    Serial.print(data.A, 2);
    Serial.println(" m");
}

sensorData getSensor(Adafruit_BMP085 sensor) {
    return {
        sensor.readTemperature(),
        sensor.readPressure(),
        sensor.readAltitude(),
    };
}

void clear() {
    Serial.write(27);
    Serial.print("[2J");
    Serial.write(27);
    Serial.print("[H");
}