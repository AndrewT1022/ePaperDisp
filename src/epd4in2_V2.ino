//This is our actual code page. The rest of the files are either configuration or libaries. =)
    //ESP32C3 powered battery weather station using an E-ink display (think Kindle screen) and two climate sensors, an AHT21 and a BMP280. 
    //We are using I2C for both the AHT21 and the BMP280, and SPI for the e-ink screen

/*       Copyright for the e-ink display libraries:
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

//Libraries!

#include <SPI.h>
#include "epd4in2_V2.h"
#include "epdpaint.h"
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>

unsigned char image[9000]; //Buffer size!
Paint paint(image, 400, 600); //Width should be the multiple of 8
Epd epd; //Define the display

Adafruit_BMP280 bmp; //Initialize the BMP280 sensor
Adafruit_AHTX0 aht; //Initialize the AHT21 sensor
int buttonPin = 5; //Assign button pin
bool toggleState = false; //Initial button toggle state
auto lastButtonState = HIGH; //Default the last button state to high

void setup() {

  Serial.begin(9600); //Begin serial for debugging (turn off if not needed, saves power)
  
     bmp.setSampling(Adafruit_BMP280::MODE_FORCED,      // Set mode to forced, lowest power.
                  Adafruit_BMP280::SAMPLING_X1,        // Set oversampling for pressure to 1, lowest power.
                  Adafruit_BMP280::SAMPLING_X1,       // Set oversampling for temperature to 1, lowest power.
                  Adafruit_BMP280::FILTER_OFF,       // Turn off noise filtering, lowest power.
                  Adafruit_BMP280::STANDBY_MS_250); // Compromise made here to give us the best of both worlds in response time and power
  
  //These configurations were found from the BMP280 documentation! https://cdn-shop.adafruit.com/datasheets/BST-BMP280-DS001-11.pdf , table 15
  
  bmp.begin(); //Start the bmp
  aht.begin(); //Start the aht
pinMode(buttonPin, INPUT); //Define button pin as an input

  if (epd.Init() != 0) { //Start the screen, but hault the code if it fails
    Serial.print("e-Paper init failed");
    return;
  }

epd.Clear(); //Clear the display
paint.SetWidth(250); //Set the canvas size
paint.SetHeight(280);
paint.Clear(1); //Clear the canvas
}

void loop() {
  
//Button code!
auto buttonState = digitalRead(buttonPin); //Read button status
if (buttonState == LOW && lastButtonState == HIGH){
  toggleState = !toggleState;              //Toggle state if button high
}
lastButtonState=buttonState;               //Set last button state

sensors_event_t humidity, temp; //I'm not completely sure why we have to do this. Don't care. It works.
aht.getEvent(&humidity, &temp); //Get the sensor event and put it in a variable
float AHTTemp = temp.temperature; //*C
float AHTTempF = ((temp.temperature)*(1.8)+32); //*F
float AHTHumidity = humidity.relative_humidity; //%
float BMPTemp = bmp.readTemperature(); //*C
float BMPPressure = bmp.readPressure(); //PA
float BMPAltitude = bmp.readAltitude(1033.8); //in millibars <------- PUT CURRENT LOCAL PRESSURE HERE ---------------------
float BMPAltitudeF = BMPAltitude*3.28084; //meters
float BMPPressureP = BMPPressure*0.295299802; //inHg

//Storage for the double to char conversion (screen wants char)
  char AHTTempStr[10];      //Char with width 10
  char AHTTempStrF[10];     //Char with width 10
  char AHTHumidityStr[10];  //Char with width 10
  char BMPTempStr[10];      //Char with width 10
  char BMPPressureStr[10];  //Char with width 10
  char BMPPressureStrP[10]; //Char with width 10
  char BMPAltitudeStr[10];  //Char with width 10
  char BMPAltitudeStrF[10]; //Char with width 10

//Convert doubles to const chars because that's what the screen input command wants
  dtostrf(AHTTemp, 3, 1, AHTTempStr);  // 3 width, 1 decimal place
  dtostrf(AHTTempF, 3, 1, AHTTempStrF);
  dtostrf(AHTHumidity, 4, 2, AHTHumidityStr);
  dtostrf(BMPTemp, 3, 1, BMPTempStr);
  dtostrf(BMPPressure / 1000.0, 6, 2, BMPPressureStr);  // Convert Pa to kPa
  dtostrf(BMPPressureP / 1000.0, 6, 2, BMPPressureStrP);
  dtostrf(BMPAltitude, 6, 2, BMPAltitudeStr);
  dtostrf(BMPAltitudeF, 6, 2, BMPAltitudeStrF);
  
    paint.SetWidth(250);
    paint.SetHeight(280);
    paint.Clear(1);

//Button used to toggle between metric and imperial

//Metric section
    if (toggleState){ 
    paint.DrawStringAt(0, 0, "Temperature:", &Font20, 0); //Print temperature on one line and the value on the next
    paint.DrawStringAt(0, 20, AHTTempStr, &Font20, 0);
    paint.DrawStringAt(60,22, "*C", &Font16, 0); //Print unit next to text

    paint.DrawStringAt(0, 120, "Pressure:", &Font20, 0); //Print pressure on one line and the value on the next
    paint.DrawStringAt(0, 140, BMPPressureStr, &Font20, 0);
    paint.DrawStringAt(100,143, "kPA", &Font16, 0); //Print unit next to text

    paint.DrawStringAt(0, 180, "Altitude:", &Font20, 0); //Print altitude on one line and the value on the next
    paint.DrawStringAt(-10, 200, BMPAltitudeStr, &Font20, 0);
    paint.DrawStringAt(75,203, "m", &Font16, 0); //Print unit next to text
    }

//Imperial section
    else{
    paint.DrawStringAt(0, 0, "Temperature:", &Font20, 0); //Print temperature on one line and the value on the next
    paint.DrawStringAt(0, 20, AHTTempStrF, &Font20, 0);
    paint.DrawStringAt(60,22, "*F", &Font16, 0); //Print unit next to text

    paint.DrawStringAt(0, 120, "Pressure:", &Font20, 0); //Print pressure on one line and the value on the next
    paint.DrawStringAt(0, 140, BMPPressureStrP, &Font20, 0);
    paint.DrawStringAt(85,143, "inHg", &Font16, 0); //Print unit next to text

    paint.DrawStringAt(0, 180, "Altitude:", &Font20, 0); //Print altitude on one line and the value on the next
    paint.DrawStringAt(-10, 200, BMPAltitudeStrF, &Font20, 0);
    paint.DrawStringAt(90,203, "ft", &Font16, 0); //Print unit next to text
    }
   
    paint.DrawStringAt(0, 60, "Humidity:", &Font20, 0); //Print humidity on one line and the value on the next
    paint.DrawStringAt(0, 80, AHTHumidityStr, &Font20, 0); 
    paint.DrawStringAt(70,82, "%", &Font16, 0); //Print unit next to text


//Credits
paint.DrawStringAt(0, 245, "Andrew Thomas,", &Font16, 0); //Add credits at the bottom
paint.DrawStringAt(0, 265, "Kyle Davis", &Font16, 0); 
  epd.Display_Partial(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight()); //Push paint to display
  epd.Sleep();

esp_sleep_enable_timer_wakeup(6000000); //Sleep for 60s
esp_light_sleep_start();

}
