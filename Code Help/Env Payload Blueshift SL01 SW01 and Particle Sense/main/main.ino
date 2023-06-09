#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "FS.h"


#include <xCore.h>

#include <xSL01.h>
xSL01 SL01;

#include <xSW01.h>
xSW01 SW01;
#include <xSG35.h>

xSG35 SG35;
xSG35::DATA data;

#define I2C_SDA 26
#define I2C_SCL 27

#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_SCK 14
#define SD_CS 5

// bool SDerror = false;
// bool airQualError = false;


String header = "time(ms),lux,UVAIntensity(uW/m^2),UVBIntensity,UVindex,temp(C),humi(%),press(Pa),PM1.0,PM2.5,PM10(ug/m3)\n";

void setup() {
  Serial.begin(115200);

  Wire.setPins(I2C_SDA, I2C_SCL);
  Wire.begin();
  SG35.begin();

  if(!SW01.begin()){
    Serial.println("please help me weather");
  }

  if(!SL01.begin()){
    Serial.println("please help me light");
  }

  SPIClass spi = SPIClass(HSPI);
  spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);  
  if (!SD.begin(SD_CS, spi,80000000)) {
    Serial.println(F("Card Mount Failed"));
    return;
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
      Serial.println(F("No SD card attached"));
      return;
  }

  if (SD.exists("/data.txt")) {
    SD.remove(F("/data.txt"));
  }
  

  writeFile(SD, "/data.txt", header.c_str());

  while (true){


    long time = millis();
    float tempC = 0;
    float humidity = 0;
    double press = 0;
    int pm1 = 69;
    int pm25 = 69;
    int pm10 = 69;
    String dataa = "";
    float lux = 0;
    float uva = 0;
    float uvb = 0;
    float uvindex = 0;

    SL01.poll();
    lux = SL01.getLUX();
    uva = SL01.getUVA();
    uvb = SL01.getUVB();
    uvindex =  SL01.getUVIndex();
    
    
    


    SW01.poll();
    humidity = SW01.getHumidity();
    press = SW01.getPressure();
    tempC = SW01.getTempC(); 
    
    if (SG35.read(data))
    {
      pm1 = data.PM_AE_UG_1_0;

      pm25 = data.PM_AE_UG_2_5;

      pm10 = data.PM_AE_UG_10_0;
    
    }


    dataa += String(time);
    dataa.concat(",");
    dataa += String(lux);
    dataa.concat(",");
    dataa += String(uva);
    dataa.concat(",");
    dataa += String(uvb);
    dataa.concat(",");
    dataa += String(uvindex);
    dataa.concat(",");
    dataa += String(tempC);
    dataa.concat(",");
    dataa += String(humidity);
    dataa.concat(",");
    dataa += String(press);
    dataa.concat(",");
    dataa += String(pm1);
    dataa.concat(",");
    dataa += String(pm25);
    dataa.concat(",");
    dataa += String(pm10);
    dataa += "\n";
    
    if (pm1 != 69 && pm25 != 69 && pm10 != 69){
      // Serial.print(header);
      // Serial.println(dataa);
      appendFile(SD, "/data.txt", dataa.c_str());
    }
    
  
    
  }

}

void loop() {
}

//SD CARD
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

