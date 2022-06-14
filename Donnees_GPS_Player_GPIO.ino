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
#define LOG 0
#define LOGGPS 0


//Define Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
float vol = 0.4;
int playingPoint = -1, playingLoop = -1;

//Define GPIO
#define BTN_1 22
#define BTN_2 17
#define BTN_3 16
unsigned long lastPress = 0;
bool pressedBtns[3];
bool btnAction = false;

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
AudioPlaySdWav           playSdWav1;     //xy=148,286
AudioPlaySdWav           playSdWav4; //xy=150,736
AudioPlaySdWav           playSdWav2;     //xy=158,416
AudioPlaySdWav           playSdWav3; //xy=159,618
AudioEffectFade          fade3;          //xy=346,399
AudioEffectFade          fade4;          //xy=346,451
AudioEffectFade          fade2;          //xy=352,327
AudioEffectFade          fade1;          //xy=353,246
AudioMixer4              mixer1;         //xy=790,417
AudioMixer4              mixer2;         //xy=795,542
AudioOutputI2S           i2s2;           //xy=1211,513
AudioConnection          patchCord1(playSdWav1, 0, fade1, 0);
AudioConnection          patchCord2(playSdWav1, 1, fade2, 0);
AudioConnection          patchCord3(playSdWav4, 0, mixer1, 3);
AudioConnection          patchCord4(playSdWav4, 1, mixer2, 3);
AudioConnection          patchCord5(playSdWav2, 0, fade3, 0);
AudioConnection          patchCord6(playSdWav2, 1, fade4, 0);
AudioConnection          patchCord7(playSdWav3, 0, mixer1, 2);
AudioConnection          patchCord8(playSdWav3, 1, mixer2, 2);
AudioConnection          patchCord9(fade3, 0, mixer1, 1);
AudioConnection          patchCord10(fade4, 0, mixer2, 1);
AudioConnection          patchCord11(fade2, 0, mixer2, 0);
AudioConnection          patchCord12(fade1, 0, mixer1, 0);
AudioConnection          patchCord13(mixer1, 0, i2s2, 0);
AudioConnection          patchCord14(mixer2, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=651,1204
// GUItool: end automatically generated code





//Class defining all the set listening points
class Point {
  private:
    

  public:
    float x;
    float y;
    float rayon;
    float hyst;
    const char* filename;
    bool isTrigger;
    bool looping;

    /*
     * rayon in meters
     * y = latitude
     * x = longitude
     */
    Point(float y, float x, float rayon, float hyst, const char* filename, bool isTrigger, bool looping) {
      this->rayon = rayon;
      this-> hyst = hyst;
      this->x = x;
      this->y = y;
      this->filename = filename;
      this->isTrigger = isTrigger;
      this->looping = looping;
    }
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
    if((digitalRead(BTN_1)||digitalRead(BTN_2)||digitalRead(BTN_3)) && millis()-lastPress>100){
      pressedBtns[0] = digitalRead(BTN_1);
      pressedBtns[1] = digitalRead(BTN_2);
      pressedBtns[2] = digitalRead(BTN_3);
      
      lastPress = millis();
      
      verifBouton();

      if(LOG){
        Serial.print("Bouton1 = ");
        Serial.println(pressedBtns[0]);
        Serial.print("Bouton2 = ");
        Serial.println(pressedBtns[1]);
        Serial.print("Bouton3 = ");
        Serial.println(pressedBtns[2]);
        Serial.println();
      }
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


  //Test Plages de Baud
  addPoints();

  //Startup
  playSdWav3.play("06 BOUCLE TRANSITION GUITARE ENFANTS CALE.WAV");
  delay(2500);
  playSdWav3.stop();
  delay(10);
}




void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0){
    if (gps.encode(gpsSerial.read()))
      displayInfo();
      

    //Ars Nomadis Latitude : 48.107255 Longitude: -1.652680
    //FabLab Latitude: 48.118805 Longitude: -1.702768

    //Checking zones
    checkPosition();

    //Checking if main button pressed
    if(btnAction){
      newZone();

      btnAction = false;
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


//Point(float y, float x, float rayon, float hyst, const char* filename, bool isTrigger, bool looping)
void addPoints(void){
  
  Point *point1 = new Point(48.111216058669335,  -1.6461818402264587, 10, 0, "01 DEBUT YOURNENAR.WAV", false, false);
  listePoints.add(point1);

  Point *point2 = new Point(48.110068, -1.652422, 10, 0, "02 BOUCLE TRANSITION MUSICALE YOURNENAR-VERTUGADIN.WAV", true, true);
  listePoints.add(point2);

  Point *point3 = new Point(48.110091041227626, -1.6522863928835734, 10, 0, "03 VERTUGADIN PASSERELLE.WAV", false, false);
  listePoints.add(point3);

  Point *point4 = new Point(48.10927072930142, -1.6527283832604704, 10, 0, "04 BOUCLE TRANSITION AMBIANCE PASSERELLE ENFANTS.WAV", true, true);
  listePoints.add(point4);

  Point *point5 = new Point(48.109171, -1.651403, 10, 0, "05 ENFANTS INDICATIONS POUR CALE.WAV", false, false);
  listePoints.add(point5);

  Point *point6 = new Point(48.108619, -1.650568, 10, 0, "06 BOUCLE TRANSITION GUITARE ENFANTS CALE.WAV", true, false);
  listePoints.add(point6);
}




//Main function that checks all points
void checkPosition(void){
  if(listePoints.size()>0){
    Point *actualPoint;
    float distance;

    for(int i=0; i<listePoints.size(); i++){
      actualPoint = listePoints.get(i);
      distance = distanceToPoint(actualPoint);

      //Check if in zone
      if(distance < actualPoint->rayon){

        //If it is a "main" point
        if (actualPoint->looping == false) {
          if(playSdWav1.isPlaying() == false){
            if(LOG){
              Serial.print("Start playing - ");
              Serial.println(actualPoint->filename);
            }
            
            playingPoint = i;
  
            const char* joue = actualPoint->filename;
            playSdWav1.play(joue);
            delay(10); // wait for library to parse WAV info
          }
          else{
            if(playingPoint!=i){
              fade1.fadeOut(1000);
              fade2.fadeOut(1000);
              delay(1000);
              
              playingPoint = i;
  
              const char* joue = actualPoint->filename;
              playSdWav1.play(joue);
              delay(10); // wait for library to parse WAV info

              fade1.fadeIn(1);
              fade2.fadeIn(1);
            }
          }    
        }

        //If it is a loop
        else{
          if(playSdWav2.isPlaying() == false){
            if(LOG){
              Serial.print("Start playing - ");
              Serial.println(actualPoint->filename);
            }
            playingLoop = i;
  
            const char* joue = actualPoint->filename;
            playSdWav2.play(joue);
            delay(10); // wait for library to parse WAV info
          }
          else{
            if(playingLoop!=i){
              fade3.fadeOut(1000);
              fade4.fadeOut(1000);
              delay(1000);
              
              playingLoop = i;
  
              const char* joue = actualPoint->filename;
              playSdWav2.play(joue);
              delay(10); // wait for library to parse WAV info

              fade3.fadeIn(1);
              fade4.fadeIn(1);
            }
          }
        }

        if(LOG){
          Serial.print("Distance to point : ");
          Serial.println(distanceToPoint(actualPoint));
        }
      }

      //If not in zone
      else if(distance > (actualPoint->rayon+actualPoint->hyst)){
        //Looping
        if(playingLoop==i && playSdWav2.isPlaying()==false){
          const char* joue = actualPoint->filename;
          playSdWav2.play(joue);
          delay(10); // wait for library to parse WAV info
        }
        //Stop main
        if(playingPoint==i && playSdWav1.isPlaying()==true && actualPoint->isTrigger==false){
          if(LOG){
            Serial.print("Stop playing - ");
            Serial.println(actualPoint->filename);
          }
          fade1.fadeOut(1000);
          fade2.fadeOut(1000);
          delay(1000);
  
          playSdWav1.stop();
          fade1.fadeIn(1);
          fade2.fadeIn(1);
        }
      }
    }
  }    
}



/*
 * returns value in meters
 */
float distanceToPoint(Point* p){
  //111 111 m in the y direction = 1° of latitude
  //111,111 * cos(latitude) m in the x direction = 1° of longitude

  //conversion to meters
  float disty = (latitude - p->y)*111111;
  float distx = (longitude - p->x)*111111*cos((p->y*6.28)/360); //!! radians

  float dist = sqrt(sq(distx) + sq(disty));
  
  return(dist);
}



void verifBouton(void){
  
  if(LOG){
    Serial.println("checking buttons...");
  }
  
  //Bouton principal
  if(pressedBtns[0]==HIGH){

    //btnAction = true;
  }

  //Volume -
  else if(pressedBtns[1]==HIGH){
    
    playSdWav4.play("SYGSONVOL.WAV");
    delay(10); // wait for library to parse WAV info
    if(vol>=0.1){
      vol -= 0.1;
      sgtl5000_1.volume(vol);

      Serial.print("Volume : ");
      Serial.println(int(vol*100));
    }
  }
  
  //Volume +
  else if(pressedBtns[2]==HIGH){
    playSdWav4.play("SYGSONVOL.WAV");
    delay(10); // wait for library to parse WAV info
    if(vol<=0.9){
      vol += 0.1;
      sgtl5000_1.volume(vol);

      Serial.print("Volume : ");
      Serial.println(int(vol*100));
    }
  }
  
  else{
    Serial.println("Aucun bouton détecté");
  }
}



void newZone(void){
  Serial.println("Traitement...");
  
  if (gps.location.isValid()){

    //Add a new point
    Point *point1 = new Point(latitude, longitude, 10, 0, "HELLO.WAV", false, false);
    listePoints.add(point1);

    //Point successful added
    playSdWav4.play("SYGSONPLACE.WAV");
    delay(10); // wait for library to parse WAV info
    Serial.println("Point posé");

    //Shows the added coordinates
    Serial.print("x = ");
    Serial.print(listePoints.get(listePoints.size()-1)->x);
    Serial.print(", y = ");
    Serial.println(listePoints.get(listePoints.size()-1)->y);
  }
  else{
    //Failed to add the point
    playSdWav4.play("SYGSONVOL.WAV");
    delay(10); // wait for library to parse WAV info
    Serial.println("Hors de portée GPS");
  }
}



void displayInfo(void)
{
  if (gps.location.isValid())
  {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    
    if(LOGGPS){
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
            
      Serial.print("Altitude: ");
      Serial.println(gps.altitude.meters());
    }
  }
  else if(LOGGPS)
  {
    Serial.println("Location: Not Available");
  }


  if(LOGGPS){
    Serial.print("Date: ");
    if (gps.date.isValid())
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
