#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "RTClib.h"

#define TFT_CS    10
#define TFT_RST   6 
#define TFT_DC    7 
#define TFT_SCLK 13   
#define TFT_MOSI 11

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Declared Variables
int pushButton1 = 0;
int pushButton2 = 0;
int pushButton3 = 0;
int userInput = 0;
const int ledPin = 5; // LED connected to pin 5
const int lightThreshold = 200; // Light threshold for turning on the LED

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); // Set LED pin as output

  tft.initR(INITR_BLACKTAB); 
  tft.fillScreen(ST77XX_BLACK); 
  tft.setRotation(1); 

  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); 
  tft.setTextSize(1); 

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC! Check connections.");
  }

  // Seed random number generator for sensor values
  randomSeed(analogRead(0));
}

void loop() {
  displayMainMenu();
  userInput = waitforInput();

  switch(userInput) {
    case 1:
      readBME680Data();
      break;
    case 2:
      readBH1750Data();
      break;
    default:
      break;
  }
}

// Function to read data from BME680 Sensor
void readBME680Data() {
  while (true) {
    float humidity = random(0, 101);      // 0 to 100 %
    float temperature = random(0, 51);    // 0 to 50 C

    displayDataOnLCD(0, humidity, temperature, true);
    displayGraph(0, humidity, temperature, true);

    if (checkReturn()) {
      break;
    }

    delay(1000);  // Update every second
  }
}

// Function to read data from BH1750 Sensor
void readBH1750Data() {
  while (true) {
    float lightLevel = random(0, 1001);  // 0 to 1000 Lux

    displayDataOnLCD(lightLevel, 0, 0, false);
    displayGraph(lightLevel, 0, 0, false);

    if (lightLevel < lightThreshold) {
      digitalWrite(ledPin, HIGH); // Turn on the LED
    } else {
      digitalWrite(ledPin, LOW); // Turn off the LED
    }

    if (checkReturn()) {
      break;
    }

    delay(1000);  // Update every second
  }
}

// Function to display heading
void displayDataOnLCD(float light, float humidity, float temperature, bool isBME680) {
  tft.fillRect(0, 0, 128, 160, ST77XX_BLACK); // Clear the screen

  tft.setCursor(10, 20); 
  tft.println("Sensor Data:");
}

// Function to display the graph on the LCD
void displayGraph(float light, float humidity, float temperature, bool isBME680) {
  tft.fillRect(0, 100, 128, 60, ST77XX_BLACK);

  if (isBME680) {
    tft.setCursor(10, 35);
    tft.print("Humidity: ");
    tft.print(humidity);
    tft.println(" %");
    tft.drawRect(10, 45, 100, 10, ST77XX_WHITE);
    tft.fillRect(10, 45, map(humidity, 0, 100, 0, 100), 10, ST77XX_BLUE);

    tft.setCursor(10, 65);
    tft.print("Temperature: ");
    tft.print(temperature);
    tft.println(" C");
    tft.drawRect(10, 75, 100, 10, ST77XX_WHITE);
    tft.fillRect(10, 75, map(temperature, 0, 50, 0, 100), 10, ST77XX_RED);
  } else {
    tft.setCursor(10, 35);
    tft.print("Light:  ");
    tft.print(light);
    tft.println(" Lux");
    tft.drawRect(10, 45, 100, 10, ST77XX_WHITE);
    tft.fillRect(10, 45, map(light, 0, 1000, 0, 100), 10, ST77XX_GREEN);
  }
}

// Function to get current time for RTC
time_t getCurrentTime() {
  rtc.adjust(DateTime(2024, 5, 17, 13, 25, 0));
  DateTime now = rtc.now();
  return now.unixtime();
}

// Function to display main menu on LCD
void displayMainMenu() {
  tft.fillRect(0, 0, 128, 160, ST77XX_BLACK); // Clear the screen

  tft.setCursor(10, 20); 
  tft.println("Greenhouse Detector:");

  tft.setCursor(10, 40);
  tft.println("1. BME680 Sensor");

  tft.setCursor(10, 60);
  tft.println("2. BH1750 Sensor");
}

// To switch from main menu to other sensor pages thru pushbuttons
int waitforInput() {
  while (true) {
    pushButton1 = digitalRead(2);
    pushButton2 = digitalRead(3);

    if (pushButton1 == LOW) {
      return 1;
    }

    if (pushButton2 == LOW) {
      return 2;
    }
  }
}

// Check for return button press
bool checkReturn() {
  pushButton3 = digitalRead(4);

  if (pushButton3 == LOW) {
    while (digitalRead(4) == LOW);  // Wait for button release
    return true;
  }
  return false;
}

// Wait for a button press to return to the main menu
void waitForReturn() {
  while (true) {
    if (checkReturn()) {
      break;
    }
  }
}
