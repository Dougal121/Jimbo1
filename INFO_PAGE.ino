void handleInfo(){
  int i , ii , iTmp , iX ;
  uint8_t j , k , kk ;
  String message ;  
  String MyNum ;  
  String MyColor ;
  String MyColor2 ;
  byte mac[6];

//  SerialOutParams();
  
  for (uint8_t j=0; j<server.args(); j++){
  }
  
  SendHTTPHeader();

  server.sendContent(F("<br><center><b>Node Info</b><br>"));
  server.sendContent(F("<table border=1 title='Device Info'>"));
  server.sendContent("<tr><td>ESP ID</td><td align=center>0x" + String(ESP.getChipId(), HEX) + "</td><td align=center>"+String(ESP.getChipId())+"</td></tr>" ) ; 
  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIP[0],MyIP[1],MyIP[2],MyIP[3]);
  server.sendContent("<tr><td>Network Node IP Address</td><td align=center>" + String(buff) + "</td><td>.</td></tr>" ) ; 
  snprintf(buff, BUFF_MAX, "%03u.%03u.%03u.%03u", MyIPC[0],MyIPC[1],MyIPC[2],MyIPC[3]);
  server.sendContent("<tr><td>Config Node IP Address</td><td align=center>" + String(buff) + "</td><td>.</td></tr>" ) ; 
  
  server.sendContent("<tr><td>WiFi RSSI</td><td align=center>" + String(WiFi.RSSI()) + "</td><td>(dBm)</td></tr>" ) ; 
  WiFi.macAddress(mac);      
  snprintf(buff, BUFF_MAX, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  server.sendContent("<tr><td>MAC Address</td><td align=center>" + String(buff) + "</td><td align=center>.</td></tr>" ) ; 
 
  server.sendContent("<tr><td>Last Scan Speed</td><td align=center>" + String(lScanLast) + "</td><td>(per second)</td></tr>" ) ;    

  server.sendContent("<tr><td>ESP Core Version</td><td align=center>" + String(ESP.getCoreVersion()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>ESP Full Version</td><td align=center>" + String(ESP.getFullVersion()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>SDK Version</td><td align=center>" + String(ESP.getSdkVersion()) + "</td><td>.</td></tr>" ) ;    
  server.sendContent("<tr><td>CPU Volts</td><td align=center>" + String(ESP.getVcc()) + "</td><td>(V)</td></tr>" ) ;    
  server.sendContent("<tr><td>CPU Frequecy</td><td align=center>" + String(ESP.getCpuFreqMHz()) + "</td><td>(MHz)</td></tr>" ) ;    
  server.sendContent("<tr><td>Get Rest Reason</td><td align=center>" + String(ESP.getResetReason()) + "</td><td></td></tr>" ) ;    
  server.sendContent("<tr><td>Get Reset Into</td><td align=center>" + String(ESP.getResetInfo()) + "</td><td></td></tr>" ) ;    
  server.sendContent("<tr><td>Get Sketch Size</td><td align=center>" + String(ESP.getSketchSize()) + "</td><td>(Bytes)</td></tr>" ) ;    
  server.sendContent("<tr><td>Free Sketch Space</td><td align=center>" + String(ESP.getFreeSketchSpace()) + "</td><td>(Bytes)</td></tr>" ) ;    

  server.sendContent(F("</table><br>"));    
  SendHTTPPageFooter();
}

