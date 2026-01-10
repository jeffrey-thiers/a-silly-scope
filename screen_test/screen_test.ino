#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Define pins for the Nano
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8

// Initialize the display using hardware SPI
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  Serial.println("ILI9341 Test");
  tft.begin();
  
  // Set orientation (0-3)
  tft.setRotation(0); 
  
  // Clear screen
  tft.fillScreen(ILI9341_BLACK);
  
  // Write "Hello World"
  tft.setCursor(20, 100);       // (x, y) coordinates
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(3);          // Font size 1-5
  tft.println("Hello World!");
  
  // Write colored text
  tft.setCursor(3, 3);
  tft.setTextColor(ILI9341_PINK);
  tft.setTextSize(2);
  tft.println("My name is Jef");

  // Draw a red horizontal line
  tft.drawLine(0, 160, 240, 160, ILI9341_RED); 

// Draw a green diagonal line
  tft.drawLine(0, 0, 240, 320, ILI9341_GREEN); 

// Draw a blue vertical line
  tft.drawLine(120, 0, 120, 320, ILI9341_BLUE);
}

void loop() {
  // Empty loop
}
