void LoadParamsFromEEPROM(bool bLoad){
long lTmp ;  
int i ;
int j ;
int bofs ,ofs ;
int eeAddress ;
  if ( bLoad ) {
    eeAddress = 0 ;
    EEPROM.get(eeAddress,ghks);
    eeAddress += sizeof(ghks) ;
    EEPROM.get(eeAddress,lp) ; 
    eeAddress += sizeof(lp) ;
//    Serial.println("Final Load EEPROM adress " +String(eeAddress));   
    
  }else{
    eeAddress = 0 ;
    EEPROM.put(eeAddress,ghks);
    eeAddress += sizeof(ghks) ;
    EEPROM.put(eeAddress,lp);
    eeAddress += sizeof(lp) ;
//    Serial.println("Final Save EEPROM adress " +String(eeAddress));   

    EEPROM.commit();                                                       // save changes in one go ???
  }
}


