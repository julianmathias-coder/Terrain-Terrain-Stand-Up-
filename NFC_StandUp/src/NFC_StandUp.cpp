/* Project: NFC_standUp
 * Author: Julian Mathias
 * Date: 10-31-2024
 * Line 142 in CNM IoT wan't change from int "%i" to "%li", since hueHue is a long integer. I will not change the library because it is not necessary for the Photon 2.
 */

#include "Particle.h"
#include "IoTClassroom_CNM.h"
#include <DFRobot_PN532.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Grove_4Digit_Display.h"
#include <neopixel.h>
#include "Button.h"
#include "DFRobotDFPlayerMini.h"


DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

const int CLK = D4;
const int DIO = D5;
bool keepCounting;

TM1637 tm1637(CLK,DIO);
Button startCountdown(D3);

bool countDown(bool restart = false, int countStart = 60);
void displayNFCData();
bool nfcScanned = false;

const int PIXELCOUNT = 1;
int litPixel;
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);

const int OLED_RESET=-1;
Adafruit_SSD1306 display(OLED_RESET);

#define PN532_IRQ 2
#define POLLING 0
#define READ_BLOCK_NO 2

DFRobot_PN532_IIC nfc(PN532_IRQ, POLLING);
uint8_t dataRead[16] = {0};

//bool onOFF; //toggle switch to true/false/on/off trying to flash multip
int bri = 255;
int sat = 255;
int color;

SYSTEM_MODE(MANUAL); //should this be manual?

const int BULB=4;

const int MYWEMOFan=3; //fan
const int MYWEMOLight=2; //table light

  void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);
  Serial1.begin(9600);
  delay(1000);

  Serial.printf("DFRobot DFPlayer Mini Demo\n");
  Serial.printf("Initializing DFPlayer ... (May take 3~5 seconds)\n");

  myDFPlayer.begin(Serial1);

  WiFi.on();
  WiFi.clearCredentials();
  WiFi.setCredentials("IoTNetwork");
 
 WiFi.connect();
  while (WiFi.connecting()) {
  Serial.printf(".");
 }
  Serial.printf("\n\n");
  
  tm1637.init();
  tm1637.set(7);
  tm1637.point(POINT_ON);

  keepCounting = false;

  while (!nfc.begin()) {
    Serial.printf("NFC initialization failed. Retrying...\n");
    delay(1000);
  }
  Serial.printf("NFC initialized. Waiting for a card...\n");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(12,8);
  display.printf(" Scan\n  Now\n");
  display.display();

  pixel.begin();
  pixel.setBrightness(255);
  pixel.show();
  pixel.clear();
}

void loop() {
  if (startCountdown.isClicked()) {
    countDown(true, 60);
    keepCounting = true;
    nfcScanned = false; //Reset NFC scan flag
    myDFPlayer.volume(0); // Mute the player when starting a new
    //myDFPlayer.loop(1);
    //onOFF = !onOFF; //toggle bool true/flase
    setHue(BULB,true,HueRed,bri,sat); 
    wemoWrite(MYWEMOLight, HIGH);
    wemoWrite(MYWEMOFan, HIGH);
    Serial.printf("Turning on Fan %i\n Turning on Light %i\n", MYWEMOFan, MYWEMOLight);
    //wemoWrite((MYWEMOLight,HIGH) && (MYWEMOFan,HIGH)); //Connected to fan & light, starts when button clicked, check to see if it turns off if button clicked twice, but there's no bool so doubt it.
    //Serial.printf("Turning on Fan %i\n Turning on Light %i\n",MYWEMOFan,MYWEMOLight);
  }
    
    if (keepCounting) {
      keepCounting = countDown();
    if (!keepCounting) {
      Serial.printf("Countdown is complete\n");
    if (!nfcScanned) {
      myDFPlayer.volume(30); //Set volume to 30 if NFC not scanned
      myDFPlayer.loop(1); //Play the first mp3
    //myDFPlayer.enableLoopAll();
      // setHue(BULB,true,HueRed,bri,sat);
      }  
    }
  }
  
  static unsigned long lastNfcScanTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastNfcScanTime >= 100) {  // 100ms delay between scans to reduce power consumption
    lastNfcScanTime = currentTime; //test to see if this messes anything up.
  

  if (nfc.scan()) {
    nfcScanned = true; // Set NFC scan flag
    myDFPlayer.volume(0); // Mute the player when NFC is scanned
    setHue(BULB,true,green,bri,255); //turn Hue green
    wemoWrite(MYWEMOFan,LOW); //turn off fan
    wemoWrite(MYWEMOLight,LOW); //turn off WEMO fan
    Serial.printf("Turning off Fan %i\n",MYWEMOFan);
    Serial.printf("Turning off Light %i\n",MYWEMOLight);

      
  if (nfc.readData(dataRead, READ_BLOCK_NO) == 1) {
      Serial.printf("Block %d read success!\n", READ_BLOCK_NO);
      Serial.printf("Data read (string): %s\n", (char *)dataRead);
      displayNFCData();
  } 
  else {
      Serial.printf("Block %d read failure!\n", READ_BLOCK_NO);
    }
   }
  }
}

void displayNFCData() {
  for (litPixel=0; litPixel<1; litPixel++) { //order: initiallization, condition, incremement--where to start, where do you want to go, how do you want to get there
      pixel.setPixelColor (litPixel, 0, 255, 0); //first number red, second green, thrid blue -- RGB 
      pixel.show();
      pixel.clear();  
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 32);
  display.printf("%s\n", (char *)dataRead);
  display.display();
  delay(5000);

  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(12, 8);
  display.printf("Great\n Job!\n");
  display.display();
  delay(1800);

  for (litPixel=0; litPixel<1; litPixel++) { //order: initiallization, condition, incremement--where to start, where do you want to go, how do you want to get there
      pixel.setPixelColor (litPixel, 255, 0, 0); //first number red, second green, thrid blue -- RGB 
      pixel.show();
      pixel.clear();  
  }

  display.clearDisplay();
  display.setCursor(12, 8);
  display.printf(" Scan\n Below\n");
  display.display();
}


bool countDown(bool restart, int countStart) {
  static int count = 60;
  static unsigned long lastTime = 0;

  if (restart) {
    count = countStart;
  }

  if (millis() - lastTime > 1000) {
    lastTime = millis();
    count--;

    int min10 = count / 600;
    int min01 = (count / 60) % 10;
    int sec10 = (count % 60) / 10;
    int sec01 = (count % 60) % 10;

    tm1637.display(0, min10);
    tm1637.display(1, min01);
    tm1637.display(2, sec10);
    tm1637.display(3, sec01);

    Serial.printf("Countdown: %02d:%02d\n", count / 60, count % 60);
  }

  if (count <= 0) {
    return false;
  }

  return true;
} 


