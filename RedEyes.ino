#include <ShiftRegister74HC595.h>
#include <DFMiniMp3.h>
#include <SoftwareSerial.h>


// implement a notification class,
// its member methods will get called 
//
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};


int latchPin = 5;  // Latch pin of 74HC595 is connected to Digital pin 5
int clockPin = 6; // Clock pin of 74HC595 is connected to Digital pin 6
int dataPin = 4;  // Data pin of 74HC595 is connected to Digital pin 4
int vibPin = 3;
int eyePin = 7;
int intArrLeds[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
int gUbound = 16;
int intEye = 0;

SoftwareSerial secondarySerial(10, 11); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);

//DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial);
ShiftRegister74HC595<2> sr(dataPin, clockPin, latchPin);

void setup() {
  pinMode(vibPin, OUTPUT);
  pinMode(eyePin, INPUT);
  sr.setAllLow();
  Serial.begin(9600);
  Serial.println("mp3 begin");
  mp3.begin();
  uint16_t volume = mp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  mp3.setVolume(0);
  
  uint16_t count = mp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files ");
  Serial.println(count);
} 

void loop() {
  delay(1000);
  intEye = digitalRead(eyePin);
  Serial.println(intEye);
  if (intEye > 0){
    delay(1000);    
    //musicFadeIn();
    shuffleLeds(gUbound);
    WakeOrSleep(1);
    do{
      Stare(gUbound);
    } while (digitalRead(eyePin) != 0);
    shuffleLeds(gUbound);
    WakeOrSleep(0);
    //musicFadeOut();
  }  
}

void musicPlay(){
  mp3.playGlobalTrack(1);
}

void musicFadeIn(){
  musicPlay();
  for (int intCounter = 0; intCounter < 30; intCounter++){
    mp3.setVolume(intCounter);
    delay(250);
  }
}

void musicFadeOut(){
  for (int intCounter = 30; intCounter > -1; intCounter--){
    mp3.setVolume(intCounter);
    delay(250);
  }
}
void Stare(int aintUbound){
  for (int intCounter = 0; intCounter < random(10, 18); intCounter++){
    int intBlink = 5;
    if (random(0, 9) > intBlink){
      int intREye = random(0, aintUbound);
      Blinky(intREye);
    }
    int intShake = 5;
    if (random(0, 9) > intShake){
      //Shake();
    }
    delay(1000);
  }
}
void Shake(){
  digitalWrite(vibPin, HIGH);
  delay(random(300,1000));
  digitalWrite(vibPin, LOW);
}

volumeUpDown(int aintIncrement){
  uint16_t volume = mp3.getVolume();
  mp3.setVolume(volume + aintIncrement);
}

void Blinky(int aintREye){
  turnOn(aintREye, 0);
  delay(random(200, 400));
  turnOn(aintREye, 1);
}

void WakeOrSleep(int aintAwake){
  for (int intCounter = 0; intCounter < gUbound; intCounter++){
      turnOn(intArrLeds[intCounter], aintAwake);
      delay(random(400, 1200));
  }
}

void turnOn(int aintLed, int aintPower){
  sr.setNoUpdate(aintLed, aintPower);
  sr.updateRegisters();
}

void shuffleLeds(int aintUbound){
  for (int a=0; a < aintUbound; a++)
  {
    int r = random(a, aintUbound);
    int temp = intArrLeds[a];
    intArrLeds[a] = intArrLeds[r];
    intArrLeds[r] = temp;
  }
}
