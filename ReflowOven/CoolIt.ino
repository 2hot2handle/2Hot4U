/*Coolit: function that controls the cooling stage of the reflow run
Uses PID controller with set coefficients to control proportional, integral, and derivative gain values
CurTemp = current temperature. calculated using linearization between voltage reading
  and thermocouple output
TargetTemp = set point temperature (target). calculated as the slope of each ramp
  multiplied by the time elapsed in the stage and added to the starting temperature of
  the stage
Output = output of PID to heater (controls ON/OFF state)
*/
# include <PID_v1.h>

int CoolIt(int MinTemp, double Length, int TotalTime){//, int CurveStartTime){
unsigned long windowStartTime = millis();
unsigned long StartTime = millis();
//Define Variables we'll be connecting to
double TargetTemp, Output,CurTemp;
double kp = 5; //(proportional gain - difference between the current temperature and the set point)
double ki = 5; //(integral gain - proportional to the magnitude and duration of error)
double kd = 1; //(derivative gain - slope of the error over time)

 pinMode(TempPin, INPUT);
 pinMode(HeaterPin, OUTPUT);

PID myPID(&CurTemp, &Output, &TargetTemp,kp,ki,kd, DIRECT);
double StartTemp =  (analogRead(5))*.0047/.005; //voltage readin * slope +/- intercept
int WindowSize = 1000;
  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  int newMin=0;
  //lcd.clear();
  int counter = 0;
while(((millis()-StartTime)/1000)<Length){
  CurTemp =  (analogRead(5))*.0047/.005; //voltage reading * slopescale
  if(counter>1000){
  Serial.print(CurTemp);
  Serial.print('\t');
  Serial.print(millis()/1000);
  Serial.print('\n');
  counter=0;
  if(CurTemp<newMin) newMin = CurTemp;
  
  //Print Info
  lcd.setCursor(0, 1);
  lcd.print("Run Time: ");
  lcd.print(TotalTime-(millis()/1000));
  lcd.print("        ");
  lcd.setCursor(15, 1);
  lcd.print("s");
  lcd.setCursor(0, 0);
  lcd.print("COOL T: ");
  lcd.print(CurTemp);
  lcd.setCursor(13,0);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");
  }
  counter+=1;
   myPID.Compute();

/*
turn the output pin on/off based on pid output
*/
   unsigned long now = millis();
   if(now - windowStartTime>WindowSize){ //time to shift the Relay Window
     windowStartTime += WindowSize;
   }
  if(Output > now - windowStartTime) digitalWrite(HeaterPin,HIGH);
  else digitalWrite(HeaterPin,LOW);
}
return newMin;
}
