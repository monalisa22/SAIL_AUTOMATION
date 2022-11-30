

float a0, a1, a2, a3, a4, a5, a6, a7, vj_star, ua_star, dvdt, v_star, vj_star_bar, uth, v2_star;


void setup() {
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);

  pinMode(9, OUTPUT); //
  pinMode(A8, INPUT); 

  pinMode(5, INPUT);

  Serial.begin(9600);

}


void loop() {
  
  a0 = analogRead(A0);  a0 = a0 * 5/1023;
  a1 = analogRead(A1);  a1 = a1 * 5/1023;
  a2 = analogRead(A2);  a2 = a2 * 5/1023;
  a3 = analogRead(A3);  a3 = a3 * 5/1023;
  a4 = analogRead(A4);  a4 = a4 * 5/1023;
  a5 = analogRead(A5);  a5 = a5 * 5/1023;
  a6 = analogRead(A6);  a6 = a6 * 5/1023;
  a7 = analogRead(A7);  a7 = a7 * 5/1023;

  vj_star = a0/5 * a1;
  
  Serial.print("\t vj_star: "); Serial.print(vj_star);  Serial.print("\t");
  
  ua_star = a2/5 * a3;
 
  Serial.print("\t ua_star: "); Serial.print(ua_star);  Serial.print("\t");
  
  dvdt = a4/5 * a5;
  
  Serial.print("\t dvdt: ");  Serial.print(dvdt); Serial.print("\t");
  
  uth = a6/5 * a7;
  
  Serial.print("\t uth: "); Serial.print(uth);  Serial.print("\t");

  v2_star = analogRead(A8);  v2_star = v2_star * 5/1023;
  
  Serial.print("\t v2_star: "); Serial.print(v2_star);  Serial.print("\t");
  

  v_star = (ua_star + dvdt + uth)/3 + v2_star;

  vj_star_bar = vj_star * 1.5;
  if (vj_star_bar >= 5) vj_star_bar = 5;

  analogWrite(9, v_star);
  
  Serial.print("\t v_star: ");  Serial.print(v_star); Serial.print("\t");
  
  analogWrite(8, vj_star_bar);
  
  Serial.print("\t vj_star_bar: "); Serial.print(vj_star_bar); Serial.print("\t");
  Serial.println("");
  
  
}
