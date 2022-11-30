#define ClockPin 3 // Must be pin 2 or 3

#define Multiplier  150000.0
volatile long count = 0;
volatile long EncoderCounter = 0;
volatile int32_t dTime; // Delt in time
volatile bool DataPinVal;

float vfilt, vprev = 0;

long prevT = 0;
float eintegral = 0;

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
  pinMode(7, OUTPUT);
}

void loop() {
  
//  digitalWrite(5, HIGH);
//  digitalWrite(6, LOW);
//  analogWrite(7, 60);
  float DeltaTime;
  float SpeedInRPM = 0;

  static unsigned long SpamTimer;
  if ( (unsigned long)(millis() - SpamTimer) >= (100)) {
    SpamTimer = millis();
    noInterrupts ();
    DeltaTime = dTime;
    dTime = 0; 
    interrupts ();
    SpeedInRPM = Multiplier / DeltaTime; 
    
  //filtering
    vfilt = 0.854*vfilt + 0.0728*SpeedInRPM + 0.0728*vprev;
    vprev = SpeedInRPM;

  //PI
    long currT = micros();
    float deltaT = ((float) (currT-prevT))/1.0e6;
    prevT = currT;

    float vt = 100*(sin(currT/1e6)>0);
    float kp = 7; //put changed 1
    float ki = 10;//change
    float e = vt-vfilt;
    eintegral = eintegral + e*deltaT;
    float u = kp*e + ki*eintegral;
    int pwr = u;
    pwr = constrain(pwr, -255, 255);
    motor(pwr);
   
    
    Serial.print(SpeedInRPM , 3);
    Serial.print(" RPM, ");
    Serial.println();

    Serial.print(vfilt , 3);
    Serial.print(" RPM");
    Serial.println();
    delay(1);
  }
}

void motor(int spd){
  digitalWrite(5, spd>0 ? 1:0);
  digitalWrite(6, spd>0 ? 0:1);
  analogWrite(7, abs(spd)); 
}
