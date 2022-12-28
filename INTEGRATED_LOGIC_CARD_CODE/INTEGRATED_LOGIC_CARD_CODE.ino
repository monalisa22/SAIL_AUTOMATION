

                                                                  //CYBORG, NIT ROURKELA
                                                              
                                      /* INTEGRATED FINAL LOGIC CARD CODE || TANDEM MILL, ROURKELA STEEL PLANT, SAIL */
                                      
#define DRON        22
#define RUN         23
#define JF          24
#define JR          25
#define ACCELERATE  26
#define DECELERATE  27
#define CONST       28
#define QSTOP       29
#define LED_DRON    13
#define ENCA         2
#define ENCB         3
#define PWM         11
#define IN1         12

#define RUNRPM      30
#define POT         A0


static volatile bool a, b, c, d, e, f, g ,h, drCmd, runCmd, jfCmd, jrCmd, acclCmd, declCmd, constCmd, runStopCmd;
static long runrpm; static float jogrpm;
volatile int32_t dTime=33000; static uint32_t lTime=0; 
volatile static bool encb; volatile static signed int sign=1;
float vfilt, vprev = 0;
float eintegral = 0; float le = 0; float diff = 0;


void onPin2CHANGECallBackFunction() {
    
  uint32_t cTime;         
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

  pinMode(LED_DRON, OUTPUT); digitalWrite(LED_DRON, 0);
  
}


void loop() {
  
  setMotor(0, 0);
  check_inputs();
  
  // .................. Drive On .............. //
  while(drCmd){
    
    check_inputs();
    digitalWrite(LED_DRON, 1);
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
//  Serial.print("__");
   
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
  
  float DeltaTime; float SpeedInRPM; static unsigned long SpamTimer;
  
  if ((unsigned long)(millis() - SpamTimer) >= (100)) { 
    SpamTimer = millis();
    noInterrupts();
    DeltaTime = dTime;                                                    // RPM calculation
    dTime = 0;
    interrupts();
    SpeedInRPM =(33000.0 / DeltaTime)*sign;
    vfilt = 0.854 * vfilt + 0.0728 * SpeedInRPM + 0.0728 * vprev;  //25Hz                                                     
    vprev = SpeedInRPM;
   }
  
  // Compute the control signal u
  float kp = 3.5;
  float ki = 0.00035;
  float kd = 0;
  
  float e = vt - vfilt;
  eintegral = eintegral + e * DeltaTime;                                  //PID
  diff = (e - le)/DeltaTime;
  le = e;
  
  float u = kp * e + ki * eintegral + kd * diff;
  
  // Set the motor speed and direction
  int dir = 1;  if (u < 0) dir = -1;
  int pwr = (int)fabs(u); if (pwr > 255) pwr = 255;
  
  setMotor(dir, pwr);
  
  //target vs current reading
  Serial.print(vfilt);  Serial.print(" ");  Serial.print(vt); Serial.println(); delay(2);
  //  int filt = vfilt; Serial.print(filt);  Serial.print(" ");  Serial.print(vt); Serial.println(); delay(2);
  
  }




//////////////////////////////////////////      END       //////////////////////////////////////////////

                                                                                                                    //violet2. blue3_encoder, m2a- red n m2b -black_motor power
