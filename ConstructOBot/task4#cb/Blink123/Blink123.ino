void setup() {
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  Serial.begin(57600);
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("========================");
  Serial.println(analogRead(A1));
  Serial.println(analogRead(A2));
  Serial.println(analogRead(A3));
}
