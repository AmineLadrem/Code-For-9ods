#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);

char address[6] = "00010";
bool inWriteMode = true;

void setup() {
  Serial.begin(9600); 
  radio.begin();
  setRadioAddress(address); 
  radio.setPALevel(RF24_PA_MIN);
  if (!inWriteMode) {
    radio.startListening(); 
  }
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readString();
    if (command.startsWith("w")) {
      inWriteMode = true;
      radio.stopListening(); 
      Serial.println("Switched to write mode.");
    } else if (command.startsWith("r")) {
      inWriteMode = false;
      radio.startListening(); 
      Serial.println("Switched to read mode.");
    } else if (command.startsWith("setadd")) {
      command = command.substring(10);
      setRadioAddress(command.c_str());
      Serial.println("Address set to: " + command);
    } else if (inWriteMode) {
      const char* text = command.c_str();
      radio.write(text, strlen(text)); 
      Serial.println("Message sent: " + command);
    }
  }

  if (!inWriteMode && radio.available()) {
    char receivedText[32];
    radio.read(receivedText, sizeof(receivedText)); 
    Serial.print("Received message: ");
    Serial.println(receivedText);
  }
}

void setRadioAddress(const char* newAddress) {
  if (strlen(newAddress) == 5) {
    strncpy(address, newAddress, 5);
    address[5] = '\0';
    radio.openWritingPipe(address);
    radio.openReadingPipe(1, address);
  }
}