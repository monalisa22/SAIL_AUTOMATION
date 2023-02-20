//CYBORG, NIT ROURKELA
                                                              
                                      /* INTEGRATED FINAL LOGIC CARD CODE || TANDEM MILL, ROURKELA STEEL PLANT, SAIL */
                                      
#define DRON        A0
#define RUN         A4
#define JF          32
#define JR          A3
#define ACCELERATE  52
#define DECELERATE  A6
#define CONST       38
#define QSTOP       A2
#define ENCA        21
#define ENCB        20
#define PWM         12
#define IN1         11

#define RUNRPM      20
#define POT         A1


static volatile bool a, b, c, d, e, f, g ,h, drCmd, runCmd, jfCmd, jrCmd, acclCmd, declCmd, constCmd, runStopCmd;
static long runrpm; static float jogrpm;
volatile float dTime=150000; static float lTime=0; 
volatile static bool encb; volatile static signed int sign=1;
volatile float vfilt, vprev = 0;
float eintegral = 0; float le = 0; float diff = 0;


void onPin2CHANGECallBackFunction() {
    
  float cTime;         
  cTime = micros();       
  
  // calculate the DeltaT between pulses
  dTime = cTime - lTime;
  lTime = cTime;

  encb=digitalRead(ENCB);
  if (encb == 0) sign=-1; else sign=+1; 
  
}


void setup() {
  
  Serial.begin(96e2);

  pinMode(ENCA, INPUT_PULLUP);
  pinMode(ENCB, INPUT_PULLUP);
  pinMode(PWM, OUTPUT);
  pinMode(IN1, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ENCA), onPin2CHANGECallBackFunction, RISING);

  pinMode(DRON, INPUT);         //a
  pinMode(RUN, INPUT);          //b
  pinMode(JF, INPUT);           //c
  pinMode(JR, INPUT);           //d
  pinMode(ACCELERATE, INPUT);   //e
  pinMode(DECELERATE, INPUT);   //f
  pinMode(CONST, INPUT);        //g
  pinMode(QSTOP, INPUT);        //h

  pinMode(A5,OUTPUT);
  pinMode(50, OUTPUT);
  digitalWrite(A5,HIGH);
  digitalWrite(50,HIGH);
  
}


void loop() {
  
  setMotor(0, 0);
  check_inputs();
  
  // .................. Drive On .............. //
  while(drCmd){
    
    check_inputs();
    setMotor(0, 0);

    while(jfCmd){
      
      pid(jogrpm);
      check_inputs();
      
    }
      
    while(jrCmd){
      
      pid(-jogrpm);
      check_inputs();
      
    }
    
    runrpm= RUNRPM;
    
    while(runCmd){
      
      int accflag = 0;
      int decflag = 0;
      pid(runrpm);
      check_inputs();
     
      if(acclCmd==1)  accflag = 1;
      while (accflag==1 &&  constCmd != 1 && runStopCmd!=1){
        
        pid(runrpm);
        runrpm++;
        delay(500);
        if(runrpm>40) runrpm=40;
        check_inputs();
        
      }
      accflag = 0;
 
      if(declCmd==1)  decflag=1;
      while(decflag==1 && constCmd !=1   && runStopCmd!=1){
        
        pid(runrpm);
        runrpm--;
        delay(500);
        if(runrpm<0) runrpm=0;
        check_inputs();
        
      }
      decflag=0;
     
      while(constCmd){
      
        pid(runrpm);
        delay(10);
        check_inputs();
        
      }
    }   
  }
}


void setMotor(int dir, int pwmVal) {
  
  analogWrite(PWM, pwmVal);  
  if (dir == 1) digitalWrite(IN1, HIGH); else if (dir == -1) digitalWrite(IN1, LOW); 
  
}


void check_inputs(){
  
  a = digitalRead(DRON);
  b = digitalRead(RUN);
  c = digitalRead(JF);
  d = digitalRead(JR);
  e = digitalRead(ACCELERATE);
  f = digitalRead(DECELERATE);
  g = digitalRead(CONST);
  h = digitalRead(QSTOP);

  int pot_value= analogRead(POT);
  jogrpm = map(pot_value,0,1023,0,40);
  
//  Serial.print(a);
//  Serial.print(b);
//  Serial.print(c);
//  Serial.print(d);
//  Serial.print(e);
//  Serial.print(f);
//  Serial.print(g);
//  Serial.print(h); 
////  Serial.println("__");
   
  drCmd   = a&!b&!c&!d&!e&!f&!g&!h;
  runCmd  = a&b&!c&!d&!e&!f&!g&!h;
  jfCmd   = a&!b&c&!d&!e&!f&!g&!h;
  jrCmd   = a&!b&!c&d&!e&!f&!g&!h;
  acclCmd = a&b&!c&!d&e&!f&!g&!h;
  declCmd = a&b&!c&!d&!e&f&!g&!h;
  constCmd= a&b&!c&!d&g&!h;
  runStopCmd = !a|!b|h;

//  Serial.print(drCmd);
//  Serial.print(runCmd);
//  Serial.print(jfCmd);
//  Serial.print(jrCmd);
//  Serial.print(acclCmd);
//  Serial.print(declCmd);
//  Serial.println(constCmd); 

}


void pid(int vt){

  
   float DeltaTime; volatile float SpeedInRPM; static unsigned long SpamTimer;
  
  if ((unsigned long)(millis() - SpamTimer) >= (100)) { 
    SpamTimer = millis();
    noInterrupts();
    DeltaTime = dTime;                                                    // RPM calculation
    dTime = 0;
    interrupts();
    if (DeltaTime == 0) DeltaTime = 15000.0;
    SpeedInRPM =(150000.0 / DeltaTime)*sign;
//    vfilt = 0.85435899 * vfilt + 0.07282051 * SpeedInRPM + 0.07282051 * vprev;  //25Hz  //    vfilt = 0.80187364 * vfilt + 0.09906318 * SpeedInRPM + 0.09906318 * vprev;  //35Hz //    vfilt = 0.90999367 * vfilt + 0.04500317 * SpeedInRPM + 0.04500317 * vprev;  //15Hz
//    vfilt = 0.93908194 * vfilt + 0.03045903 * SpeedInRPM + 0.03045903 * vprev;  //10Hz  

    vfilt = 0.85435899 * vfilt + 0.07282051 * SpeedInRPM + 0.07282051 * vprev;  //25Hz        
    vprev = SpeedInRPM; 
//    if (SpeedInRPM < 40) {
//      vfilt = 0.85435899 * vfilt + 0.07282051 * SpeedInRPM + 0.07282051 * vprev;  //25Hz        
//      vprev = SpeedInRPM;                                         
//    } else {
//      vfilt = 0.85435899 * vfilt + 0.07282051 * vprev;
//    }
    
   }
  
  // Compute the control signal u

// float kp = 0.0005;
//  float ki = 0.00003;
//  float kd = 0;
  float kp = 0.0009;
  float ki = 0.000028;
  float kd = 0;
  
  volatile float err  = vt - vfilt;
  eintegral = eintegral + err * (DeltaTime);                                  //PID
  diff = (err - le)/(DeltaTime);
  le =err ;
  
  volatile float u = kp * err  + ki * eintegral + kd * diff;
  
  // Set the motor speed and direction
  volatile int dir = 1;  if (u < 0) dir = -1;
  volatile int pwr = (volatile int)fabs(u); if (pwr > 255) pwr = 255;
  
  setMotor(dir, pwr);
  
  //target vs current reading
  Serial.print(vfilt);  Serial.print(" ");  Serial.print(vt); Serial.println(); delay(2);
//    int filt = vfilt; Serial.print(filt);  Serial.print(" ");  Serial.print(vt); Serial.println(); delay(2);

//  Serial.print(a);
//  Serial.print(b);
//  Serial.print(c);
//  Serial.print(d);
//  Serial.print(e);
//  Serial.print(f);
//  Serial.print(g);
//  Serial.print(h); 
//  Serial.println();
//  Serial.print(runCmd);  Serial.print(" ");  Serial.print(jfCmd); Serial.println(); delay(2);
//  delay(2);
  
  }
 



//////////////////////////////////////////      END       //////////////////////////////////////////////

                                                                                                                    
