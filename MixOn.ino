/*********************************************************************
  This is an example for our nRF51822 based Bluefruit LE modules

  Pick one up today in the adafruit shop!

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  MIT license, check LICENSE for more information
  All text above, and the splash screen below must be included in
  any redistribution
*********************************************************************/
#include "TimerOne.h"
#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
#include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#include <Adafruit_NeoPixel.h>
/*=========================================================================
    APPLICATION SETTINGS

      FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
     
                                Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                                running this at least once is a good idea.
     
                                When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                                Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
         
                                Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
#define PIN                     5
#define NUMPIXELS               9
/*=========================================================================*/

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN);
// Create the bluefruit object, either software serial...uncomment these lines
/*
  SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

  Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
uint32_t wheel_pattern(byte WheelPos)
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

int st = 0;
int timer_count = 0;

int future_RGB[3] = {0, 0, 0};
int RGB[3] = {0, 0, 0};
void led_timer() {
  if (st == 0) {
    if (timer_count > 255) {
      timer_count = 0;
    }
    strip.setBrightness(0);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }
  if (st == 1) {
//    int R = exRGB[0] + (float(timer_count) * (RGB[0] - exRGB[0]) / feed);
//    int G = exRGB[1] + (float(timer_count) * (RGB[1] - exRGB[1]) / feed);
//    int B = exRGB[2] + (float(timer_count) * (RGB[2] - exRGB[2]) / feed);
    float rate = 0.1;
    for(int i=0; i<3;i++){
      if ((future_RGB[i] - RGB[i]) < 10){ 
        RGB[i] = future_RGB[i];
      }
    }
    RGB[0] += int((future_RGB[0] - RGB[0])*rate);
    RGB[1] += int((future_RGB[1] - RGB[1])*rate);
    RGB[2] += int((future_RGB[2] - RGB[2])*rate);
    strip.setBrightness(255);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, RGB[0], RGB[1], RGB[2]);
    }
  }
  else if (st == 100) {
    if (timer_count > 255) {
      timer_count = 0;
    }
    strip.setBrightness(255);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel_pattern(i + timer_count));
    }
  }
  else if (st == 200) {
    int interval = 255;
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
    strip.setBrightness(255);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel_pattern(i + timer_count));
    }
  }
  else if (st == 300) {
    int interval = 150;
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
      strip.setPixelColor(i, 255, 0, 0);
    }
  }
  else if (st == 400) {
    if (timer_count >= 255) {
      timer_count = 0;
    }
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel_pattern(i + timer_count));
    }
    if ((timer_count % 15) < 3) {
      strip.setBrightness(255);
    }
    else {
      strip.setBrightness(0);
    }
  }
  else if (st == 500) {
    if (timer_count >= 100) {
      timer_count = 0;
    }
    if (timer_count < 5) {
      for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0, 0, 255);
      }
      strip.setBrightness(255);
    }
    else if (timer_count >= 50 && timer_count < 55) {
      for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 255, 255, 0);
      }
      strip.setBrightness(50);
    }
    else {
      strip.setBrightness(0);
    }
  }
  else if (st == 600) {
    int interval = 3;
    int led_num[8] = {0, 1, 2, 5, 8, 7, 6, 3};
    if (timer_count >= interval * 8) {
      timer_count = 0;
    }
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    strip.setPixelColor(led_num[int(timer_count / interval)], 255, 40, 40);
    strip.setBrightness(255);

  }
  else if (st == 700) {
    strip.setBrightness(0);
  }
  strip.show();
  timer_count++;
}

void setup(void)
{
  //  while (!Serial);  // required for Flora & Micro
  //  delay(500);

  strip.begin(); // This initializes the NeoPixel library.
  for (uint8_t i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // off
  }
  strip.show();
  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
    delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
  // timer
  Timer1.initialize(10000); //マイクロ秒単位で設定
  Timer1.attachInterrupt(led_timer);
  //  MsTimer2::set(10, led_timer);
  //  MsTimer2::start();

}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
//int ave[10] = {0};
//int sum = 0;
//float R1 = 10.0;
bool b_run = false;

void loop() {
  //  // put your main code here, to run repeatedly:
  int atsuryoku = analogRead(A0);

  // send data
  if (b_run)
  {
    delay(100);
    ble.print(atsuryoku);
  }
  // Echo received data
  String ble_data[10];
  for (int i = 0; i < 10; i++) {
    ble_data[i] = "\0";
  }
  String ble_buf = "";
  int data_count = 0;
  while ( ble.available() )
  {
    int c = ble.read();
    if (c == 0x2C) { // ","
      ble_data[data_count] = ble_buf;
      data_count++;
      ble_buf = "";
      continue;
    }
    ble_buf += (char)c;
  }
  ble_data[data_count] = ble_buf;

  int data_id = -1;
  if (ble_data[0] != "\0") {
    data_id = ble_data[0].toInt();
  }
  switch (data_id) {
    case 0: // bledata 送信
      b_run = true;
      st = 0;
      break;
    case 1: // LEDcolor変更
      if (ble_data[4].toInt() == 22) {
        future_RGB[0] = ble_data[1].toInt();
        future_RGB[1] = ble_data[2].toInt();
        future_RGB[2] = ble_data[3].toInt();
        timer_count = 0;
        st = 1;
      }
      break;
    case 2:
      b_run = false;
      st = 0;
      break;
  }
}

