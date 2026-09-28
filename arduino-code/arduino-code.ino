/*

  Code written by Dion Timmermann dion.timmermann@ptb.de 

  This code requires the 
    Raspberry Pi Pico/RP2040/RP2350 Board by Earle F. Philhower.
  Other boards for the Pi Pico do not support assigning random pins for the two IC2 busses!
  
  To installthe Raspberry Pi Pico/RP2040/RP2350 Board:
    - In the arduino IDE. File->Preferences under "Additional boards manager URLs" add
        https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
    - Go to the Boards Manager and Install
        "Raspberry Pi Pico/RP2040/RP2350"
    - Select the Board "Raspberry Pi Pico/RP2040/RP2350" -> "Raspberry Pi Pico" (for hand-soldered board).

  Required Libraries:
    - Adafruit SSD1306 by Adafruit
    - Adafruit seesaw Library
    (possibly others)

*/


#include <Wire.h>
#include <Adafruit_NeoTrellis.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <algorithm>
#include "FreeSansBold40pt7b.h"
#include "probabilities.h"

const uint32_t infernoLUT[256] PROGMEM = {
  0x000003UL, 0x000004UL, 0x000006UL, 0x010007UL, 0x010109UL, 0x01010BUL, 0x02010EUL, 0x020210UL, 0x030212UL, 0x040314UL, 0x040316UL, 0x050418UL, 0x06041BUL, 0x07051DUL, 0x08061FUL, 0x090621UL,
  0x0A0723UL, 0x0B0726UL, 0x0D0828UL, 0x0E082AUL, 0x0F092DUL, 0x10092FUL, 0x120A32UL, 0x130A34UL, 0x140B36UL, 0x160B39UL, 0x170B3BUL, 0x190B3EUL, 0x1A0B40UL, 0x1C0C43UL, 0x1D0C45UL, 0x1F0C47UL,
  0x200C4AUL, 0x220B4CUL, 0x240B4EUL, 0x260B50UL, 0x270B52UL, 0x290B54UL, 0x2B0A56UL, 0x2D0A58UL, 0x2E0A5AUL, 0x300A5CUL, 0x32095DUL, 0x34095FUL, 0x350960UL, 0x370961UL, 0x390962UL, 0x3B0964UL,
  0x3C0965UL, 0x3E0966UL, 0x400966UL, 0x410967UL, 0x430A68UL, 0x450A69UL, 0x460A69UL, 0x480B6AUL, 0x4A0B6AUL, 0x4B0C6BUL, 0x4D0C6BUL, 0x4F0D6CUL, 0x500D6CUL, 0x520E6CUL, 0x530E6DUL, 0x550F6DUL,
  0x570F6DUL, 0x58106DUL, 0x5A116DUL, 0x5B116EUL, 0x5D126EUL, 0x5F126EUL, 0x60136EUL, 0x62146EUL, 0x63146EUL, 0x65156EUL, 0x66156EUL, 0x68166EUL, 0x6A176EUL, 0x6B176EUL, 0x6D186EUL, 0x6E186EUL,
  0x70196EUL, 0x72196DUL, 0x731A6DUL, 0x751B6DUL, 0x761B6DUL, 0x781C6DUL, 0x7A1C6DUL, 0x7B1D6CUL, 0x7D1D6CUL, 0x7E1E6CUL, 0x801F6BUL, 0x811F6BUL, 0x83206BUL, 0x85206AUL, 0x86216AUL, 0x88216AUL,
  0x892269UL, 0x8B2269UL, 0x8D2369UL, 0x8E2468UL, 0x902468UL, 0x912567UL, 0x932567UL, 0x952666UL, 0x962666UL, 0x982765UL, 0x992864UL, 0x9B2864UL, 0x9C2963UL, 0x9E2963UL, 0xA02A62UL, 0xA12B61UL,
  0xA32B61UL, 0xA42C60UL, 0xA62C5FUL, 0xA72D5FUL, 0xA92E5EUL, 0xAB2E5DUL, 0xAC2F5CUL, 0xAE305BUL, 0xAF315BUL, 0xB1315AUL, 0xB23259UL, 0xB43358UL, 0xB53357UL, 0xB73456UL, 0xB83556UL, 0xBA3655UL,
  0xBB3754UL, 0xBD3753UL, 0xBE3852UL, 0xBF3951UL, 0xC13A50UL, 0xC23B4FUL, 0xC43C4EUL, 0xC53D4DUL, 0xC73E4CUL, 0xC83E4BUL, 0xC93F4AUL, 0xCB4049UL, 0xCC4148UL, 0xCD4247UL, 0xCF4446UL, 0xD04544UL,
  0xD14643UL, 0xD24742UL, 0xD44841UL, 0xD54940UL, 0xD64A3FUL, 0xD74B3EUL, 0xD94D3DUL, 0xDA4E3BUL, 0xDB4F3AUL, 0xDC5039UL, 0xDD5238UL, 0xDE5337UL, 0xDF5436UL, 0xE05634UL, 0xE25733UL, 0xE35832UL,
  0xE45A31UL, 0xE55B30UL, 0xE65C2EUL, 0xE65E2DUL, 0xE75F2CUL, 0xE8612BUL, 0xE9622AUL, 0xEA6428UL, 0xEB6527UL, 0xEC6726UL, 0xED6825UL, 0xED6A23UL, 0xEE6C22UL, 0xEF6D21UL, 0xF06F1FUL, 0xF0701EUL,
  0xF1721DUL, 0xF2741CUL, 0xF2751AUL, 0xF37719UL, 0xF37918UL, 0xF47A16UL, 0xF57C15UL, 0xF57E14UL, 0xF68012UL, 0xF68111UL, 0xF78310UL, 0xF7850EUL, 0xF8870DUL, 0xF8880CUL, 0xF88A0BUL, 0xF98C09UL,
  0xF98E08UL, 0xF99008UL, 0xFA9107UL, 0xFA9306UL, 0xFA9506UL, 0xFA9706UL, 0xFB9906UL, 0xFB9B06UL, 0xFB9D06UL, 0xFB9E07UL, 0xFBA007UL, 0xFBA208UL, 0xFBA40AUL, 0xFBA60BUL, 0xFBA80DUL, 0xFBAA0EUL,
  0xFBAC10UL, 0xFBAE12UL, 0xFBB014UL, 0xFBB116UL, 0xFBB318UL, 0xFBB51AUL, 0xFBB71CUL, 0xFBB91EUL, 0xFABB21UL, 0xFABD23UL, 0xFABF25UL, 0xFAC128UL, 0xF9C32AUL, 0xF9C52CUL, 0xF9C72FUL, 0xF8C931UL,
  0xF8CB34UL, 0xF8CD37UL, 0xF7CF3AUL, 0xF7D13CUL, 0xF6D33FUL, 0xF6D542UL, 0xF5D745UL, 0xF5D948UL, 0xF4DB4BUL, 0xF4DC4FUL, 0xF3DE52UL, 0xF3E056UL, 0xF3E259UL, 0xF2E45DUL, 0xF2E660UL, 0xF1E864UL,
  0xF1E968UL, 0xF1EB6CUL, 0xF1ED70UL, 0xF1EE74UL, 0xF1F079UL, 0xF1F27DUL, 0xF2F381UL, 0xF2F485UL, 0xF3F689UL, 0xF4F78DUL, 0xF5F891UL, 0xF6FA95UL, 0xF7FB99UL, 0xF9FC9DUL, 0xFAFDA0UL, 0xFCFEA4UL
};

#define MAX_GROVER_STEP 12

uint32_t heatmap(uint8_t idx) {
  // Read packed RGB from flash
  uint8_t safeIdx = (idx > 255) ? 255 : idx;
  uint32_t color = pgm_read_dword(&infernoLUT[safeIdx]);
  
  // Extract RGB components
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  
  // Reduce brightness to 25% to prevent power issues
  r = r / 4;
  g = g / 4;
  b = b / 4;
  
  // Recombine into 32-bit color
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// Wire bus (I2C0) pins for NeoTrellis
#define TRELLIS_SDA 12
#define TRELLIS_SCL 13

// Wire1 bus (I2C1) pins for SSD1306s
#define OLED_SDA 14
#define OLED_SCL 15

// SSD1306 display addresses
#define OLED1_ADDR 0x3C
#define OLED2_ADDR 0x3D
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define PIXELS_PER_ROW 8

Adafruit_SSD1306 displayClassic(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1);
Adafruit_SSD1306 displayQuantum(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1);

// Assuming you have 4 NeoTrellis boards connected on the same bus
#define NUM_BOARDS_X 2
#define NUM_BOARDS_Y 2

#define LED_CLASSIC 2
#define BTN_CLASSIC 3
#define LED_QUANTUM 6
#define BTN_QUANTUM 7
#define LED_GROVER 10
#define BTN_GROVER 11

#define BTN_PROB 28
#define SW_MODE_1 16
#define SW_MODE_3 17

void flashAllMultiTrellis(uint32_t color = 0xFFFFFF, uint16_t delayMs = 5, uint8_t times = 3);

const uint16_t totalKeys = NUM_BOARDS_X * NUM_BOARDS_Y * NEO_TRELLIS_NUM_KEYS;

bool wasPressed[totalKeys];
uint8_t brightness[totalKeys];
long counter;
uint8_t goal;
uint8_t random_offset;
long groverStep;
uint8_t model = 0;

uint16_t currentProbabilities[64];
uint16_t currentCumsum[64];
void updateProbabilities() {
    uint8_t model_clamped = std::clamp(model, (uint8_t) 0, (uint8_t) 2);
    long step_clamped = std::clamp(groverStep, (long) 0, (long) 12);

    for (uint8_t i = 0; i < 64; i++) {
        // Use pgm_read_word to read from PROGMEM
        currentProbabilities[i] = pgm_read_word(&(probabilities[model_clamped][step_clamped][i]));
    }

    // Compute cumulative sum
    currentCumsum[0] = currentProbabilities[0];
    for (uint8_t i = 1; i < 64; i++) {
      currentCumsum[i] = currentCumsum[i-1] + currentProbabilities[i];
    }
}

uint8_t randomShot() {

  // Generate random number from 0 to total sum - 1
  uint16_t total = currentCumsum[63];
  uint16_t r = random(0L, total);

  for (uint8_t i = 0; i < 64; i++) {
      if (r < currentCumsum[i]) return i;
  }
  return 63;  // fallback
}

// Addresses of each board
Adafruit_NeoTrellis trellis_boards[NUM_BOARDS_Y][NUM_BOARDS_X] = {
  { Adafruit_NeoTrellis(0x2E, &Wire), Adafruit_NeoTrellis(0x2F, &Wire) },
  { Adafruit_NeoTrellis(0x30, &Wire), Adafruit_NeoTrellis(0x31, &Wire) }
};

Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)trellis_boards, NUM_BOARDS_Y, NUM_BOARDS_X);

void setPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t row = index / PIXELS_PER_ROW;
  uint8_t col = index % PIXELS_PER_ROW;
}

void displayNumber(Adafruit_SSD1306 &display, int number) {
  // Clamp number to 0-99
  if (number < 0) number = 0;
  if (number > 99) number = 99;

  display.clearDisplay();
  display.setFont(&FreeSansBold40pt7b);
  display.setTextColor(SSD1306_WHITE);

  // Convert number to string
  char buf[3];
  sprintf(buf, "%02d", number);

  // Measure text bounds
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);

  // Center text
  int16_t x = (display.width() - w) / 2 - x1;
  int16_t y = (display.height() - h) / 2 - y1;

  display.setCursor(x, y);
  display.print(buf);
  display.display();
}

#define TOTAL_KEYS (NUM_BOARDS_X * NUM_BOARDS_Y * NEO_TRELLIS_NUM_KEYS)
bool keyPressed[TOTAL_KEYS]; // global array to track button states

// Callback for MultiTrellis keys
TrellisCallback keyPressCallback(keyEvent evt) {
    int keyNum = evt.bit.NUM;
    if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
        keyPressed[keyNum] = true;
    } else {
        keyPressed[keyNum] = false;
    }
    return nullptr;
}

// Returns unique button number if exactly one button is pressed
int getSinglePressedButton() {
    trellis.read();
    int pressedButton = -1;
    for (int i = 0; i < TOTAL_KEYS; i++) {
        if (keyPressed[i]) {
            if (pressedButton == -1) {
                pressedButton = i;
            } else {
                return -1; // more than one pressed
            }
        }
    }
    return pressedButton;
}

void setAllPixels(uint32_t color) {
  for (uint16_t i = 0; i < totalKeys; i++) {
    trellis.setPixelColor(i, color);
  }
  trellis.show();
}

void setSinglePixel(uint16_t pixel, uint32_t color) {
  trellis.setPixelColor(pixel, color);
  trellis.show();
}

inline bool isPressed(uint8_t button) {
  return !digitalRead(button);
}

const uint8_t gammaSteps = 20;
uint8_t gammaTable[gammaSteps];

void setupGamma() {
  const float gamma = 3.3;
  for (uint8_t i = 0; i < gammaSteps; i++) {
    float normalized = (float)i / (gammaSteps - 1); // 0..1
    gammaTable[i] = round(pow(normalized, gamma) * 255.0); // 0..255
  }
}



void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("Starting dual I2C test...");

  // Init I2C0 for Trellis
  Wire.setSDA(12);
  Wire.setSCL(13);
  Wire.begin();
  // Wire.setClock(400000);

  // Init I2C1 for Displays
  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire1.begin();

  // Setup Trellis
  trellis.begin();

  for (int y = 0; y < PIXELS_PER_ROW; y++) {
    for (int x = 0; x < PIXELS_PER_ROW; x++) {
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, keyPressCallback);
    }
  }
  for (int i = 0; i < TOTAL_KEYS; i++) {
    keyPressed[i] = false;
  }

  // Setup LEDs
  pinMode(LED_CLASSIC, OUTPUT);
  pinMode(LED_QUANTUM, OUTPUT);
  pinMode(LED_GROVER,  OUTPUT);

  // Setup Buttons
  pinMode(BTN_CLASSIC, INPUT_PULLUP);
  pinMode(BTN_QUANTUM, INPUT_PULLUP);
  pinMode(BTN_GROVER,  INPUT_PULLUP);

  pinMode(BTN_PROB,   INPUT_PULLUP);
  pinMode(SW_MODE_1,  INPUT_PULLUP);
  pinMode(SW_MODE_3,  INPUT_PULLUP);

  // Setup Displays
  displayClassic.begin(SSD1306_SWITCHCAPVCC, OLED1_ADDR);
  displayClassic.clearDisplay();
  displayClassic.display();

  displayQuantum.begin(SSD1306_SWITCHCAPVCC, OLED2_ADDR);
  displayQuantum.clearDisplay();
  displayQuantum.display();

  setupGamma();

}

void winAnimation() {

  // Flash goal
  for (int i=0; i<5; i++) {
    delay(200);
    setSinglePixel(goal, 0x000000);
    delay(200);
    setSinglePixel(goal, 0x00FF00);
  }
  delay(100);

  // Fill all others in increasingly big red diamonds.
  int goal_y = goal / 8;
  int goal_x = goal % 8;
  for (int d_step=1; d_step<16; d_step++) {
    for (int i=0; i<64; i++) {
      int y = i / 8;
      int x = i % 8;
      
      int d = abs(goal_x - x) + abs(goal_y - y);
      if (d == d_step) {
        if (!wasPressed[i]) {
          trellis.setPixelColor(i, 0x030000);
        }  
      }
    }
    trellis.show();
    delay(100);
  }
  
  delay(1000);

}

void classicGame() {

  counter = 0;
  goal = random(0, totalKeys);

  for (int8_t i; i<totalKeys; i++) {
    wasPressed[i] = false;
  }

  setAllPixels(heatmap(255/64));
  while(true) {
    int8_t pressed = getSinglePressedButton();
    if (pressed != -1 && !wasPressed[pressed]) {
      counter += 1;
      displayNumber(displayClassic, counter);

      wasPressed[pressed] = true;
      if (pressed == goal) {
        trellis.setPixelColor(pressed, 0x008F00);
        trellis.show();
        break;
      } else {
        trellis.setPixelColor(pressed, 0x5F0000);
      }
      for (int i=0; i<64; i++) {
        if (!wasPressed[i]) {
          trellis.setPixelColor(i, heatmap(255/(64-counter)));
        }
      }
      trellis.show();
    }
  }

  winAnimation();

}

void setPixelsBasedOnBrightnessMap() {
  for (int16_t i=0; i<totalKeys; i++) {
    if (brightness[i]) {
      brightness[i] -= 1;

      uint8_t b = gammaTable[brightness[i]];
      uint32_t color;

      if (wasPressed[i]) {
        color = (uint32_t) b;

        // Ensure minimal brighness
        if (color < 0x03) {
          color = 0x03;
        }

        // Make it green or red.
        if (i == goal) {
          color = color << 8;
        } else {
          color = color << 16;
        }
      } else {
        b = b/3; // Dimm white pixels by factor of 3 so the are as bright as red and green pixels.
        color = ((uint32_t)b << 16) | ((uint32_t)b << 8) | b;
      }
      
      trellis.setPixelColor(i, color);
    }
  }
  trellis.show();
}

void setPixelsBasedOnProbabilities() {
  for (int16_t i=0; i<totalKeys; i++) {

    int16_t shiftedIndex = (i - random_offset + totalKeys) % totalKeys;
    if (wasPressed[i]) {
      if (i == goal) {
        trellis.setPixelColor(i, 0x00FF00);  
      } else {
        trellis.setPixelColor(i, 0x030000);  
      }
    } else {
      trellis.setPixelColor(i, heatmap(currentProbabilities[shiftedIndex] / 16));
    }
    // trellis.setPixelColor((i + random_offset) % totalKeys, heatmap(currentProbabilities[i] / 16));
  }
  trellis.show();
}

uint8_t readModeButton() {
    if (isPressed(SW_MODE_1)) {
      return 2;
    } else if (isPressed(SW_MODE_3)) {
      return 0;
    } else {
      return 1;
    }
}

// Entprellzeit in ms
const uint16_t DEBOUNCE_MS = 20;

// Globale Variablen pro Button
bool lastButtonState = HIGH;  // HIGH wegen Pull-up
unsigned long lastDebounceTime = 0;

bool readProbabilitiesButton() {
  bool reading = digitalRead(BTN_PROB);

  if (reading != lastButtonState) {
    // Zustand hat sich geändert -> Timer starten
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
    // Stabiler Zustand
    static bool buttonStateStable = HIGH;
    if (reading != buttonStateStable) {
      buttonStateStable = reading;
      if (buttonStateStable == LOW) {
        // gedrückt (Pull-up), aber wir wollen RISING edge
        lastButtonState = reading;
        return true;  // steigende Flanke
      }
    }
  }

  lastButtonState = reading;
  return false;
}

void quantumGame() {

  // Setup
  counter = 0;
  random_offset = random(0, totalKeys);
  goal = (21 + random_offset) % totalKeys;//random(0, totalKeys);
  groverStep = 0;

  for (int8_t i; i<totalKeys; i++) {
    wasPressed[i] = false;
  }

  for (int16_t i; i<totalKeys; i++) {
    brightness[i] = 0;
  }

  // Main game

  bool newShot = true;
  uint8_t groverGammaMin = 8;
  uint8_t groverGamma = groverGammaMin;
  bool showProbabilities = true;

  updateProbabilities();
  setPixelsBasedOnProbabilities();
  while (true) {

    uint8_t newModel = readModeButton();
    if (newModel != model) {
      model = newModel;
      updateProbabilities();

      if (showProbabilities) {
        setPixelsBasedOnProbabilities();
      }
    }

    if (readProbabilitiesButton()) {
      showProbabilities = !showProbabilities;
      if (showProbabilities) {
        setPixelsBasedOnProbabilities();
      } else {
        for (int i=0; i<64; i++) {
          if (wasPressed[i]) {
            trellis.setPixelColor(i, 0x030000);
          } else {
            trellis.setPixelColor(i, 0x000000);
          }
        }
        trellis.show();
      }
    }

    int8_t pressed = getSinglePressedButton();
    if (pressed != -1 && !wasPressed[pressed]) {
      counter += 1;
      displayNumber(displayQuantum, counter);

      wasPressed[pressed] = true;
      brightness[pressed] = gammaSteps;

      if (showProbabilities) {
        setPixelsBasedOnProbabilities();
      }
    }

    if (!showProbabilities) {
      if (newShot) {
        int16_t shot = (randomShot() + random_offset) % totalKeys;
        brightness[shot] = gammaSteps;
      }
      newShot = !newShot;

      setPixelsBasedOnBrightnessMap();
    }

    if (pressed == goal) {
      break;
    }

    if (isPressed(BTN_GROVER) && groverGamma == groverGammaMin && groverStep < MAX_GROVER_STEP) {
      groverGamma = gammaSteps;
      counter += 1;
      groverStep +=1;
      updateProbabilities();
      if (showProbabilities) {
        setPixelsBasedOnProbabilities();
      }
      displayNumber(displayQuantum, counter);
    } else if (showProbabilities && isPressed(BTN_CLASSIC) && groverGamma <= groverGammaMin && groverStep > 0) {
      // Secret undo grover step when showing probabilties
      groverGamma = gammaSteps;
      counter -= 1;
      groverStep -=1;
      updateProbabilities();
      if (showProbabilities) {
        setPixelsBasedOnProbabilities();
      }
      displayNumber(displayQuantum, counter);
    }
    if (groverStep < MAX_GROVER_STEP) {
      if (groverGamma > groverGammaMin) {
        groverGamma -= 1;
      }
    } else {
      if (groverGamma > 0) {
        groverGamma -= 1;
      }
    }
    analogWrite(LED_GROVER, gammaTable[groverGamma]);

    delay(17);


  }

  // Fade out all shots, but keep goal bright green
  for (uint8_t i=0; i<gammaSteps; i++) {
    brightness[goal] = gammaSteps;
    setPixelsBasedOnBrightnessMap();

    if (groverGamma > 0) {
      groverGamma -= 1;
    }
    analogWrite(LED_GROVER, gammaTable[groverGamma]);

    delay(17);
  }

  winAnimation();

  // for (uint32_t i=0; i<64; i++) {
  //   trellis.setPixelColor(i, heatmap(i*4));
    
  // }
  //   delay(100);
  //   trellis.show();
  // //   delay(500);


  // while (true) {

  //   if (isPressed(SW_MODE_1)) {
  //     displayNumber(displayQuantum, 1);
  //   }
  //   else if (isPressed(SW_MODE_3)) {
  //     displayNumber(displayQuantum, 3);
  //   } else {
  //     displayNumber(displayQuantum, 2);
  //   }


  // }
}

enum State {
  SELECT_GAME,
  CLASSIC,
  CLASSIC_END,
  QUANTUM,
  QUANTUM_END
};

State currentState = SELECT_GAME;

void loop() {
  if (currentState == SELECT_GAME) {

    setAllPixels(0x000000);
    digitalWrite(LED_CLASSIC, HIGH);
    digitalWrite(LED_QUANTUM, HIGH);
    digitalWrite(LED_GROVER, LOW);

    while (true) {
      if (isPressed(BTN_CLASSIC)) {
        currentState = CLASSIC;
        break;
      }
      if (isPressed(BTN_QUANTUM)) {
        currentState = QUANTUM;
        break;
      }
      delay(10);
    }

  } else if (currentState == CLASSIC) {

    digitalWrite(LED_CLASSIC, LOW);
    digitalWrite(LED_QUANTUM, LOW);

    displayNumber(displayClassic, 0);

    classicGame();
    currentState = SELECT_GAME;

  } else if (currentState == QUANTUM) {

    digitalWrite(LED_CLASSIC, LOW);
    digitalWrite(LED_QUANTUM, LOW);
    analogWrite(LED_GROVER, 30);

    displayNumber(displayQuantum, 0);

    quantumGame();

    displayQuantum.setTextColor(30);
    displayNumber(displayQuantum, counter);
    displayQuantum.setTextColor(255);

    currentState = SELECT_GAME;

  }
}
