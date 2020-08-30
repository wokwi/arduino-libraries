//Metaballs
//16x16 rgb led matrix demo
//Yaroslaw Turbin 20.07.2020
//https://vk.com/ldirko
//https://www.reddit.com/user/ldirko/
#include "FastLED.h"

// Matrix size
#define NUM_ROWS 16
#define NUM_COLS 16
#define NUM_LEDS (NUM_ROWS * NUM_COLS)

enum XY_cfg {LINEAR = 0, SERPENTINE = 1, COLUMNMAJOR = 0,
             ROWMAJOR = 2, FLIPMAJOR = 4, FLIPMINOR = 8
            };
#define XY_MATRIX         (SERPENTINE | ROWMAJOR /*| FLIPMINOR*/)
#define kMatrixWidth      NUM_ROWS
#define kMatrixHeight     NUM_COLS


// LEDs pin
#define DATA_PIN 2

// LED brightness
#define BRIGHTNESS 16

// Define the array of leds
CRGB leds[NUM_LEDS + 1];
uint8_t buffer[NUM_LEDS];
uint8_t divide[256];

DEFINE_GRADIENT_PALETTE( lava) {
  0,      255,  30,  0,
  32,     0,    0,   0,
//  64,     0,    0,   0,
  128,    90,   90,  0,
  150,    180,  180, 0,
  255,    255,  50,  0
};

CRGBPalette16 myPal = lava;

void setup() {
  Serial.begin(250000);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(DISABLE_DITHER);
  for (uint16_t i = 0; i < 256; i++)
    divide[i] = 200 / (i + 1);
}


void loop() {

  uint8_t bx1 = beatsin8(15, 0, NUM_COLS - 1, 0, 0);
  uint8_t by1 = beatsin8(18, 0, NUM_ROWS - 1, 0, 0);
  uint8_t bx2 = beatsin8(28, 0, NUM_COLS - 1, 0, 32);
  uint8_t by2 = beatsin8(23, 0, NUM_ROWS - 1, 0, 32);
  uint8_t bx3 = beatsin8(30, 0, NUM_COLS - 1, 0, 64);
  uint8_t by3 = beatsin8(24, 0, NUM_ROWS - 1, 0, 64);
  uint8_t bx4 = beatsin8(17, 0, NUM_COLS - 1, 0, 128);
  uint8_t by4 = beatsin8(25, 0, NUM_ROWS - 1, 0, 128);
  uint8_t bx5 = beatsin8(19, 0, NUM_COLS - 1, 0, 170);
  uint8_t by5 = beatsin8(21, 0, NUM_ROWS - 1, 0, 170);

  if (0) {
    for (uint8_t i = 0; i < NUM_COLS; i++)    {
      for (uint8_t j = 0; j < NUM_ROWS; j++) {

        int16_t sum = dist2(i, j, bx1, by1);
        sum += dist2(i, j, bx2, by2);
        sum += dist2(i, j, bx3, by3);
        sum += dist2(i, j, bx4, by4);
        sum += dist2(i, j, bx5, by5);

        byte col =  constrain(sum, 0, 240);
        leds[XY (i, j)] =  ColorFromPalette(myPal, col, 255);

      }
    }
  } else {
    radial_fill(buffer, bx1 - kMatrixWidth / 1, by1 - kMatrixHeight / 1);
    radial_fill(buffer, bx2 - kMatrixWidth / 1, by2 - kMatrixHeight / 1);
    radial_fill(buffer, bx3 - kMatrixWidth / 1, by3 - kMatrixHeight / 1);
    radial_fill(buffer, bx4 - kMatrixWidth / 1, by4 - kMatrixHeight / 1);
    radial_fill(buffer, bx5 - kMatrixWidth / 1, by5 - kMatrixHeight / 1);
    uint8_t* src = buffer;
    for (uint8_t y = 0; y < kMatrixHeight; y++) {
      uint8_t xy0 = XY(0, y), xydx = XY(1, y) - xy0;
      for (uint8_t x = 0; x < kMatrixWidth; x++) {
        leds[xy0] = ColorFromPalette(myPal, *src, 255);
        xy0 += xydx;
        *src++ = 0;
      }
    }
  }

  FastLED.show();
  static int frame = 0;
  if (frame++ % 32 == 0)
    Serial.println(FastLED.getFPS());
} //loop


byte dist4(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)  {
  byte dx = abs8(y2 - y1);
  byte dy = abs8(x2 - x1);
  if ( dx < dy )  return 200 / dy;
  else    return 200 / dx;
}

byte dist3(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)  {
  byte a = y2 - y1;
  byte b = x2 - x1;
  a *= a;
  b *= b;
  byte dist = 200 / sqrt16(a + b);
  return dist;
}

byte dist2(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)  {
  byte a = y2 - y1;
  byte b = x2 - x1;
  a *= a;
  b *= b;
  return 200.0 / sqrt16(a + b + 1);
}

uint16_t XY(uint8_t x, uint8_t y) {
  uint8_t major, minor, sz_major, sz_minor;
  if (x >= kMatrixWidth || y >= kMatrixHeight)
    return NUM_LEDS;
  if (XY_MATRIX & ROWMAJOR)
    major = x, minor = y, sz_major = kMatrixWidth,  sz_minor = kMatrixHeight;
  else
    major = y, minor = x, sz_major = kMatrixHeight, sz_minor = kMatrixWidth;
  if ((XY_MATRIX & FLIPMAJOR) ^ (minor & 1 && (XY_MATRIX & SERPENTINE)))
    major = sz_major - 1 - major;
  if (XY_MATRIX & FLIPMINOR)
    minor = sz_minor - 1 - minor;
  return (uint16_t) minor * sz_major + major;
}

// visits each pixel in raster order whilst maintaining xroot = sqrt(x^2 + y^2)
// using only a single sqrt() per frame rather than one per pixel.
// © 2020 Steve Dommett <sutaburosu@gmail.com> - CC BY-SA 2.0
void radial_fill(uint8_t* dst, int8_t x_offset, int8_t y_offset) {
  uint8_t  screenx, screeny, xroot, yroot;
  uint16_t xsumsquares, ysumsquares, xnextsquare, ynextsquare;
  int8_t   x, y;

  // offset the origin in screen space
  x = x_offset;
  y = y_offset;
  ysumsquares = x_offset * x_offset + y * y;
  yroot = sqrt16(ysumsquares);
  ynextsquare = yroot*yroot;

  // Quadrant II (top-left)
  screeny = kMatrixHeight;
  while (y < 0 && screeny) {
    x = x_offset;
    screenx = kMatrixWidth;
    xsumsquares = ysumsquares;
    xroot = yroot;
    if (x < 0) {
      xnextsquare = xroot * xroot;
      while (x < 0 && screenx) {
        screenx--;
        *dst = qadd8(*dst, divide[xroot]); dst++;
        xsumsquares += 2 * x++ + 1;
        if (xsumsquares < xnextsquare)
          xnextsquare -= 2 * xroot-- - 1;
      }
    }
    // Quadrant I (top right)
    if (screenx) {
      xnextsquare = (xroot + 1) * (xroot + 1);
      while (screenx) {
        screenx--;
        *dst = qadd8(*dst, divide[xroot]); dst++;
        xsumsquares += 2 * x++ + 1;
        if (xsumsquares >= xnextsquare)
          xnextsquare += 2 * ++xroot + 1;
      }
    }
    ysumsquares += 2 * y++ + 1;
    if (ysumsquares < ynextsquare)
      ynextsquare -= 2 * yroot-- - 1;
    screeny--;
  }
  // Quadrant III (bottom left)
  ynextsquare = (yroot + 1) * (yroot + 1);
  while (screeny) {
    x = x_offset;
    screenx = kMatrixWidth;
    xsumsquares = ysumsquares;
    xroot = yroot;
    if (x < 0) {
      xnextsquare = xroot * xroot;
      while (x < 0 && screenx) {
        screenx--;
        *dst = qadd8(*dst, divide[xroot]); dst++;
        xsumsquares += 2 * x++ + 1;
        if (xsumsquares < xnextsquare)
          xnextsquare -= 2 * xroot-- - 1;
      }
    }
    // Quadrant IV (bottom right)
    if (screenx) {
      xnextsquare = (xroot + 1) * (xroot + 1);
      while (screenx--) {
        *dst = qadd8(*dst, divide[xroot]); dst++;
        xsumsquares += 2 * x++ + 1;
        if (xsumsquares >= xnextsquare)
          xnextsquare += 2 * ++xroot + 1;
      }
    }
    ysumsquares += 2 * y++ + 1;
    if (ysumsquares >= ynextsquare)
      ynextsquare += 2 * ++yroot + 1;
    screeny--;
  }
}
