#define BLYNK_TEMPLATE_ID "TMPL6y_OzZz73"
#define BLYNK_TEMPLATE_NAME "PIR"

#define BLYNK_PRINT Serial
#define APP_DEBUG
#define USE_NODE_MCU_BOARD
#include <ESP8266WiFi.h>  // Include the ESP8266 Wi-Fi library
#include <BlynkSimpleEsp8266.h>

// Replace with your Wi-Fi credentials
char ssid[] = "T&B 305";
char password[] = "8888a8888";

BlynkTimer timer;
int timerID1, timerID2;
int pirPin = D1;       // HC-SR501 PIR sensor pin
int buzzerPin = D5;    // Buzzer pin
int ledMode = D6;      // LED to indicate the alarm status
int alarmThreshold = 1; // Number of consecutive detections required to trigger the alarm
int pirState = LOW;
int pirDetectedCount = 0;
boolean alarmState = false;

#define PIRSENSOR V1
#define ALARMSTATE V2

void setup() {
  Serial.begin(115200);
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledMode, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledMode, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.config("XpmqWRo3HFDfmRoVoKDoNMUHPrZHXfpc"); // Replace with your Blynk authentication token
  timerID1 = timer.setInterval(1000L, handleTimerID1);
}

void loop() {
  Blynk.run();
  timer.run();
}

void handleTimerID1() {
  pirState = digitalRead(pirPin);
  Blynk.virtualWrite(PIRSENSOR, pirState);
  
  if (pirState == HIGH) {
    pirDetectedCount++;
    if (pirDetectedCount >= alarmThreshold && !alarmState) {
      triggerAlarm();
    }
  } else {
    pirDetectedCount = 0;
    if (alarmState) {
      resetAlarm();
    }
  }
}

void triggerAlarm() {
  alarmState = true;
  Blynk.virtualWrite(ALARMSTATE, HIGH);
  digitalWrite(buzzerPin, HIGH);
  digitalWrite(ledMode, HIGH);
  Blynk.logEvent("burglar_alarm", "Motion detected: Intruder alert!");
  Serial.println("Intruder alert! Burglar alarm triggered!");
}

void resetAlarm() {
  alarmState = false;
  Blynk.virtualWrite(ALARMSTATE, LOW);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledMode, LOW);
  Blynk.logEvent("burglar_alarm", "Alarm reset: All clear!");
  Serial.println("Alarm reset: All clear!");
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(PIRSENSOR, ALARMSTATE);
}

BLYNK_WRITE(ALARM_STATE) {
  int value = param.asInt();
  if (value == 1) {
    triggerAlarm();
  } else {
    resetAlarm();
  }
}