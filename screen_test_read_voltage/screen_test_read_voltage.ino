#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
#define ANALOG_PIN A0

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

const int bufferSize = 320; // One point for every pixel of width
int samples[bufferSize];
int oldSamples[bufferSize];
float max_voltage = 5;


void setup() {
  Serial.begin(9600); // Set baud rate (bits per second)
  
  tft.begin(8000000L); // Force max SPI speed (8MHz) for Nano
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  
  // Speed up ADC sampling (Prescaler 16 = ~77kHz)
  ADCSRA = (ADCSRA & 0xf8) | 0x04; 

  // set pwm pin for testing
  pinMode(3, OUTPUT);
  analogWrite(3,127);

  drawGrid(/*num_divisions*/ 6, /*max_voltage*/ max_voltage);
  tft.setCursor(1, 225);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("Input max: " + String(max_voltage) + "V");
}


void loop() {
  capture_data();
}


void drawGrid(int num_div, float max_voltage) {
  int pix_per_div = 240/num_div;
  int cursor_y = -1;//some reason it can't plot at certain even values..
  float voltage_step = max_voltage/(num_div-2);
  for(int i = 0; i < num_div-1; i += 1){
    cursor_y += pix_per_div;
    //Serial.print("drawing line at: ");
    //Serial.println(cursor);
    tft.drawFastHLine(0, cursor_y, 320, ILI9341_RED);
    tft.setCursor(1, cursor_y - 8);
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.setTextSize(1);
    tft.print(String(max_voltage) + "V");
    max_voltage-=voltage_step;
  }
}


void capture_data(){
  // 1. Trigger: Wait for rising edge to stabilize wave
  while(analogRead(ANALOG_PIN) > 100); // Wait for signal to be LOW
  while(analogRead(ANALOG_PIN) < 500); // Wait for signal to go HIGH

  // 2. High-Speed Capture
  for (int i = 0; i < bufferSize; i++) {
    samples[i] = analogRead(ANALOG_PIN);
  }

  // 3. Draw & Erase (Avoid fillScreen to stop flickering)
  for (int i = 0; i < bufferSize - 1; i++) {
    // Erase old line
    int yOld0 = map(oldSamples[i], 0, 1023, 200, 40);
    int yOld1 = map(oldSamples[i+1], 0, 1023, 200, 40);
    tft.drawLine(i, yOld0, i + 1, yOld1, ILI9341_BLACK);

    // Draw new line
    int yNew0 = map(samples[i], 0, 1023, 200, 40);
    int yNew1 = map(samples[i+1], 0, 1023, 200, 40);
    tft.drawLine(i, yNew0, i + 1, yNew1, ILI9341_YELLOW);
    
    // Store for next erase cycle
    oldSamples[i] = samples[i];
  } 
}
