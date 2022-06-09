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
#define LOGGPS 0


//Define Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
float vol = 0.3;

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
AudioPlaySdWav           playSdWav2;     //xy=471,454
AudioPlaySdWav           playSdWav4; //xy=472,662
AudioPlaySdWav           playSdWav3; //xy=474,550
AudioPlaySdWav           playSdWav1;     //xy=476,350
AudioMixer4              mixer1;         //xy=790,417
AudioMixer4              mixer2;         //xy=795,542
AudioOutputI2S           i2s2;           //xy=1211,513
AudioConnection          patchCord1(playSdWav2, 0, mixer1, 1);
AudioConnection          patchCord2(playSdWav2, 1, mixer2, 1);
AudioConnection          patchCord3(playSdWav4, 0, mixer1, 3);
AudioConnection          patchCord4(playSdWav4, 1, mixer2, 3);
AudioConnection          patchCord5(playSdWav3, 0, mixer1, 2);
AudioConnection          patchCord6(playSdWav3, 1, mixer2, 2);
AudioConnection          patchCord7(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord8(playSdWav1, 1, mixer2, 0);
AudioConnection          patchCord9(mixer1, 0, i2s2, 0);
AudioConnection          patchCord10(mixer2, 0, i2s2, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=651,1204
// GUItool: end automatically generated code





//Class defining all the set listening points
class Point {
  private:
    

  public:
    float x;
    float y;
    /*float x1;
    float x2;
    float y1;
    float y2;*/
    float rayon;
    const char* filename;

    /*
     * rayon in meters
     * y = latitude
     * x = longitude
     */
    Point(float y, float x, float rayon, const char* filename) {
      this->rayon = rayon;
      this->x = x;
      this->y = y;
      this->filename = filename;
      /*this->x1 = x-(rayon/(111111*cos(y))); //!! radians
      this->x2 = x+(rayon/(111111*cos(y)));
      this->y1 = y-(rayon/111111);
      this->y2 = y+(rayon/111111);*/
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



void verifBouton(void){
  
  if(LOG){
    Serial.println("checking buttons...");
  }
  
  //Bouton principal
  if(pressedBtns[0]==HIGH){

    btnAction = true;
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
    Point *point1 = new Point(latitude, longitude, 10, "HELLO.WAV");
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



void checkPosition(void){
  if(listePoints.size()>0){
    Point *actualPoint;

    //int i = 0;
    //for(int i=0; i<listePoints.size(); i++){
      actualPoint = listePoints.get(listePoints.size()-1);
    
      /*square*/
      //if(latitude>actualPoint->y1 && latitude<actualPoint->y2 && longitude>actualPoint->x1 && longitude<actualPoint->x2){
      /*circle*/
      if(distanceToPoint(actualPoint) < actualPoint->rayon){
        if (playSdWav1.isPlaying() == false) {
          Serial.println("Start playing");

          const char* joue = actualPoint->filename;
          playSdWav1.play(joue);
          delay(10); // wait for library to parse WAV info
    
          playSdWav3.play("ENTREEZONE.WAV");
          delay(10); // wait for library to parse WAV info
        }
      }
      else{
        if (playSdWav1.isPlaying() == true){
          Serial.println("Stop playing");
          playSdWav1.stop();
    
          playSdWav3.play("SORTIEZONE.WAV");
          delay(10); // wait for library to parse WAV info
        }
      }
    //}
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
  
  return(sqrt(sq(distx) + sq(disty)));
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
