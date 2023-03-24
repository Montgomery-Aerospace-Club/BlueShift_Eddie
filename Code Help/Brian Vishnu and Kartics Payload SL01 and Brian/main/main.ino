#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"

#include <SPI.h>
#include <SD.h>
#include "FS.h"

#define I2C_SDA 26
#define I2C_SCL 27

#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_SCK 14
#define SD_CS 5


//libraries
#include <xCore.h>
#include <xSL01.h>

const int DELAY_TIME = 1000;

String header = "time(ms),uvIndex,\n";


xSL01 SL01;

Adafruit_MCP9600 mcp;

void setup() {
    Serial.begin(115200);

    // initalize communication stuff

    Wire.begin(I2C_SDA, I2C_SCL);
    SPIClass spi = SPIClass(HSPI);
    spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);  



    //makes sure serial is open 
    // DELETE BEFORE PUTTING INTO PAYLOAD BOXX
    while (!Serial) {}


    SD_INIT(spi);
    MCP9600_INIT();

    SL01.begin();

    while(true){
        long time = millis();
        float uv=0;


        SL01.poll();
        uv = SL01.getUVIndex();
        Serial.print("UVB Index: ");
        Serial.println(uv);

        float mcpfloat = mcp.readAmbient()

        Serial.print("Cold Junction: "); 
        Serial.println(mcpfloat);
        Serial.println();


        String uvStr = String(uv,3);
        String mcpStr = String(mcpfloat,3);

        String data = "";
        data += String(time);
        data += ",";
        data += uvStr;
        data += ",";
        data += mcpStr;
        data += "\n";

        appendFile(SD, "/data.txt", data.c_str());

        delay(DELAY_TIME);
    }
   
}

void loop() {

   
}

void writeFile(fs::FS &fs, const char * path, const char * message) {


    File file = fs.open(path, FILE_WRITE);
    if(!file) {
        return;
    }
    if(file.print(message)) {
    } else {
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {

    File file = fs.open(path, FILE_APPEND);
    if(!file) {
        return;
    }
    if(file.print(message)) {
    } else {
    }
    file.close();
}

void MCP9600_INIT(){
    /* Initialise the driver with I2C_ADDRESS and the default I2C bus. */
    if(!mcp.begin(0x67)){
        Serial.println("Sensor not found. Check wiring!");
        while(1);
    }

    Serial.println("Found MCP9600!");

    mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
    Serial.print("ADC resolution set to ");
    switch (mcp.getADCresolution()) {
        case MCP9600_ADCRESOLUTION_18:   Serial.print("18"); break;
        case MCP9600_ADCRESOLUTION_16:   Serial.print("16"); break;
        case MCP9600_ADCRESOLUTION_14:   Serial.print("14"); break;
        case MCP9600_ADCRESOLUTION_12:   Serial.print("12"); break;
    }
    Serial.println(" bits");

    mcp.setThermocoupleType(MCP9600_TYPE_K);
    Serial.print("Thermocouple type set to ");
    switch (mcp.getThermocoupleType()) {
        case MCP9600_TYPE_K:  Serial.print("K"); break;
        case MCP9600_TYPE_J:  Serial.print("J"); break;
        case MCP9600_TYPE_T:  Serial.print("T"); break;
        case MCP9600_TYPE_N:  Serial.print("N"); break;
        case MCP9600_TYPE_S:  Serial.print("S"); break;
        case MCP9600_TYPE_E:  Serial.print("E"); break;
        case MCP9600_TYPE_B:  Serial.print("B"); break;
        case MCP9600_TYPE_R:  Serial.print("R"); break;
    }
    Serial.println(" type");

    mcp.setFilterCoefficient(3);
    Serial.print("Filter coefficient value set to: ");
    Serial.println(mcp.getFilterCoefficient());

    mcp.setAlertTemperature(1, 30);
    Serial.print("Alert #1 temperature set to ");
    Serial.println(mcp.getAlertTemperature(1));
    mcp.configureAlert(1, true, true);  // alert 1 enabled, rising temp

    mcp.enable(true);

    Serial.println(F("------------------------------"));

}

void SD_INIT(SPIClass spi){
    if (!SD.begin(SD_CS, spi,80000000)) {
        Serial.println(F("Card Mount Failed"));
        return;
    }

    uint8_t cardType = SD.cardType();

    while (cardType == CARD_NONE){
        Serial.println(F("No SD card attached"));
    }

    if (SD.exists("/data.txt")) {
        SD.remove(F("/data.txt"));
    }
    

    writeFile(SD, "/data.txt", header.c_str());
}