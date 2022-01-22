/*
    ArduClock V1.1 by Vnon246
    Copyright (C) 2021  Vnon246

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

//Network Libraries

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <WiFiManager.h>

ESP8266WebServer web_ui(80); //Local web server for web GUI
WiFiManager net_portal; //WiFi manager initialization

String api_url = "https://maker.ifttt.com/trigger/{event}/with/key/{webhooks_key}"; //IFTTT API URL

//RTC Libraries

#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 d_rtc;

//LCD libraries

#include <LiquidCrystal.h>
LiquidCrystal d_lcd(D0,D3,D4,D5,D6,D7);

//Definition of variables for SmartSwitch

int sw_out = D8; //Pin for SmartSwitch sensor
int sw_state = 0; //Auxiliar variable to store the state of SmartSwitch sensor
int sw_mode = 1; //Auxiliar variable to store the mode of SmartSwitch sensor

//Auxiliar variable for device mode selection

int d_mode = 1;

//Icons for the differents mode of the device

byte temp_icon[] = { //Temperature mode
  B01110,
  B10001,
  B10001,
  B01010,
  B01010,
  B01010,
  B01010,
  B00100
};
byte time_icon[] = { //Time mode
  B11111,
  B10001,
  B10101,
  B10101,
  B11101,
  B10001,
  B10001,
  B11111
};
byte date_icon[] = { //Date mode
  B10101,
  B10101,
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111
};

void setup() {

  //Device initialization

  Wire.begin(); //Initialize the i2c communication for RTC Module
  Serial.begin(115200); //Initialize the serial port for debugging purposes
  d_lcd.begin(16, 2); //Initialize the device screen

  //Initialization/Creation of the icons for the different device modes

  d_lcd.createChar(1, temp_icon);
  d_lcd.createChar(2, time_icon);
  d_lcd.createChar(3, date_icon);

  //Welcome splash screen

  welcome_screen();

  delay(2000);

  d_lcd.clear();

}

void loop() {

  web_ui.handleClient();

  //SmartSwitch mode 1
  
  if (sw_mode == 1){

   //System mode selector

  sw_state = digitalRead(sw_out); //Assignation of sw_state variable

  if (sw_state == 1) {

    delay(500);

    if (d_mode == 1) {

      d_mode = 2;

    } else if (d_mode == 2) {

      d_mode = 3;

    } else if (d_mode == 3) {

      d_mode = 1;

    }

  }

  //SmartSwitch mode 2
  
  }else if(sw_mode == 2){

    sw_state = digitalRead(sw_out); //Assignation of sw_state variable

    //Alarm trigger
  
    if (sw_state == 1) {

        HTTPClient net_alarm; //Create an HTTP client
        net_alarm.begin(api_url); //Begin HTTP communication with IFTTT API
        
        delay(1000);
      
        net_alarm.GET(); //Make an HTTP GET request to the IFTTT API
        net_alarm.end(); //Finish the HTTP communication
         
      }
       
  }

  //System function selector

  switch (d_mode) {

    case 1:

      time_mode();

      break;

    case 2:

      date_mode();

      break;

    case 3:

      temp_mode();

      break;

  }

}

//[SYSTEM FUNCTIONS]


//Welcome splash screen

void welcome_screen() {
  
  d_lcd.setCursor(3, 0);
  d_lcd.print("ArduClock");
  d_lcd.setCursor(6, 1);
  d_lcd.print("1.1");

  delay(3000);

  net_setup();

}

//Time mode function

void time_mode() {

  d_lcd.clear();

  DateTime now = d_rtc.now();

  d_lcd.setCursor(2, 0);
  d_lcd.print("ArduClock V1");

  d_lcd.setCursor(3, 1);
  d_lcd.write(2);

  d_lcd.setCursor(4, 1);
  d_lcd.print(now.hour());
  d_lcd.print(":");
  d_lcd.print(now.minute());
  d_lcd.print(":");
  d_lcd.print(now.second());

}

//Date mode function

void date_mode() {

  d_lcd.clear();

  DateTime now = d_rtc.now();

  d_lcd.setCursor(2, 0);
  d_lcd.print("ArduClock V1");

  d_lcd.setCursor(3, 1);
  d_lcd.write(3);

  d_lcd.setCursor(4, 1);
  d_lcd.print(now.day());
  d_lcd.print("/");
  d_lcd.print(now.month());
  d_lcd.print("/");
  d_lcd.print(now.year());

}


//Temperature mode function

void temp_mode() {

  d_lcd.clear();

  d_lcd.setCursor(2, 0);
  d_lcd.print("ArduClock V1");

  d_lcd.setCursor(4, 1);
  d_lcd.write(1);

  d_lcd.setCursor(5, 1);
  d_lcd.print(d_rtc.getTemperature());

}


//[NETWORK SETUP FUNCTIONS]


//Initial network setup function

void net_setup(){

  d_lcd.clear();

  d_lcd.setCursor(2, 0);
  d_lcd.print("ArduClock V1");
  d_lcd.setCursor(0, 1);
  d_lcd.print("Waiting for WiFi network...");

  net_portal.autoConnect("ArduClock V1" , "ArduClock"); //Create the captive portal and WiFi network

  delay(2000);

  d_lcd.clear();
  d_lcd.setCursor(2, 0);
  d_lcd.print("ArduClock V1");
  d_lcd.setCursor(1, 1);
  d_lcd.print("WiFi connected");

  delay(2000);

  d_lcd.clear();
  d_lcd.setCursor(2, 0);
  d_lcd.print("ArduClock V1");
  d_lcd.setCursor(2, 1);
  d_lcd.print(WiFi.localIP()); //Show local IP for web-GUI

  web_ui.on("/", webpanel_show); //Web-GUI routing
  
  web_ui.on("/time", webpanel_time);
  web_ui.on("/date", webpanel_date);
  web_ui.on("/temp", webpanel_temp);
  web_ui.on("/reset", net_reset);
  
  web_ui.on("/on", webpanel_alarmon);
  web_ui.on("/off", webpanel_alarmoff);
  
  web_ui.begin(); 

  delay(2000);
 
}

//Network reset function

void net_reset(){

  net_portal.resetSettings();
  delay(2000);
  net_setup();
  
}


//[WEB-GUI FUNCTIONS]


//Web-GUI data function 

String webpanel_data(){

 
  String webpanel=
  
   "<!DOCTYPE html>"
   "<html>"
   "<head>"
   "<meta charset='utf-8'>"
   "<style>"
   "body{"
   "text-align: center;"
   "font-family: 'Courier New', Courier, monospace;"
   "}"
   "header{"
   "border: 2px solid black;"
   "border-radius: 4px;"
   "background-color: whitesmoke;"
   "}"
   "footer{"
   "margin: 60px;"
   "}"
   ".version_header{"
   "font-size: small;"
   "color: black;"
   "text-align: right;"
   "}"
   ".state_header{"
   "text-align: left;"
   "font-size:small;"
   "}"
   ".control_header{"
   " font-size: large;"
   "}"
   ".on_bt{"
   " text-decoration: none;"
   " font-size: x-large;"
   " border: 2px solid black;"
   " border-radius: 10px;"
   " background-color: rgba(0, 128, 0, 0.63);"
   " color: black;"
   " margin:50px;"
   "}"
   ".off_bt{"
   " text-decoration: none;"
   " font-size: x-large;"
   " border: 2px solid black;"
   " border-radius: 10px;"
   " background-color: rgba(128, 0, 0, 0.63);"
   " color: black;"
   " margin:50px;"
   "}"
   ".time_bt{"
   " text-decoration: none;"
   " font-size: x-large;"
   " border: 2px solid black;"
   " border-radius: 10px;"
   " background-color: rgba(128, 115, 0, 0.63);"
   " color: black;"
   " margin:50px;"
   " }"
   ".date_bt{"
   " text-decoration: none;"
   " font-size: x-large;"
   " border: 2px solid black;"
   " border-radius: 10px;"
   " background-color: rgba(0, 128, 90, 0.63);"
   " color: black;"
   " margin:50px;"
   "}"
   ".temp_bt{"
   " text-decoration: none;"
   " font-size: x-large;"
   " border: 2px solid black;"
   " border-radius: 10px;"
   " background-color: rgba(0, 70, 128, 0.63);"
   " color: black;"
   " margin:50px;"
   " }"
   ".reset_bt{"
   " text-decoration: none;"
   " font-size: x-large;"
   " border: 2px solid black;"
   " border-radius: 10px;"
   " background-color: rgba(68, 128, 0, 0.63);"
   " color: black;"
   " margin:50px;"
   " }"
   "</style>"
   "</head>";
   
   webpanel+= "<body>";
   webpanel+="<header>";
   webpanel+="<img src='https://xtinformatica2.000webhostapp.com/images/ArduClock_logo.png'>";
   webpanel+="</header>";
   webpanel+="<article>";
   webpanel+="<br/>";
   webpanel+="<br/>";

   if(sw_mode == 1){

    webpanel+="<h2 class='state_header'> Alarm status:OFF</h2>";

    
   }else{

    webpanel+="<h2 class='state_header'> Alarm status:ON</h2>";

   }

    switch (d_mode) {

    case 1:

      webpanel+="<h2 class='state_header'> Mode selected:TIME</h2>";
      break;

    case 2:

      webpanel+="<h2 class='state_header'> Mode selected:DATE</h2>";
      break;

    case 3:

      webpanel+="<h2 class='state_header'> Mode selected:TEMPERATURE</h2>";
      break;

  }
  
   webpanel+="<br/>";
   webpanel+="<br/>";
   webpanel+="<h3 class='control_header'> MODES </h3>";
   webpanel+="<br/>";
   webpanel+="<a href='/date' class='date_bt'>DATE</a>";
   webpanel+="<a href='/time' class='time_bt'>TIME</a>";
   webpanel+="<a href='/temp' class='temp_bt'>TEMP</a>";
   webpanel+="<a href='/reset' class='reset_bt'>RESET</a>";
   webpanel+="<br/>";
   webpanel+="<br/>";
   webpanel+="<br/>";
   webpanel+="<h3 class='control_header'>ALARM</h3>";
   webpanel+="<br/>";
   webpanel+="<a href='/on' class='on_bt'>ON</a>";
   webpanel+="<a href='/off' class='off_bt'>OFF</a>";
   webpanel+="<br/>";
   webpanel+="<br/>";
   webpanel+="<br/>";
   webpanel+="</article>";
   webpanel+="<footer>";
   webpanel+="<img src='https://www.gnu.org/graphics/gplv3-with-text-84x42.png'/>";
   webpanel+="<h2 class='version_header'> V1.1 </h2>";
   webpanel+="</footer>";
   webpanel+="</body>";
   webpanel+="</html>";

   return webpanel;
}

//Web-GUI show function

void webpanel_show(){

  web_ui.send(200,"text/html",webpanel_data());
  
}

//Web-GUI mode selectors

void webpanel_time(){

 d_mode = 1;
 delay(500);
 webpanel_show();
 
}
void webpanel_date(){

 d_mode = 2;
 delay(500);
 webpanel_show();
  
}
void webpanel_temp(){

 d_mode = 3;
 delay(500);
 webpanel_show();
 
}

//Web-GUI alarm selectors

void webpanel_alarmon(){

 sw_mode = 2;
 delay(500);
 webpanel_show();
  
}
void webpanel_alarmoff(){

 sw_mode = 1;
 delay(500);
 webpanel_show();
  
}


  
  
