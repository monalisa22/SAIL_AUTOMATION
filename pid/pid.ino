#define ClockPin 3 // Must be pin 2 or 3

#define Multiplier  149000.0
volatile long count = 0;
//pid
double KP = 0.43;
double KI = 0.57;
double KD = 0.45;
int intg = 0;
int lastError = 0;

int setRPM=2200;

//pid
volatile long EncoderCounter = 0;
volatile int32_t dTime; // Delt in time
volatile bool DataPinVal;

float vfilt, vprev = 0;

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
  
  digitalWrite(6, LOW);
  analogWrite(5, 70);
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

    vfilt = 0.854*vfilt + 0.0728*SpeedInRPM + 0.0728*vprev;
// vfilt = 0.969*vfilt + 0.0155*SpeedInRPM + 0.0155*vprev;
// vfilt = 0.828*vfilt + 0.0861*SpeedInRPM + 0.0861*vprev;
    vprev = SpeedInRPM;
    
//    Serial.print(SpeedInRPM , 3);
//    Serial.print(" RPM, ");
//    Serial.println();


  
  Serial.print("Vfilt: ");
  Serial.print(vfilt, 2);
//  Serial.print(",");
//  Serial.print("rpm:");
//  Serial.print(SpeedInRPM);
  Serial.print(", ");
  Serial.print("setrpm:");
  Serial.println(setRPM);
//    delay(100);
  }
  double balance = PID(vfilt-setRPM);
  int Signal = (int)(110 - constrain(balance, -110, 110));
  analogWrite(5, rpmTOpwm(Signal));
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
  double rtrn = KP*error+KI*intg+KD*diff;
  return rtrn;
}
