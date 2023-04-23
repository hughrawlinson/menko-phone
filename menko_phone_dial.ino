#include <AcksenButton.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WTV020SD16P.h>

#define BUTTON_PIN 13
#define CLICK_TIME 500
#define BUTTON_DEBOUNCE_INTERVAL 10

#define SSD1306_NO_SPLASH

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define SOUND_RESET_PIN 26
#define SOUND_CLK_PIN 27
#define SOUND_DATA_PIN 14
#define SOUND_BUSY_PIN 12

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int count = 0;

int sent = 1;
int pressSendTime = 0;

AcksenButton rotary = AcksenButton(BUTTON_PIN, ACKSEN_BUTTON_MODE_NORMAL, BUTTON_DEBOUNCE_INTERVAL, INPUT);
WTV020SD16P wtv020sd16p(SOUND_RESET_PIN, SOUND_CLK_PIN, SOUND_DATA_PIN, SOUND_BUSY_PIN);

void setup() {
  Serial.begin(115200);

  wtv020sd16p.reset();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  pinMode(BUTTON_PIN, INPUT);
  
  display.clearDisplay();
}

void loop() {
  rotary.refreshStatus();

  int newTime = millis();

  if (rotary.onPressed()) {
    count++;
    pressSendTime = newTime + CLICK_TIME;
    sent = 0;
  }

  if (!sent && newTime > pressSendTime) {
    if (count < 11) {
      int value = count;
      if (value == 10) {
        value = 0;
      }

      Serial.println(value);

      display.clearDisplay();
      
      display.setTextSize(4);      // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.setCursor(0, 0);     // Start at top-left corner
      display.cp437(true);         // Use full 256 char 'Code Page 437' font
      display.write(0x30 + value);
      display.display();

      wtv020sd16p.stopVoice();
      wtv020sd16p.asyncPlayVoice(value);
      
      count = 0;
      sent = 1;
    } else {
      count = 0;
      sent = 1;
    }
  }

  delay(1);
}
