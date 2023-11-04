#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define SIM_RX 5
#define SIM_TX 4
#define BUTTON_PIN 2

#define GPS_RX 9
#define GPS_TX 8

SoftwareSerial gpsM(GPS_RX, GPS_TX);
TinyGPSPlus gps;

SoftwareSerial SIM(SIM_RX, SIM_TX);

// Variables
String data;
double lat = 0, lng = 0;
int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
boolean locationfound = false;
boolean buttonPressed = false;

// Functions/Procedures
void SIM_init();
void SIM_respond();
String get_location(double lat, double lng, int year, int month, int day, int hour, int minute, int second);
void sendLocationViaSMS();

void setup() {
  Serial.begin(9600);

  gpsM.begin(9600);
  Serial.println("Init GPS...");
  delay(500);

  SIM.begin(9600);
  Serial.println("Init SIM...");
  delay(500);

  SIM_init();

  pinMode(BUTTON_PIN, INPUT_PULLUP);  
}

void loop() {
  gpsM.listen();
  while (gpsM.available() > 0) {
    gps.encode(gpsM.read());
    if (gps.location.isUpdated()) {
      locationfound = true;
      lat = gps.location.lat();
      lng = gps.location.lng();
      Serial.print(lat, 8);
      Serial.print("       ");
      Serial.println(lng, 8);
    }
  }

 
  if (digitalRead(BUTTON_PIN) == HIGH) {
    if (!buttonPressed) {
      buttonPressed = true;
      sendLocationViaSMS();
    }
  } else {
    buttonPressed = false;
  }
}

void SIM_respond() {
  while (SIM.available()) {
    Serial.write(SIM.read());
  }
}

void SIM_init() {
  mySerial.println("AT");
  mySerial.println("AT+CSQ");
  mySerial.println("AT+CCID");
  mySerial.println("AT+CREG?");
  mySerial.println("AT+CMGF=1");
}

String get_location(double lat, double lng, int year, int month, int day, int hour, int minute, int second) {
   String data ;
  String slat = String(lat, 8);
  String slng = String(lng, 8);
  String syear = String(year);  
  String smonth = String(month);  
  String sday = String(day);  
  String shour = String(hour);  
  String sminute = String(minute);  
  String ssecond = String(second);  

  data = "{";

  data += "\"lat\":\"" + slat + "\",";
  data += "\"lng\":\"" + slng + "\",";
  data += "\"timestamp\":\"" + syear + "-" + smonth + "-" + sday + "   " + shour + ":" + sminute + ":" + ssecond + " \"";
    
  data += "}";

  return data;  
}

void sendLocationViaSMS() {
  if (locationfound) {
    String locationSMS = "EMERGENCY: http://maps.google.com/maps?q=" + String(lat, 6) + "," + String(lng, 6);
    SIM.print("AT+CMGS=\"911\"\r");  
    delay(1000);
    SIM.print(locationSMS);
    SIM.write(0x1A);
    delay(5000); 
  }
}