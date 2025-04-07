#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

const int SCREEN_W = 480;
const int SCREEN_H = 320;

// Telemetry (simulated)
float speed   = 0;  // km/h
float rpm     = 0;  // in thousands
int temp      = 0;  // °C
int fuel      = 0;  // %
int battery   = 0;  // %

void drawArcThick(int centerX, int centerY, int rOuter, int rInner,
                  int startAngle, int endAngle, uint16_t color) {
  float step = 1.0f; 
  for (float angle = startAngle; angle <= endAngle; angle += step) {
    float rad = angle * 0.0174532925; 
    float c = cos(rad);
    float s = sin(rad);

    int xOuter = centerX + (int)(rOuter * c);
    int yOuter = centerY + (int)(rOuter * s);
    int xInner = centerX + (int)(rInner * c);
    int yInner = centerY + (int)(rInner * s);

    tft.drawLine(xInner, yInner, xOuter, yOuter, color);
  }
}
 
void drawHorizontalBar(int x, int y, int w, int h, int percentage, 
                       uint16_t fillColor, uint16_t frameColor) {
  // Draw the border rectangle
  tft.drawRect(x, y, w, h, frameColor);
  // Fill portion
  int fillWidth = map(percentage, 0, 100, 0, w);
  tft.fillRect(x, y, fillWidth, h, fillColor);
}
 
/* Splash screen */
void showSplashScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setTextSize(3); 
  tft.drawString("BLUE FORCE ", tft.width() / 2 + 5, tft.height() / 2, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1); 
  tft.drawString(" v.1.0.0", tft.width() / 2 + 5, tft.height() / 1.5, 4);

  delay(3000); 
}
 
/* Main K3 Dashboard */
void drawK3Dashboard(float spd, float rpmK, int temperature, int fuelP, int battP) {
  tft.fillScreen(TFT_BLACK);

  // LEFT ARC (SPEED)
  int leftArcX = 120;  
  int leftArcY = SCREEN_H / 2;
  int outerRadius = 90;
  int innerRadius = 70;
  // Arc angles ~135° to 405°
  drawArcThick(leftArcX, leftArcY, outerRadius, innerRadius, 135, 405, TFT_DARKGREY);

  // Speed value (unchanged)
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3); 
  tft.drawString(String((int)spd), leftArcX, leftArcY - 10, 4); 
  tft.setTextSize(1);
  tft.drawString("km/h", leftArcX, leftArcY + 40, 2);

  //RIGHT ARC (RPM) 
  int rightArcX = 360; 
  int rightArcY = SCREEN_H / 2;
  drawArcThick(rightArcX, rightArcY, outerRadius, innerRadius, 135, 405, TFT_DARKGREY);

  // RPM value (unchanged)
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  char rpmBuf[6];
  dtostrf(rpmK, 2, 1, rpmBuf);
  tft.drawString(String(rpmBuf), rightArcX, rightArcY - 10, 4);
  tft.setTextSize(1);
  tft.drawString("x1000 rpm", rightArcX, rightArcY + 40, 2);

  //TOP: Temperature
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("TEMP: " + String(temperature) + " " + "C", SCREEN_W / 2, 50, 2);

  //WARNINGS BELOW TEMPERATURE
  int warnY = 60; 
  tft.setTextSize(1);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  bool anyWarning = false;
  
  // Low Fuel Warning
  if (fuelP < 10) {
    tft.drawString("Warning: Low Fuel!", SCREEN_W / 2, warnY, 2);
    warnY += 15;
    anyWarning = true;
  }
  // Low Battery Warning
  if (battP < 10) {
    tft.drawString("Warning: Low Battery!", SCREEN_W / 2, warnY, 2);
    warnY += 15;
    anyWarning = true;
  }
  // High Temperature Warning
  if (temperature > 100) {
    tft.drawString("Warning: High Temperature!", SCREEN_W / 2, warnY, 2);
    warnY += 15;
    anyWarning = true;
  }
  
  //BOTTOM: Fuel & Battery Bars 
  int barWidth = 200;
  int barHeight = 15;
  int barX = (SCREEN_W - barWidth) / 2;

  // Fuel Bar
  int fuelBarY = SCREEN_H - 60;
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);
  tft.drawString("FUEL", SCREEN_W / 2, fuelBarY - 2, 2);
  drawHorizontalBar(barX, fuelBarY, barWidth, barHeight, fuelP, TFT_GREEN, TFT_WHITE);

  // Display fuel percentage
  tft.setTextDatum(CC_DATUM);
  tft.drawString(String(fuelP) + "%", barX + barWidth + 10, fuelBarY + barHeight / 2, 2);

  // Battery Bar (below fuel bar)
  int battBarY = fuelBarY + 25;
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("BATT", SCREEN_W / 2, battBarY - 2, 2);
  drawHorizontalBar(barX, battBarY, barWidth, barHeight, battP, TFT_CYAN, TFT_WHITE);
  // Display battery percentage
  tft.setTextDatum(CC_DATUM);
  tft.drawString(String(battP) + "%", barX + barWidth + 10, battBarY + barHeight / 2, 2);

  //"BRAKE" if speed=0
  if ((int)spd == 0) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(BC_DATUM);
    tft.drawString("BRAKE", 70, 50, 2);
  }
}
 
// ------------------------------------------------------
/* SET - UP */
void setup() {
  tft.init();
  tft.setRotation(1); 
  showSplashScreen();
}
 
void loop() {
  // Simulate random data
  speed   = random(0, 201);             // 0..200 km/h
  rpm     = random(0, 1001) / 100.0f;     // 0.0..10.0 => up to 10k RPM
  temp    = random(60, 121);            // 60..120 °C
  fuel    = random(0, 101);             // 0..100 %
  battery = random(0, 101);             // 0..100 %

  drawK3Dashboard(speed, rpm, temp, fuel, battery);
  delay(1500);
}
