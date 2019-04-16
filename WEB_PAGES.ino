
void handleRoot() {
  boolean currentLineIsBlank = true;
  tmElements_t tm;
  long  i = 0 ;
  int ii  ;
  int iProgNum = 0;
  int j ;
  int k , kk , iTmp ;
  boolean bExtraValve = false ;
  uint8_t iPage = 1 ;
  boolean bDefault = true ;
//  int td[6];
  long lTmp ; 
  String MyCheck , MyColor , MyNum  ;
  byte mac[6];
  String message ;
  uint32_t pcol ;
  uint32_t xcol ;
  uint8_t r, g, b ;
  
//  SerialOutParams();
  
  for (uint8_t j=0; j<server.args(); j++){
    i = String(server.argName(j)).indexOf("command");
    if (i != -1){  // 
      switch (String(server.arg(j)).toInt()){
        case 1:  // load values
//          LoadParamsFromEEPROM(true);
          iDoSave = 3 ;
        break;
        case 2: // Save values
//          LoadParamsFromEEPROM(false);
          iDoSave = 2 ;
        break;
        case 3: // 
        break;
        case 4: // 
        break;
        case 5: // 
        break;
        case 8: //  Cold Reboot
          ESP.reset() ;
        break;
        case 9: //  Warm Reboot
          ESP.restart() ;
        break;
        case 667: // wipe the memory to factory default
//          BackIntheBoxMemory();
          iDoSave = 4 ;
        break;
        case 665:
        break;
        case 668:
        break;
      }  
    }
    
    i = String(server.argName(j)).indexOf("nssid");    // ssid setup
    if (i != -1){  
      String(server.arg(j)).toCharArray(ghks.nssid,sizeof(ghks.nssid));
    }        
    i = String(server.argName(j)).indexOf("cssid");    // ssid setup
    if (i != -1){  
      String(server.arg(j)).toCharArray(ghks.cssid,sizeof(ghks.cssid));
    }        
    i = String(server.argName(j)).indexOf("npass");   // password setup
    if (i != -1){  
      String(server.arg(j)).toCharArray(ghks.npassword,sizeof(ghks.npassword));
    }        
    i = String(server.argName(j)).indexOf("cpass");   // password setup
    if (i != -1){  
      String(server.arg(j)).toCharArray(ghks.cpassword,sizeof(ghks.cpassword));
    }        

    i = String(server.argName(j)).indexOf("naddr");   // ip address setup
    if (i != -1){  // have a request to set the time zone
      MyIP[0] = String(server.arg(j)).substring(0,3).toInt() ;
      MyIP[1] =String(server.arg(j)).substring(4,7).toInt() ;
      MyIP[2] = String(server.arg(j)).substring(8,11).toInt() ;
      MyIP[3] =String(server.arg(j)).substring(12,15).toInt() ;
    }        

    i = String(server.argName(j)).indexOf("stime");
    if (i != -1){  // 
      tm.Year = (String(server.arg(j)).substring(0,4).toInt()-1970) ;
      tm.Month =(String(server.arg(j)).substring(5,7).toInt()) ;
      tm.Day = (String(server.arg(j)).substring(8,10).toInt()) ;
      tm.Hour =(String(server.arg(j)).substring(11,13).toInt()) ;
      tm.Minute = (String(server.arg(j)).substring(14,16).toInt()) ;
      tm.Second = 0 ;
      setTime(makeTime(tm));    
    }        
     
    i = String(server.argName(j)).indexOf("lpqno");
    if (i != -1){  // 
      iProgNum = String(server.arg(j)).toInt() ;
      constrain(iProgNum,0,MAX_MEM_SEQ);
    }
    for ( ii = 0 ; ii < MAX_STRIPS ; ii++ ){
      i = String(server.argName(j)).indexOf("lpqsp"+String(ii));
      if (i != -1){  // 
        lp.sequence[iProgNum].sp[ii] = String(server.arg(j)).toInt() ;
        constrain(lp.sequence[iProgNum].sp[ii],0,MAX_MEM_MESSAGE);
      }
    }
    i = String(server.argName(j)).indexOf("lpqpt");
    if (i != -1){  // 
      lp.sequence[iProgNum].ptxt = String(server.arg(j)).toInt() ;
      constrain(lp.sequence[iProgNum].ptxt,0,MAX_MEM_MESSAGE);
    }
    i = String(server.argName(j)).indexOf("lpqsn");
    if (i != -1){  // 
      lp.sequence[iProgNum].sndno = String(server.arg(j)).toInt() ;
      constrain(lp.sequence[iProgNum].sndno,0,MAX_MEM_MESSAGE);
    }
    i = String(server.argName(j)).indexOf("lpqsv");
    if (i != -1){  // 
      lp.sequence[iProgNum].sndvol = String(server.arg(j)).toInt() ;
      constrain(lp.sequence[iProgNum].sndvol,0,30);
    }
    i = String(server.argName(j)).indexOf("lpqtm");
    if (i != -1){  // 
      lp.sequence[iProgNum].iTime = String(server.arg(j)).toInt() ;
      constrain(lp.sequence[iProgNum].iTime,0,600000);
    }
    
    i = String(server.argName(j)).indexOf("max");
    if (i != -1){  // 
      lp.iMax = String(server.arg(j)).toInt() ;
      constrain(lp.iMax,0,MAX_MEM_SEQ);
    }
    i = String(server.argName(j)).indexOf("min");
    if (i != -1){  // 
      lp.iMin = String(server.arg(j)).toInt() ;
      constrain(lp.iMin,0,MAX_MEM_SEQ);
    }
    i = String(server.argName(j)).indexOf("bri");
    if (i != -1){  // 
      lp.iBright = String(server.arg(j)).toInt() ;
      constrain(lp.iBright,0,255);
    }
    i = String(server.argName(j)).indexOf("sam");
    if (i != -1){  // 
      iSamplePixel = String(server.arg(j)).toInt() ;
      constrain(iSamplePixel,0,strip[0].numPixels());
    }
    
    i = String(server.argName(j)).indexOf("cur");
    if (i != -1){  // 
      lp.iSeq = String(server.arg(j)).toInt() ;
      constrain(lp.iSeq,0,MAX_MEM_SEQ);
    }

    i = String(server.argName(k)).indexOf("run");
    if (i != -1){  // 
      lp.bRun = String(server.arg(j)).toInt() ;           
    }

    i = String(server.argName(k)).indexOf("onf");
    if (i != -1){  // 
      lp.bOnOff = String(server.arg(j)).toInt() ;           
    }

    for ( ii = 0 ; ii < MAX_MEM_PATTERN ; ii++){                 // 
      MyNum = String(ii) ;
      if ( MyNum.length() == 1 ) {
        MyNum = "0" + MyNum ;
      }      
      i = String(server.argName(j)).indexOf("ds" + MyNum);  // description
      if (i != -1){  // 
        String(server.arg(j)).toCharArray( lp.pattern[ii].Description , sizeof(lp.pattern[ii].Description)) ;        
      }        
      i = String(server.argName(j)).indexOf("dl" + MyNum);  // delay  ms
      if (i != -1){  // 
        lp.pattern[ii].lDelay = String(server.arg(j)).toInt() ; 
      }        
      i = String(server.argName(j)).indexOf("pa" + MyNum);  // parameter a
      if (i != -1){  // 
        lp.pattern[ii].paramA = String(server.arg(j)).toInt() ; 
      }        
      i = String(server.argName(j)).indexOf("pb" + MyNum);  // parameter b
      if (i != -1){  // 
        lp.pattern[ii].paramB = String(server.arg(j)).toInt() ; 
      }        
      i = String(server.argName(j)).indexOf("pc" + MyNum);  // parameter c
      if (i != -1){  // 
        lp.pattern[ii].paramC = String(server.arg(j)).toInt() ; 
      }        
      i = String(server.argName(j)).indexOf("pd" + MyNum);  // parameter d
      if (i != -1){  // 
        lp.pattern[ii].paramD = String(server.arg(j)).toInt() ; 
      }        
      i = String(server.argName(j)).indexOf("pe" + MyNum);  // parameter e
      if (i != -1){  // 
        lp.pattern[ii].paramE = String(server.arg(j)).toInt() ; 
      }        
    } // botom of the for next loop for patterns

/*    for ( ii = 0 ; ii < MAX_MEM_SEQ ; ii++){                 // 
      MyNum = String(ii) ;
      if ( MyNum.length() == 1 ) {
        MyNum = "0" + MyNum ;
      }  
    }
*/

    i = String(server.argName(j)).indexOf("msgno");  // message number - comes in first
    if (i != -1){  // 
      kk = String(server.arg(j)).toInt() ; 
//      Serial.println(kk);
    }        
    i = String(server.argName(j)).indexOf("msgtxt");  // message number - comes in first
    if (i != -1){  // text
      String(server.arg(j)).toCharArray( lp.message[kk].msg , sizeof(lp.message[kk].msg) ) ;        
//      Serial.println(lp.message[kk].msg);
    }        
    i = String(server.argName(j)).indexOf("msgcol");  // message number - comes in first
    if (i != -1){  // color
      lp.message[kk].msgcol =  String(server.arg(j)).toInt() ; 
    }        
    i = String(server.argName(j)).indexOf("msgbgc");  // message number - comes in first
    if (i != -1){  // color
      lp.message[kk].msgbgcol =  String(server.arg(j)).toInt() ; 
    }        
    i = String(server.argName(j)).indexOf("msgeff");  // message number - comes in first
    if (i != -1){  // effects
      lp.message[kk].msgeffects = String(server.arg(j)).toInt() ; 
    }        

                  
  }          

  SendHTTPHeader();
  
  server.sendContent(F("<a href='/?command=2'>Save Parameters to EEPROM</a><br>")) ;         

  snprintf(buff, BUFF_MAX, "%d/%02d/%02d %02d:%02d:%02d", year(), month(), day() , hour(), minute(), second());
  server.sendContent("<b>"+ String(buff)) ; 
  if ( year() < 2000 ) {
    server.sendContent(F("--- CLOCK NOT SET ---")) ;
  }
  server.sendContent(F("</b><br>")) ;  

  if (String(server.uri()).indexOf("setup")>0) {  // setup of the node
    server.sendContent("<form method=get action=" + server.uri() + "><table border=1 title='WiFi Node Settings'>");
    server.sendContent(F("<tr><th>Parameter</th><th>Value</th><th><input type='submit' value='SET'></th></tr>"));

    server.sendContent(F("<tr><td>Local UDP Port NTP</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='lpntp' value='" + String(ghks.localPort) + "' size=20></td><td></td></tr>");
  
    server.sendContent(F("<tr><td>Network SSID</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='nssid' value='" + String(ghks.nssid) + "' maxlength="+String(sizeof(ghks.nssid)-1)+" size=20></td><td></td></tr>");

    server.sendContent(F("<tr><td>Network Password</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='npass' value='" + String(ghks.npassword) + "' maxlength="+String(sizeof(ghks.npassword)-1)+" size=20></td><td></td></tr>");

    server.sendContent(F("<tr><td>Config SSID</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='cssid' value='" + String(ghks.cssid) + "' maxlength="+String(sizeof(ghks.cssid)-1)+" size=20></td><td></td></tr>");

    server.sendContent(F("<tr><td>Config Password</td><td align=center>")) ; 
    server.sendContent("<input type='text' name='cpass' value='" + String(ghks.cpassword) + "' maxlength="+String(sizeof(ghks.cpassword)-1)+" size120></td><td></td></tr>");
     
    server.sendContent(F("</form></table>"));
    bDefault = false ;
  }
  
  if (String(server.uri()).indexOf("pattern")>0) {  // setup of the patterns
    server.sendContent(F("</table><br>")) ;
    server.sendContent(F("<table title='Patterns' border=1><tr><th>No</th><th>Name</th><th>Delay</th><th>Param A<br>Red</th><th>Param B<br>Green</th><th>Param C<br>Blue</th><th>Param D</th><th>Param E</th><th>.</th></tr>")) ;
    for ( i = 0 ; i < MAX_MEM_PATTERN ; i++ ) {
      MyNum = String(i) ;
      if ( MyNum.length() == 1 ) {
        MyNum = "0" + MyNum ;
      }
//      if ( lp.sequence[lp.iSeq] == i ) {
//        MyColor = F("bgcolor='yellow'") ;
//      }else{
        MyColor = F("") ;
//      }
      server.sendContent("<tr><form method=get action=" + server.uri() + "><input type='hidden' name='num" + MyNum + "' value='1'><td "+String(MyColor)+ " align=center><a href='/?cur="+String(i)+"'>"+String(i)+"</a></td><td>") ;
      server.sendContent("<input type='text' name='ds" + MyNum + "' maxlength=20 size=20 value='" + String(lp.pattern[i].Description) +"'");
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='dl" + MyNum + "' maxlength=8 size=6 value='" + String(lp.pattern[i].lDelay) +"'");
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='pa" + MyNum + "' maxlength=8 size=6 value='" + String(lp.pattern[i].paramA) +"'");
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='pb" + MyNum + "' maxlength=8 size=6 value='" + String(lp.pattern[i].paramB) +"'");
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='pc" + MyNum + "' maxlength=8 size=6 value='" + String(lp.pattern[i].paramC) +"'");
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='pd" + MyNum + "' maxlength=8 size=6 value='" + String(lp.pattern[i].paramD) +"'");
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='pe" + MyNum + "' maxlength=8 size=6 value='" + String(lp.pattern[i].paramE) +"'");
      server.sendContent(F("</td><td><input type='submit' value='SET'></td></form></tr>")) ;
    }
    server.sendContent(F("</table>")) ;
    bDefault = false ;
  }


  if (String(server.uri()).indexOf("message")>0) {  // setup of the messages
    server.sendContent(F("<table title='Messages' border=1><tr><th>No</th><th>Message</th><th>Colour</th><th>Background<br>Colour</th><th>Effects<th>.</th></tr>")) ;
    for ( i = 0 ; i < MAX_MEM_MESSAGE ; i++ ) {
      MyNum = String(i) ;
      server.sendContent("<tr><td align=center><form method=get action=" + server.uri() + "><input type='hidden' name='msgno' value='"+MyNum+"'>"+MyNum) ;
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='msgtxt' maxlength="+String(MAX_MSG_LENGTH)+" size=20 value='" + String(lp.message[i].msg) +"'");
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='msgcol' maxlength=12 size=8 value='" + String(lp.message[i].msgcol) +"'");
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='msgbgc' maxlength=12 size=8 value='" + String(lp.message[i].msgbgcol) +"'");
      server.sendContent(F("</td><td>")) ;
      server.sendContent("<input type='text' name='msgeff' maxlength=12 size=8 value='" + String(lp.message[i].msgeffects) +"'");
      server.sendContent(F("</td><td><input type='submit' value='SET'></td></form></tr>")) ;
    }
    server.sendContent(F("</table>")) ;
    bDefault = false ;
  }

     
  if (bDefault) {     // default valve control and setup
    server.sendContent(F("<table border=1>")) ;
    server.sendContent(F("<tr><th>Parameter</th><th colspan=2>Value</th><th>.</th></tr>")) ;
    if ( lp.bRun == 0 ) {
      MyColor = F("bgcolor=red") ;
      MyCheck = F("Automation OFF")  ;    
    }else{
      MyColor = F("bgcolor=green") ;
      MyCheck = F("Automation ON")  ;    
    } 
    server.sendContent("<tr><form method=get action=" + server.uri() + "><input type='hidden' name='run' value='1'><td align=center "+String(MyColor)+">"+String(MyCheck)+"</td><td align=center><input type='submit' value='ON'></form></td><td align=center><form method=get action=" + server.uri() + "><input type='hidden' name='run' value='0'><input type='submit' value='OFF'></td></form><td></td></tr>") ;   
    if ( lp.bOnOff == 0 ) {
      MyColor = F("bgcolor=red") ;
      MyCheck = F("Light OFF")  ;    
    }else{
      MyColor = F("bgcolor=green") ;
      MyCheck = F("Light ON")  ;    
    } 
    server.sendContent("<tr><form method=get action=" + server.uri() + "><input type='hidden' name='onf' value='1'><td align=center "+String(MyColor)+">"+String(MyCheck)+"</td><td align=center><input type='submit' value='ON'></form></td><td align=center><form method=get action=" + server.uri() + "><input type='hidden' name='onf' value='0'><input type='submit' value='OFF'></td></form><td></td></tr>") ;   

    server.sendContent("<tr><form method=get action=" + server.uri() + "><td>Current Pattern</td><td colspan=2><input type='text' name='cur' maxlength=6 size=6 value='" + String(lp.iSeq) +"'>") ;
    server.sendContent(F("</td><td><input type='submit' value='SET'></td></form></tr>"));
    
    server.sendContent("<tr><form method=get action=" + server.uri() + "><td>Min Sequence</td><td colspan=2><input type='text' name='min' maxlength=6 size=6 value='" + String(lp.iMin) +"'>") ;
    server.sendContent(F("</td><td><input type='submit' value='SET'></td></form></tr>"));

    server.sendContent("<tr><form method=get action=" + server.uri() + "><td>Max Sequence</td><td colspan=2><input type='text' name='max' maxlength=6 size=6 value='" + String(lp.iMax) +"'>") ;
    server.sendContent(F("</td><td><input type='submit' value='SET'></td></form></tr>"));   
    
    server.sendContent("<tr><form method=get action=" + server.uri() + "><td>Brightness</td><td colspan=2><input type='text' name='bri'  maxlength=3 size=5  value='"+String(lp.iBright)+"'>");
    server.sendContent(F("</td><td><input type='submit' value='SET'></td></form></tr>"));   

    pcol = strip[0].getPixelColor(iSamplePixel) ;
    xcol = pcol ^ 0xffffff ;
    r = ( pcol >> 16 ) & 0xff ;
    g = ( pcol >> 8 ) & 0xff ;
    b = ( pcol  ) & 0xff ;
    snprintf(buff, BUFF_MAX, " 0x%02X%02X%02X", r, g ,b );    
    server.sendContent("<tr><form method=get action=" + server.uri() + "><td>Sample pixel</td><td colspan=2><input type='text' name='sam'  maxlength=3 size=5  value='"+String(iSamplePixel)+"'>"+String(buff)+"</td><td><input type='submit' value='SET'></td></form></tr>") ;
    server.sendContent("<tr><td>Number of pixels</td><td align=center colspan=2>"+String(strip[0].numPixels())+"</td><td>.</td></tr>") ;
    server.sendContent("<tr><td>Cycles per second</td><td align=center colspan=2>"+String(lScanLast)+"</td><td>.</td></tr>") ;
    server.sendContent("<tr><td>loop counter</td><td align=center colspan=2>"+String(lp.iloopCount)+"</td><td>.</td></tr>") ;
    server.sendContent("<tr><td>Time</td><td align=center colspan=2>"+String(now())+"</td><td>(s)</td></tr>") ;
    server.sendContent("<tr><td>Millis</td><td align=center colspan=2>"+String(millis())+"</td><td>(ms)</td></tr>") ;
    server.sendContent("<tr><td>Change at</td><td align=center colspan=2>"+String(lp.iChange)+"</td><td>(ms)</td></tr>") ;
    server.sendContent(F("</table>")) ;

    server.sendContent(F("<br><br><table border=1>")) ;
    server.sendContent(F("<tr><th>Sequence</th>"));
/*    for ( ii = 0 ; ii < MAX_STRIPS ; ii++ ){
      server.sendContent("<th>String "+String(ii)+" </th>");    
    }*/
    server.sendContent("<th>Outside</th><th>Small</th><th>Center</th>");    
    server.sendContent(F("<th>Message No</th><th>Sound No</th><th>Vol No<br>(0-30)</th><th>Time<br>(ms)</th><th>.</th></tr>")) ;
    for ( kk = 0 ; kk < MAX_MEM_SEQ ; kk++ ){
      server.sendContent("<tr><form method=get action=" + server.uri() + "><td><input type='hidden' name='lpqno' value='"+String(kk)+"'><a href='" + server.uri() + "?cur="+String(kk)+"'>"+String(kk)+"</a></td>");    
      for ( ii = 0 ; ii < MAX_STRIPS ; ii++ ){
        server.sendContent("<td><input type='text' name='lpqsp"+String(ii)+"' maxlength=2 size=2 value='"+String(lp.sequence[kk].sp[ii])+"'></td>");    
      }
      server.sendContent("<td><input type='text' name='lpqpt' maxlength=2 size=2 value='"+String(lp.sequence[kk].ptxt)+"'></td>");
      server.sendContent("<td><input type='text' name='lpqsn' maxlength=2 size=2 value='"+String(lp.sequence[kk].sndno)+"'></td>");
      server.sendContent("<td><input type='text' name='lpqsv' maxlength=2 size=2 value='"+String(lp.sequence[kk].sndvol)+"'></td>");
      server.sendContent("<td><input type='text' name='lpqtm' maxlength=12 size=4 value='"+String(lp.sequence[kk].iTime)+"'></td>");
      server.sendContent("<td><input type='submit' value='SET'></td></form></tr>") ;
    }
    server.sendContent(F("</table>")) ;
    
  }
  SendHTTPPageFooter(); 
}


void handleNotFound(){
  String message = F("Seriously - No way DUDE\n\n");
  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, F("text/plain"), message);
//  Serial.print(message);
}

void SendHTTPHeader(){
  server.sendHeader(F("Server"),F("ESP8266-on-ice"),false);
  server.sendHeader(F("X-Powered-by"),F("Dougal-1.0"),false);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(F("<!DOCTYPE HTML>"));
  server.sendContent(F("<link rel='icon' href='data:,'></head><body><html><center><h2>WS2812 LED Lights</h2>"));
  server.sendContent(F("<a href='/'>Refresh</a><br><br>")) ;    
  server.sendContent(F("<meta name=viewport content='width=320, auto inital-scale=1'>"));
}

void SendHTTPPageFooter(){
  server.sendContent(F("<br><br><a href='/?command=1'>Load Parameters from EEPROM</a><br><br><a href='/?command=667'>Reset Memory to Factory Default</a><br><a href='/?command=665'>Sync UTP Time</a><br><a href='/stime'>Manual Time Set</a><br>")) ;         
  server.sendContent(F("<a href='/setup'>WiFi Node Setup</a><br><a href='/pattern'>Pattern Setup</a><br><br><a href='/message'>Message Setup</a><br><a href='/scope'>Pixel Scope</a><br>"));
  server.sendContent("<a href='/?reboot=" + String(lRebootCode) + "'>Reboot</a><br>");    
  server.sendContent(F("<a href='/info'>Node Infomation</a><br>"));
  server.sendContent(F("<a href='/eeprom'>Display EEPROM data</a><br>"));
  snprintf(buff, BUFF_MAX, "%u.%u.%u.%u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
  server.sendContent("<a href='http://" + String(buff) + ":81/update'>OTA Firmware Update</a><br>");  
  server.sendContent(F("</body></html>\r\n"));
}


void SerialOutParams(){
String message ;
   
  message = "Web Request URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  Serial.println(message);
  message = "";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  Serial.println(message);
}


