#define IRSENSOR_PIN 4
long newTime = millis();
long oldTime = millis();
bool lastRead = 0;
bool newRead = 0;
#define BLACK 0
#define WHITE 1
float Time;
float sped;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // serial_connection.begin(9600);
  pinMode(IRSENSOR_PIN, INPUT);

}

void loop() {
  newRead = digitalRead(IRSENSOR_PIN);
  if (newRead == BLACK && newRead != lastRead) {
    lastRead = newRead;
  }else 
  if (newRead == WHITE && newRead != lastRead) {
    newTime = millis();
    lastRead = newRead;
    Time = (newTime - oldTime);
    oldTime =  millis();
    sped = (6/(Time/1000));
    if (sped > 50) {
      Serial.println(sped);
    }
  }
}
