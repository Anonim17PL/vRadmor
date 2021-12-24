void setup() {
  Serial.begin(9600);
  pinMode(8,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(13,OUTPUT);
}

int idsel, previd;

void loop() {
  digitalWrite(13,HIGH);
  if (digitalRead(8) == LOW && 1 != previd) {
    digitalWrite(13,LOW);
    delay(500);
    digitalWrite(13,HIGH);
      if (digitalRead(8) == LOW) {idsel = 2924;}
  }
  if (digitalRead(7) == LOW && 2 != previd) {
    digitalWrite(13,LOW);
    delay(500);
    digitalWrite(13,HIGH);
      if (digitalRead(7) == LOW) {idsel = 2925;}
  }
  if (digitalRead(6) == LOW && 4 != previd) {
    digitalWrite(13,LOW);
    delay(500);
    digitalWrite(13,HIGH);
      if (digitalRead(6) == LOW) {idsel = 2926;}
  }
    if (idsel != previd) {
      previd = idsel;
      Serial.print(idsel);
      delay(100);
      Serial.print("INFO");
    } 
}
