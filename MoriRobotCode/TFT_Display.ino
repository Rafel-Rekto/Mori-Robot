#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS 5
#define TFT_RST 4
#define TFT_DC 2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

enum Emotion {
  IDLE,
  LISTENING,
  THINKING,
  TALKING,
  HAPPY,
  CONFUSED,
  SLEEPING
};

Emotion currentEmotion = IDLE;

// =======================
// FACE CONSTANTS
// =======================

const int EYE_W = 40;
const int EYE_H = 50;
const int EYE_R = 20;

const int LEFT_X = 30;
const int RIGHT_X = 100;
const int EYE_Y = 35;

// =======================
// BASIC FUNCTIONS
// =======================

void clearFaceArea() {
  tft.fillRect(10, 15, 140, 95, ST7735_BLACK);
}

void drawEyes(
  int leftX,
  int rightX,
  int y,
  int width,
  int height,
  uint16_t color
) {
  tft.fillRoundRect(
    leftX,
    y,
    width,
    height,
    EYE_R,
    color
  );

  tft.fillRoundRect(
    rightX,
    y,
    width,
    height,
    EYE_R,
    color
  );
}

void siapkanLayar() {

  tft.initR(INITR_BLACKTAB);

  tft.setRotation(1);

  tft.fillScreen(ST7735_BLACK);
}

// =======================
// IDLE
// =======================

void drawIdle() {

  clearFaceArea();

  drawEyes(
    LEFT_X,
    RIGHT_X,
    EYE_Y,
    EYE_W,
    EYE_H,
    ST7735_CYAN
  );
}

// =======================
// LISTENING
// =======================

void animateListening() {

  int pulse[] = {0,2,4,6,4,2,0};

  for(int i=0;i<7;i++) {

    clearFaceArea();

    int size = pulse[i];

    drawEyes(
      LEFT_X - size/2,
      RIGHT_X - size/2,
      EYE_Y - size/2,
      EYE_W + size,
      EYE_H + size,
      ST7735_CYAN
    );

    delay(150);
  }
}

// =======================
// THINKING
// =======================

void drawThinkingEyes() {

  drawEyes(
    45,
    95,
    30,
    28,
    22,
    ST7735_CYAN
  );
}

void drawThinkingDots(int count) {

  int startX = 50;

  for(int i=0;i<count;i++) {

    tft.fillRoundRect(
      startX + (i*15),
      85,
      10,
      4,
      2,
      ST7735_CYAN
    );
  }
}

void animateThinking() {

  for(int dots=1; dots<=5; dots++) {

    clearFaceArea();

    drawThinkingEyes();

    drawThinkingDots(dots);

    delay(250);
  }
}

// =======================
// TALKING
// =======================

void animateTalking() {

  int heights[] = {
    50,
    35,
    45,
    30,
    50
  };

  for(int i=0;i<5;i++) {

    clearFaceArea();

    int h = heights[i];

    drawEyes(
      LEFT_X,
      RIGHT_X,
      EYE_Y + (EYE_H - h)/2,
      EYE_W,
      h,
      ST7735_CYAN
    );

    delay(150);
  }
}

// =======================
// HAPPY
// =======================

void drawHappy() {

  clearFaceArea();

  int r = 18;

  tft.fillCircle(
    LEFT_X + EYE_W/2,
    EYE_Y + 15,
    r,
    ST7735_CYAN
  );

  tft.fillCircle(
    RIGHT_X + EYE_W/2,
    EYE_Y + 15,
    r,
    ST7735_CYAN
  );

  tft.fillRect(
    LEFT_X - 10,
    EYE_Y + 15,
    EYE_W + 20,
    30,
    ST7735_BLACK
  );

  tft.fillRect(
    RIGHT_X - 10,
    EYE_Y + 15,
    EYE_W + 20,
    30,
    ST7735_BLACK
  );
}

// =======================
// CONFUSED
// =======================

void drawConfused() {

  clearFaceArea();

  tft.fillRoundRect(
    LEFT_X - 5,
    EYE_Y - 10,
    EYE_W + 10,
    EYE_H + 20,
    EYE_R,
    ST7735_CYAN
  );

  tft.fillRoundRect(
    RIGHT_X + 5,
    EYE_Y + 10,
    EYE_W - 10,
    EYE_H - 20,
    EYE_R,
    ST7735_CYAN
  );
}

// =======================
// SLEEPING
// =======================

void drawSleeping() {

  clearFaceArea();

  tft.fillRoundRect(
    LEFT_X,
    80,
    EYE_W,
    6,
    3,
    ST7735_CYAN
  );

  tft.fillRoundRect(
    RIGHT_X,
    80,
    EYE_W,
    6,
    3,
    ST7735_CYAN
  );
}

// =======================
// BLINK
// =======================

void blink() {

  int h[] = {
    50,
    30,
    15,
    5,
    15,
    30,
    50
  };

  for(int i=0;i<7;i++) {

    clearFaceArea();

    drawEyes(
      LEFT_X,
      RIGHT_X,
      EYE_Y + (50-h[i])/2,
      EYE_W,
      h[i],
      ST7735_CYAN
    );

    delay(20);
  }
}

void drawListening() {
  clearFaceArea();
  // Menggambar mata lebih besar sebagai tanda fokus mendengar
  drawEyes(LEFT_X - 4, RIGHT_X - 4, EYE_Y - 4, EYE_W + 8, EYE_H + 8, ST7735_CYAN);
}