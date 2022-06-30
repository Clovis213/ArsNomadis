//Function for parsing of incoming GPS data
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
