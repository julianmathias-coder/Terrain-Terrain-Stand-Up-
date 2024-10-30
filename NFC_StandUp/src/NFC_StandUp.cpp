//Brian or EJ can click button to start timer since we don't always start lunch at the same time each day



#include "Particle.h"
#include <DFRobot_PN532.h>//The order matters
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
//#include "TM1637Display.h"
#include "Grove_4Digit_Display.h"
#include <neopixel.h>
#include "DFRobotDFPlayerMini.h"
#include "Button.h"

//DFRobotDFPlayerMini myDFPlayer;
//unsigned int lastSong;
//void printDetail(uint8_t type, int value);

const int CLK = D4;
const int DIO = D5;
bool keepCounting;




TM1637 tm1637(CLK,DIO);
Button startCountdown(D3)//need to wire a button

bool countDown(bool restart = false, int countStart = 60);


unsigned int lastTime;
int min10,min01,sec10,sec01;

 int8_t NumTab[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};//0~9,A,b,C,d,E,F
 int8_t ListDisp[4];
 int i = 0;
 int count = 0;


//const int blank[]={0,0,0,0};

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

  Serial.begin(9600); //would I put another bracket here since I have other things in void setup?
  waitFor(Serial.isConnected,10000);

  tm1637.init();
  tm1637.set(7);//can also use BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  tm1637.point(POINT_ON);

  keepCounting = false;
}
  
  Serial.begin(9600);//do I need to keep putting serial begin since it's the same for each device?
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
 

  tm1637.init();
  tm1637.set(BRIGHTEST);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  tm1637.point(POINT_ON);
  lastTime = millis();
  //Ldisplay.setBrightness(7); // Set maximum brightness (max brightness is 7, not 15)
  //Ldisplay.clearDisplay();
  //Ldisplay.clear(); does not compile with .clearDisplay or .clear
  //Ldisplay.setSegments(blank); // Clear the display
}

void loop() {
  
  if(startCountdown.isClicked()) {
    countDown(true,90);
    keepCounting = true;
  }
  
  if(keepCounting) {
    keepCounting = countDown();
    if (keepCounting) {
      Serial.printf("Countdown is complete");
      //negative punishement Hue flash, annoying noise here 
      //Serial.printf("DFRobot DFPlayer Mini Demo\n");
      //Serial.printf("Initializing DFPlayer ... (May take 3~5 seconds)\n");
  
      //myDFPlayer.volume(30);  //Set volume value. From 0 to 30
      //myDFPlayer.loop(1);  //Play the first mp3
      //myDFPlayer.enableLoopAll();
    }
  }
    for (int i = 0; i < BLOCK_SIZE; i++) {
      Serial.print(dataRead[i], HEX);
      Serial.print(" ");
      dataRead[i] = 0;
  }     
  
    for (litPixel=0; litPixel<1; litPixel++) { //order: initiallization, condition, incremement--where to start, where do you want to go, how do you want to get there
      pixel.setPixelColor (litPixel, 255, 0, 0); //first number red, second green, thrid blue -- RGB 
      pixel.show();
      pixel.clear();
  }
  
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
    }
      Serial.println();

  pixel.setPixelColor (litPixel, 0, 255, 0);
  pixel.show();
  delay(1800);
  pixel.clear();
    

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
//delay (500);
  }
}

    //control Hue lights and turn on Wemo switches, but I want these commands at the beginning of countdown.
    //add Hue and WeMo code here as well.
  
  bool countDown(bool restart, int countStart) {
    static int count = 60;
    static unsigned int lastTime = 0;
    int min10,min01,sec10,sec01;

    if(restart) {
      count = countStart;
    }

    if(millis()-lastTime > 1000) {
      lastTime = millis();
      count--;
      
      min10 = count/600;
      min01 = (count/60)%10;
      sec10 = (count%60)/10;
      sec01 = (count%60)%10;

      tm1637.display(0,min10);
      tm1637.display(1,min01);
      tm1637.display(2,sec10);
      tm1637.display(3,sec01);
    }

    if(count <=0) {
      count = 60;
      return false;
    }
    
    return true;
  }
  
  
  //count = 60; //create function
  //while(count >= 0) {
   // if(millis()-lastTime > 1000) {
     // lastTime = millis();
      //count--;
  
      //min10 = count/600;
     // min01 = (count/60)%10;
      //sec10 = (count%60)/10;
      //sec01 = (count%60)%10;

      //tm1637.display(0,min10);
      //tm1637.display(1,min01);
      //tm1637.display(2,sec10);
      //tm1637.display(3,sec01);
    //}
  
  



