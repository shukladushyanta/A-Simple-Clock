//Project Name: A Simple Clock
//Project Author: Dushyanta Shukla
//Date: 24/05/2021

//including the RTC library
#include "RTClib.h"

//defining the RTC module
RTC_DS1307 rtc;

//Control pins for shift register
#define DATA 6
#define CLOCK 7
#define CLEAR 8

//Control pins for Anode of 7 segment display
#define DIGIT1 5
#define DIGIT2 4
#define DIGIT3 3
#define DIGIT4 2

//Control pins for DOTS, AM, PM, and Date indicator
#define DOTS 9
#define PM 10
#define DATE 11
#define AM 12

//Pins for Push-buttons
#define BUTTON1 A0
#define BUTTON2 A1
#define BUTTON3 A2

//Defining the variable for storing data coming from RTC module
DateTime now;


//Variable declearation
//AMPM_state is 1 for PM and 0 for AM
//button _state is 0 when button1 is not pressed, 1 once it is pressed
byte Hours, Mins, AMPM_state = 0, button_state = 0;

//Flag variables to measure elapsed time after last date or time was displayed
long TimeCounter, DateCounter, ButtonCounter;

//Encoded values of shift register pins for numerical values 0 to 9
byte digits[10] = {126, 48, 109, 121, 51, 91, 95, 112, 127, 123};




void setup() {

  //Pin type decleration
  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(CLEAR, OUTPUT);

  pinMode(DIGIT4, OUTPUT);
  pinMode(DIGIT3, OUTPUT);
  pinMode(DIGIT2, OUTPUT);
  pinMode(DIGIT1, OUTPUT);

  pinMode(DOTS, OUTPUT);
  pinMode(AM, OUTPUT);
  pinMode(PM, OUTPUT);
  pinMode(DATE, OUTPUT);

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);


  //CLEAR pin is active LOW. Deactivating it
  digitalWrite(CLEAR, HIGH);

  //DIGIT pins are active low. Deactivating them
  digitalWrite(DIGIT1, HIGH);
  digitalWrite(DIGIT2, HIGH);
  digitalWrite(DIGIT3, HIGH);
  digitalWrite(DIGIT4, HIGH);


  //RTC starts working
  rtc.begin();

  //Assigning value to DateCounter variable
  DateCounter = millis();
}





void loop() {

  //Function for dispalying date
  dateDisplay();          

  //Funtion for updating the time value coming from RTC
  segmentDisplay((Hours * 100) + Mins, 0000, 1);       


  //Displaying time with DOTS turned ON for 1 seconds
  TimeCounter = millis();
  while (millis() < TimeCounter + 1000)         
  {
    digitalWrite(DOTS, LOW);
    buttonCheck();
    digitalWrite(DOTS, HIGH);     //DOTS are turned on
    segmentDisplay((Hours * 100) + Mins, 0000, 0);
  }



  //Funtion for updating the time value coming from RTC
  segmentDisplay((Hours * 100) + Mins, 0000, 1);
  //Turning DOTS OFF
  digitalWrite(DOTS, LOW);            

  
  
  //Displaying time when DOTS are turned OFF for 1 seconds
  TimeCounter = millis();
  while (millis() < TimeCounter + 1000)         
  {
    buttonCheck();
    segmentDisplay((Hours * 100) + Mins, 0000, 0);
  }

}







//Functions for printing values on 7-segment Display
void segmentDisplay(int inputValues, int pinState, byte timeUpdate)
{

  //Local Variable Declearation
  byte value4, value3, value2, value1;
  byte pinState1, pinState2, pinState3, pinState4;



  //Extracting value of individual digits and its state from received values
  value4 = inputValues % 10;
  pinState4 = (pinState) % 10;

  //Sending encoded value of digit's numerical value from the array, to Shift Register
  shiftOut(DATA, CLOCK, MSBFIRST, digits[value4]);
  digitalWrite(DIGIT4, pinState4);

  //RTC takes 1ms to transfer the time values to microcontroller. This casuses flickering in the display
  //Using this 1ms as a delay for display to prevent flickering
  //Doing this only when we want to update time otherwise. Working normally
  if (timeUpdate == 1)
  {
    now = rtc.now();
    delay(2);
  }
  else
    delay(3);
  digitalWrite(DIGIT4, HIGH);


  //Extracting value of individual digits and its state from received values
  value3 = (inputValues / 10) % 10;
  pinState3 = (pinState / 10) % 10;

  //Sending encoded value of digit's numerical value from the array, to Shift Register
  shiftOut(DATA, CLOCK, MSBFIRST, digits[value3]);
  digitalWrite(DIGIT3, pinState3);
  delay(3);
  digitalWrite(DIGIT3, HIGH);


  value2 = (inputValues / 100) % 10;
  pinState2 = (pinState / 100) % 10;
  shiftOut(DATA, CLOCK, MSBFIRST, digits[value2]);
  digitalWrite(DIGIT2, pinState2);
  delay(3);
  digitalWrite(DIGIT2, HIGH);



  value1 = (inputValues / 1000);
  pinState1 = (pinState / 1000);

  //A check statement to disable the first digit if it is zero
  if (value1 != 0)
  {
    shiftOut(DATA, CLOCK, MSBFIRST, digits[value1]);
    digitalWrite(DIGIT1, pinState1);
    delay(3);
    digitalWrite(DIGIT1, HIGH);
  }



  //If time value is updated, then refresh our code's global variable
  if (timeUpdate == 1)
  {
    Hours = now.hour();
    Mins = now.minute();

    //To check AM/PM and turn on their respective indictors
    //To convert 24 hour clock to 12 hour
    if (Hours >= 12)
    {
      Hours -= 12;
      AMPM_state = 1;
      digitalWrite(PM, HIGH);
      digitalWrite(AM, LOW);
    }
    else if (Hours < 12)
    {
      AMPM_state = 0;
      digitalWrite(PM, LOW);
      digitalWrite(AM, HIGH);
    }


    //If zero in 24 hour clock, then make it 12
    if (Hours == 0)
      Hours = 12;
  }

}










//Function for displaying date
void dateDisplay()
{
  //Checkig whether 10 seconds have elapsed after last date was dispalayed
  if (millis() > (DateCounter + 10000))
  {
    DateCounter = millis();

    //Turning the AM,PM indicator off and Date indicator On
    digitalWrite(AM, LOW);
    digitalWrite(PM, LOW);
    digitalWrite(DATE, HIGH);

    //Getting today's date
    byte CurrentDate = now.day();
    byte CurrentMonth = now.month();

    //Dispalying date for a period of 3 seconds
    while (millis() < (DateCounter + 3000))
    {
      segmentDisplay((CurrentDate * 100) + CurrentMonth, 0000, 0);
    }

    //Turning OFF the date indicator and turning ON the AM/PM indicator
    digitalWrite(DATE, LOW);

    if (AMPM_state == 1)
      digitalWrite(PM, HIGH);
    else
      digitalWrite(AM, HIGH);

    DateCounter = millis();
  }
}




//Function for checking whether any button is being pressed or not
void buttonCheck()
{
  //If BUTTON2 or BUTTON3 is being pressed, display the date
  if ((digitalRead(BUTTON2) == LOW || digitalRead(BUTTON3) == LOW) && button_state == 0)
  {
    //10 seconds are removed from DateCounter variable because 
    //the dateDispaly() function has if condition, which is true when
    //this counter is 10 secconds less than the millis() function
    DateCounter -= 10000;
    dateDisplay();
  }

  //If BUTTON1 is pressed, start the ButtonCounter variable
  //This is done to check whether this button is pressed for more or less than 5 seconds 
  else if (digitalRead(BUTTON1) == LOW && button_state == 0)
  {
    ButtonCounter = millis();
    button_state = 1;
  }

  else if (digitalRead(BUTTON1) == LOW && button_state == 1)
  {
    //If the button is pressed more than 5 seconds, enter the settings mode
    if (millis() > (ButtonCounter + 5000))
      dateChange();
  }

  //If BUTTON1 is not being pressed anymore
  else if (digitalRead(BUTTON1) == HIGH && button_state == 1)
  {
    //If the button was pressed for more than 5 seconds, enter the settings mode
    if (millis() > (ButtonCounter + 5000))
      dateChange();
    //Otherwise diplay the current date
    else
    {
      button_state = 0;
      DateCounter-=10000;
      dateDisplay();
    }
  }
}



void dateChange()
{
  ButtonCounter = millis();

  //Temporary variable when clock is in settings mode
  byte set_hour, set_minute, set_date, set_month, set_state;
  //Because values of years are out of bound of byte datatype
  int set_year;

  //Putting current time values in temporary variables to display on setting mode's startup
  set_hour = now.hour();
  set_minute = now.minute();
  set_date = now.day();
  set_month = now.month();
  set_year = now.year();;
  set_state = 0;

  //Turning AM, PM and date indicator ON to signify clock is in the settings mode
  digitalWrite(AM, HIGH);
  digitalWrite(PM, HIGH);
  digitalWrite(DATE, HIGH);

  //Loop will be executed untill 10 seconds of inactivity
  //When any button is pressed, it restarts
  while (millis() < (ButtonCounter + 10000))
  {
    //If BUTTON1 is pressed, change the time variable being adjusted
    if (digitalRead(BUTTON1) == LOW)
    {
      //Reset the ButtonCounter variable to again start counting 10 seconds of inactivity
      ButtonCounter = millis();
      set_state++;

      if (set_state == 5)
        set_state = 0;
    }

    //BUTTON2 will increment the values
    if (digitalRead(BUTTON2) == LOW)
    {
      //Reset the ButtonCounter variable to again start counting 10 seconds of inactivity
      ButtonCounter = millis();

      //switch case to increment the time variable which is selected
      switch (set_state)
      {
        case 0:
          set_hour++;
          if (set_hour == 24)
            set_hour = 0;
          break;

        case 1:
          set_minute++;
          if (set_minute == 60)
            set_minute = 0;
          break;

        case 2:
          set_date++;
          if (set_date == 32)
            set_date = 1;
          break;

        case 3:
          set_month++;
          if (set_month == 13)
            set_month = 1;
          break;

        case 4:
          set_year++;
          break;
      }
    }

    //BUTTON3 will decrement the values
    else if (digitalRead(BUTTON3) == LOW)
    {
      //Reset the ButtonCounter variable to again start counting 10 seconds of inactivity
      ButtonCounter = millis();

      //Switch case to decrement the time variable which is selected
      switch (set_state)
      {
        case 0:
          set_hour--;
          if (set_hour == -1)
            set_hour = 23;
          break;

        case 1:
          set_minute--;
          if (set_minute == -1)
            set_minute = 59;
          break;

        case 2:
          set_date--;
          if (set_date == 0)
            set_date = 31;
          break;

        case 3:
          set_month--;
          if (set_month == 0)
            set_month = 12;
          break;

        case 4:
          set_year--;
          break;
      }
    }


    //If any button is pressed, this produced a delay of 250 ms
    //This is done to match reaction time of Arduino to that of a human
    while (millis() < (ButtonCounter + 250))
    {
      if (set_state == 0 || set_state == 1)
        segmentDisplay((set_hour * 100) + set_minute, 0000, 0);
      else if (set_state == 2 || set_state == 3)
        segmentDisplay((set_date * 100) + set_month, 0000, 0);
      else
        segmentDisplay(set_year, 0000, 0);
    }

    //Turn OFF the digits which are displaying the selected variable
    //This creats a blinking effect in the variable being changed
    if ((millis() % 1000) < 500)
    {
      if (set_state == 0)
        segmentDisplay((set_hour * 100) + set_minute, 1100, 0);
      else if (set_state == 1)
        segmentDisplay((set_hour * 100) + set_minute, 11, 0);
      else if (set_state == 2)
        segmentDisplay((set_date * 100) + set_month, 1100, 0);
      else if (set_state == 3)
        segmentDisplay((set_date * 100) + set_month, 11, 0);
      else if (set_state == 4)
        segmentDisplay(set_year, 1111, 0);
    }
    
    else if ((millis() % 1000) > 500)
    {
      if (set_state == 0 || set_state == 1)
        segmentDisplay((set_hour * 100) + set_minute, 0000, 0);
      else if (set_state == 2 || set_state == 3)
        segmentDisplay((set_date * 100) + set_month, 0000, 0);
      else if (set_state == 4)
        segmentDisplay(set_year, 0000, 0);
    }
  }

  //Once clock is out of setttings mode, turn OFF the AM, PM, and date indicator
  digitalWrite(AM, LOW);
  digitalWrite(PM, LOW);
  digitalWrite(DATE, LOW);

  //Update time values in RTC with the chosen value of user
  rtc.adjust(DateTime(set_year, set_month, set_date, set_hour, set_minute, 0));

  //Give a delay of 2 ms for above line of code to take effect
  delay(2);

  //Refresh the time values in the 7 segment diplay variables
  segmentDisplay((set_hour * 100) + set_minute, 0000, 1);
  //Reset the button 1 state to zero
  button_state = 0;
  //This is done so that after time is changed, date is not displayed immidiately
  //Done for look, not a necessity
  DateCounter=millis();
}
