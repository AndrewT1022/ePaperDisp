//This is our actual code page. The rest is either configuration or libaries. =)

//Copyright for the e-ink display libraries:
/*
 *  @filename   :   epd4in2_V2.cpp
 *  @brief      :   Implements for Dual-color e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <SPI.h>
#include <string>
#include "epd4in2_V2.h"
#include "epdpaint.h"

#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>

#define COLORED     0
#define UNCOLORED   1
unsigned char image[9000];  // Adjusted buffer size for resolution
Paint paint(image, 400, 600);    // width should be the multiple of 8
Epd epd;

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

    paint.SetWidth(300);
    paint.SetHeight(220);

paint.Clear(UNCOLORED);
}

void loop() {
sensors_event_t humidity, temp;
aht.getEvent(&humidity, &temp);
float AHTTemp = temp.temperature; // *C
float AHTHumidity = humidity.relative_humidity; // %
float BMPTemp = bmp.readTemperature(); // *C
float BMPPressure = bmp.readPressure(); // PA
float BMPAltitude = bmp.readAltitude(1028.4); // m ------- PUT CURRENT LOCAL PRESSURE HERE ---------------------


//blehg
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
    paint.SetWidth(300);
    paint.SetHeight(220);
    paint.Clear(UNCOLORED);

    //Note: add units later!!
    paint.DrawStringAt(0, 0, "Temperature:", &Font20, COLORED); //Print temperature on one line and the value on the next
    paint.DrawStringAt(0, 20, AHTTempStr, &Font20, COLORED);
    paint.DrawStringAt(60,22, "*C", &Font16, COLORED);
  
    paint.DrawStringAt(0, 60, "Humidity:", &Font20, COLORED); //Print humidity on one line and the value on the next
    paint.DrawStringAt(0, 80, AHTHumidityStr, &Font20, COLORED); 
      paint.DrawStringAt(70,82, "%", &Font16, COLORED);

    paint.DrawStringAt(0, 120, "Pressure:", &Font20, COLORED); //Print pressure on one line and the value on the next
    paint.DrawStringAt(0, 140, BMPPressureStr, &Font20, COLORED);
    paint.DrawStringAt(100,143, "kPA", &Font16, COLORED);

    paint.DrawStringAt(0, 180, "Altitude:", &Font20, COLORED); //Print altitude on one line and the value on the next
    paint.DrawStringAt(-10, 200, BMPAltitudeStr, &Font20, COLORED);
    paint.DrawStringAt(75,203, "m", &Font16, COLORED);

  epd.Display_Partial(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
delay(1000);
}