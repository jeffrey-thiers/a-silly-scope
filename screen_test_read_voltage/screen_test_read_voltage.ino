#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

const int graphWidth = 320;
const int graphHeight = 200;
int dataPoints[graphWidth]; // Array to store past voltage readings
int xPos = 0;
const int bufferSize = 320; 
int samples[bufferSize];


void setup() {
  tft.begin();
  tft.setRotation(1); // Landscape
  tft.fillScreen(ILI9341_BLACK);

  // This increases sampling from ~9.6kHz to ~77kHz
  // ADPS2=1, ADPS1=0, ADPS0=0 sets the ADC prescaler to 16
  ADCSRA &= ~(1 << ADPS2); // Clear ADPS2
  ADCSRA &= ~(1 << ADPS1); // Clear ADPS1
  ADCSRA &= ~(1 << ADPS0); // Clear ADPS0
  ADCSRA |= (1 << ADPS2);  // Set ADPS2 to 1 for prescaler 16
  
  // Initialize the array with 0
  for (int i = 0; i < graphWidth; i++) {
    dataPoints[i] = graphHeight; // Bottom of graph
  }

  pinMode(3, OUTPUT);
  analogWrite(3, 64);
}


void loop() {
  // Fast sampling burst: filling the buffer at max speed
  for (int i = 0; i < bufferSize; i++) {
    samples[i] = analogRead(A0);
  }

  // Rapidly plot the buffer to the screen
  tft.fillScreen(ILI9341_BLACK); // Optional: clear once per burst
  for (int i = 0; i < bufferSize - 1; i++) {
    int y0 = map(samples[i], 0, 1023, 200, 0);
    int y1 = map(samples[i+1], 0, 1023, 200, 0);
    tft.drawLine(i, y0, i + 1, y1, ILI9341_GREEN);
  }

  // // Display numeric voltage at the top
  // tft.setCursor(10, 210);
  // tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  // tft.setTextSize(2);
  // tft.print("Voltage: ");
  // tft.print(raw * (5.0 / 1023.0));
  // tft.print("V  ");

  // delay(20); // Adjust for desired scroll speed
}

