void BackIntheBoxMemory(){
int i  ;  
//  Serial.println("Backin the box");

  String("********").toCharArray(ghks.nssid,sizeof(ghks.nssid)) ;        // your credentials go here
  String("********").toCharArray(ghks.npassword,sizeof(ghks.npassword)) ;

  String("").toCharArray(ghks.cpassword,sizeof(ghks.cpassword)) ;
  sprintf(ghks.NodeName,"Control_%08X\0",ESP.getChipId());  
  String(ghks.NodeName).toCharArray(ghks.cssid,sizeof(ghks.cssid)) ;
  
  ghks.localPortCtrl = 8088 ;
  ghks.RemotePortCtrl= 8089 ;
  
  lp.bRun = true ;
  lp.iChange = 0 ;
  lp.iMax = 20 ;
  lp.iSeq = 0  ;
  lp.iMin = 0  ;
  lp.bOnOff = true  ;  // light on off
  lp.iBright = 5 ;   // brightness


  for ( i = 0 ; i < MAX_MEM_SEQ ; i++ ) {
    lp.sequence[i].iTime = 10000 ;
    lp.sequence[i].sndvol = 20 ;
    lp.sequence[i].sndno = i+1 ;
    lp.sequence[i].ptxt = i ;
    lp.sequence[i].sp[0] = 11 ;
    lp.sequence[i].sp[1] = 12 ;
    lp.sequence[i].sp[2] = 13 ;
    switch(i){
      case 0:
        lp.sequence[i].sp[0] = 16 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 23 ;
        lp.sequence[i].iTime = 17000 ;
      break;
      case 1:
        lp.sequence[i].sp[0] = 16 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 16 ;
        lp.sequence[i].iTime = 15000 ;
      break;
      case 2:
        lp.sequence[i].sp[0] = 28;
        lp.sequence[i].sp[1] = 15 ;
        lp.sequence[i].sp[2] = 15 ;
        lp.sequence[i].iTime = 48000 ;
      break;
      case 3:
        lp.sequence[i].sp[0] = 16 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 8 ;
        lp.sequence[i].iTime = 5000 ;
      break;
      case 4:
        lp.sequence[i].sp[0] = 25 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 24 ;
        lp.sequence[i].iTime = 8000 ;
      break;
      case 5:
        lp.sequence[i].sp[0] = 8 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 8 ;
        lp.sequence[i].ptxt = 3 ;
        lp.sequence[i].iTime = 5000 ;
      break;
      case 6:
        lp.sequence[i].sp[0] = 17 ;
        lp.sequence[i].sp[1] = 17 ;
        lp.sequence[i].sp[2] = 17 ;
        lp.sequence[i].ptxt = 5 ;
        lp.sequence[i].iTime = 37000 ;
      break;
      case 7:
        lp.sequence[i].sp[0] = 16 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 8 ;
        lp.sequence[i].ptxt = 3 ;
        lp.sequence[i].iTime = 5000 ;
      break;
      case 8:
        lp.sequence[i].sp[0] = 18 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 18 ;
        lp.sequence[i].ptxt = 6 ;
        lp.sequence[i].iTime = 6000 ;
      break;
      case 9:
        lp.sequence[i].sp[0] = 16 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 8 ;
        lp.sequence[i].ptxt = 3 ;
        lp.sequence[i].iTime = 5000 ;
      break;
      case 10:
        lp.sequence[i].sp[0] = 19 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 19 ;
        lp.sequence[i].ptxt = 7 ;
        lp.sequence[i].iTime = 7000 ;
      break;
      case 11:
        lp.sequence[i].sp[0] = 16 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 8 ;
        lp.sequence[i].ptxt = 3 ;
        lp.sequence[i].iTime = 5000 ;
      break;
      case 12:
        lp.sequence[i].sp[0] = 20 ;
        lp.sequence[i].sp[1] = 20 ;
        lp.sequence[i].sp[2] = 20 ;
        lp.sequence[i].ptxt = 8 ;
        lp.sequence[i].iTime = 107000 ;
      break;
      case 13:
        lp.sequence[i].sp[0] = 16 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 21 ;
        lp.sequence[i].ptxt = 15 ;
        lp.sequence[i].iTime = 5000 ;
      break;
      case 14:
        lp.sequence[i].sp[0] = 22 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 21 ;
        lp.sequence[i].ptxt = 15 ;
        lp.sequence[i].iTime = 7000 ;
      break;
      case 15:
        lp.sequence[i].sp[0] = 16 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 25 ;
        lp.sequence[i].ptxt = 15 ;
        lp.sequence[i].iTime = 5000 ;
      break;
      case 16:
        lp.sequence[i].sp[0] = 28 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 16 ;
        lp.sequence[i].ptxt = 15 ;
        lp.sequence[i].iTime = 5000 ;
      break;
      case 17:
        lp.sequence[i].sp[0] = 16 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 27 ;
        lp.sequence[i].ptxt = 15 ;
        lp.sequence[i].iTime = 2500 ;
      break;
      case 18:
        lp.sequence[i].sp[0] = 10 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 10 ;
        lp.sequence[i].ptxt = 9 ;
        lp.sequence[i].iTime = 10000 ;
      break;
      case 19:
        lp.sequence[i].sp[0] = 17 ;
        lp.sequence[i].sp[1] = 16 ;
        lp.sequence[i].sp[2] = 16 ;
        lp.sequence[i].ptxt = 10 ;
        lp.sequence[i].iTime = 17000 ;
      break;
      default:
      break;
    }  
  }
  
  for ( i = 0 ; i < MAX_MEM_MESSAGE ; i++ ) {
    lp.message[i].msgbgcol = 0 ;
    switch(i){
      case 0:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("Caution - May contain EXPLOSIVES\0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 1:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("5  4  3  2  1  Go!!! ... \0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 2:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("Happy 60th Birthday Mike ... AKA Mr Grumpy :(  :(\0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 3:
         lp.message[i].msgcol = matrix.Color(255, 255, 255) ;
         String("*$#?!...WTF \0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 4:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("He's NOT messiah!! \0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 5:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("Oh when the SAINTS GO Marching in ...Go Dragons...\0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 6:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("...Go Bombers >>>> \0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 7:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("Bugger.... \0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 8:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("...Go The Mighty CROWS ....\0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 9:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("Beauty Mate !!! \0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 10:
         lp.message[i].msgcol = matrix.Color(255, 0, 0) ;
         String("That's All Folks....\0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 11:
         lp.message[i].msgcol = matrix.Color(255, 255, 255) ;
         String("Don't worry be HAPPY....\0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      case 12:
         lp.message[i].msgcol = matrix.Color(255, 255, 255) ;
         String("Don't worry be HAPPY....\0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
      default:
         lp.message[i].msgcol = matrix.Color(0, 0, 0) ;
         String("\0").toCharArray(lp.message[i].msg,sizeof(lp.message[i].msg));
      break;
    }  
  }
    
  for ( i = 0 ; i < MAX_MEM_PATTERN ; i++ ) {
    lp.pattern[i].paramA = 0 ;
    lp.pattern[i].paramB = 0 ;
    lp.pattern[i].paramC = 0 ;
    lp.pattern[i].paramD = 0 ;
    lp.pattern[i].paramE = 0 ;
    lp.pattern[i].paramF = 0 ;
    lp.pattern[i].lDelay = 15 ;        // delay in ms if used  
    switch(i){
      case 0:
        String("Warm White Shimmer\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].lDelay = 5 ; 
      break;
      case 1:
        String("Bright Twinkle Gr\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 2 ;
        lp.pattern[i].paramB = 0 ;
        lp.pattern[i].paramC = 30 ;
      break;
      case 2:
        String("Bright Twinkle Rd\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 1 ;
        lp.pattern[i].paramB = 0 ;
        lp.pattern[i].paramC = 30 ;
      break;
      case 3:
        String("Bright Twinkle Wh\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 0 ;
        lp.pattern[i].paramB = 0 ;
        lp.pattern[i].paramC = 30 ;
      break;
      case 4:
        String("Bright Twinkle GRW\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 0 ;
        lp.pattern[i].paramB = 3 ;
        lp.pattern[i].paramC = 30 ;
      break;
      case 5:
        String("Collision\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].lDelay = 5 ; 
       break;
      case 6:
        String("Traditionaln Colors\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].lDelay = 1 ; 
      break;
      case 7:
        String("RGW Gradient\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
      case 8:
        String("Color Explosion\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
      case 9:
        String("Rainbow\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
      case 10:
        String("Random Colors\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
      case 13:
        lp.pattern[i].paramA = 255 ;
        lp.pattern[i].paramB = 0 ;
        lp.pattern[i].paramC = 0 ;
        lp.pattern[i].paramD = 3 ;
        String("Theater chase Red\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
      case 12:
        lp.pattern[i].paramA = 0 ;
        lp.pattern[i].paramB = 255 ;
        lp.pattern[i].paramC = 0 ;
        lp.pattern[i].paramD = 3 ;
        String("Theater chase Green\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
      case 11:
        lp.pattern[i].paramA = 0 ;
        lp.pattern[i].paramB = 0 ;
        lp.pattern[i].paramC = 255 ;
        lp.pattern[i].paramD = 3 ;
        String("Theater chase Blue\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
      case 14:
        String("Rainbow Chase\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
      case 15:
        String("Rainbow Mk2\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
      case 16:
        lp.pattern[i].paramD = 1 ;   // blank by default
        String("Stream/Blank\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;

      case 17:  //  st george red white
        String("st george Chase "+String(i-17)+"\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 16711680 ;
        lp.pattern[i].paramB = 16777215 ;
        lp.pattern[i].paramC = 0 ;
        lp.pattern[i].lDelay = 50 ;        // delay in ms if used  
      break ;  
      case 18:   // bombers red with blanks
        String("bombers Chase "+String(i-17)+"\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 16711680 ;
        lp.pattern[i].paramB = 1 ;
        lp.pattern[i].paramC = 1 ;
        lp.pattern[i].lDelay = 50 ;        // delay in ms if used  
      break;  
      case 19:    // port adelaide  white light blue blank
        String("Port Adel Chase "+String(i-17)+"\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 16777215 ;
        lp.pattern[i].paramB = 8454143 ;
        lp.pattern[i].paramC = 1 ;
        lp.pattern[i].lDelay = 50 ;        // delay in ms if used  
      break;  
      case 20:  // adelaide 
        String("Adelaide Chase "+String(i-17)+"\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 16711680 ;
        lp.pattern[i].paramB = 16776960 ;
        lp.pattern[i].paramC = 153 ;
        lp.pattern[i].lDelay = 50 ;        // delay in ms if used  
      break;  
      case 21:  // RGB 
        String("RGB Chase "+String(i-17)+"\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 16711680 ;
        lp.pattern[i].paramB = 16776960 ;
        lp.pattern[i].paramC = 0 ;
      break;
      case 22:  // fire engine chase
        String("Fire R/B Chase "+String(i-17)+"\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 16711680 ;
        lp.pattern[i].paramB = 255 ;
        lp.pattern[i].paramC = 0 ;
        lp.pattern[i].paramD = 250 ;
      break ;     
      case 23:
        String("Solid/Flash Rd\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 255 ;
        lp.pattern[i].paramD = 500 ;
      break  ;
      case 24:
        String("Solid/Flash Wh\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 255 ;
        lp.pattern[i].paramB = 255 ;
        lp.pattern[i].paramC = 255 ;
        lp.pattern[i].paramD = 500 ;
      break;
      case 25:
        String("Solid/Flash Bl\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramC = 255 ;
        lp.pattern[i].paramA = 0 ;
        lp.pattern[i].paramE = 1 ;
      case 26:
        String("Solid/Flash Gr\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramD = 0 ;
        lp.pattern[i].paramE = 1 ;
      break;
      
      case 29:
        String("Sgl Pixel Chase Wt\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 255 ;
        lp.pattern[i].paramB = 255 ;
        lp.pattern[i].paramC = 255 ;
      break;
      case 28:
        String("Sgl Pixel Chase Rd\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 255 ;
        lp.pattern[i].paramB = 0 ;
        lp.pattern[i].paramC = 0 ;
        lp.pattern[i].lDelay = 20 ;        // delay in ms if used  
      break;
      case 27:
        String("Sgl Pixel Chase Yl\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
        lp.pattern[i].paramA = 0 ;
        lp.pattern[i].paramB = 255 ;
        lp.pattern[i].paramC = 255 ;
        lp.pattern[i].lDelay = 50 ;        // delay in ms if used  
      break;
      default:
        String("\0").toCharArray(lp.pattern[i].Description,sizeof(lp.pattern[i].Description))  ; // Pattern Name
      break;
    }
  }
}

