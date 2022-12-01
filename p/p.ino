#define ClockPin 3 // Must be pin 2 or 3

#define Multiplier  150000.0
volatile long count = 0;
volatile long EncoderCounter = 0;
volatile int32_t dTime; // Delt in time
volatile bool DataPinVal;



void onPin2CHANGECallBackFunction() {
  static uint32_t lTime; // Saved Last Time of Last Pulse
  uint32_t cTime; // Current Time
  cTime = micros(); // Store the time for RPM Calculations

  // calculate the DeltaT between pulses
  dTime = cTime - lTime;
  lTime = cTime;

}

void setup() {
  Serial.begin(115200); //115200
  // put your setup code here, to run once:
  pinMode(ClockPin, INPUT);
  //  pinMode(DataPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(ClockPin), onPin2CHANGECallBackFunction, RISING);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop() {
  
  float DeltaTime;
  float SpeedInRPM = 0;
  float vfilt = 0; 
  float vprev = 0;

  static unsigned long SpamTimer;
  if ( (unsigned long)(millis() - SpamTimer) >= (100)) {
    SpamTimer = millis();
    noInterrupts ();
    DeltaTime = dTime;
    dTime = 0; 
    interrupts ();
    SpeedInRPM = Multiplier / DeltaTime; 
    
  //filtering
//    vfilt = (0.854*vfilt + 0.0728*SpeedInRPM + 0.0728*vprev);//25hz
// vfilt = (0.969*vfilt + 0.0155*SpeedInRPM + 0.0155*vprev);//5hz
//  vfilt = 0.828*vfilt + 0.0861*SpeedInRPM + 0.0861*vprev;//30hz
    vprev = SpeedInRPM;delay(1);

  //P
    long currT = micros();
//    float vt = 1500*(sin(currT/1e6)>0);
    float vt = 1500;
    float kp = 0.1; //change
    float e = vt-SpeedInRPM;
    float u = kp*e ;

    float pwr = u;
//    pwr = constrain(pwr, -255, 255);
    pwr = constrain(pwr, -70.0, 70.0); 
    motor(pwr);
   
    
//   Serial.print("Vfilt:");
//  Serial.print(vfilt, 3);
//  Serial.print(",");
  Serial.print("vt:");
  Serial.println(vt);
   Serial.print(",");
  Serial.print("speedrpm:");
  Serial.println(SpeedInRPM);
    
  }
}

void motor(int spd){
  digitalWrite(6, spd>0 ? 1:0);
  analogWrite(5, abs(spd)); 
}
