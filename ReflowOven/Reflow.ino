/*
This file is the top level code for the reflow oven tutorial.
Calls other functions to control the behavior of the oven in all stages of
the reflow curve

Includes libraries at the beginning for LCD and PID functionality

TempPin = pin that recieves temperature input from the thermocouple
HeaterPin = provides output to control the on/off state of the heater

Loop:
Begins with a few welcoming messages to the user
Then allows user to select reflow curve parameters via SelectMaxTemp,
  SelectSoak,SelectTime
Controls curve through use of PID controller and CoolIt, RampIt, SoakIt

Displays messages to user upon completion of reflow curve
  During curve operation: Displays current temperature and time remaining
  in reflow run

After completion messages, statistics are provided to show the agreement
  between the actual and ideal curves. These are presented in the form of 
  percent error values for the RAMP (soak) and FLOW (peak) temperatures
  
At the end of the run, the user can press SELECT to input new values
and begin another run

Also, the user can press SELECT at any point in the run to interrupt and return
to the initial welcome screen

*/
#include <LiquidCrystal.h>
#include <PID_v1.h>

// Define Pins
# define HeaterPin 2
# define TempPin 5

// Define ADC buttons
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// Initialize LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup(){
 	pinMode(TempPin, INPUT);
 	pinMode(HeaterPin, OUTPUT);
        Serial.begin(9600);
	lcd.begin(16,2);
	lcd.clear();
}

void loop(){
   lcd.clear();
   lcd.setCursor(4, 0);
   lcd.print("WELCOME!"); // Inform user that they will be inputing values
   lcd.setCursor(3,1);
   lcd.print("to 2Hot4U");
   delay(2000);
   lcd.clear();
   lcd.setCursor(5,0);
   lcd.print("Let's ");
   lcd.setCursor(2,1);
   lcd.print("Get Cookin'! ");
  delay(2000);
   for (int positionCounter = 0; positionCounter < 10; positionCounter++) {
    lcd.scrollDisplayLeft(); 
    delay(150);
  }
  lcd.clear();
  lcd.setCursor(16,0);
  lcd.print("Choose Your");
  lcd.setCursor(16,1);
  lcd.print("Reflow Curve:");
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    lcd.scrollDisplayLeft(); 
    delay(200);
  }
 
  
  lcd.clear();
  int CurTemp = (analogRead(5))*.0047/.005; //voltage reading * slope 
  char strRamp1[20] ="RAMP Temp:";
  char strRamp2[20] ="RAMP Time:";
  int RampMax = SelectMaxTemp(false,strRamp1,150,170,130);  
  int RampTime = SelectTime(strRamp2,RampMax+20,CurTemp); 
    
  int SoakTime = SelectSoak();
 
  char strFlow1[20] ="FLOW Temp:";
  char strFlow2[20] ="FLOW Time:";  
  int MaxTemp = SelectMaxTemp(true,strFlow1,245,270,218);  
  int ReflowTime = SelectTime(strFlow2,MaxTemp+20,RampMax);
  
  char strCool[20] ="COOL Time:";  
  int CoolTime = SelectTime(strCool,100,MaxTemp); 
  int TotalTime = CoolTime + ReflowTime + SoakTime + RampTime+ (int )(millis()/1000)+30;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PREHEAT STAGE");
  digitalWrite(HeaterPin,HIGH);
  int PreheatStart = millis();
  delay(30000);
  digitalWrite(HeaterPin,LOW);
  lcd.setCursor(0,0);
  lcd.print("Begin Reflow");
  lcd.setCursor(0,1);
  lcd.print("Run");

/*
while condition ensures that code won't run for more than approximately a 
minute-and-a-half more than total time
*/
while(millis()<(TotalTime+100)*1000){
  char strJustRamp[5] = "RAMP";
  int RampActual = RampIt(RampMax,RampTime,TotalTime,strJustRamp);
  int RDiff = RampMax-RampActual;
  int RDiffabs = abs(RDiff);
  lcd.clear();
  int SoakActual = SoakIt(SoakTime,TotalTime,RampMax);
  char strJustFlow[5] = "FLOW";  
  int MaxActual = RampIt(MaxTemp,ReflowTime,TotalTime,strJustFlow);
  int MDiff = MaxActual-MaxTemp;
  int MDiffabs = abs(MDiff);
  int CoolRate = CoolIt(200,CoolTime,TotalTime);
  digitalWrite(HeaterPin,LOW);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Reflow Complete!");
  delay(10000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Run Statistics:");
  lcd.setCursor(0,1);
  lcd.print("Target Temps");
  delay(5000);
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Tgt RAMP:");
  lcd.print(RampMax);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Real RAMP:");
  lcd.print(RampActual);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");
  delay(5000);
  
  lcd.setCursor(0,0);
  lcd.print("Tgt FLOW:");
  lcd.print(MaxTemp);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Real FLOW:");
  lcd.print(MaxActual);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");
  delay(5000);
  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("SOAK %Err:");
  lcd.print((RDiffabs*100)/RampMax);
  lcd.setCursor(0, 0);
  lcd.print("FLOW %Err:");
  lcd.print((MDiffabs*100)/MaxTemp);  
  delay(15000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("All Finished!");
  lcd.setCursor(0,1);
  lcd.print("Press Select");
  while(getButtonPress()!=btnSELECT){
    delay(100);
  }
}
}