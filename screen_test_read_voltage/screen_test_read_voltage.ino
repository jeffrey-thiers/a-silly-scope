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
int grid_height = grid_bound_bottom - grid_bound_top;
int pot_value = 0;
int last_raw = 0;
int last_grid_bound_top = 10;
int last_grid_bound_bottom = 210;


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

  //Draw grid
  //drawGrid(/*num_divisions*/ 4, /*max_voltage*/ max_voltage, "ILI9341_RED");
  tft.setCursor(1, 225);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("max V: " + String(max_voltage) + "V");
}


void loop() {
  pot_value = getSmoothReading(A6);
  updateGridBounds();
  capture_data(pot_value);
}


void drawGrid(int num_div, float max_voltage, uint16_t color) {
  int pix_per_div = grid_height/num_div;
  int cursor_y = grid_bound_top - 1;//some reason it can't plot at certain even values..
  float voltage_step = max_voltage/num_div;
  for(int i = 0; i <= num_div; i += 1){
    //Serial.print("drawing line at: ");
    //Serial.println(cursor_y);
    tft.drawFastHLine(0, cursor_y, 320, color);
    tft.setCursor(1, cursor_y - 8);
    tft.setTextColor(color, ILI9341_BLACK);
    tft.setTextSize(1);    
    tft.print(String(max_voltage - i*voltage_step) + "V");
    cursor_y += pix_per_div;
  }
}


void capture_data(int time_scale){
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
    //int yOld0 = map(oldSamples[i], 0, 1023, last_grid_bound_bottom, last_grid_bound_top);
    //int yOld1 = map(oldSamples[i+1], 0, 1023, last_grid_bound_bottom, last_grid_bound_top);
    //tft.drawLine(i, yOld0, i + 1, yOld1, ILI9341_BLACK);
    tft.drawLine(i, oldY[i], i + 1, oldY[i+1], ILI9341_BLACK);

    // Draw new line
    int yNew0 = map(samples[i], 0, 1023, grid_bound_bottom, grid_bound_top);
    int yNew1 = map(samples[i+1], 0, 1023, grid_bound_bottom, grid_bound_top);
    tft.drawLine(i, yNew0, i + 1, yNew1, ILI9341_YELLOW);
    
    // Store for next erase cycle
    //oldSamples[i] = samples[i];
    oldY[i] = yNew0;
    if (i == bufferSize - 2) oldY[i+1] = yNew1; // Store the very last point
  }
}

void capture_data(){
    // 1. Trigger: Wait for rising edge to stabilize wave
  while(analogRead(ANALOG_PIN) > 100); // Wait for signal to be LOW
  while(analogRead(ANALOG_PIN) < 500); // Wait for signal to go HIGH

  // 2. High-Speed Capture
  unsigned long start_time = micros();
  for (int i = 0; i < bufferSize; i++) {
    samples[i] = analogRead(ANALOG_PIN);
    //delayMicroseconds(time_scale);
  }

  unsigned long delta_time = micros() - start_time;
  tft.setCursor(190, 225);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.print("dt: " + String(delta_time / 1000.0, 2) + "ms"); // Convert to milliseconds
}

void erase_data(){
  for (int i = 0; i < bufferSize - 1; i++) {
    // Erase old line
    int yOld0 = map(oldSamples[i], 0, 1023, last_grid_bound_bottom, last_grid_bound_top);
    int yOld1 = map(oldSamples[i+1], 0, 1023, last_grid_bound_bottom, last_grid_bound_top);
    tft.drawLine(i, yOld0, i + 1, yOld1, ILI9341_BLACK);
  }
}

void draw_data(){
  for (int i = 0; i < bufferSize - 1; i++) {
    // Draw new line
    int yNew0 = map(samples[i], 0, 1023, grid_bound_bottom, grid_bound_top);
    int yNew1 = map(samples[i+1], 0, 1023, grid_bound_bottom, grid_bound_top);
    tft.drawLine(i, yNew0, i + 1, yNew1, ILI9341_YELLOW);
    oldSamples[i] = samples[i];
  }
}


int getSmoothReading(int pin) {
  long sum = 0;
  int numReadings = 10; // Increase this for more smoothing
  for (int i = 0; i < numReadings; i++) {
    sum += analogRead(pin);
    delayMicroseconds(50); // Small delay for ADC stability
  }
  return map((sum / numReadings), 0, 1023, 0, 100);
}


void updateGridBounds() {  
  int raw = analogRead(A5);

  // Use a "Deadzone" or Hysteresis (e.g., a change of at least 3)
  // This prevents the screen from flickering due to tiny noise fluctuations
  if (abs(raw - last_raw) > 5) {
    last_raw = raw;
    //erase current grid
    drawGrid(/*num_divisions*/ 4, /*max_voltage*/ max_voltage, /*color*/ ILI9341_BLACK);
    
    //store previous variables for erasing other data.
    last_grid_bound_top = grid_bound_top;
    last_grid_bound_bottom = grid_bound_bottom;
    
    // Map to your desired range (e.g., 0-100)
    grid_bound_top = map(raw, 0, 1023, 120, -120);
    grid_bound_bottom = map(raw, 0, 1023, 210, 330);
    
    // Update the UI only when the value actually changes
    drawGrid(/*num divisions*/ 4, /*max voltage*/ max_voltage, /*color*/ ILI9341_RED);
  }
}




