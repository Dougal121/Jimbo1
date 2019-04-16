/* NeoPixel Stick - Examples
 * Adafruit NeoPixel Stick consists of 8x 5050 RGB WS2812B LEDs
 * http://adafruit.com/products/1426
 *
 * Connections:
 * D2 --- DIN
 *     -- 5VDC  Supplied externally
 * G ---- GND (either)
 *
 * Adafruit recommendations:
 * - add a 1000uF capacitor across the power lines 5VDC and GND.
 * - add a 300-500 Ohm resistor on first pixel's data input DIN.
 * - avoid connecting on a live circuit... if you must, connect GND first.
 *
 * Dependencies:
 * https://github.com/adafruit/Adafruit_NeoPixel
 */
#include <ESP8266WiFi.h>
#include <WiFiUDP.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266httpUpdate.h>
#include <TimeLib.h>
#include <EEPROM.h>
#include <stdio.h>
//#include <DNSServer.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>

#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini myDFPlayer;

#define MAX_MEM_PATTERN 30
#define MAX_MEM_MESSAGE 16
#define MAX_MEM_SEQ 50
#define MAX_MSG_LENGTH 60
#define MAX_DESCRIPTION_TEXT 20
#define MAX_STRIPS 3
const byte MAX_WIFI_TRIES = 45 ;


typedef struct __attribute__((__packed__)) {            // eeprome stuff
  byte      sp[MAX_STRIPS] ;     // string patters 0 ... 2
  byte      ptxt  ;              // text for pattern 0 - Off   1->  - Display Text
  byte      sndno ;              // sound number
  byte      sndvol ;             // sound volume from 0..30  default 10
  long      iTime ;              // amount of time in ms to run
} seq_t ;                        // 10  bytes  x  50  => 500

typedef struct __attribute__((__packed__)) {            // eeprome stuff
  char  msg[MAX_MSG_LENGTH] ;  // messages
  long  msgcol  ;              // display color
  long  msgbgcol  ;              // display color
  long  msgeffects ;       
} mes_t ;                          // 64 bytes  x 16 => 1024     576 

typedef struct __attribute__((__packed__)) {            // eeprom stuff
  char      Description[MAX_DESCRIPTION_TEXT] ; // Pattern Name 
  long      paramA ;
  long      paramB ;
  long      paramC ;
  long      lDelay   ;        // delay in ms if used  
  uint16_t  paramD ;
  uint16_t  paramE ;
  uint16_t  paramF ;
} sub_pattern_t ;             // 42 bytes  x 30 => 1260

typedef struct __attribute__((__packed__)) {            // eeprom ish stuff
bool           bRun     ;  // run sequencer
byte           bOnOff   ;  // light on off     x4 strings  .. 4 bits
int            iBright ;   // brightness
int            iSeq ;
int            iMin ;
int            iMax ;
long           iChange ;   // next change of sequence
long           iloopCount ; 
sub_pattern_t  pattern[MAX_MEM_PATTERN] ;
mes_t          message[MAX_MEM_MESSAGE] ;
seq_t          sequence[MAX_MEM_SEQ] ;
} patterns_t ;       // bytes

patterns_t lp ;      // light patterns


typedef struct __attribute__((__packed__)) {     // eeprom stuff
  unsigned int localPort = 2390;          // 2 local port to listen for NTP UDP packets
  unsigned int localPortCtrl = 8666;      // 4 local port to listen for Control UDP packets
  unsigned int RemotePortCtrl = 8664;     // 6 local port to listen for Control UDP packets
  char NodeName[24] ;                     // 46 
  char nssid[24] ;                        // 62  
  char npassword[24] ;                    // 78
  char cssid[24] ;
  char cpassword[24] ;                    // 126
} general_housekeeping_stuff_t ;          // computer says it's 136 not 126 ??? is my maths crap ????

general_housekeeping_stuff_t ghks ;



const byte FLASH_BTN = D3 ;    // GPIO 0 = FLASH BUTTON 
const byte SCOPE_PIN = D1 ;
const byte LED = BUILTIN_LED ;  // = D4 ;

#define BUFF_MAX 32
char buff[BUFF_MAX]; 

IPAddress MyIP(192,168,2,110) ;
IPAddress MyIPC(0,0,0,0) ;

byte rtc_sec ;
byte rtc_min ;
byte rtc_hour ;
byte rtc_fert_hour ;
float rtc_temp ;
unsigned int localPort2 = 8080;     // local port to listen for NTP UDP packets
int iDoSave ;
uint8_t rtc_status ;
long lScanCtr = 0 ;
long lScanLast = 0 ;
int  iSamplePixel = 10 ;
long lRebootCode = 0 ;

WiFiUDP patternudp;
ESP8266WebServer server(80);
ESP8266WebServer OTAWebServer(81);
ESP8266HTTPUpdateServer OTAWebUpdater;

// NeoPixel stick DIN pin
#define DIN_PIN1 D2
#define DIN_PIN2 D3
#define DIN_PIN3 D5
#define DIN_PIN4 D6

// How many NeoPixels on the stick?
int NUM_PIXELS[MAX_STRIPS] = { 75 , 37 , 204 } ; // flexable bit round outside + wick
//#define NUM_PIXELS2 37 // little dot thingy
//#define NUM_PIXELS3 204 // Big ring thingy

// Third parameter:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream (NeoPixel Stick)
//   NEO_KHZ400  400 KHz bitstream for FLORA pixels
//   NEO_KHZ800  800 KHz bitstream for High Density LED strip (NeoPixel Stick)
Adafruit_NeoPixel strip[MAX_STRIPS] { Adafruit_NeoPixel(NUM_PIXELS[0], DIN_PIN1 , NEO_GRB + NEO_KHZ800),Adafruit_NeoPixel(NUM_PIXELS[1], DIN_PIN2 , NEO_GRB + NEO_KHZ800) ,Adafruit_NeoPixel(NUM_PIXELS[2], DIN_PIN3 , NEO_GRB + NEO_KHZ800)  };
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(24, 8, DIN_PIN4 , NEO_MATRIX_BOTTOM + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE, NEO_GRB + NEO_KHZ800);


long lMatrixNext ;
int lCurrentSeq ;
long lMatrixDelay = 90 ;
int  x = matrix.width();
int  lMatrixTextWidth = 0 ;
int pass = 0;
bool bConfig = false ;
long lFlashChange = 0 ;
int  iFlashState = 0  ;

unsigned long processPatternUDPpacket(void){
int i  ;  
byte MyRGB[3] ;

  patternudp.read((char *)&i, 2); // read the first byte to work out the address of the pixel data packet into the buffer
  
  while (patternudp.available()){  // procees the rest of the pixel bytes
    patternudp.read(MyRGB, sizeof(MyRGB));
    if (( i >= 0 ) && ( i < NUM_PIXELS[0]  )) {
      strip[0].setPixelColor(i++ , MyRGB[0] , MyRGB[1] , MyRGB[2] ) ;  
    }
  }
}



//#####################################  SETUP  #####################################################
void setup() {
int i , j = 0 ; 
String host ;
  
  pinMode(BUILTIN_LED,OUTPUT);
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  Serial.println(String(ESP.getResetReason()));
  Serial.println(String(ESP.getResetInfo()));

  EEPROM.begin(3800); 
  LoadParamsFromEEPROM(true);
//  BackIntheBoxMemory();
//  LoadParamsFromEEPROM(false);

  lp.iChange = 0 ;
  lp.iloopCount = 0 ;
  lp.iSeq = 0 ;
  
  for (i = 0 ; i < MAX_STRIPS ; i++ ) {  //  rgb luminance test on all 3 strips  
    strip[i].begin();
    strip[i].setPixelColor(0,63,0,0);
    strip[i].setPixelColor(1,0,63,0);
    strip[i].setPixelColor(2,0,0,63);
    strip[i].setPixelColor(strip[i].numPixels()-3,63,0,0);
    strip[i].setPixelColor(strip[i].numPixels()-2,0,63,0);
    strip[i].setPixelColor(strip[i].numPixels()-1,0,0,63);
    strip[i].setBrightness(lp.iBright);
    strip[i].show(); // Start with all pixels off
  }

  WiFi.disconnect();
  Serial.println("Configuring soft access point...");
//  WiFi.mode(WIFI_AP_STA);  // we are having our cake and eating it eee har
  sprintf(ghks.cssid,"Configure_%08X\0",ESP.getChipId());
  if ( ghks.cssid[0] == 0 || ghks.cssid[1] == 0 ){   // pick a default setup ssid if none
    sprintf(ghks.cpassword,"\0");
  }
  MyIPC = IPAddress (192, 168, 5 +(ESP.getChipId() & 0x7f ) , 1);
  WiFi.softAPConfig(MyIPC,MyIPC,IPAddress (255, 255, 255 , 0));  
  Serial.println("Starting access point...");
  Serial.print("Config SSID: ");
  Serial.println(ghks.cssid);
  Serial.print("Config Password: >");
  Serial.print(ghks.cpassword);
  Serial.println("< " + String(ghks.cpassword[0]));
  if (( ghks.cpassword[0] == 0 ) || ( ghks.cpassword[0] == 0xff)){
    WiFi.softAP((char*)ghks.cssid);                   // no passowrd
  }else{
    WiFi.softAP((char*)ghks.cssid,(char*) ghks.cpassword);
  }
  MyIPC = WiFi.softAPIP();  // get back the address to verify what happened
  Serial.print("Soft AP IP address: ");
  snprintf(buff, BUFF_MAX, ">> IP %03u.%03u.%03u.%03u <<", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);      
  Serial.println(buff);
  
  bConfig = false ;   // are we in factory configuratin mode
  
  Serial.print("Network SSID: >");
  Serial.print(ghks.nssid);
  Serial.println("<");
  if ( ghks.nssid[0] == 0 ){
    Serial.println("Invalid Network SSID");
    j = MAX_WIFI_TRIES ;
  }else{
    Serial.print("Network Password: >");
    Serial.print(ghks.npassword);
    Serial.println("<");
    Serial.print("Connecting network client ");
    if ( ghks.npassword[0] == 0 ){
      WiFi.begin((char*)ghks.nssid);                    // connect to unencrypted access point      
    }else{
      WiFi.begin((char*)ghks.nssid, (char*)ghks.npassword);  // connect to access point with encryption
    }
    while (( WiFi.status() != WL_CONNECTED ) && ( j < MAX_WIFI_TRIES )) {
      j = j + 1 ;
      delay(200);
      strip[0].setPixelColor(5+j,0,0,63);
      strip[0].show(); // Start with all pixels off
      digitalWrite(BUILTIN_LED,!digitalRead(BUILTIN_LED));
      delay(200);
      digitalWrite(BUILTIN_LED,!digitalRead(BUILTIN_LED));
      Serial.print(".");
    } 
  }
  Serial.println("");
  if ( j >= MAX_WIFI_TRIES ) {
     bConfig = true ;
     WiFi.disconnect();
     Serial.println("Client Connection Failed");
  }else{
     Serial.println("");
     Serial.println("WiFi connected");  
     Serial.print("IP address: ");
     MyIP =  WiFi.localIP() ;
//     Serial.println(MyIP) ;
     snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);            
     Serial.println(buff);
  }
  if ( !bConfig ){ 
    Serial.println("Starting UDP");
    patternudp.begin(localPort2);                 // pattern reciever buffer 8080
    Serial.print("Pattern Upload Local UDP port: ");
    Serial.println(patternudp.localPort());
  }
  sprintf(ghks.NodeName,"Control_%08X\0",ESP.getChipId());  
//  String(ghks.NodeName).toCharArray(buff,sizeof(buff));
  
/*  if (MDNS.begin(buff)) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("MDNS responder started");
    Serial.print("You can now connect to http://");
    Serial.print(host);
    Serial.println(".local");
  }
*/

  server.on("/", handleRoot);
  server.on("/setup", handleRoot);
  server.on("/pattern", handleRoot);
  server.on("/scope" , pixelscope );
  server.on("/info", handleInfo);
  server.on("/eeprom", DisplayEEPROM);  
  server.on("/message", handleRoot);    
  server.onNotFound(handleNotFound);  
  server.begin();
   
  rtc_min = minute();
  rtc_sec = second(); 

  OTAWebUpdater.setup(&OTAWebServer);
  OTAWebServer.begin(); 
  randomSeed(WiFi.RSSI());
  lRebootCode = random(0,+2147483640) ;
  lp.iloopCount = 0 ;
  Serial.println("Matrix Start");

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(lp.iBright);
  matrix.setTextSize(1);

  Serial.setDebugOutput(false);  
  Serial.println("Swapping Serial to dtrobot sound board and inialising");
  Serial.begin(9600); // reduce speed and swap over ports 
  Serial.swap();  // swap to RX GPIO D7/GPIO13     TX  D8/GPIO15
  if (!myDFPlayer.begin(Serial)) {  //Use softwareSerial to communicate with mp3.
   
  }
  myDFPlayer.volume(20);  // Set volume value. From 0 to 30
  myDFPlayer.play(1);     // Play the first mp3
  lp.iChange = millis() + lp.sequence[lp.iSeq].iTime ;
  lFlashChange = millis() ;  
}

//#######################################################################  LOOP  #####################################################
void loop() {
long lTime , dl ;  
int j , k , ii , jj , kk ;
unsigned char r, g, b ;
uint32_t pcol ;

  server.handleClient();
  OTAWebServer.handleClient();

  if ( !bConfig  ){
    if ( patternudp.parsePacket() ) {
      processPatternUDPpacket();
    }
  }
  
  lTime = millis() ;
  lScanCtr++;

  if ( lp.bOnOff != 0 ){  
    j = lp.sequence[lp.iSeq].ptxt ; // if non zero select a number
    if (( lMatrixNext < lTime ) || ((lMatrixNext - lTime) > 1000 )){
      matrix.fillScreen(lp.message[j].msgbgcol);      // set backgrounf color  
      matrix.setCursor(x, 0);    
      matrix.setTextColor(lp.message[j].msgcol);
      lMatrixTextWidth = -6 * String(lp.message[j].msg).length() ; 
      matrix.print(lp.message[j].msg);
      if(--x < lMatrixTextWidth ) {
        x = matrix.width();
      }
      lMatrixNext = lMatrixDelay + lTime ;
      matrix.show();  
    }
  }else{
    matrix.fillScreen(0);      // set backgrounf color  
    matrix.show();   // switch off    
  }
  
  if (rtc_sec != second() ){
    lScanLast = lScanCtr ;
    lScanCtr = 0 ;
    
    if ( iDoSave == 2 ) {  // save them Active via web or 
      LoadParamsFromEEPROM(false);
      iDoSave = 0 ;  // only do once
    }
    if ( iDoSave == 3 ) {  // load them
      LoadParamsFromEEPROM(true);
      iDoSave = 0 ;  // only do once
    }    
    if ( iDoSave == 4 ) {  // reset them
      BackIntheBoxMemory();
      iDoSave = 0 ;  // only do once
    }    

    
    rtc_sec = second() ;
    digitalWrite(BUILTIN_LED,!digitalRead(BUILTIN_LED));
  }

  if ( rtc_min != minute()){
    rtc_min = minute() ;
  }
  
  
  if (rtc_hour != hour()){
    rtc_hour = hour(); 
  }

  for ( k = 0 ; k < MAX_STRIPS ; k++ ) {
    if ( lp.bOnOff != 0 ){
      dl = lp.pattern[lp.sequence[lp.iSeq].sp[k]].lDelay ;
      switch (lp.sequence[lp.iSeq].sp[k]){  // pattern on strip k
        case 0:
          warmWhiteShimmer(k,lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramC);
          delay(dl);
        break;
        case 4:
        case 3: 
        case 2:
        case 1:
          if (  millis() > ( lp.iChange - lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramC )) {
            brightTwinkle(k,lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramA,lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramB,0);
          }else{
            brightTwinkle(k,lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramA,lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramB,1);          
          }
          delay(dl);
        break;
        case 5:
          collision(k);
          delay(dl);
        break;
        case 6: 
          traditionalColors(k);
          delay(dl);
        break;
        case 7:
          gradient(k);
          delay(dl);
        break;
        case 8:   // mixing colors 
          colorExplosion(k,0);
          delay(dl);
        break;
        case 9:   // rainbow colours 
          j = lp.iloopCount % 256 ;
          for(int i=0; i<strip[k].numPixels(); i++) {
            strip[k].setPixelColor(i, Wheel(k,(i+j) & 255));
          }
          delay(dl);
        break;
        case 10: // random
          strip[k].setPixelColor(random(0,strip[k].numPixels()-1), strip[k].Color(random(0,255), random(0,255), random(0,255)));
          delay(dl);
        break;
        case 13:
        case 12:
        case 11:  // theater chase 
          j = lp.iloopCount % lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD ;
            for (int i=0+j; i < strip[k].numPixels(); i=i+lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD) {
              strip[k].setPixelColor(i, strip[k].Color(lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramA, lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramB, lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramC));    //turn every third pixel on        strip[0].show();  
            }
            delay(dl);
            if ( j == 0 ){
              j = lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD - 1 ;
            }else{
              j-- ;
            }
            for (int i=0+j; i < strip[k].numPixels(); i=i+lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD) {
              strip[k].setPixelColor(i, 0);        //turn every third pixel off
            }
        break;
        case 14:  // rainbow theater chase 
          j = lp.iloopCount ;
          for (int q=0; q < 3; q++) {
            for (int i=0; i < strip[k].numPixels(); i=i+3) {
              strip[k].setPixelColor(i+q, Wheel(k, (i+j) % 255));    //turn every third pixel on
            }        
            strip[k].show();
  
            delay(dl);
      
            for (int i=0; i < strip[k].numPixels(); i=i+3) {
              strip[k].setPixelColor(i+q, 0);        //turn every third pixel off
            }
          }    
        break;
        case 15:  // rainbow 
          j = lp.iloopCount ;
          for(int i=0; i< strip[k].numPixels(); i++) {
            strip[k].setPixelColor(i, Wheel(k,((i * 256 / strip[k].numPixels()) + j) & 255));
          }
          delay(dl);
        break;
         
        case 16: // udp update of pixels or blank black
          if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD == 1 ){
            for(int i=0; i< strip[k].numPixels(); i++) {
              strip[k].setPixelColor(i, strip[k].Color(0, 0, 0));
            }
          }
        break;

        case 17:
        case 22:
        case 21:
        case 20:
        case 19:
        case 18:  // tri colour chase
            j = 0 ;
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramA != 0 ){
              j++ ;              
            }
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramB != 0 ){
              j++ ;              
            }
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramC != 0 ){
              j++ ;              
            }
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD != 0 ){
              j++ ;              
            }
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramE != 0 ){
              j++ ;              
            }
            kk = lp.iloopCount % j ;
            ii = 0 ; 
            jj = strip[k].numPixels() ;
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramA != 0 ){
              for (int i=0; i < strip[k].numPixels(); i=i+j) {
                strip[k].setPixelColor(((i+ii+kk) % jj ) , lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramA );     
              }
              ii++ ;
            }
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramB != 0 ){
              for (int i=0; i < strip[k].numPixels(); i=i+j) {
                strip[k].setPixelColor(((i+ii+kk) % jj ), lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramB );     
              } 
              ii++ ;
            }
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramC != 0 ){
              for (int i=0; i < strip[k].numPixels(); i=i+j) {
                strip[k].setPixelColor(((i+ii+kk) % jj ), lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramC );     
              }
              ii++ ;
            }
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD != 0 ){
              for (int i=0; i < strip[k].numPixels(); i=i+j) {
                strip[k].setPixelColor(((i+ii+kk) % jj ), lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD );      
              }
              ii++ ;
            }
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramE != 0 ){
              for (int i=0; i < strip[k].numPixels(); i=i+j) {
                strip[k].setPixelColor(((i+ii+kk) % jj ), lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramE );     
              }
              ii++ ;
            }
            delay(dl);
        break;

        case 24:
        case 25:
        case 26:
        case 23:  // solid flash
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD == 0 ){
              iFlashState = 0 ;
            }else{
              if ( millis() > lFlashChange ) {
                lFlashChange = millis() + lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramD   ; 
                if ( iFlashState != 0 ){
                  iFlashState = 0 ;
                }else{
                  iFlashState = 1 ;
                }
              }
            }
            if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramE < 1 ){
              ii = 1 ;
            }else{
              ii = lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramE ;
            }
            for (int i=0; i < strip[k].numPixels(); i=i+ii) {
              if ( iFlashState == 0 ){
                strip[k].setPixelColor(i, strip[k].Color(lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramA, lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramB, lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramC));    //turn every third pixel on        strip[0].show();  
              }else{
                strip[k].setPixelColor(i, 0);                   
              }
            }
            delay(dl);
        break;

        
        default:  // one pixel chase white - color set by A,B,C or 0,0,0 == random
          j = lp.iloopCount % strip[k].numPixels() ;
          for (int i=0; i < strip[0].numPixels(); i++ ) {
            strip[k].setPixelColor(i, strip[k].Color(0, 0, 0));        //turn off
          }
          if (( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramA == 0 ) && ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramB == 0 ) && ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramC == 0 )){
            strip[k].setPixelColor(j, strip[k].Color(random(255), random(255), random(255)));    //turn on          
          }else{
            strip[k].setPixelColor(j, strip[k].Color(lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramA, lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramB, lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramC));    //turn on
          }
          delay(dl);
        break;
      }
    }else{
      for (int i=0 ; i < strip[k].numPixels() ; i++ ) {
        strip[k].setPixelColor(i, strip[k].Color(0, 0, 0));          // turn off
      }
    }
    strip[k].show();    
  } // bottom of the for k
  
  constrain(lp.iMin,0,MAX_MEM_SEQ);
  constrain(lp.iMax,0,MAX_MEM_SEQ);    
  constrain(lp.iSeq,0,MAX_MEM_SEQ);    

  if ( lp.bRun != 0 ){
    if ( millis() > lp.iChange ) {
      lp.iSeq++ ; 
      if ( lp.iSeq > lp.iMax ){
        lp.iSeq = lp.iMin ;
      }
      lp.iChange = millis() + lp.sequence[lp.iSeq].iTime   ; 
      lp.iloopCount = 0  ;      
    }
  }else{
    lp.iChange = millis() + lp.sequence[lp.iSeq].iTime   ; 
  }
  lp.iloopCount ++ ;

  if ( lp.iSeq != lCurrentSeq ){
    myDFPlayer.volume(lp.sequence[lp.iSeq].sndvol);  //  Set volume value. From 0 to 30
    myDFPlayer.stop() ;
    delay(100) ;
//    if (lp.sequence[lp.iSeq].snd == 0 ){  
      myDFPlayer.loop(lp.sequence[lp.iSeq].sndno);     //  Play/loop the file number        
//    }else{
//      myDFPlayer.play(lp.sequence[lp.iSeq].sndno);  //Play the first mp3
//    }
    x = matrix.width();
    lCurrentSeq = lp.iSeq ;                          //  update this   
/*    for ( k = 0 ; k < MAX_STRIPS ; k++ ) {
      if ( lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramE != 0  ){
        strip[k].setBrightness(lp.iBright);
      }else{
        strip[k].setBrightness(lp.pattern[lp.sequence[lp.iSeq].sp[k]].paramE);        
      }
    }*/
  }

  digitalWrite(SCOPE_PIN,!digitalRead(SCOPE_PIN));  // my scope says we are doing this loop at an unreasonable speed except when we do web stuff


/*
   t_httpUpdate_return ret = ESPhttpUpdate.update("http://server/file.bin");
    //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin", "", "fingerprint");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        USE_SERIAL.println("HTTP_UPDATE_OK");
        break;
    }
    
*/  
}   //  #######################################################################  LOOP  ##################################################


// Fill the dots one after the other with a color
void colorWipe(int sp, uint32_t c, uint8_t wait) {
  uint16_t i;
  for(i=0; i<strip[sp].numPixels(); i++) {
    strip[sp].setPixelColor(i, c);
    strip[sp].show();
    delay(wait);
  }
}

void rainbow(int sp , uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip[sp].numPixels(); i++) {
      strip[sp].setPixelColor(i, Wheel(sp,(i+j) & 255));
    }
    strip[sp].show();
    delay(wait);
  }
}

void randomColors(int sp , uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<50; j++) {
    for(i=0; i<strip[sp].numPixels(); i++) {
      strip[sp].setPixelColor(i, strip[sp].Color(random(0,255), random(0,255), random(0,255)));
    }
    strip[sp].show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(int sp , uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip[sp].numPixels(); i++) {
      strip[sp].setPixelColor(i, Wheel(sp,((i * 256 / strip[sp].numPixels()) + j) & 255));
    }
    strip[sp].show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(int sp , uint32_t c, uint8_t wait) {
  uint16_t j, q, i;
  for (j=0; j<10; j++) {  //do 10 cycles of chasing
    for (q=0; q < 3; q++) {
      for (i=0; i < strip[sp].numPixels(); i=i+3) {
        strip[sp].setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip[sp].show();

      delay(wait);

      for (i=0; i < strip[sp].numPixels(); i=i+3) {
        strip[sp].setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(int sp , uint8_t wait) {
  uint16_t j, q, i;
  for (j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (q=0; q < 3; q++) {
      for (i=0; i < strip[sp].numPixels(); i=i+3) {
        strip[sp].setPixelColor(i+q, Wheel(sp, (i+j) % 255));    //turn every third pixel on
      }
      strip[sp].show();

      delay(wait);

      for (i=0; i < strip[sp].numPixels(); i=i+3) {
        strip[sp].setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(int sp , byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip[sp].Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip[sp].Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip[sp].Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}





// This function applies a random walk to val by increasing or
// decreasing it by changeAmount or by leaving it unchanged.
// val is a pointer to the byte to be randomly changed.
// The new value of val will always be within [0, maxVal].
// A walk direction of 0 decreases val and a walk direction of 1
// increases val.  The directions argument specifies the number of
// possible walk directions to choose from, so when directions is 1, val
// will always decrease; when directions is 2, val will have a 50% chance
// of increasing and a 50% chance of decreasing; when directions is 3,
// val has an equal chance of increasing, decreasing, or staying the same.
void randomWalk(unsigned char *val, unsigned char maxVal, unsigned char changeAmount, unsigned char directions)
{
  unsigned char walk = random(directions);  // direction of random walk
  if (walk == 0)
  {
    // decrease val by changeAmount down to a min of 0
    if (*val >= changeAmount)
    {
      *val -= changeAmount;
    }
    else
    {
      *val = 0;
    }
  }
  else if (walk == 1)
  {
    // increase val by changeAmount up to a max of maxVal
    if (*val <= maxVal - changeAmount)
    {
      *val += changeAmount;
    }
    else
    {
      *val = maxVal;
    }
  }
}



// This function fades val by decreasing it by an amount proportional
// to its current value.  The fadeTime argument determines the
// how quickly the value fades.  The new value of val will be:
//   val = val - val*2^(-fadeTime)
// So a smaller fadeTime value leads to a quicker fade.
// If val is greater than zero, val will always be decreased by
// at least 1.
// val is a pointer to the byte to be faded.
void fade(unsigned char *val, unsigned char fadeTime)
{
  if (*val != 0){
    unsigned char subAmt = *val >> fadeTime;  // val * 2^-fadeTime
    if (subAmt < 1) {
      subAmt = 1;  // make sure we always decrease by at least 1
    }  
    *val -= subAmt;  // decrease value of byte pointed to by val
  }
}


// ***** PATTERN WarmWhiteShimmer *****
// This function randomly increases or decreases the brightness of the 
// even red LEDs by changeAmount, capped at maxBrightness.  The green
// and blue LED values are set proportional to the red value so that
// the LED color is warm white.  Each odd LED is set to a quarter the
// brightness of the preceding even LEDs.  The dimOnly argument
// disables the random increase option when it is true, causing
// all the LEDs to get dimmer by changeAmount; this can be used for a
// fade-out effect.
void warmWhiteShimmer(int sp , unsigned char dimOnly)
{
  const unsigned char maxBrightness = 254 ;  // cap on LED brighness   120
  const unsigned char changeAmount = 2;   // size of random walk step
  unsigned char r, g, b ;
  uint32_t pcol ;
  for (int i = 0; i < strip[sp].numPixels(); i += 2)
  {
    // randomly walk the brightness of every even LED
//    randomWalk(&colors[i].red, maxBrightness, changeAmount, dimOnly ? 1 : 2);
    pcol = strip[sp].getPixelColor(i) ;
    r = ( pcol >> 16 ) & 0xff  ;
    randomWalk(&r, maxBrightness, changeAmount, dimOnly ? 1 : 2);
    
    // warm white: red = x, green = 0.8x, blue = 0.125x
//    colors[i].green = colors[i].red*4/5;  // green = 80% of red
//    colors[i].blue = colors[i].red >> 3;  // blue = red/8
    g = r*4/5;  // green = 80% of red
    b = r >> 3;  // blue = red/8
    
    strip[sp].setPixelColor(i,r,g,b);
    // every odd LED gets set to a quarter the brighness of the preceding even LED
    if (i + 1 < strip[sp].numPixels())
    {     
//      colors[i+1] = (rgb_color){colors[i].red >> 2, colors[i].green >> 2, colors[i].blue >> 2};
      pcol = strip[sp].getPixelColor(i) ;
      r = (pcol >> 16 ) & 0xff ;
      g = (pcol & 0xff00 ) >> 8  ;
      b = pcol & 0xff  ;
      strip[sp].setPixelColor(i+1,r>>2,g>>2,b>>2);
    }
  }
}

// ***** PATTERN TraditionalColors *****
// This function creates a repeating patern of traditional Christmas
// light colors: red, green, orange, blue, magenta.
// Every fourth LED is colored, and the pattern slowly moves by fading
// out the current set of lit LEDs while gradually brightening a new
// set shifted over one LED.
void traditionalColors(int sp)
{
  // loop counts to leave strip initially dark
  const unsigned char initialDarkCycles = 10;
  // loop counts it takes to go from full off to fully bright
  const unsigned char brighteningCycles = 20;
  unsigned char r, g, b ;
  uint32_t pcol ;
  
  if (lp.iloopCount < initialDarkCycles)  // leave strip fully off for 20 cycles
  {
    return;
  }

  // if NUM_PIXELS is not an exact multiple of our repeating pattern size,
  // it will not wrap around properly, so we pick the closest LED count
  // that is an exact multiple of the pattern period (20) and is not smaller
  // than the actual LED count.
  unsigned int extendedLEDCount = (((strip[sp].numPixels()-1)/20)+1)*20;

  for (int i = 0; i < extendedLEDCount; i++)
  {
    unsigned char brightness = (lp.iloopCount - initialDarkCycles)%brighteningCycles + 1;
    unsigned char cycle = (lp.iloopCount - initialDarkCycles)/brighteningCycles;

    // transform i into a moving idx space that translates one step per
    // brightening cycle and wraps around
    unsigned int idx = (i + cycle)%extendedLEDCount;
    if (idx < strip[sp].numPixels())  // if our transformed index exists
    {
      if (i % 4 == 0)
      {
        // if this is an LED that we are coloring, set the color based
        // on the LED and the brightness based on where we are in the
        // brightening cycle
        switch ((i/4)%5)
        {
           case 0:  // red
             strip[sp].setPixelColor(idx,200 * brightness/brighteningCycles,10 * brightness/brighteningCycles,10 * brightness/brighteningCycles);
//             colors[idx].red = 200 * brightness/brighteningCycles; 
//             colors[idx].green = 10 * brightness/brighteningCycles; 
//             colors[idx].blue = 10 * brightness/brighteningCycles;  
             break;
           case 1:  // green
             strip[sp].setPixelColor(idx,10 * brightness/brighteningCycles,200 * brightness/brighteningCycles,10 * brightness/brighteningCycles);
//             colors[idx].red = 10 * brightness/brighteningCycles; 
//             colors[idx].green = 200 * brightness/brighteningCycles;  
//             colors[idx].blue = 10 * brightness/brighteningCycles; 
             break;
           case 2:  // orange
             strip[sp].setPixelColor(idx,200 * brightness/brighteningCycles,120 * brightness/brighteningCycles,0 * brightness/brighteningCycles);
//             colors[idx].red = 200 * brightness/brighteningCycles;  
//             colors[idx].green = 120 * brightness/brighteningCycles; 
//             colors[idx].blue = 0 * brightness/brighteningCycles; 
             break;
           case 3:  // blue
             strip[sp].setPixelColor(idx,10 * brightness/brighteningCycles,10 * brightness/brighteningCycles,200 * brightness/brighteningCycles);
//             colors[idx].red = 10 * brightness/brighteningCycles; 
//             colors[idx].green = 10 * brightness/brighteningCycles; 
//             colors[idx].blue = 200 * brightness/brighteningCycles; 
             break;
           case 4:  // magenta
             strip[sp].setPixelColor(idx,200 * brightness/brighteningCycles,64 * brightness/brighteningCycles,145 * brightness/brighteningCycles);
//             colors[idx].red = 200 * brightness/brighteningCycles; 
//             colors[idx].green = 64 * brightness/brighteningCycles;  
//             colors[idx].blue = 145 * brightness/brighteningCycles;  
             break;
        }
      }
      else
      {
        // fade the 3/4 of LEDs that we are not currently brightening
//        fade(&colors[idx].red, 3);
//        fade(&colors[idx].green, 3);
//        fade(&colors[idx].blue, 3);

        pcol = strip[sp].getPixelColor(idx) ;
        r = pcol >> 16  ;
        g = (pcol & 0xff00 ) >> 8  ;
        b = pcol & 0xff  ;
        fade(&r, 3);
        fade(&g, 3);
        fade(&b, 3);
        strip[sp].setPixelColor(idx, r, g ,b );
      }
    }
  }
}




// Helper function for adjusting the colors for the ColorExplosion
// pattern.  Odd colors get brighter and even colors get dimmer.
// The propChance argument determines the likelihood that neighboring
// LEDs are put into the brightening stage when the central LED color
// is 31 (chance is: 1 - 1/(propChance+1)).  The neighboring LED colors
// are pointed to by leftColor and rightColor (it is not important that
// the leftColor LED actually be on the "left" in your setup).
void colorExplosionColorAdjust(int sp, unsigned char *color, unsigned char propChance,
 unsigned char *leftColor, unsigned char *rightColor)
{
  if (*color == 31 && random(propChance+1) != 0)
  {
    if (leftColor != 0 && *leftColor == 0)
    {
      *leftColor = 1;  // if left LED exists and color is zero, propagate
    }
    if (rightColor != 0 && *rightColor == 0)
    {
      *rightColor = 1;  // if right LED exists and color is zero, propagate
    }
  }
  brightTwinkleColorAdjust(color);
}


// ***** PATTERN ColorExplosion *****
// This function creates bursts of expanding, overlapping colors by
// randomly picking LEDs to brighten and then fade away.  As these LEDs
// brighten, they have a chance to trigger the same process in
// neighboring LEDs.  The color of the burst is randomly chosen from
// among red, green, blue, and white.  If a red burst meets a green
// burst, for example, the overlapping portion will be a shade of yellow
// or orange.
// When true, the noNewBursts argument changes prevents the generation
// of new bursts; this can be used for a fade-out effect.
// This function uses a very similar algorithm to the BrightTwinkle
// pattern.  The main difference is that the random twinkling LEDs of
// the BrightTwinkle pattern do not propagate to neighboring LEDs.
void colorExplosion(int sp , unsigned char noNewBursts)
{
  unsigned char r, g, b ;
  unsigned char rl, gl, bl ;
  unsigned char rr, gr, br ;
  uint32_t pcol ;
  // adjust the colors of the first LED
  pcol = strip[sp].getPixelColor(0) ;
  r = pcol >> 16  ;
  g = (pcol & 0xff00 ) >> 8  ;
  b = pcol & 0xff  ;
  pcol = strip[sp].getPixelColor(1) ;
  rr = pcol >> 16  ;
  gr = (pcol & 0xff00 ) >> 8  ;
  br = pcol & 0xff  ;
//  colorExplosionColorAdjust(&colors[0].red, 9, (unsigned char*)0, &colors[1].red);
//  colorExplosionColorAdjust(&colors[0].green, 9, (unsigned char*)0, &colors[1].green);
//  colorExplosionColorAdjust(&colors[0].blue, 9, (unsigned char*)0, &colors[1].blue);
  colorExplosionColorAdjust(sp,&r, 9, (unsigned char*)0, &rr);
  colorExplosionColorAdjust(sp,&g, 9, (unsigned char*)0, &gr);
  colorExplosionColorAdjust(sp,&b, 9, (unsigned char*)0, &br);
  strip[sp].setPixelColor(0,r,g,b);
  strip[sp].setPixelColor(1,rr,gr,br);
  
  for (int i = 1; i < strip[sp].numPixels() - 1; i++)
  {
    pcol = strip[sp].getPixelColor(i) ;
    r = pcol >> 16  ;
    g = (pcol & 0xff00 ) >> 8  ;
    b = pcol & 0xff  ;
    pcol = strip[sp].getPixelColor(i-1) ;
    rl = pcol >> 16  ;
    gl = (pcol & 0xff00 ) >> 8  ;
    bl = pcol & 0xff  ;
    pcol = strip[sp].getPixelColor(i+1) ;
    rr = pcol >> 16  ;
    gr = (pcol & 0xff00 ) >> 8  ;
    br = pcol & 0xff  ;
    // adjust the colors of second through second-to-last LEDs
//    colorExplosionColorAdjust(&colors[i].red, 9, &colors[i-1].red, &colors[i+1].red);
//    colorExplosionColorAdjust(&colors[i].green, 9, &colors[i-1].green, &colors[i+1].green);
//    colorExplosionColorAdjust(&colors[i].blue, 9, &colors[i-1].blue, &colors[i+1].blue);
    colorExplosionColorAdjust(sp,&r, 9, &rl, &rr);
    colorExplosionColorAdjust(sp,&g, 9, &gl, &gr);
    colorExplosionColorAdjust(sp,&b, 9, &bl, &br);
    strip[sp].setPixelColor(i,r,g,b);
    strip[sp].setPixelColor(i+1,rr,gr,br);
    strip[sp].setPixelColor(i-1,rl,gl,bl);
  }
  
    pcol = strip[sp].getPixelColor(strip[sp].numPixels()-1) ;
    r = pcol >> 16  ;
    g = (pcol & 0xff00 ) >> 8  ;
    b = pcol & 0xff  ;
    pcol = strip[sp].getPixelColor(strip[sp].numPixels()-2) ;
    rl = pcol >> 16  ;
    gl = (pcol & 0xff00 ) >> 8  ;
    bl = pcol & 0xff  ;
  // adjust the colors of the last LED
//  colorExplosionColorAdjust(&colors[NUM_PIXELS-1].red, 9, &colors[NUM_PIXELS-2].red, (unsigned char*)0);
//  colorExplosionColorAdjust(&colors[NUM_PIXELS-1].green, 9, &colors[NUM_PIXELS-2].green, (unsigned char*)0);
//  colorExplosionColorAdjust(&colors[NUM_PIXELS-1].blue, 9, &colors[NUM_PIXELS-2].blue, (unsigned char*)0);
  colorExplosionColorAdjust(sp,&r, 9, &rl, (unsigned char*)0);
  colorExplosionColorAdjust(sp,&g, 9, &gl, (unsigned char*)0);
  colorExplosionColorAdjust(sp,&b, 9, &bl, (unsigned char*)0);
  strip[sp].setPixelColor(strip[sp].numPixels()-1,r,g,b);
  strip[sp].setPixelColor(strip[sp].numPixels()-2,rl,gl,bl);

  if (!noNewBursts)
  {
    // if we are generating new bursts, randomly pick one new LED
    // to light up
    for (int i = 0; i < 1; i++)
    {
      int j = random(strip[sp].numPixels());  // randomly pick an LED
      pcol = strip[sp].getPixelColor(j) ;
      r = pcol >> 16  ;
      g = (pcol & 0xff00 ) >> 8  ;
      b = pcol & 0xff  ;

      switch(random(7))  // randomly pick a color
      {
        // 2/7 chance we will spawn a red burst here (if LED has no red component)
        case 0:
        case 1:
          if (r == 0)
          {
            r = 1;
          }
          break;
        
        // 2/7 chance we will spawn a green burst here (if LED has no green component)
        case 2:
        case 3:
          if (g == 0)
          {
            g = 1;
          }
          break;

        // 2/7 chance we will spawn a white burst here (if LED is all off)
        case 4:
        case 5:
          if ((r == 0) && (g == 0) && (b == 0))
          {
            r = g = b = 1 ;
          }
          break;
        
        // 1/7 chance we will spawn a blue burst here (if LED has no blue component)
        case 6:
          if (b == 0)
          {
            b = 1;
          }
          break;
          
        default:
          break;
      }
      strip[sp].setPixelColor(j,r,g,b);
    }
  }
}


// ***** PATTERN Gradient *****
// This function creates a scrolling color gradient that smoothly
// transforms from red to white to green back to white back to red.
// This pattern is overlaid with waves of brightness and dimness that
// scroll at twice the speed of the color gradient.
void gradient(int sp)
{
  unsigned int j = 0;
  unsigned char r, g, b ;
  uint32_t pcol ;
    
  // populate colors array with full-brightness gradient colors
  // (since the array indices are a function of loopCount, the gradient
  // colors scroll over time)
  while (j < strip[sp].numPixels())
  {
    // transition from red to green over 8 LEDs
    for (int i = 0; i < 8; i++)
    {
      if (j >= strip[sp].numPixels()){ break; }
      strip[sp].setPixelColor((lp.iloopCount/2 + j + strip[sp].numPixels())%strip[sp].numPixels(),160 - 20*i, 20*i, (160 - 20*i)*20*i/160  );
//      colors[(loopCount/2 + j + NUM_PIXELS)%NUM_PIXELS] = (rgb_color){160 - 20*i, 20*i, (160 - 20*i)*20*i/160};
      j++;
    }
    // transition from green to red over 8 LEDs
    for (int i = 0; i < 8; i++)
    {
      if (j >= strip[sp].numPixels()){ break; }
      strip[sp].setPixelColor((lp.iloopCount/2 + j + strip[sp].numPixels())%strip[sp].numPixels(),20*i, 160 - 20*i, (160 - 20*i)*20*i/160);
//      colors[(loopCount/2 + j + NUM_PIXELS)%NUM_PIXELS] = (rgb_color){20*i, 160 - 20*i, (160 - 20*i)*20*i/160};
      j++;
    }
  }
  
  // modify the colors array to overlay the waves of dimness
  // (since the array indices are a function of loopCount, the waves
  // of dimness scroll over time)
  const unsigned char fullDarkLEDs = 10;  // number of LEDs to leave fully off
  const unsigned char fullBrightLEDs = 5;  // number of LEDs to leave fully bright
  const unsigned char cyclePeriod = 14 + fullDarkLEDs + fullBrightLEDs;
  
  // if NUM_PIXELS is not an exact multiple of our repeating pattern size,
  // it will not wrap around properly, so we pick the closest LED count
  // that is an exact multiple of the pattern period (cyclePeriod) and is not
  // smaller than the actual LED count.
  unsigned int extendedLEDCount = (((strip[sp].numPixels()-1)/cyclePeriod)+1)*cyclePeriod;

  j = 0;
  while (j < extendedLEDCount)
  {
    unsigned int idx;
    
    // progressively dim the LEDs
    for (int i = 1; i < 8; i++)
    {
      idx = (j + lp.iloopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount){ return; }
      if (idx >= strip[sp].numPixels()){ continue; }
//      strip[0].setPixelColor(idx ,  );
      pcol = strip[sp].getPixelColor(idx) ;
      r = pcol >> 16  ;
      g = (pcol & 0xff00 ) >> 8  ;
      b = pcol & 0xff  ;

//      colors[idx].red >>= i;
//      colors[idx].green >>= i;
//      colors[idx].blue >>= i;      
      r >>= i ;
      g >>= i ;
      b >>= i ;      
      strip[sp].setPixelColor(idx,r,g,b);
    }
    
    // turn off these LEDs
    for (int i = 0; i < fullDarkLEDs; i++)
    {
      idx = (j + lp.iloopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount){ return; }
      if (idx >= strip[sp].numPixels()){ continue; }
  
      strip[sp].setPixelColor(idx,0,0,0 );
//      colors[idx].red = 0;
//      colors[idx].green = 0;
//      colors[idx].blue = 0;
    }
    
    // progressively bring these LEDs back
    for (int i = 0; i < 7; i++)
    {
      idx = (j + lp.iloopCount) % extendedLEDCount;
      if (j++ >= extendedLEDCount){ return; }
      if (idx >= strip[sp].numPixels()){ continue; }
  
//      strip[0].setPixelColor(idx,0,0,0 );
      pcol = strip[sp].getPixelColor(idx) ;
      r = pcol >> 16  ;
      g = (pcol & 0xff00 ) >> 8  ;
      b = pcol & 0xff  ;
      r >>= (7 - i);
      g >>= (7 - i);
      b >>= (7 - i);      
      strip[sp].setPixelColor(idx,r,g,b);
    }
    
    // skip over these LEDs to leave them at full brightness
    j += fullBrightLEDs;
  }
}


// Helper function for adjusting the colors for the BrightTwinkle
// and ColorExplosion patterns.  Odd colors get brighter and even
// colors get dimmer.
void brightTwinkleColorAdjust(unsigned char *color)
{
  if (*color == 255){   
    *color = 254;      // if reached max brightness, set to an even value to start fade
  } else {  
    if ((*color % 2) == 1) {   // odd
    // if odd, approximately double the brightness
    // you should only use odd values that are of the form 2^n-1,
    // which then gets a new value of 2^(n+1)-1
    // using other odd values will break things
      *color = *color * 2 + 1;
    } else {                    // even
      if (*color != 0) {
        fade(color, 4);
//        *color = *color - ( *color / 2 ) ;
        if ((*color % 2 ) == 1) {
          *color = *color  - 1 ;  // if faded color is odd, subtract one to keep it even
        }
      }
    }
  }  
}

// ***** PATTERN BrightTwinkle *****
// This function creates a sparkling/twinkling effect by randomly
// picking LEDs to brighten and then fade away.  Possible colors are:
//   white, red, green, blue, yellow, cyan, and magenta
// numColors is the number of colors to generate, and minColor
// indicates the starting point (white is 0, red is 1, ..., and
// magenta is 6), so colors generated are all of those from minColor
// to minColor+numColors-1.  For example, calling brightTwinkle(2, 2, 0)
// will produce green and blue twinkles only.
// When true, the noNewBursts argument changes prevents the generation
// of new twinkles; this can be used for a fade-out effect.
// This function uses a very similar algorithm to the ColorExplosion
// pattern.  The main difference is that the random twinkling LEDs of
// this BrightTwinkle pattern do not propagate to neighboring LEDs.
void brightTwinkle(int sp , unsigned char minColor, unsigned char numColors, unsigned char noNewBursts)
{
  // Note: the colors themselves are used to encode additional state
  // information.  If the color is one less than a power of two
  // (but not 255), the color will get approximately twice as bright.
  // If the color is even, it will fade.  The sequence goes as follows:
  // * Randomly pick an LED.
  // * Set the color(s) you want to flash to 1.
  // * It will automatically grow through 3, 7, 15, 31, 63, 127, 255.
  // * When it reaches 255, it gets set to 254, which starts the fade
  //   (the fade process always keeps the color even).
  int i ;
  unsigned char r, g, b ;
  uint32_t pcol ;
    
  for ( i = 0; i < strip[sp].numPixels(); i++) {
    pcol = strip[sp].getPixelColor(i) ;
    r = ( pcol >> 16 ) & 0xff  ;
    g = (( pcol & 0xff00 ) >> 8 )  ;
    b = ( pcol & 0xff )  ;
    brightTwinkleColorAdjust(&r);
    brightTwinkleColorAdjust(&g);
    brightTwinkleColorAdjust(&b);
    strip[sp].setPixelColor(i,r,g,b);
  }
  
  if (!noNewBursts){
    // if we are generating new twinkles, randomly pick four new LEDs
    // to light up
    for (int i = 0; i < 4; i++){
      int j = random(strip[sp].numPixels());
      pcol = strip[sp].getPixelColor(j) ;
      r = ( pcol >> 16 ) & 0xff ;
      g = (pcol & 0xff00 ) >> 8  ;
      b = pcol & 0xff  ;
//      if (colors[j].red == 0 && colors[j].green == 0 && colors[j].blue == 0)      
      if (r == 0 && g == 0 && b == 0){
        // if the LED we picked is not already lit, pick a random
        // color for it and seed it so that it will start getting
        // brighter in that color
        switch (random(numColors) + minColor){
          case 0:
            strip[sp].setPixelColor(j,1,1,1);
//            colors[j] = (rgb_color){1, 1, 1};  // white
            break;
          case 1:
            strip[sp].setPixelColor(j,1,0,0);
//            colors[j] = (rgb_color){1, 0, 0};  // red
            break;
          case 2:
            strip[sp].setPixelColor(j,0,1,0);
//            colors[j] = (rgb_color){0, 1, 0};  // green
            break;
          case 3:
            strip[sp].setPixelColor(j,0,0,1);
//            colors[j] = (rgb_color){0, 0, 1};  // blue
            break;
          case 4:
            strip[sp].setPixelColor(j,1,1,0);
//            colors[j] = (rgb_color){1, 1, 0};  // yellow
            break;
          case 5:
            strip[sp].setPixelColor(j,0,1,1);
//            colors[j] = (rgb_color){0, 1, 1};  // cyan
            break;
          case 6:
            strip[sp].setPixelColor(j,1,0,1);
//            colors[j] = (rgb_color){1, 0, 1};  // magenta
            break;
          default:
            strip[sp].setPixelColor(j,1,1,1);
//            colors[j] = (rgb_color){1, 1, 1};  // white
        }
      }
    }
  }
}


// ***** PATTERN Collision *****
// This function spawns streams of color from each end of the strip
// that collide, at which point the entire strip flashes bright white
// briefly and then fades.  Unlike the other patterns, this function
// maintains a lot of complicated state data and tells the main loop
// when it is done by returning 1 (a return value of 0 means it is
// still in progress).
unsigned char collision(int sp)
{
  const unsigned char maxBrightness = 180;  // max brightness for the colors
  const unsigned char numCollisions = 5;  // # of collisions before pattern ends
  static unsigned char state = 0;  // pattern state
  static unsigned int count = 0;  // counter used by pattern
  unsigned char r, g, b ;
  uint32_t pcol ;
  
  if (lp.iloopCount == 0)
  {
    state = 0;
  }
  
  if (state % 3 == 0)
  {
    // initialization state
    switch (state/3)
    {
      case 0:  // first collision: red streams
        strip[sp].setPixelColor(0,maxBrightness,0,0 );
//        colors[0] = (rgb_color){maxBrightness, 0, 0};
        break;
      case 1:  // second collision: green streams
        strip[sp].setPixelColor(0,0,maxBrightness,0 );
//        colors[0] = (rgb_color){0, maxBrightness, 0};
        break;
      case 2:  // third collision: blue streams
        strip[sp].setPixelColor(0,0,0,maxBrightness );
//        colors[0] = (rgb_color){0, 0, maxBrightness};
        break;
      case 3:  // fourth collision: warm white streams
        strip[sp].setPixelColor(0,maxBrightness, maxBrightness*4/5, maxBrightness>>3 );
//        colors[0] = (rgb_color){maxBrightness, maxBrightness*4/5, maxBrightness>>3};
        break;
      default:  // fifth collision and beyond: random-color streams
        strip[sp].setPixelColor(0,random(maxBrightness), random(maxBrightness), random(maxBrightness));
//        colors[0] = (rgb_color){random(maxBrightness), random(maxBrightness), random(maxBrightness)};
    }
    
    // stream is led by two full-white LEDs
    strip[sp].setPixelColor(1,255, 255, 255 );
    strip[sp].setPixelColor(2,255, 255, 255 );
//    colors[1] = colors[2] = (rgb_color){255, 255, 255};
    // make other side of the strip a mirror image of this side

    strip[sp].setPixelColor(strip[sp].numPixels() - 1,strip[sp].getPixelColor(0));
    strip[sp].setPixelColor(strip[sp].numPixels() - 2,strip[sp].getPixelColor(1));
    strip[sp].setPixelColor(strip[sp].numPixels() - 3,strip[sp].getPixelColor(2));
    
//    colors[NUM_PIXELS - 1] = colors[0];
//    colors[NUM_PIXELS - 2] = colors[1];
//    colors[NUM_PIXELS - 3] = colors[2];
    
    state++;  // advance to next state
    count = 8;  // pick the first value of count that results in a startIdx of 1 (see below)
    return 0;
  }
  
  if (state % 3 == 1)
  {
    // stream-generation state; streams accelerate towards each other
    unsigned int startIdx = count*(count + 1) >> 6;
    unsigned int stopIdx = startIdx + (count >> 5);
    count++;
    if (startIdx < (strip[sp].numPixels() + 1)/2)
    {
      // if streams have not crossed the half-way point, keep them growing
      for (int i = 0; i < startIdx-1; i++)
      {
        // start fading previously generated parts of the stream
//        fade(&colors[i].red, 5);
//        fade(&colors[i].green, 5);
//        fade(&colors[i].blue, 5);
//        fade(&colors[strip[sp].numPixels() - i - 1].red, 5);
//        fade(&colors[strip[sp].numPixels() - i - 1].green, 5);
//        fade(&colors[strip[sp].numPixels() - i - 1].blue, 5);

        pcol = strip[sp].getPixelColor(i) ;
        r = pcol >> 16  ;
        g = (pcol & 0xff00 ) >> 8  ;
        b = pcol & 0xff  ;
        fade(&r, 5);
        fade(&g, 5);
        fade(&b, 5);
        strip[sp].setPixelColor(i,r,g,b);
        pcol = strip[sp].getPixelColor(strip[sp].numPixels() - i - 1);
        r = pcol >> 16  ;
        g = (pcol & 0xff00 ) >> 8  ;
        b = pcol & 0xff  ;
        fade(&r, 5);
        fade(&g, 5);
        fade(&b, 5);
        strip[sp].setPixelColor(strip[sp].numPixels() - i - 1,r,g,b);
      }
      for (int i = startIdx; i <= stopIdx; i++)
      {
        // generate new parts of the stream
        if (i >= (strip[sp].numPixels() + 1) / 2)
        {
          // anything past the halfway point is white
//          colors[i] = (rgb_color){255, 255, 255};
          strip[sp].setPixelColor(i,255, 255, 255);
        }
        else
        {
//          colors[i] = colors[i-1];
          strip[sp].setPixelColor(i, strip[sp].getPixelColor(i-1) ) ;
        }
        // make other side of the strip a mirror image of this side
//        colors[NUM_PIXELS - i - 1] = colors[i];
        strip[sp].setPixelColor(strip[sp].numPixels() - i - 1, strip[sp].getPixelColor(i) ) ;
      }
      // stream is led by two full-white LEDs
//      colors[stopIdx + 1] = colors[stopIdx + 2] = (rgb_color){255, 255, 255};
      strip[sp].setPixelColor(stopIdx + 1,255, 255, 255);
      strip[sp].setPixelColor(stopIdx + 2,255, 255, 255);
      // make other side of the strip a mirror image of this side
//      colors[NUM_PIXELS - stopIdx - 2] = colors[stopIdx + 1];
//      colors[NUM_PIXELS - stopIdx - 3] = colors[stopIdx + 2];
      
      strip[sp].setPixelColor(strip[sp].numPixels() - stopIdx - 2,strip[sp].getPixelColor(stopIdx + 1));
      strip[sp].setPixelColor(strip[sp].numPixels() - stopIdx - 3,strip[sp].getPixelColor(stopIdx + 2));
    }
    else
    {
      // streams have crossed the half-way point of the strip;
      // flash the entire strip full-brightness white (ignores maxBrightness limits)
      for (int i = 0; i < strip[sp].numPixels() ; i++)
      {
//        colors[i] = (rgb_color){255, 255, 255};
        strip[sp].setPixelColor(i,255, 255, 255);
      }
      state++;  // advance to next state
    }
    return 0;
  }
  
  if (state % 3 == 2)
  {
    // fade state
    pcol = strip[0].getPixelColor(0) ;
    r = pcol >> 16  ;
    g = (pcol & 0xff00 ) >> 8  ;
    b = pcol & 0xff  ;
    if (r == 0 && g == 0 && b == 0)
    {
      // if first LED is fully off, advance to next state
      state++;
      
      // after numCollisions collisions, this pattern is done
      return state == 3*numCollisions;
    }
    
    // fade the LEDs at different rates based on the state
    for (int i = 0; i < strip[sp].numPixels() ; i++)
    {
      pcol = strip[sp].getPixelColor(i) ;
      r = pcol >> 16  ;
      g = (pcol & 0xff00 ) >> 8  ;
      b = pcol & 0xff  ;
      switch (state/3)
      {
        case 0:  // fade through green
//          fade(&colors[i].red, 3);
//          fade(&colors[i].green, 4);
//          fade(&colors[i].blue, 2);
          fade(&r, 3);
          fade(&g, 4);
          fade(&b, 2);
          break;
        case 1:  // fade through red
//          fade(&colors[i].red, 4);
//          fade(&colors[i].green, 3);
//          fade(&colors[i].blue, 2);
          fade(&r, 4);
          fade(&g, 3);
          fade(&b, 2);
          break;
        case 2:  // fade through yellow
//          fade(&colors[i].red, 4);
//          fade(&colors[i].green, 4);
//          fade(&colors[i].blue, 3);
          fade(&r, 4);
          fade(&g, 4);
          fade(&b, 3);
          break;
        case 3:  // fade through blue
//          fade(&colors[i].red, 3);
//          fade(&colors[i].green, 2);
//          fade(&colors[i].blue, 4);
          fade(&r, 3);
          fade(&g, 2);
          fade(&b, 4);
          break;
        default:  // stay white through entire fade
//          fade(&colors[i].red, 4);
//          fade(&colors[i].green, 4);
//          fade(&colors[i].blue, 4);
          fade(&r, 4);
          fade(&g, 4);
          fade(&b, 4);
      }
      strip[sp].setPixelColor(i,r,g,b);
    }
  }
  
  return 0;
}


