/*#include <avr/io.h>
#include <avr/interrupt.h>*/

/*rayons :
 * 0.000080 - 13m
 */

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <LinkedList.h>


//Useful flags
#define LOG 1


//Define Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
float vol = 0.3;

//Define GPIO
#define BTN_1 2
#define BTN_2 3
#define BTN_3 4
bool btnPressed = false;
unsigned long lastPress = 0;

// Communication GPS
int RXPin = 0;
int TXPin = 1;
int GPSBaud = 9600;
// Create a TinyGPS++ object
TinyGPSPlus gps;
// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);
//Actual data
float latitude = 0, longitude = 0;

//Audio objects
// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav2;     //xy=471,454
AudioPlaySdWav           playSdWav1;     //xy=473,361
AudioMixer4              mixer1;         //xy=775,340
AudioMixer4              mixer2;         //xy=776,466
AudioOutputI2S           i2s2;           //xy=959,394
AudioConnection          patchCord1(playSdWav2, 0, mixer1, 1);
AudioConnection          patchCord2(playSdWav2, 1, mixer2, 1);
AudioConnection          patchCord3(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord4(playSdWav1, 1, mixer2, 0);
AudioConnection          patchCord5(mixer1, 0, i2s2, 0);
AudioConnection          patchCord6(mixer2, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=560,644
// GUItool: end automatically generated code




//Class defining all the set listening points
class Point {
  private:
    

  public:
    float x;
    float y;
    float x1;
    float x2;
    float y1;
    float y2;
    float rayon;
    
    Point(float y, float x, float rayon) {
      this->rayon = rayon;
      this->x = x;
      this->y = y;
      this->x1 = x-rayon;
      this->x2 = x+rayon;
      this->y1 = y-rayon;
      this->y2 = y+rayon;
    }

    /*float getCoord() {
      return(this->x, this->y);
    }*/
};


//List of all created points
LinkedList<Point*> listePoints = LinkedList<Point*>();




//Pin interruption
void myInterrupt() {

    if(LOG){
      Serial.print("System interrupted ");
      Serial.println(millis());
    }
    
    //Button press check
    if(millis()-lastPress>300){
      lastPress = millis();

      verifBouton();
      btnPressed=false;
    }
}





void setup()
{
   
  //Communication PC
  Serial.begin(9600);

  //Init Audio
  AudioMemory(8);
  sgtl5000_1.enable();
  sgtl5000_1.volume(vol);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  delay(1000);

  //Init communication GPS
  gpsSerial.begin(GPSBaud);

  //Init GPIO
  pinMode(BTN_1, INPUT_PULLDOWN);
  pinMode(BTN_2, INPUT_PULLDOWN);
  pinMode(BTN_3, INPUT_PULLDOWN);

  //Init interruption
  attachInterrupt(digitalPinToInterrupt(BTN_1), myInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_2), myInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_3), myInterrupt, RISING);
}




void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0){
    if (gps.encode(gpsSerial.read()))
      displayInfo();
      

    //Ars Nomadis Latitude : 48.107255 Longitude: -1.652680
    //FabLab Latitude: 48.118805 Longitude: -1.702768


    if(listePoints.size()>0){
      
      Point *actualPoint;

      //début boucle for
      
      actualPoint = listePoints.get(0);
      
      //if(latitude>listePoints.get(0)->y1 && latitude<listePoints.get(0)->y2 && longitude>listePoints.get(0)->x1 && longitude<listePoints.get(0)->x2){
      if(distanceToPoint(actualPoint) < actualPoint->rayon){
        if (playSdWav1.isPlaying() == false) {
          Serial.println("Start playing");
          
          playSdWav1.play("HELLO.WAV");
          delay(10); // wait for library to parse WAV info

          playSdWav2.play("ENTREEZONE.WAV");
          delay(10); // wait for library to parse WAV info
        }
      }
      else{
        if (playSdWav1.isPlaying() == true){
          Serial.println("Stop playing");
          playSdWav1.stop();

          playSdWav2.play("SORTIEZONE.WAV");
          delay(10); // wait for library to parse WAV info
        }
      }
    }    

        
  }
  
  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS module detected");
    while(true);
  }
}



float distanceToPoint(Point* p){
  return(sqrt(sq(p->x - longitude) + sq(p->y - latitude)));
}



void verifBouton(){
  if(LOG){
    Serial.println("checking buttons...");
  }
  
  //Bouton principal
  if(digitalRead(BTN_1)==HIGH){
        
    playSdWav1.play("SYGSONPLACE.WAV");
    delay(10); // wait for library to parse WAV info

    newZone();
    
  }
  //Volume +
  else if(digitalRead(BTN_3)==HIGH){
    playSdWav1.play("SYGSONVOL.WAV");
    delay(10); // wait for library to parse WAV info
    if(vol<1.00){
      vol += 0.1;
      sgtl5000_1.volume(vol);

      Serial.print("Volume : ");
      Serial.println(int(vol*100));
    }
  }
  //Volume -
  else if(digitalRead(BTN_2)==HIGH){
    
    playSdWav1.play("SYGSONVOL.WAV");
    delay(10); // wait for library to parse WAV info
    if(vol>=0.1){
      vol -= 0.1;
      sgtl5000_1.volume(vol);

      Serial.print("Volume : ");
      Serial.println(int(vol*100));
    }
  }
}



void newZone(){
  playSdWav1.play("SYGSONVOL.WAV");
  delay(10); // wait for library to parse WAV info
  playSdWav1.play("SYGSONVOL.WAV");
  delay(10); // wait for library to parse WAV info
  Serial.println("Traitement...");
  
    
  if (gps.location.isValid()){

    //Add a new point
    Point *point1 = new Point(latitude, longitude, 0.000030);
    listePoints.add(point1);

    //Point successful added
    playSdWav1.play("SYGSONPLACE.WAV");
    delay(10); // wait for library to parse WAV info
    Serial.println("Point posé");

    //Shows the added coordinates
    Serial.println(listePoints.get(listePoints.size()-1)->x1);
  }
  else{
    //Failed to add the point
    Serial.println("Hors de portée GPS");

    playSdWav1.play("SYGSONVOL.WAV");
    delay(10); // wait for library to parse WAV info
  }

  
}


void displayInfo()
{
  if (gps.location.isValid())
  {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    
    if(LOG){
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
            
      Serial.print("Altitude: ");
      Serial.println(gps.altitude.meters());
    }
  }
  else if(LOG)
  {
    Serial.println("Location: Not Available");
  }


  if(LOG){
    Serial.print("Date: ");
    if (gps.date.isValid() && LOG)
    {
      Serial.print(gps.date.day());
      Serial.print("/");
      Serial.print(gps.date.month());
      Serial.print("/");
      Serial.println(gps.date.year());
    }
    else
    {
      Serial.println("Not Available");
    }

 
    Serial.print("Time: ");
    if (gps.time.isValid())
    {
      if (gps.time.hour() < 8) Serial.print(F("0"));
      Serial.print((gps.time.hour()+2)%24);
      Serial.print(":");
      if (gps.time.minute() < 10) Serial.print(F("0"));
      Serial.print(gps.time.minute());
      Serial.print(":");
      if (gps.time.second() < 10) Serial.print(F("0"));
      Serial.print(gps.time.second());
      Serial.print(".");
      if (gps.time.centisecond() < 10) Serial.print(F("0"));
      Serial.println(gps.time.centisecond());
    }
    else
    {
      Serial.println("Not Available");
    }

    Serial.print("Speed: ");
    if (gps.speed.isValid())
    {
      Serial.print((float(gps.speed.value())/100)*1.852, 3);
      Serial.print(" km/h");
    }
    else
    {
      Serial.println("Not Available");
    }
  
    Serial.println();
    Serial.println();
    delay(10);
  }
  
}
