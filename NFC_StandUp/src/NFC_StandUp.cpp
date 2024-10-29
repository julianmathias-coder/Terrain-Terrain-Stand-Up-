#include "Particle.h"
#include <DFRobot_PN532.h>//The order matters
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "TM1637Display.h"
#include "Grove_4Digit_Display.h"
#include <neopixel.h>
#include "DFRobotDFPlayerMini.h"

//DFRobotDFPlayerMini myDFPlayer;
//unsigned int lastSong;
//void printDetail(uint8_t type, int value);

#define CLK D4
#define DIO D5
//const uint8_t COLON = 0b01000000;
const int blank[]={0,0,0,0};
//TM1637Display Ldisplay(CLK, DIO);
TM1637 tm1637(CLK,DIO);
int countdown = 60000;
unsigned long lastUpdate = 0;


const int PIXELCOUNT = 1; //Total number of NeoPixels
int litPixel;
Adafruit_NeoPixel pixel(PIXELCOUNT, SPI1, WS2812B);


const int OLED_RESET=-1;
Adafruit_SSD1306 display(OLED_RESET);
int xPos = 0;
int yPos = 1;
int zPos = 2;

#define BLOCK_SIZE       16
#define  PN532_IRQ        2
#define  INTERRUPT        1
#define  POLLING          0
#define  READ_BLOCK_NO    2 // The block to be read

DFRobot_PN532_IIC  nfc(PN532_IRQ, POLLING);
uint8_t dataRead[16] = {0};

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

  Serial.begin(9600);
  delay(2000);
  Serial.println("Initializing");
  while (!nfc.begin()) {
    Serial.println(".");
    delay (1000);
  }
  Serial.println();
  Serial.println("Waiting for a card......");

  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(12,8);
  display.printf(" Scan\n Below\n");
  display.display();
  delay(2000);
  display.clearDisplay();

pixel.begin();
  pixel.setBrightness(255);
  pixel.show(); //initialize all off
 
{
  tm1637.init();
  tm1637.set(BRIGHTEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  tm1637.point(POINT_ON);
  //Ldisplay.setBrightness(0x0f); // Set maximum brightness (max brightness is 7, not 15)
  //Ldisplay.clearDisplay();
  //Ldisplay.clear(); does not compile with .clearDisplay or .clear
  //Ldisplay.setSegments(blank); // Clear the display
}

}

void loop() {
  for (int i = 0; i < BLOCK_SIZE; i++) {
  Serial.print(dataRead[i], HEX);
  Serial.print(" ");
  dataRead[i] = 0;
  } 
  
  //Ldisplay.showNumberDec(1234); // Display the number 1234
  //delay(1000);
  {
  int8_t NumTab[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};//0~9,A,b,C,d,E,F
  int8_t ListDisp[4];
  unsigned char i = 0;
  unsigned char count = 0;
  delay(150);

  while(1)
  {
    i = count;
    count ++;
    if(count == sizeof(NumTab)) count = 0;
    for(unsigned char BitSelect = 0;BitSelect < 4;BitSelect ++)
    {
      ListDisp[BitSelect] = NumTab[i];
      i ++;
      if(i == sizeof(NumTab)) i = 0;
    }
    tm1637.display(0,ListDisp[0]);
    tm1637.display(1,ListDisp[1]);
    tm1637.display(2,ListDisp[2]);
    tm1637.display(3,ListDisp[3]);
    delay(1000);
  }
}
  
  
 // if (millis() - lastUpdate >= 60000) {
       // lastUpdate = millis();
        
       // if (countdown > 0) {
          //  countdown--;
            
            //int minutes = countdown / 60;
            //int seconds = countdown % 60;

        //if (minutes > 0) {
                // Display minutes and seconds with colon
            //tm1637.showNumberDec(minutes * 100 + seconds, true,2,2);
            
        //} else {
             // Display only seconds with leading zero
                //Ldisplay.showNumberDec(seconds, true, 2, 4);
            // Countdown finished, blink 00:00
            //if ((millis() / 500) % 2 == 0) {
                //Ldisplay.showNumberDec(0, COLON, true);
               // } 
         //} else {
          // Countdown finished, blink 00:00
            //if ((millis() / 500) % 2 == 0) {
                //Ldisplay.showNumberDec(0, true);
            //} else {
               // Ldisplay.setSegments(blank); //.setSegments(blank) does not work .
            //}
       // }
                //Ldisplay.clearDisplay();
            //}
        
  
  for (litPixel=0; litPixel<1; litPixel++) { //order: initiallization, condition, incremement--where to start, where do you want to go, how do you want to get there
  pixel.setPixelColor (litPixel, 255, 0, 0); //first number red, second green, thrid blue -- RGB 
  pixel.show();
  pixel.clear();
 
  
  // Write data to card
     //  uint8_t blockNumber = 2; // Choose a writable block
     //  uint8_t dataToWrite[] = "Lightning";
      // if (nfc.writeData(blockNumber, dataToWrite)) {
        //    Serial.println("Data written successfully.");
       // } else {
          // Serial.println("Failed to write data.");
        //}
  
  // For S50 card/tag, block 1-2, 4-6, 8-10, 12-14... 56-58, 60-62 are for user data
  // You can read/write these blocks freely.
  // Use "MifareClassic_ReadAllMemory.ino" to check all the blocks
  if (nfc.scan()) {
    if (nfc.readData(dataRead, READ_BLOCK_NO) != 1) {
      Serial.println("Block ");
      Serial.print(READ_BLOCK_NO);
      Serial.println(" read failure!");
    }
    else {
      Serial.print("Block ");
      Serial.print(READ_BLOCK_NO);
      Serial.println(" read success!");

      Serial.print("Data read(string):");
      Serial.println((char *)dataRead);
      Serial.print("Data read(HEX):");
  
      Serial.println();

  pixel.setPixelColor (litPixel, 0, 255, 0);
  pixel.show();
  delay(1800);
  pixel.clear();
    }

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,32);//left and middle of display
  //display.printf(" Read\n  success!\n");
  //delay(500);
  display.printf("%s\n",(char *)dataRead);
  display.display();
  delay(5000);
  display.clearDisplay();


  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(12,8);
  display.printf("Great\n Job!\n");
  display.display();
  delay(1800);
  display.clearDisplay();

  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(12,8);
  display.printf(" Scan\n Below\n");
  display.display();
  display.clearDisplay();
}
delay (500);
  }
//Serial.printf("DFRobot DFPlayer Mini Demo\n");
//Serial.printf("Initializing DFPlayer ... (May take 3~5 seconds)\n");
  
  //myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  //myDFPlayer.loop(1);  //Play the first mp3
  //myDFPlayer.enableLoopAll();

}

