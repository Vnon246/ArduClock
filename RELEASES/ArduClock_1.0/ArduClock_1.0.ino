/*
    ArduClock V1 by Vnon246
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


//RTC Libraries

#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 d_rtc;

//LCD libraries

#include <LiquidCrystal.h>
LiquidCrystal d_lcd(D0,D3,D4,D5,D6,D7);

//Definition of variables for SmartSwitch

int sw_out = D8; //Pin for SmartSwitch button
int sw_state = 0; //Auxiliar variable to store the state of SmartSwitch button

//Auxiliar variable for mode selection

int mode = 1;


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

  //System mode selector

  sw_state = digitalRead(sw_out); //Assignation of sw_state variable

  if (sw_state == 1) {

    delay(500);

    if (mode == 1) {

      mode = 2;

    } else if (mode == 2) {

      mode = 3;

    } else if (mode == 3) {

      mode = 1;

    }

  }

  //System function selector

  switch (mode) {

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

  DateTime now = d_rtc.now();

  d_lcd.setCursor(3, 0);
  d_lcd.print("ArduClock");
  d_lcd.setCursor(6, 1);
  d_lcd.print("1.O");

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

  DateTime now = d_rtc.now();

  d_lcd.clear();

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
