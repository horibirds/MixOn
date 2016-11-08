#include <Adafruit_NeoPixel.h>
#include "MsTimer2.h"
#define PIN 6
#define LED_NUM 9
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, PIN, NEO_GRB + NEO_KHZ800);

uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

int timer_count = 0;
int timer_pattern = 1;
void led_timer() {
  if (timer_pattern = 0) {
    if (timer_count > 255) {
      timer_count = 0;
    }
    strip.setBrightness(255);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(i + timer_count));
    }
  }
  if (timer_pattern = 1) {
    int interval = 200;
    if (timer_count > interval) {
      timer_count = 0;
    }
    int brightness;
    if (timer_count < (interval / 2)) {
      brightness = map(timer_count, 0, interval / 2, 0, 255);
    }
    else {
      brightness = map(timer_count, interval / 2, interval, 255, 0);
    }
    strip.setBrightness(brightness);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(i + timer_count));
    }
  }
  strip.show();
  timer_count++;
}

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  //while(!Serial){};

  // Adafruit
  strip.begin();
  strip.show();

  // timer
  MsTimer2::set(10, led_timer);
  MsTimer2::start();

}

void loop() {
  //  // put your main code here, to run repeatedly:
  //  int S1 = analogRead(A1);
  //  int S2 = analogRead(A2);
  //Serial.print(S1);
  //Serial.print("\t");
  //Serial.print(S2);
  //Serial.print("\t");
  //Serial.print(S1-S2);
  //Serial.print("\n");
}
