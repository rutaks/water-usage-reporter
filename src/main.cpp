#include "SIM900.h"
#include "inetGSM.h"
#include <SoftwareSerial.h>

InetGSM inet;

int i=0;
char msg[250];
String result ="";
boolean started=false;
boolean canRead=false;

const int greenLED = 12;
const int redLED = 11;

int respin = A5;
int resval = 0;

/**
 * Configures GSM Module & Serial display
*/
void configureDevice () {
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  Serial.println("Setting up Serial port display...");
  Serial.begin(9600);
  
  Serial.println("Setting up GSM Device...");
  
  //Start configuration of shield with baudrate.
  //For http uses is recommended to use 4800 or slower.
  if (gsm.begin(2400)) {
    Serial.println("\nGSM status=READY");
    started=true;
  } else Serial.println("\nGSM status=IDLE");

  if(started) {
    //GPRS attach, put in order APN, username and password.
    //If no needed auth let them blank.
    if (inet.attachGPRS("internet.mtn", "", "")) Serial.println("GSM status=ATTACHED");
    else Serial.println("GSM status=ERROR, could not attach GRPS");
    
    delay(1000);

    //Read IP address.
    gsm.SimpleWriteln("AT+CIFSR");
    
    delay(5000);
    
    //Read until serial buffer is empty.
    gsm.WhileSimpleRead();
    canRead=true;
  }
}

/**
 * Send current device's water level percentage
*/
void sendCurrentWaterLevel () {
  inet.httpGET("<BASE_URL>", 80, "<PATH>", msg, 250);
}

String calculateWaterLevel() {
  resval = analogRead(respin);
  Serial.println(resval);

  if(resval <= 100) {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    return "LOW";
  } else if(resval > 100 && resval <= 200) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, HIGH);    
    return "MID";
  } else if(resval > 295) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    return "HIGH";
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    return "ERROR";
  }
}

void setup() {
  configureDevice();
};

void loop() {
  if(canRead) {
    String status = calculateWaterLevel();

    if(status == "LOW") {
      Serial.println("It is low");
      sendCurrentWaterLevel();
    } else if(status == "MID") {
      Serial.println("It is in the middle");
    } else if(status == "HIGH") {
      Serial.println("It is in the High");
    }
    delay(5000);

  } else {
    Serial.println("ERROR: FAILED TO CONNECT TO INTERNET");
    delay(5000);
  }
};