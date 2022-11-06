#define ena 7
#define in1 6
#define in2 5

volatile unsigned int temp, counter = 0; //This variable will increase or decrease depending on the rotation of encoder
int intg = 0;
int lastError = 0;
int setRPM = 15000;
int rpm =0;
  


double KP = 0.1;
double KI = 0.1;
double KD = 0.1;
void setup() {
  Serial.begin (9600);

  pinMode(2, INPUT_PULLUP); // internal pullup input pin 2 
  
  pinMode(3, INPUT_PULLUP);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT); 

  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  // internal pullup input pin 3
//Setting up interrupt
  //A rising pulse from encodenren activated ai0(). AttachInterrupt 0 is DigitalPin nr 2 on moust Arduino.
  attachInterrupt(0, ai0, RISING);
   
  //B rising pulse from encodenren activated ai1(). AttachInterrupt 1 is DigitalPin nr 3 on moust Arduino.
//  attachInterrupt(1, ai1, RISING);
  analogWrite(ena,120);
  }
   
  void loop() {
  // Send the value of counter

//Serial.println ((counter-temp)*60);
  if( counter != temp ){
    delay(1000);
    int rpm = ((counter-temp)*60);
//    double balance = PID(rpm-setRPM);
//  int pwmSignal = (int)(125 - constrain(balance, -125, 125));
//  analogWrite(ena, balance);
  Serial.println ((counter-temp)*60);
  temp = counter;
  }
    
  }
   
  void ai0() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  if(digitalRead(3)==LOW) {
  counter++;
  }else{
  counter--;
  }
  }
int rpmTOpwm (int rpmVal){


  int pwmVal = 0.00451343 + 67.1758 * rpmVal;
  return pwmVal;
}
double PID(int error) {
  intg += error;
  intg = constrain(intg, -300, 300);
  int diff = error - lastError;
  lastError = error;
  return KP * error + KI * intg + KD * diff;
}
