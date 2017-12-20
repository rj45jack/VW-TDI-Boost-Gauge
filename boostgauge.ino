/* 
Digital Gauge by rj45jack.
This software uses a HD44780 display, 16x2. Sensors vary.
Contact killabytes.net
Created November 2009 
Edited March 7th 2010
*/
#include <LiquidCrystal.h> //includes the liquid crystal library
int sensor = 0; //Sets the sensor input to pin 0
int boost = 0; //sets the boost to 0
int sensorval = 0; //sets sensor value to 0
int digital = 9; // sets pins to digital inputs
int button = 0; // sets button to 0
int buttonstate = 0; // sets buttonstate to 0
float math; // sets math to a floating unit
float vs = 5.1; // sets vs to a floating unit 
float a = 0.0048828; // sets a to floating unit
float b = 0.04; // sets b to a floating unit
float c = .00369; // sets c to a foloating unit
int bargraph; // sets bargraph as a interger
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //sets the pins for the LCD

 void setup() {
  lcd.begin(16,2); //sets the LCD to 16x2 row/coloumns
  pinMode(digital, INPUT); 
  lcd.noCursor(); //disables the cursor
  lcd.setCursor(0,0); //sets cursor to position
   lcd.print("Digital Gauge By"); //prints data to screen
  lcd.setCursor(3,1); //moves cursor to position
  lcd.print("rj45jack"); //prints data to screen
  delay(3000); //waits 2 seconds for start up screen
  lcd.clear(); //clears the LCD
  lcd.setCursor(5,0); // moves the cursor
  lcd.print("Visit:"); // prints information to the screen
  lcd.setCursor(1,1); // moves the cursor
  lcd.print("Killabytes.net");
  delay(4000); // waits 4 seconds
  }
void loop() { // starts the boost sensor loop
 button = digitalRead(digital); // reads the digital input for hi/lo
 lcd.clear();  // clears the screen
   lcd.setCursor(0,0); //sets the cursor position
  lcd.print("VNT15:"); //prints data to screen
  
   // lcd.setCursor(0,1); // moves the cursor for diagnostics
  // lcd.setCursor(2,1); // moves the cursor
  // lcd.print("Sensor:"); //used for diagnostic purposes, hows voltage
  //lcd.print("EGT:"); // prints static data
  //lcd.setCursor(11,1); // moves the cursor
  // lcd.setCursor(13,1); // sets cursor for diagnostics
  //lcd.print((char)223); // prints the degree symbol to the screen
  //lcd.setCursor(12,1); // moves the cursor
  //lcd.print("C"); // prints the celcius
  // lcd.print("V"); //used for diagnostic purposes, hows voltage
 
  if (button == LOW && buttonstate == LOW){ //checks the state of the button input
     math = (float)0.145; //changes math to a float of 0.145 if it's high
       lcd.setCursor(11,0); //sets cursor position
       lcd.print("PSI"); //prints psi to screen  
  }
   else if (button == HIGH && buttonstate == LOW){
     math = (float)10; //if the input is low changes math to 1
     lcd.setCursor(12,0); //sets cursor position
     lcd.print("mBar"); //prints kpa to screen    
  }
   else if (button == LOW && buttonstate == HIGH){
     math = (float)10; //if the input is low changes math to 1
     lcd.setCursor(12,0); //sets cursor position
     lcd.print("mBar"); //prints kpa to screen    
  }
    if (button == HIGH && buttonstate == HIGH){ //checks the state of the button input
     math = (float)0.145; //changes math to a float of 0.145 if it's high
       lcd.setCursor(11,0); //sets cursor position
       lcd.print("PSI"); //prints psi to screen  
  }
    sensorval = analogRead(sensor); //Polls the sensor for data
   // boost = ((((((sensorval*0.0048828)/5.1)-.04)/.00369)*(float)math)+1);  //converts sensorval into a usable PSI value
  boost = ((((((sensorval*(float)a)/(float)vs)-(float)b)/(float)c)*(float)math)+1);
     lcd.setCursor(7,0); //sets the cursor position
     if (boost <1){
       boost = 0;}
     lcd.print(boost); //prints boost data
     
     //This entire set is for the bargraph
     lcd.setCursor(0,1);  // sets the cursor
     lcd.print("B:"); // prints "B" to the screen to indicate boost
     bargraph = ((sensorval/5)/10); // the math for the bargraph
    //lcd.print(bargraph); //Diagnostic line for the bar graph

       if (bargraph == 1){
       lcd.setCursor(2,1);
       lcd.print(">");}
       
       if (bargraph == 2){
       lcd.setCursor(2,1);
       lcd.print(">>");}
       
       if (bargraph == 3){
       lcd.setCursor(2,1);
       lcd.print(">>>");}
       
       if (bargraph == 4){
       lcd.setCursor(2,1);
       lcd.print(">>>>");}
       
       if (bargraph == 5){
       lcd.setCursor(2,1);
       lcd.print(">>>>>");}
       
       if (bargraph == 6){
       lcd.setCursor(2,1);
       lcd.print(">>>>>>");}
       
       if (bargraph == 7){
       lcd.setCursor(2,1);
       lcd.print(">>>>>>>");}
       
       if (bargraph == 8){
       lcd.setCursor(2,1);
       lcd.print(">>>>>>>>");}
       
       if (bargraph == 9){
       lcd.setCursor(2,1);
       lcd.print(">>>>>>>>>");}
       
       if (bargraph == 10){
       lcd.setCursor(2,1);
       lcd.print(">>>>>>>>>>");}
       
       if (bargraph == 11){
       lcd.setCursor(2,1);
       lcd.print(">>>>>>>>>>>");}
       
       if (bargraph == 12){
       lcd.setCursor(2,1);
       lcd.print(">>>>>>>>>>>>");}
      
       if (bargraph == 13){
       lcd.setCursor(2,1);
       lcd.print(">>>>>>>>>>>>>");}
       
       //this line is a joke, not ment for production.
       //if (bargraph > 13){
       //lcd.setCursor(0,1);
       //lcd.print("Manifold Damage!");}
   
   //lcd.setCursor(0,1); // this is a diagnostic line to show the input from the sensor
  //lcd.print(sensorval*0.004883);
  //lcd.setCursor(7,1); // moves the cursor
  //lcd.print("XXX"); //prints the value of the EGT
  
  delay(200);//slows the refresh rate of the loop, reduces ghosting
}
