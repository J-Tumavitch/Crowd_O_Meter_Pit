#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// This could also be defined as matrix->color(255,0,0) but those defines
// are meant to work for adafruit_gfx backends that are lacking color()
#define LED_BLACK		0

#define LED_RED_VERYLOW 	(3 <<  11)
#define LED_RED_LOW 		(7 <<  11)
#define LED_RED_MEDIUM 		(15 << 11)
#define LED_RED_HIGH 		(31 << 11)

#define LED_GREEN_VERYLOW	(1 <<  5)   
#define LED_GREEN_LOW 		(15 << 5)  
#define LED_GREEN_MEDIUM 	(31 << 5)  
#define LED_GREEN_HIGH 		(63 << 5)  

#define LED_BLUE_VERYLOW	3
#define LED_BLUE_LOW 		7
#define LED_BLUE_MEDIUM 	15
#define LED_BLUE_HIGH 		31

#define LED_ORANGE_VERYLOW (LED_RED_VERYLOW + LED_GREEN_VERYLOW)
#define LED_ORANGE_LOW		 (LED_RED_LOW     + LED_GREEN_LOW)
#define LED_ORANGE_MEDIUM	 (LED_RED_MEDIUM  + LED_GREEN_MEDIUM)
#define LED_ORANGE_HIGH		 (LED_RED_HIGH    + LED_GREEN_HIGH)

#define LED_PURPLE_VERYLOW (LED_RED_VERYLOW + LED_BLUE_VERYLOW)
#define LED_PURPLE_LOW		 (LED_RED_LOW     + LED_BLUE_LOW)
#define LED_PURPLE_MEDIUM	 (LED_RED_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_PURPLE_HIGH		 (LED_RED_HIGH    + LED_BLUE_HIGH)

#define LED_CYAN_VERYLOW	 (LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_CYAN_LOW		   (LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_CYAN_MEDIUM		 (LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_CYAN_HIGH		   (LED_GREEN_HIGH    + LED_BLUE_HIGH)

#define LED_WHITE_VERYLOW	 (LED_RED_VERYLOW + LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_WHITE_LOW		   (LED_RED_LOW     + LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_WHITE_MEDIUM	 (LED_RED_MEDIUM  + LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_WHITE_HIGH		 (LED_RED_HIGH    + LED_GREEN_HIGH    + LED_BLUE_HIGH)

#define ledPin 5
#define matrixWidth 8
#define matrixHeight 32 
#define tilesX 2
#define tilesY 1
#define matrixType NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG + NEO_TILE_BOTTOM + NEO_TILE_RIGHT + NEO_TILE_COLUMNS + NEO_TILE_PROGRESSIVE

#define ledType

#define ampPin A1

int sampleWindow = 50; // ms
unsigned int sig[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned int sample = 0;
double sigFloor = 0;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
//  matrixWidth, matrixHeight, tilesX, tilesY, ledPin, matrixType, ledType);
  matrixWidth, matrixHeight, tilesX, tilesY, ledPin, matrixType);

void setup() {
  Serial.begin(9600);

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
}

void loop() {
  unsigned long startMillis = millis();
  unsigned int peakToPeak = 0;

  unsigned signalMax = 0;
  unsigned signalMin = 2048;

  int i;

  while(millis() - startMillis < sampleWindow) {
    sample = analogRead(ampPin);
    if ( sample < 2048) // Toss out
    {
      if ( sample > signalMax) {
        signalMax = sample;
      }
      if ( sample < signalMin) {
        signalMin = sample;
        sigFloor = (1.0-0.01)*sigFloor + 0.01*(double)sample;
      }
    }
  }
  //peakToPeak = signalMax - signalMin;
  peakToPeak = signalMax - (unsigned int)sigFloor;

  for (i=0; i<15; i++) {
    sig[i] = sig[i+1];
  }
  //sig[15] = map(peakToPeak, 0, 1024, 31, 0);
  sig[15] = map((log(1+peakToPeak)/log(1024)*255), 100, 255, 31, 0);

  //matrix.fillScreen(LED_BLACK);
  matrix.fillRect(0,   0, 16, 1,  LED_WHITE_MEDIUM  );
  matrix.fillRect(0,   1, 16, 5,  LED_RED_MEDIUM    );
  matrix.fillRect(0,   6, 16, 10,  LED_ORANGE_MEDIUM );
  matrix.fillRect(0,  16, 16, 16, LED_GREEN_MEDIUM  );

  for (i=0; i<=15; i++) {
    matrix.drawLine(15-i, 0, 15-i, sig[i], LED_BLACK);
    matrix.drawPixel(15-i, sig[i], LED_WHITE_MEDIUM);
  }
  matrix.show();
}
