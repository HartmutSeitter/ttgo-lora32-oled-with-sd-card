#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LoRa.h>        //  LoRa
#include "./mySD.h"

//#include <SPI.h>
//#include <Wire.h> 
//#include "SSD1306Wire.h" 

ext::File root;
ext::File root1;
//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);



//  OLED screen text rows:
#define  row1     0     //  y=0 is top row of size 1 text
#define  row2    10
#define  row3    20
#define  row4    30
#define  row5    40
#define  row6    50     //  row7 at 70 is too low

//  SPI port #2:  SD Card Adapter
#define  SD_CLK     17
#define  SD_MISO    13
#define  SD_MOSI    14
#define  SD_CS      23

//  SPI port:  LoRa (SX1278) 
//       LoRa      Pin
#define  LoRa_SCK    5
#define  LoRa_MISO  19
#define  LoRa_MOSI  27
#define  LoRa_CS    18
#define  LoRa_RST   14    //  LoRa_Reset
#define  DI0        26    //  LoRa_IRQ
#define  BAND    866E6    //  other freq: 433E6  // or ? LoRa f(MHz)

#define  Select    LOW   //  Low CS means that SPI device Selected
#define  DeSelect  HIGH  //  High CS means that SPI device Deselected

ext::File     sessionFile;   //  SD card filenames are restricted to 8 characters + extension

int counter = 0;        //  count sent LoRa messages
int count = 0;
String recv = "";
String temp = "";
/***********************************************************/
void setup(){  
  // set output pins
  //hs001 pinMode(oRST,OUTPUT);
  pinMode(SD_CS,OUTPUT);
  pinMode(LoRa_CS,OUTPUT);
  digitalWrite(LoRa_CS, DeSelect);


  Serial.begin(115200);
  while(!Serial);                     //  wait to connect to computer

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,row1);
  display.print("LORA RECEIVER ");
  display.display();

  Serial.print("Initializing SD card...");
  digitalWrite(SD_CS, Select);    //  SELECT (Low) SD Card SPI

  // now start the SD card interface on 2nd SPI

  if (!SD.begin( SD_CS, SD_MOSI, SD_MISO, SD_CLK )) {
    Serial.println("initialization failed!");
    display.setCursor(0,row2);
    display.println("SD Card NOT OK!" );
    display.display();
    //  now what?
  } else {
    Serial.println("initialization SD card done.");
    display.setCursor(0,row2);
    display.println("SD Card OK!" );
    display.display();
    delay(1000);
  }
  /* open "test.txt" for writing */
  Serial.println("open test.txt for write");
  root = SD.open("test.txt", FILE_WRITE);
  if (root) {
    root.println("Hello world! von hartmut");
    root.flush();
    root.close();
  } else {    //  file open error
    Serial.println("error opening test.txt");
  }
  Serial.println("data written to test1.txt");
  display.setCursor(0,row3);
  display.println("Wrote in test.txt" );
  display.display();
  delay(100);
  
  
  /* after writing, then reopen the file and read it */
  Serial.println("open test2.txt for read");
  root = SD.open("test2.txt", FILE_READ);
  if (root) {    /* read from the file to the end of it */
    while (root.available()) {  // read the file
      Serial.write(root.read());
    }
    root.close();
  } else {
    Serial.println("error opening test.txt");
  }
  Serial.println("data read from test2.txt");
  display.setCursor(0,row4);
  display.println("Read from tes2t.txt");
  display.display();
  delay(100);
  //  done testing the SD Card
  Serial.println("sd card - cs deselect");
  digitalWrite(SD_CS, DeSelect); 
  //SD.end();

  //  now test the LoRa
  Serial.println("in setup - start lora");
  SPI.begin( LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_CS );
  LoRa.setPins( LoRa_CS, LoRa_RST, DI0 );
  // select the LoRa Modul via cs
  digitalWrite(LoRa_CS, Select);   //  SELECT (low) LoRa SPI 

  Serial.println("LoRa Receiver starting");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    display.setCursor(0,row5);
    display.println("LoRa Init Failed!" );
    display.display();
    // now what?
  } else {
    Serial.println("LoRa Initial OK!");
    display.setCursor(0,row5);
    display.println("LoRa Initialized OK!" );
    display.display();
    delay(1000);
  }
  //hs001 digitalWrite(LoRa_CS, DeSelect);  
  Serial.println("Setup for lora done!");
  delay( 100 ); 

}
 
       
void loop() {
    digitalWrite(LoRa_CS, Select);

    if (LoRa.parsePacket()) {
        Serial.println("in lora.parsePacket loop");
        recv = "";
        while (LoRa.available()) {
            recv += (char)LoRa.read();
        }
        count++;
        display.clearDisplay();
        display.setCursor(1,row1);
        display.print(recv);
        Serial.print("recv = ");
        Serial.println(recv);
        String info = "[" + String(count) + "]" + "RSSI " + String(LoRa.packetRssi());
        display.setCursor(0,row2);
        display.print(info);
        display.display();
        Serial.print("info = ");
        Serial.println(info);
    }
    if(temp != recv){
      temp = recv;  
      Serial.println("lora - cs deselect");
      digitalWrite(LoRa_CS, DeSelect);
      delay(50);    
      Serial.println("sd card - cs select");
      digitalWrite(SD_CS, Select);   

      /* open "test.txt" for writing */
      Serial.println("open text1.txt for write");
      root = SD.open("test1.txt", FILE_WRITE);
      Serial.print(" root = ");
      Serial.println(root);
      if (root) {
        root.print("new msg received = ");
        root.println(temp);
        root.flush();
        root.close();
      } else {    //  file open error
        Serial.println("error opening test1.txt");
      }
      display.setCursor(0,row4);
      display.println(temp);
      display.setCursor(0,row6);
      display.println("Wrote in test1.txt" );
      display.display();
      delay(50);
      
      Serial.println("sd card - cs deselect");
      digitalWrite(SD_CS, DeSelect);  
      delay(50);
      Serial.println("lora - cs select");
      digitalWrite(LoRa_CS, Select);
    }
    delay(100);
 
}