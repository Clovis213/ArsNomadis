// size_t inputLength; (optional)


//Opens and reads info.json file
void readFile(void){
  myFile = SD.open("info.json");
  char input[8192];

  //Copies all data in the 'input' variable
  if(myFile) {
    int i = 0;
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      input[i] = myFile.read();
      i++;
    }
    // close the file:
    myFile.close();
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening info.json");
  }

  //Size of the json document, compute on https://arduinojson.org/v6/assistant
  DynamicJsonDocument doc(8192);
  
  DeserializationError error = deserializeJson(doc, input);//, inputLength);
  
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  //Stores all data into variables
  const char* uuid = doc["uuid"]; // "90491c35-a21b-4ced-ae03-e0ab29f6abb2"
  const char* name = doc["name"]; // "Untitled"
  
  double initialLocation_0 = doc["initialLocation"][0]; // 48.107878
  double initialLocation_1 = doc["initialLocation"][1]; // -1.651773
  
  JsonArray zones = doc["zones"];

  if(LOG){
    Serial.println("Size of zone list :");
    Serial.println(zones.size());
  }
  
  //Scanning of all created zones
  for(int i = 0; i<zones.size(); i++){
    JsonObject zone = zones[i];
    
    const char* zone_uuid = zone["uuid"]; // "45b435f5-d4c1-44da-ba84-24c2fe0873fb"
    const char* zone_name = zone["name"]; // "Point 1 Pont"
    
    float zone_location_0 = zone["location"][0]; // 48.11122
    float zone_location_1 = zone["location"][1]; // -1.64611
    
    int zone_radius = zone["radius"]; // 25
    bool zone_visible = zone["visible"]; // true

    JsonArray zone_sounds = zone["sounds"];
    int nbSounds = zone_sounds.size();

    if(LOG){
      Serial.println("");
      Serial.print("Zone : ");
      Serial.println(i);
      Serial.println(zone_uuid);
      Serial.println(zone_name);
      Serial.println(zone_location_0, 6);
      Serial.println(zone_location_1, 6);
      Serial.println(nbSounds);
    }

    const char* zone_sound_uuid[nbSounds];
    const char* zone_sound_filename[nbSounds];
    const char* zone_sound_playEvent[nbSounds];
    const char* zone_sound_stopEvent[nbSounds];
    const char* zone_sound_zoneCondition[nbSounds];
    int zone_sound_fadein[nbSounds];
    int zone_sound_fadeout[nbSounds];
    bool zone_sound_loop[nbSounds];


    //Scans through all the sounds in this zone
    for(int j=0; j<nbSounds; j++){
      JsonObject zone_sound = zone_sounds[j];
      
      zone_sound_uuid[j] = zone_sound["uuid"]; // "346dab4a-c45d-41c6-a3f1-8c74e8b5d858", ...
      zone_sound_filename[j] = zone_sound["filename"]; // "POINT1PONT.WAV", "BOUCLE1.WAV"
      zone_sound_playEvent[j] = zone_sound["playEvent"]; // "enter", ...
      zone_sound_stopEvent[j] = zone_sound["stopEvent"]; // "enter_new_zone", "enter_new_zone"
      zone_sound_zoneCondition[j] = zone_sound["zoneCondition"]; // "any", "any"
      zone_sound_fadein[j] = zone_sound["fadein"]; // 0, 0
      zone_sound_fadeout[j] = zone_sound["fadeout"]; // 0, 1
      zone_sound_loop[j] = zone_sound["loop"]; // false, true

      if(LOG){
        Serial.println("Sound information :");
        Serial.println(zone_sound_filename[j]);
      }
    }

    
    if(zone_sounds.size()==1){
      Point *point1 = new Point(zone_location_0, zone_location_1, zone_radius, 0, zone_sound_filename[0], false, "");
      listePoints.add(point1);
    }
    else{
      Point *point1 = new Point(zone_location_0, zone_location_1, zone_radius, 0, zone_sound_filename[0], true, zone_sound_filename[1]);
      listePoints.add(point1);
    }
  
  }
}




/*JsonObject zones_0 = zones[0];
const char* zones_0_uuid = zones_0["uuid"]; // "45b435f5-d4c1-44da-ba84-24c2fe0873fb"
const char* zones_0_name = zones_0["name"]; // "Point 1 Pont"

float zones_0_location_0 = zones_0["location"][0]; // 48.11122
float zones_0_location_1 = zones_0["location"][1]; // -1.64611

int zones_0_radius = zones_0["radius"]; // 25
bool zones_0_visible = zones_0["visible"]; // true

for (JsonObject zones_0_sound : zones_0["sounds"].as<JsonArray>()) {

  const char* zones_0_sound_uuid = zones_0_sound["uuid"]; // "346dab4a-c45d-41c6-a3f1-8c74e8b5d858", ...
  const char* zones_0_sound_filename = zones_0_sound["filename"]; // "POINT1PONT.WAV", "BOUCLE1.WAV"
  const char* zones_0_sound_playEvent = zones_0_sound["playEvent"]; // "enter", ...
  const char* zones_0_sound_stopEvent = zones_0_sound["stopEvent"]; // "enter_new_zone", "enter_new_zone"
  const char* zones_0_sound_zoneCondition = zones_0_sound["zoneCondition"]; // "any", "any"
  int zones_0_sound_fadein = zones_0_sound["fadein"]; // 0, 0
  int zones_0_sound_fadeout = zones_0_sound["fadeout"]; // 0, 1
  bool zones_0_sound_loop = zones_0_sound["loop"]; // false, true

}

JsonObject zones_1 = zones[1];
const char* zones_1_uuid = zones_1["uuid"]; // "3d57071b-b7d9-436c-a5e7-fb6a6e71fbfc"
const char* zones_1_name = zones_1["name"]; // "Point 2 Escaliers"

float zones_1_location_0 = zones_1["location"][0]; // 48.11013
float zones_1_location_1 = zones_1["location"][1]; // -1.65205

int zones_1_radius = zones_1["radius"]; // 13
bool zones_1_visible = zones_1["visible"]; // true

for (JsonObject zones_1_sound : zones_1["sounds"].as<JsonArray>()) {

  const char* zones_1_sound_uuid = zones_1_sound["uuid"]; // "02367d07-fc6f-41e9-8245-1618056c7ff4", ...
  const char* zones_1_sound_filename = zones_1_sound["filename"]; // "POINT2ESCALIERS.WAV", "BOUCLE2.WAV"
  const char* zones_1_sound_playEvent = zones_1_sound["playEvent"]; // "enter", ...
  const char* zones_1_sound_stopEvent = zones_1_sound["stopEvent"]; // "enter_new_zone", "enter_new_zone"
  const char* zones_1_sound_zoneCondition = zones_1_sound["zoneCondition"]; // "any", "any"
  int zones_1_sound_fadein = zones_1_sound["fadein"]; // 0, 0
  int zones_1_sound_fadeout = zones_1_sound["fadeout"]; // 0, 1
  bool zones_1_sound_loop = zones_1_sound["loop"]; // false, true

}

JsonObject zones_2 = zones[2];
const char* zones_2_uuid = zones_2["uuid"]; // "7cc1cf73-a560-4af2-be1c-9d8dafd547f8"
const char* zones_2_name = zones_2["name"]; // "Point 3 Vertugadin"

float zones_2_location_0 = zones_2["location"][0]; // 48.10981
float zones_2_location_1 = zones_2["location"][1]; // -1.65271

int zones_2_radius = zones_2["radius"]; // 15
bool zones_2_visible = zones_2["visible"]; // true

for (JsonObject zones_2_sound : zones_2["sounds"].as<JsonArray>()) {

  const char* zones_2_sound_uuid = zones_2_sound["uuid"]; // "aec2498f-391e-4d88-92da-7a0401b24dc5", ...
  const char* zones_2_sound_filename = zones_2_sound["filename"]; // "POINT3VERTUGADIN.WAV", "BOUCLE3.WAV"
  const char* zones_2_sound_playEvent = zones_2_sound["playEvent"]; // "enter", ...
  const char* zones_2_sound_stopEvent = zones_2_sound["stopEvent"]; // "enter_new_zone", "enter_new_zone"
  const char* zones_2_sound_zoneCondition = zones_2_sound["zoneCondition"]; // "any", "any"
  int zones_2_sound_fadein = zones_2_sound["fadein"]; // 0, 0
  int zones_2_sound_fadeout = zones_2_sound["fadeout"]; // 0, 1
  bool zones_2_sound_loop = zones_2_sound["loop"]; // false, true

}

JsonObject zones_3 = zones[3];
const char* zones_3_uuid = zones_3["uuid"]; // "a8a5c64e-d408-4949-87d6-c338044acfc6"
const char* zones_3_name = zones_3["name"]; // "Point 4 Fin passerelle"

float zones_3_location_0 = zones_3["location"][0]; // 48.10925
float zones_3_location_1 = zones_3["location"][1]; // -1.65154

int zones_3_radius = zones_3["radius"]; // 15
bool zones_3_visible = zones_3["visible"]; // true

for (JsonObject zones_3_sound : zones_3["sounds"].as<JsonArray>()) {

  const char* zones_3_sound_uuid = zones_3_sound["uuid"]; // "9c7953c5-8e8e-4f12-a656-4a0b470e11f7", ...
  const char* zones_3_sound_filename = zones_3_sound["filename"]; // "POINT4FINPASSERELLE.WAV", ...
  const char* zones_3_sound_playEvent = zones_3_sound["playEvent"]; // "enter", ...
  const char* zones_3_sound_stopEvent = zones_3_sound["stopEvent"]; // "enter_new_zone", "enter_new_zone"
  const char* zones_3_sound_zoneCondition = zones_3_sound["zoneCondition"]; // "any", "any"
  int zones_3_sound_fadein = zones_3_sound["fadein"]; // 0, 0
  int zones_3_sound_fadeout = zones_3_sound["fadeout"]; // 0, 1
  bool zones_3_sound_loop = zones_3_sound["loop"]; // false, true

}

JsonObject zones_4 = zones[4];
const char* zones_4_uuid = zones_4["uuid"]; // "a9e60735-0ba9-4881-9425-7d0f1bde6a4e"
const char* zones_4_name = zones_4["name"]; // "Point 5 Jeux"

float zones_4_location_0 = zones_4["location"][0]; // 48.10878
float zones_4_location_1 = zones_4["location"][1]; // -1.65129

int zones_4_radius = zones_4["radius"]; // 20
bool zones_4_visible = zones_4["visible"]; // true

for (JsonObject zones_4_sound : zones_4["sounds"].as<JsonArray>()) {

  const char* zones_4_sound_uuid = zones_4_sound["uuid"]; // "5be76978-8c3f-48b4-83a6-595df91cc02e", ...
  const char* zones_4_sound_filename = zones_4_sound["filename"]; // "POINT5JEUX.WAV", "BOUCLE5.WAV"
  const char* zones_4_sound_playEvent = zones_4_sound["playEvent"]; // "enter", ...
  const char* zones_4_sound_stopEvent = zones_4_sound["stopEvent"]; // "enter_new_zone", "enter_new_zone"
  const char* zones_4_sound_zoneCondition = zones_4_sound["zoneCondition"]; // "any", "any"
  int zones_4_sound_fadein = zones_4_sound["fadein"]; // 0, 0
  int zones_4_sound_fadeout = zones_4_sound["fadeout"]; // 0, 1
  bool zones_4_sound_loop = zones_4_sound["loop"]; // false, true

}

JsonObject zones_5 = zones[5];
const char* zones_5_uuid = zones_5["uuid"]; // "f24f48d4-5d68-422e-9875-052654920687"
const char* zones_5_name = zones_5["name"]; // "Point 6 Transats"

float zones_5_location_0 = zones_5["location"][0]; // 48.11008
double zones_5_location_1 = zones_5["location"][1]; // -1.649258

int zones_5_radius = zones_5["radius"]; // 30
bool zones_5_visible = zones_5["visible"]; // true

for (JsonObject zones_5_sound : zones_5["sounds"].as<JsonArray>()) {

  const char* zones_5_sound_uuid = zones_5_sound["uuid"]; // "b62d9427-b751-476f-ba93-1c505ccf70e2", ...
  const char* zones_5_sound_filename = zones_5_sound["filename"]; // "POINT6TRANSATS.WAV", "BOUCLE6.WAV"
  const char* zones_5_sound_playEvent = zones_5_sound["playEvent"]; // "enter", ...
  const char* zones_5_sound_stopEvent = zones_5_sound["stopEvent"]; // "enter_new_zone", "exit"
  const char* zones_5_sound_zoneCondition = zones_5_sound["zoneCondition"]; // "any", "any"
  int zones_5_sound_fadein = zones_5_sound["fadein"]; // 0, 0
  int zones_5_sound_fadeout = zones_5_sound["fadeout"]; // 0, 1
  bool zones_5_sound_loop = zones_5_sound["loop"]; // false, true

}

JsonObject zones_6 = zones[6];
const char* zones_6_uuid = zones_6["uuid"]; // "44753557-e94c-443c-8750-bcabaea6adec"
const char* zones_6_name = zones_6["name"]; // "Point 7 Entrée grille blanche"

float zones_6_location_0 = zones_6["location"][0]; // 48.10989
float zones_6_location_1 = zones_6["location"][1]; // -1.64743

int zones_6_radius = zones_6["radius"]; // 15
bool zones_6_visible = zones_6["visible"]; // true

for (JsonObject zones_6_sound : zones_6["sounds"].as<JsonArray>()) {

  const char* zones_6_sound_uuid = zones_6_sound["uuid"]; // "97256ae9-bf04-4d50-a9c4-2e37c7c245bd", ...
  const char* zones_6_sound_filename = zones_6_sound["filename"]; // "POINT7ENTREEGRILLEBLANCHE.WAV", ...
  const char* zones_6_sound_playEvent = zones_6_sound["playEvent"]; // "enter", ...
  const char* zones_6_sound_stopEvent = zones_6_sound["stopEvent"]; // "enter_new_zone", "enter_new_zone"
  const char* zones_6_sound_zoneCondition = zones_6_sound["zoneCondition"]; // "any", "any"
  int zones_6_sound_fadein = zones_6_sound["fadein"]; // 0, 0
  int zones_6_sound_fadeout = zones_6_sound["fadeout"]; // 0, 1
  bool zones_6_sound_loop = zones_6_sound["loop"]; // false, true

}

JsonObject zones_7 = zones[7];
const char* zones_7_uuid = zones_7["uuid"]; // "cfad5fdc-9399-4b70-becc-4677f90a17c8"
const char* zones_7_name = zones_7["name"]; // "Point 8 Bancs sirène"

float zones_7_location_0 = zones_7["location"][0]; // 48.10892
float zones_7_location_1 = zones_7["location"][1]; // -1.64722

int zones_7_radius = zones_7["radius"]; // 15
bool zones_7_visible = zones_7["visible"]; // true

for (JsonObject zones_7_sound : zones_7["sounds"].as<JsonArray>()) {

  const char* zones_7_sound_uuid = zones_7_sound["uuid"]; // "88e9fbfc-c876-4f16-8a44-8c3138599620", ...
  const char* zones_7_sound_filename = zones_7_sound["filename"]; // "POINT8BANCSIRENE.WAV", "BOUCLE8.WAV"
  const char* zones_7_sound_playEvent = zones_7_sound["playEvent"]; // "enter", ...
  const char* zones_7_sound_stopEvent = zones_7_sound["stopEvent"]; // "enter_new_zone", "enter_new_zone"
  const char* zones_7_sound_zoneCondition = zones_7_sound["zoneCondition"]; // "any", "any"
  int zones_7_sound_fadein = zones_7_sound["fadein"]; // 0, 0
  int zones_7_sound_fadeout = zones_7_sound["fadeout"]; // 0, 1
  bool zones_7_sound_loop = zones_7_sound["loop"]; // false, true

}

JsonObject zones_8 = zones[8];
const char* zones_8_uuid = zones_8["uuid"]; // "a7d90180-ec3f-485d-9389-658fc4ee4e22"
const char* zones_8_name = zones_8["name"]; // "Point 9 Dernier croisement"

float zones_8_location_0 = zones_8["location"][0]; // 48.10863
float zones_8_location_1 = zones_8["location"][1]; // -1.64779

int zones_8_radius = zones_8["radius"]; // 15
bool zones_8_visible = zones_8["visible"]; // true

JsonObject zones_8_sounds_0 = zones_8["sounds"][0];
const char* zones_8_sounds_0_uuid = zones_8_sounds_0["uuid"]; // "fe11d0da-5649-43af-b8d3-cb6c672f85f0"
const char* zones_8_sounds_0_filename = zones_8_sounds_0["filename"]; // "POINT9DERNIERCROISEMENT.WAV"
const char* zones_8_sounds_0_playEvent = zones_8_sounds_0["playEvent"]; // "enter"
const char* zones_8_sounds_0_stopEvent = zones_8_sounds_0["stopEvent"]; // "enter_new_zone"
const char* zones_8_sounds_0_zoneCondition = zones_8_sounds_0["zoneCondition"]; // "any"
int zones_8_sounds_0_fadein = zones_8_sounds_0["fadein"]; // 0
int zones_8_sounds_0_fadeout = zones_8_sounds_0["fadeout"]; // 0
bool zones_8_sounds_0_loop = zones_8_sounds_0["loop"]; // false*/
