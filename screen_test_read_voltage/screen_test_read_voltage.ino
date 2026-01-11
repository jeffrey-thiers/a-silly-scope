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
int oldY[bufferSize];
float max_voltage = 5;
int grid_bound_top =10;
int grid_bound_bottom = 210;
int pot_value = 0;
int last_raw_A5 = 0;
int last_raw_A6 = 0;
int time_scale = 0;


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
}


void loop() {
  updateGridBounds();
  updateTimeBounds();
  drawGrid(/*num divisions*/ 4, /*max voltage*/ max_voltage, /*color*/ ILI9341_RED);
  capture_data();
}


void drawGrid(int num_div, float max_voltage, uint16_t color) {
  int pix_per_div = (grid_bound_bottom - grid_bound_top)/num_div;
  int cursor_y = grid_bound_top - 1;//some reason it can't plot at certain even values..
  float voltage_step = max_voltage/num_div;
  for(int i = 0; i <= num_div; i += 1){
    
    //Serial.print("grid bound top: ");
    //Serial.println(grid_bound_top);
    //Serial.print("grid bound bottom: ");
    //Serial.println(grid_bound_bottom);

    tft.drawFastHLine(0, cursor_y, 320, color);
    tft.setCursor(1, cursor_y - 8);
    tft.setTextColor(color, ILI9341_BLACK);
    tft.setTextSize(1);    
    tft.print(String(max_voltage - i*voltage_step) + "V");
    cursor_y += pix_per_div;
  }
  
  // write some data on the screen
  tft.setCursor(1, 225);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("max V: " + String(max_voltage) + "V");
}


void capture_data(){
  // 1. Trigger: Wait for rising edge to stabilize wave
  while(analogRead(ANALOG_PIN) > 100); // Wait for signal to be LOW
  while(analogRead(ANALOG_PIN) < 500); // Wait for signal to go HIGH

  // 2. High-Speed Capture
  unsigned long start_time = micros();
  for (int i = 0; i < bufferSize; i++) {
    samples[i] = analogRead(ANALOG_PIN);
    delayMicroseconds(time_scale);
  }

  unsigned long delta_time = micros() - start_time;
  tft.setCursor(190, 225);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("dt: " + String(delta_time / 1000.0, 2) + "ms"); // Convert to milliseconds

  // 3. Draw & Erase (Avoid fillScreen to stop flickering)
  for (int i = 0; i < bufferSize - 1; i++) {
    // Erase old line
    tft.drawLine(i, oldY[i], i + 1, oldY[i+1], ILI9341_BLACK);

    // Draw new line
    int yNew0 = map(samples[i], 0, 1023, grid_bound_bottom, grid_bound_top);
    int yNew1 = map(samples[i+1], 0, 1023, grid_bound_bottom, grid_bound_top);
    tft.drawLine(i, yNew0, i + 1, yNew1, ILI9341_YELLOW);
    
    // Store for next erase cycle
    oldY[i] = yNew0;
    if (i == bufferSize - 2) oldY[i+1] = yNew1; // Store the very last point
  }
}


void updateTimeBounds() {  
  int raw = analogRead(A6);

  // Use a "Deadzone" or Hysteresis (e.g., a change of at least 3)
  // This prevents the screen from flickering due to tiny noise fluctuations
  if (abs(raw - last_raw_A6) > 5) {
    last_raw_A6 = raw;
    time_scale = map(raw, 0, 1023, 0, 120);
  }
}


void updateGridBounds() {  
  int raw = analogRead(A5);

  // Use a "Deadzone" or Hysteresis (e.g., a change of at least 3)
  // This prevents the screen from flickering due to tiny noise fluctuations
  if (abs(raw - last_raw_A5) > 10) {
    last_raw_A5 = raw;
    //erase current grid
    drawGrid(/*num_divisions*/ 4, /*max_voltage*/ max_voltage, /*color*/ ILI9341_BLACK);  
    
    // Map to your desired range (e.g., 0-100)
    grid_bound_top = map(raw, 0, 1023, 30, -120);
    grid_bound_bottom = map(raw, 0, 1023, 190, 340);
  }
}




