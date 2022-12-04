                                    
                           /**** # SEQUENCE LOGIC CARD # ****/
                                   //CYBORG, NITRKL//
                            /* TANDEM MILL(CRM), RSP, SAIL */


#define DRON        2
#define RUN         3 
#define JF          4
#define JR          5
#define ACCELERATE  6
#define DECELERATE  7 
#define CONST       8
#define QSTOP       9

#define LED_DRON    13


#define IN1         24
#define IN2         23
#define EN          12


#define RUNPWM      150
#define JOGPWM      120


static bool a, b, c, d, e, f, g ,h, drCmd, runCmd, jfCmd, jrCmd, acclCmd, declCmd;

static long runpwm;   

void setup() {
  Serial.begin(9600);
  
  pinMode(DRON, INPUT_PULLUP);         //a
  pinMode(RUN, INPUT_PULLUP);          //b
  pinMode(JF, INPUT_PULLUP);           //c
  pinMode(JR, INPUT_PULLUP);           //d
  pinMode(ACCELERATE, INPUT_PULLUP);   //e
  pinMode(DECELERATE, INPUT_PULLUP);   //f
  pinMode(CONST, INPUT_PULLUP);        //g
  pinMode(QSTOP, INPUT_PULLUP);        //h


  pinMode(LED_DRON, OUTPUT);
  digitalWrite(LED_DRON, 0);


  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
  
}

void loop() {
  check_inputs();
  
  // .................. Drive On .............. //
  while(drCmd){
    check_inputs();
    digitalWrite(LED_DRON, 1);
    motor(0);
    while(jfCmd){
      motor(JOGPWM);
      check_inputs();
    }
    while(jrCmd){
      motor(-JOGPWM);
      check_inputs();
    }

    runpwm= RUNPWM;
    
    while(runCmd){
      motor(runpwm);
      check_inputs();
      
      while(acclCmd){
        motor(runpwm);
        runpwm++;
        delay(10);
        if (runpwm>255) runpwm= 255;
        check_inputs();
      }

      while(declCmd){
        motor(runpwm);
        runpwm--;
        delay(10);
        if (runpwm<0) runpwm= 0;
        check_inputs();
      }
    }
  }


  // ............. Default Stop ............ //
  
  while(!a|h){
    check_inputs();
    digitalWrite(LED_DRON, 0);
    motor(0);
  }
}


  // .............. Function Definations .............//

void motor(int spd){
  digitalWrite(IN1, spd>0 ? 1:0);
  digitalWrite(IN2, spd>0 ? 0:1);
  analogWrite(EN, abs(spd)); 
}

void check_inputs(){
  a = digitalRead(DRON);
  a=!a;
  b = digitalRead(RUN);
  c = digitalRead(JF);
  d = digitalRead(JR);
  e = digitalRead(ACCELERATE);
  f = digitalRead(DECELERATE);
  g = digitalRead(CONST);
  h = digitalRead(QSTOP);
   b=!b; c=!c; d=!d; e=!e; f=!f; g=!g; h=!h;

  drCmd   = a&!b&!c&!d&!e&!f&!g&!h;
  runCmd  = a&b&!c&!d&!e&!f&!g&!h;
  jfCmd   = a&!b&c&!d&!e&!f&!g&!h;
  jrCmd   = a&!b&!c&d&!e&!f&!g&!h;
  acclCmd = a&b&!c&!d&e&!f&!g&!h;
  declCmd = a&b&!c&!d&!e&f&!g&!h;
}
