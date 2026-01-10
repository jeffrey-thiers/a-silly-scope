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

  // Draw grid
  // horizontal
  tft.drawLine(0, 80, 240, 80, ILI9341_RED);
  tft.drawLine(0, 160, 240, 160, ILI9341_RED);
  tft.drawLine(0, 240, 240, 240, ILI9341_RED);
  // vertical
  tft.drawLine(60, 0, 60, 320, ILI9341_RED);
  tft.drawLine(120, 0, 120, 320, ILI9341_RED);
  tft.drawLine(180, 0, 180, 320, ILI9341_RED);
  //border
  tft.drawLine(2, 2, 2, 318, ILI9341_RED);
  tft.drawLine(2, 318, 238, 318, ILI9341_RED);
  tft.drawLine(238, 2, 238, 318, ILI9341_RED);
  tft.drawLine(238, 2, 2, 2, ILI9341_RED);



}

void loop() {
  // Empty loop
}
