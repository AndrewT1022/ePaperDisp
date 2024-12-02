//This is our actual code page. The rest is either configuration or libaries. =)


#include <SPI.h>
#include "epd4in2_V2.h"
#include "epdpaint.h"

#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>

unsigned char image[5000];  // Adjusted buffer size for resolution
Paint paint(image, 400, 600);    // width should be the multiple of 8
Epd epd;

#define COLORED     0
#define UNCOLORED   1

Adafruit_BMP280 bmp; //Initialize BMP280 Sensor
Adafruit_AHTX0 aht; //Initialize AHT21 Sensor

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
     bmp.setSampling(Adafruit_BMP280::MODE_FORCED,      // Set mode to forced, lowest power.
                  Adafruit_BMP280::SAMPLING_X1,        // Set oversampling for pressure to 1, lowest power.
                  Adafruit_BMP280::SAMPLING_X1,       // Set oversampling for temperature to 1, lowest power.
                  Adafruit_BMP280::FILTER_OFF,       // Turn off noise filtering, lowest power.
                  Adafruit_BMP280::STANDBY_MS_250); // Compromise made here to give us the best of both worlds in response time and power
  bmp.begin();
  aht.begin();

  if (epd.Init() != 0) {
    Serial.print("e-Paper init failed");
    return;
  }

  /* This clears the SRAM of the e-paper display */
  epd.Clear();

  /**
    * Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
    * In this case, a smaller image buffer is allocated and you have to 
    * update a partial display several times.
    * 1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
    */
    paint.SetWidth(200);
    paint.SetHeight(200);

paint.Clear(UNCOLORED);
}

void loop() {
  // put your main code here, to run repeatedly:

sensors_event_t humidity, temp;
aht.getEvent(&humidity, &temp);
float AHTTemp = temp.temperature; // *C
float AHTHumidity = humidity.relative_humidity; // %
float BMPTemp = bmp.readTemperature(); // *C
float BMPPressure = bmp.readPressure(); // PA
float BMPAltitude = bmp.readAltitude(); // m

char AHTTempStr[10];
  char AHTHumidityStr[10];
  char BMPTempStr[10];
  char BMPPressureStr[10];
  char BMPAltitudeStr[10];

  dtostrf(AHTTemp, 3, 1, AHTTempStr);  // 3 width, 1 decimal place
  dtostrf(AHTHumidity, 4, 2, AHTHumidityStr);
  dtostrf(BMPTemp, 3, 1, BMPTempStr);
  dtostrf(BMPPressure / 100.0, 6, 2, BMPPressureStr);  // Convert Pa to kPa
  dtostrf(BMPAltitude, 6, 2, BMPAltitudeStr);
    paint.SetWidth(200);
    paint.SetHeight(200);
    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 0, "Temperature:", &Font20, COLORED);
    paint.DrawStringAt(0, 20, AHTTempStr, &Font20, COLORED);
  
    paint.DrawStringAt(0, 60, "Humidity:", &Font20, COLORED);
    paint.DrawStringAt(0, 80, AHTHumidityStr, &Font20, COLORED);

    paint.DrawStringAt(0, 120, "Pressure:", &Font20, COLORED);
    paint.DrawStringAt(0, 140, BMPPressureStr, &Font20, COLORED);

  epd.Display_Partial(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
delay(1000);
}