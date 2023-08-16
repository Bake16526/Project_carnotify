#include <TinyGPS++.h>
#include <HardwareSerial.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>
#include <TridentTD_LineNotify.h>


#define BLYNK_TEMPLATE_ID           "TMPL64oAat5VX"
#define BLYNK_TEMPLATE_NAME         "CarNotify"
#define BLYNK_AUTH_TOKEN            "i8oDNdS2tQIfPl_nFGpPtjVo2r8zLUnW"
#define LINE_TOKEN  "2dsgd0ZJvw2XCtIW5xNgEQC0cn5qmCPl8X5uxM3KsRh"




char ssid[] = "Bake.iPhone";
char pass[] = "noem9615";

#define RXPin (27)
#define TXPin (26)

static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
WidgetMap myMap(V6);




float speeding;
float state;
unsigned int move_index = 1;
const int vibration = 2; // แรงสั่นสะเทือน
bool statusApp = false;
// The serial connection to the GPS device
HardwareSerial ss(2);

void setup(){
  WiFi.begin(ssid, pass);
  Serial.printf("WiFi connecting to %s\n",  ssid);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());  

  Serial.println(LINE.getVersion());
  Serial.begin(115200);
  ss.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin, false);
  Serial.println(TinyGPSPlus::libraryVersion());
  pinMode(vibration, INPUT);

  LINE.setToken(LINE_TOKEN);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop(){

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
    displayInfo();

    if (millis() > 5000 && gps.charsProcessed() < 10){
      Serial.println(F("No GPS detected: check wiring."));
    while(true);
    }
    if(statusApp)
    chkVibration();

    


    Blynk.run();
    delay(500);  
}

void chkVibration(){
  Serial.println(digitalRead(vibration));
        if (digitalRead(vibration) != 0){
        Serial.println("Sensor ON");
        LINE.notify("มีการสั่นสะเทือน");
        delay(3000);

        } else {
        Serial.println("Sensor OFF");
        }

}


BLYNK_WRITE(V0) {
  Serial.println(param.asInt());
    if(param.asInt()) {
      //LEDBlynk.on();
      statusApp = true;       
    }else{
      //LEDBlynk.off();
      statusApp = false;
    }
}

void displayInfo(){

    float latitude = (gps.location.lat());
    float longitude =  (gps.location.lng());

    


  Serial.print(F("Location: "));
  if (gps.location.isValid()){
  Serial.print(latitude, 6);
  Serial.print(F(","));
  Serial.print(longitude, 6);

  Blynk.virtualWrite(V3, String(latitude, 6));
  Blynk.virtualWrite(V1, String(longitude, 6));
  myMap.location(move_index, latitude, longitude, "GPS_Location");

  speeding = gps.speed.kmph();
  Blynk.virtualWrite(V9, speeding);
  Serial.println("");
  Serial.print("speed :");
  Serial.println(speeding);


  if(statusApp == true){
      chkVibration();
        if (speeding >= 3){
        Serial.println(speeding);
        LINE.notify("รถมีการเคลื่อนที่555รถหายแล้ว!!!!!");

    }
}
    

  Serial.println(digitalRead(speeding));
  state = gps.satellites.value();
  Blynk.virtualWrite(V7, state);
  }else{
    Serial.print(F("INVALID"));
  }
  Serial.println();
}


