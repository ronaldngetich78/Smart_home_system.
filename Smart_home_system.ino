#include <avr/wdt.h>
#include <EEPROM.h>
#include <SoftwareSerial.h> //Create software serial object to communicate with SIM800L
SoftwareSerial GSM(8, 9);   //SIM800L Tx & Rx is connected to Arduino #8 & #9

String phone_no1 = "+254725267878"; 
String phone_no2 = "+254726457980"; 

String        RxString  = ""; // Will hold the incoming String  from the GSM shield
char          RxChar    = ' ';
int           Counter   = 0;
String        GSM_Nr    = "";
String        GSM_Msg   = "";
int           count1    = 0;
int           count2    = 0;
int           ledState  = 0;
int           Calling   = 0;
int           Calling1  = 0;
int           Calling2  = 0;
int           highThreshold = 200;
int           lowThreshold  = 100;
int           numSamples    = 100;
unsigned long  currentTime1 = 0;
unsigned long  currentTime2 = 0;
unsigned long  currentTime3 = 0;
unsigned long  previousTime1= 0;
unsigned long  previousTime2= 0;
unsigned long  previousTime3= 0;
unsigned long  speed= 0;
long           ledDelay =1000;

#define Relay1 2 // Load1 Pin Out
#define Relay2 3 // Load2 Pin Out
#define Relay3 4 // Load3 Pin Out
#define Relay4 A2 // Load4 Pin Out
#define sittingRoomBulb 10
#define bedroomBulb     11
#define securityBulb    12
#define solarPower       6
#define Relay5 A1 //LDR Pin Out
#define LDR A0// LDR input pin
#define door 7
#define redLed 13
#define reset A3
#define PIR 5

int D1, D2, D3, D4, call, Alarm, averageValue, ldrStatus, sensorValue, A;
bool load1, load2, load3, load4, load5, load6, load7, load8, Ldr, LdrTimer, Pir, Door, lightDetected, sendingSMS; 
boolean a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z;



void setup(){ // put your setup code here, to run once

  //wdt_enable(WDTO_8S);
  pinMode(Relay1, OUTPUT); digitalWrite(Relay1, 1); 
  pinMode(Relay2, OUTPUT); digitalWrite(Relay2, 1); 
  pinMode(Relay3, OUTPUT); digitalWrite(Relay3, 1); 
  pinMode(Relay4, OUTPUT); digitalWrite(Relay4, 1); 
  pinMode(redLed, OUTPUT); digitalWrite(redLed, 0);
  pinMode(Relay5, OUTPUT); 
  pinMode(reset,  OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(LDR, INPUT);
  pinMode(sittingRoomBulb, INPUT_PULLUP);
  pinMode(bedroomBulb,     INPUT_PULLUP);
  pinMode(securityBulb,   INPUT_PULLUP);
  pinMode(solarPower,      INPUT_PULLUP);
  pinMode(door,            INPUT_PULLUP);

  digitalWrite(reset, 0);
  delay(40000);
  digitalWrite(reset, 1);
  delay(3000);


  wdt_enable(WDTO_8S);
  Serial.begin(9600);//Begin serial communication with Arduino and Arduino IDE (Serial Monitor)  
  GSM.begin(9600);   //Begin serial communication with Arduino and SIM800L


  Serial.println("Initializing....");
  initModule("AT","OK",1000);                //Scan for GSM Module
  initModule("AT+CPIN?","READY",1000);       //this command is used to check whether SIM card is inserted in GSM Module or not
  initModule("AT+CMGF=1","OK",1000);         //Set SMS mode to ASCII
  initModule("AT+CNMI=2,2,0,0,0","OK",1000); //Set device to read SMS if available and print to serial
  Serial.println("Initialized Successfully"); 

  a=c=e=g=i=j=n=o=p=q=r=s=u=v=w=x=z=true;
  b=d=f=h=k=l=m=t=y=lightDetected=false;

  load1     = EEPROM.read(1);
  load2     = EEPROM.read(2);
  load3     = EEPROM.read(3);
  load4     = EEPROM.read(4);
  load5     = EEPROM.read(5);
  load6     = EEPROM.read(6);
  load7     = EEPROM.read(7);
  load8     = EEPROM.read(8);

  relays();
  delay(100);
  initBools();
  wdt_reset();

}

void loop(){

  wdt_reset(); 
  ldrStatus    = analogRead(LDR);   //read the status of the LDR value
  sensorValue  = digitalRead(PIR);
  D1 = digitalRead(sittingRoomBulb);
  D2 = digitalRead    (bedroomBulb);
  D3 = digitalRead   (securityBulb);
  D4 = digitalRead     (solarPower);

  //int averageValue = ldrAverageValue();
  //Serial.println(averageValue);

  currentTime1= millis();
  if (currentTime1 - previousTime1 >= 300000){
    initModule("AT+CPIN?","READY",1000);
  /*             
  if (!sendSMS("+254725267878", "Hello from Arduino!")) {
    Serial.println("SMS failed. Restarting GSM...");
    //resetGSM();
    delay(10000); // Wait for module to restart
    //initGSM();
  } else {
    Serial.println("SMS sent successfully.");
  }
     */        
    previousTime1= currentTime1;
  }
  
  currentTime3= millis();
  if (currentTime3 - previousTime3 >= ledDelay && ldrStatus <= 300  ){
    ledState= !ledState;
    digitalWrite(redLed, ledState);
    previousTime3= currentTime3;
  }

  if(D1==0&&D2==0&&D3==0&&x==true){ sendingSMS = sendSMS(phone_no1,"All lights On"); sentSMSStatus();  
    sendingSMS = sendSMS(phone_no2,"All lights On");  sentSMSStatus();a=b=c=d=e=f=x=y= false; }
  if(D1==1&&D2==1&&D3==1&&y==false){sendingSMS =  sendSMS(phone_no1,"All lights Off");  sentSMSStatus();
    sendingSMS = sendSMS(phone_no2,"All lights off");a=b=c=d=e=f=x=y= true;}

  if(D1 == 0 && a==true){ sendingSMS = sendSMS(phone_no1,"Sitting room light On");  sentSMSStatus();
    sendingSMS = sendSMS(phone_no2,"Sitting room light On");  sentSMSStatus(); a=b= false; x=true;}
  if(D1 == 1 && b==false){ sendingSMS = sendSMS(phone_no1,"Sitting room light Off");  sentSMSStatus();
    sendingSMS = sendSMS(phone_no2,"Sitting room ligt Off"); sentSMSStatus(); a=b= true; y=false;}
  
  if(D2 == 0 && c==true){ sendingSMS = sendSMS(phone_no1," Bedroom light On");  sentSMSStatus();  
    sendingSMS = sendSMS(phone_no2,"Bedroom light On"); sentSMSStatus(); c=d= false;x=true; }
  if(D2 == 1 && d==false){ sendingSMS = sendSMS(phone_no1,"Bedroom light Off"); sentSMSStatus(); 
    sendingSMS = sendSMS(phone_no2,"Bedroom light Off"); sentSMSStatus(); c=d= true; y=false;}
             
  if(D3 == 0 && e==true){ sendingSMS = sendSMS(phone_no1,"Security lights On"); sentSMSStatus(); 
    sendingSMS = sendSMS(phone_no2,"Security lights On"); sentSMSStatus(); e=f= false;x=true; }
  if(D3 == 1 && f==false){ sendingSMS = sendSMS(phone_no1,"Security lights Off"); sentSMSStatus(); 
    sendingSMS = sendSMS(phone_no2,"Security lights Off"); sentSMSStatus(); e=f= true; y=false;}
                         
  if(D4 == 0 && g==true){ sendingSMS = sendSMS(phone_no1,"Solar power On"); sentSMSStatus(); 
    sendingSMS = sendSMS(phone_no2,"Solar power on"); sentSMSStatus(); g=h= false;}
  if(D4 == 1 && h==false){ sendingSMS = sendSMS(phone_no1,"Solar power Off"); sentSMSStatus(); 
    sendingSMS = sendSMS(phone_no2,"Solar power Off"); sentSMSStatus(); g=h= true;}
  
  // scan for data from software serial port
  //-----------------------------------------------
  RxString = "";
  Counter = 0;
  while(GSM.available()){
    delay(1);  // short delay to give time for new data to be placed in buffer
    // get new character
    RxChar = char(GSM.read());
    //add first 200 character to string
    if (Counter < 200) {RxString.concat(RxChar);Counter = Counter + 1;}
  }
    
    // Is there a new SMS?
    //-----------------------------------------------
  if (Received(F("CMT:")) ) GetSMS();

  if(GSM_Nr==phone_no1 || GSM_Nr==phone_no2){

    if(GSM_Msg=="Sitting room on"  && D1 == 1 && load1==0 ) {load1=1;j=false;}
    if(GSM_Msg=="Sitting room off" && D1 == 0 && load1==0){load1=1; j=false;}
    if(GSM_Msg=="Sitting room on"  && D1 == 1 && load1==1 && j== true) {load1=0; }
    if(GSM_Msg=="Sitting room off" && D1 == 0 && load1==1 && j== true){load1=0;}

    if(GSM_Msg=="Bedroom on" && D2 == 1 && load2==0) {load2=1; j=false;}
    if(GSM_Msg=="Bedroom off" && D2 == 0 && load2==0){load2=1; j=false;}
    if(GSM_Msg=="Bedroom on" && D2 == 1 && load2==1 && j== true) {load2=0;}
    if(GSM_Msg=="Bedroom off" && D2 == 0 && load2==1 && j== true){load2=0;}

    if(GSM_Msg=="Security on" && D3 == 1 && load3==0) {load3=1; j=false;}
    if(GSM_Msg=="Security off" && D3 == 0 && load3==0){load3=1; j=false;}
    if(GSM_Msg=="Security on" && D3 == 1 && load3==1 && j== true) {load3=0;}
    if(GSM_Msg=="Security off" && D3 == 0 && load3==1 && j== true){load3=0;} 

    if(GSM_Msg=="Ldr on") {load5=1; sendingSMS = sendSMS(GSM_Nr,"Ok Ldr is activated"); sentSMSStatus();}
    if(GSM_Msg=="Ldr off") {load5=0; sendingSMS = sendSMS(GSM_Nr,"Ok Ldr is deactivated"); sentSMSStatus();}
    if(GSM_Msg=="Timer on") {load6=1; sendingSMS = sendSMS(GSM_Nr,"Ok timer is activated"); sentSMSStatus(); previousTime2 = currentTime2;}
    if(GSM_Msg=="Timer off") {load6=0; sendingSMS = sendSMS(GSM_Nr,"Ok timer is deactivated"); sentSMSStatus();}
    if(GSM_Msg=="Pir on") {load7=1; sendingSMS = sendSMS(GSM_Nr,"Ok Pir is activated"); sentSMSStatus();}
    if(GSM_Msg=="Pir off") {load7=0; sendingSMS = sendSMS(GSM_Nr,"Ok Pir is deactivated"); sentSMSStatus(); }
    if(GSM_Msg=="Door on") {load8=1; sendingSMS = sendSMS(GSM_Nr,"Ok Door is activated"); sentSMSStatus();}
    if(GSM_Msg=="Door off") {load8=0; sendingSMS = sendSMS(GSM_Nr,"Ok Door is deactivated"); sentSMSStatus();}

    if(GSM_Msg=="Call") {
      GSM.println("ATD+ +254725267878;");
      for (Calling = 0; Calling < 24; Calling++){ wdt_reset(); delay(5000);}
      GSM.println("ATH"); 
      Calling = 0; 
    }

    if(GSM_Msg=="All on") {
      if(D1==1){load1=!load1;}
      if(D2==1){load2=!load2;}
      if(D3==1){load3=!load3;}
      if(D4==1){load4=!load4;}
      x=true; y=false;
    }

    if(GSM_Msg=="All off") {
      if(D1==0){load1=!load1;}
      if(D2==0){load2=!load2;}
      if(D3==0){load3=!load3;}
      if(D4==0){load4=!load4;}
      x=true; y=false;
    }

    if(GSM_Msg=="Bulbs status"){
      String loadst1 = "";
      String loadst2 = "";
     
      if(D1==0&&D2==0&&D3==0){loadst1="All lights on.";  sendingSMS = sendSMS(GSM_Nr,loadst1); sentSMSStatus(); z=false;}
      if(D1==1&&D2==1&&D3==1){loadst1="All lights off."; sendingSMS = sendSMS(GSM_Nr,loadst1); sentSMSStatus(); z=false;}

      if(z==true){
        
        if(D1==0){loadst2="SR 1. ";}
                else{loadst2="SR 0. ";}

        if(D2==0){loadst2=loadst2 + "BR 1. ";}
                else{loadst2=loadst2 + "BR 0. ";}

        if(D3==0){loadst2=loadst2 + "SC 1";}
                else{loadst2=loadst2 + "SC 0";}

        sendingSMS = sendSMS(GSM_Nr,loadst2); sentSMSStatus();        

      }
      z=true;
    }

    if(GSM_Msg=="Solar status"){
      String solarst = "";

      if(D4==0){solarst="Solar power On.";}
      if(D4==1){solarst="Solar power Off.";}

      sendingSMS = sendSMS(GSM_Nr,solarst); sentSMSStatus();
    }
    /* 
    if(GSM_Msg == "Door status"){
      String doorst = "";
      call          = digitalRead(door);

      if(call == 0) {doorst = "Door locked." ;}
      if(call == 1) {doorst = "Door open." ;}

      sentSMSStatus(); sendSMS(GSM_Nr,doorst);
      
    }
    */
    if(GSM_Msg=="Restart"){delay(20000);}

    eeprom_write();
    relays();
    wdt_reset(); 
    //GSM.println("AT+CMGDA=\"DEL ALL\"");
    GSM. println("AT+CMGD=1, 4");
    delay(5000);
    wdt_reset();  
    Serial.println("Hello Hello");
  }

  GSM_Nr="";
  GSM_Msg="";

  if (load8==1){
    call = digitalRead(door);
    if (call == HIGH && i == true){
      GSM.println("ATD+ +254725267878;"); 
      Serial.println("callig1");
      for (Calling1 = 0; Calling1 < 5; Calling1++){ wdt_reset(); delay(5000);}
      GSM.println("ATH");
      Serial.println("ATH");
      Calling1=0;
      /*
      GSM.println("ATD+ +254726457980;"); 
      Serial.println("callig2");
      for (Calling2 = 0; Calling2 < 5; Calling2++){ wdt_reset(); delay(5000);}
      GSM.println("ATH");
      Serial.println("ATH");
      Calling2=0; 
      i=false;
      */
    } 
   
    if(call == LOW && i == false){
      Serial.println("a");
      delay(1000);
      i = true;
    }
  }

  if(!lightDetected && ldrStatus<=lowThreshold){
    averageValue = ldrAverageValue();
    if(averageValue <= 100) {
      m = true; 
      k=true; 
      lightDetected=true;         
      Serial.println("LDR is DARK, Relay3 is ON");
    }
  }

  if(lightDetected && ldrStatus>=highThreshold){
    averageValue = ldrAverageValue();
    if(averageValue >= 200) {
      l= true;
      t= true;
      k=false;
      lightDetected=false;   
      Serial.println("LDR is LIGHT, Relay3 is OFF");
    }
  }

  if(load5==1 && D3==1 && k==true && load3 ==0 && l ==true  ){ load3 = 1;
    eeprom_write();
    relays();
    Serial.println(load1);
    Serial.println("night1");
    l= false;
  }

  if(load5==1 && D3==1 && k==true && load3 ==1 && l ==true  ){ load3 = 0;
    eeprom_write();
    relays();
    Serial.println(load1);
    Serial.println("night2");
    l= false;
  }

  if(load5==1 && D3==0 && k==true && l ==true  ){ 
    Serial.println("nightA");
    l= false;
  }

  if(load5==1 && D3==0 && k==false && load3 ==0  && m == true){ load3 = 1;
    eeprom_write();
    relays();
    Serial.println("day1");
    m= false;
  }

  if(load5==1 && D3==0 && k==false && load3 ==1  && m == true){ load3 = 0;
    eeprom_write();
    relays();
    Serial.println("day2");
    m= false;
  }

  if(load5==1 && D3==1 && k==false   && m == true){ 
    Serial.println("dayA");
    m= false;
  }

  
  if(load6 == 1 && D3==0 && w== true) {
    load3 = !load3;
    eeprom_write();
    relays();
    w=false;
  }
  
  if(load6 == 0) {u=true;}
  
  if(load6 == 1 && D3==0 && u==true) {
     previousTime2= currentTime2;
     u=false;   
       }
  
  if(load6 == 1 && D3==1){
    previousTime2= currentTime2;
    u=true;
     }

  if(currentTime2 - previousTime2 >= 7200000 && load6 == 1 && D3==0){ 
    load3 = !load3;
    eeprom_write();
    relays();
    u=true;       
  }

  if(load7== 1 && sensorValue ==HIGH && ldrStatus <=300 && D3==1 && load3 == 0&& n==true ){  load3 = 1;
    digitalWrite (Relay3, load3);
    Serial.println("10");
    delay(1000);
    digitalWrite (Alarm, HIGH);
    n=false;
    o=false;
    p=true;
    q= true;
  }

  if(load7== 1 && sensorValue ==HIGH && ldrStatus <=300 && D3==1 && load3 == 1 && o==true ){  load3 = 0;
    digitalWrite (Relay3, load3);
    Serial.println("20");
    delay(1000);
    digitalWrite (Alarm, HIGH);
    n=false;
    o=false;
    p=true;
    q= true;
  }

  if(load7== 1 && sensorValue ==LOW && ldrStatus <=300 && D3==0 && load3 == 0 && p == true  ){  load3 = 1;
    digitalWrite (Relay3, load3);
    Serial.println("30");
    delay(1000);
    digitalWrite (Alarm, LOW);
    n=true;
    o=true;
    p=false;
    q= false;
  }

  if(load7== 1 && sensorValue ==LOW && ldrStatus <=300 && D3==0 && load3 == 1 && q==true ){  load3 = 0;
    digitalWrite (Relay3, load3);
    Serial.println("40");
    delay(1000);
    digitalWrite (Alarm, LOW);
    n=true;
    o=true;
    p=false;
    q= false; 
  }
    
}

void eeprom_write(){
  EEPROM.write(1,load1);
  EEPROM.write(2,load2);
  EEPROM.write(3,load3);
  EEPROM.write(4,load4);  
  EEPROM.write(5,load5);
  EEPROM.write(6,load6);
  EEPROM.write(7,load7);
  EEPROM.write(8,load8); 
}

void relays(){  
  digitalWrite(Relay1, load1); 
  digitalWrite(Relay2, load2); 
  digitalWrite(Relay3, load3); 
  digitalWrite(Relay4, load4); 
  j= true;
}

// Send SMS  
bool sendSMS(String number, String msg){
  digitalWrite(redLed, 1);
  GSM.println("AT+CMGF=1");
  delay(1000);
  digitalWrite(redLed, 0);
  GSM.print("AT+CMGS=\"");GSM.print(number);GSM.println("\"\r\n"); //AT+CMGS=”Mobile Number” <ENTER> - Assigning recipient’s mobile number
  delay(500);
  GSM.println(msg); // Message contents
  delay(500);
  digitalWrite(redLed, 1);
  wdt_reset(); 
  GSM.write(byte(26)); //Ctrl+Z  send message command (26 in decimal).
  delay(5000);
  wdt_reset();
  digitalWrite(redLed, 0);

  // Check if the response contains "OK"
  if (GSM.find("OK")) {
    return true;
  } else {
    return false;
  }

}

// Get SMS Content
void GetSMS() {
  //Get SMS number
  //================================================
  GSM_Nr  = RxString;
  //get number
  int t1 = GSM_Nr.indexOf('"');
  GSM_Nr.remove(0,t1 + 1);
  t1 = GSM_Nr.indexOf('"');
  GSM_Nr.remove(t1);

  // Get SMS message
  //================================================
  GSM_Msg = RxString;
  //Serial.println(GSM_Msg);
  t1 = GSM_Msg.indexOf('"');
  Serial.println(t1);
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  Serial.println(t1);
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  GSM_Msg.remove(0,1);
  GSM_Msg.trim();

  Serial.print("Number:"); Serial.println(GSM_Nr);
  Serial.print("SMS:"); Serial.println(GSM_Msg);
  
}

// Search for specific characters inside RxString 
boolean Received(String S) {
  if (RxString.indexOf(S) >= 0) return true; else return false;
}

// Init GSM Module 
void initModule(String cmd, char *res, int t){
  while(1){
    Serial.println(cmd);
    GSM.println(cmd);
    delay(100);
    while(GSM.available()>0){
      if(GSM.find(res)){
        Serial.println(res);
        delay(t);
        wdt_reset(); 
        return;
      }
      else{Serial.println("Error");}
    }
    delay(t);
  }
}

void initBools(){
  int ldrStatus = analogRead(LDR);
  D1 = digitalRead(sittingRoomBulb);
  D2 = digitalRead(bedroomBulb);
  D3 = digitalRead(securityBulb);
  D4 = digitalRead(solarPower);

 sendingSMS = sendSMS(phone_no1,"Restarting");
 sentSMSStatus();
  //sendSMS(phone_no2,"Restarting");

  if(D1 == 0){ a=b= false; x=true;}
  else{ a=b= true; y=false; }
  
  if(D2 == 0){c=d= false;x=true; }
  else{c=d= true; y=false;}
             
  if(D3 == 0){e=f= false; x=true; }
  else{e=f= true; y=false;}
                         
  if(D4 == 0){ g=h= false;}
  else{g=h= true;}

  if(D1==0&&D2==0&&D3==0) a=b=c=d=e=f=x=y=false; 
  if(D1==1&&D2==1&&D3==1) a=b=c=d=e=f=x=y=true;
  

  if(ldrStatus<=300){lightDetected=false; }
  else{lightDetected=true; }
}

int ldrAverageValue(){
  unsigned long total     = 0;
  int           sampleNum = 0;
  for(int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
    total += analogRead(LDR);
    wdt_reset();
    delay(10);
  }

  return total / numSamples;
  delay(100);

}


void sentSMSStatus(){
  if (sendingSMS == false) {
  Serial.println("SMS failed. Restarting GSM...");
  /*
  digitalWrite(reset, 0);
  A = 0;
  for (A = 0; A < 10; A++){ wdt_reset(); delay(5000);} 
  digitalWrite(reset, 1);
  delay(3000);
  wdt_reset();

  Serial.println("Initializing....");
  initModule("AT","OK",1000);                //Scan for GSM Module
  initModule("AT+CPIN?","READY",1000);       //this command is used to check whether SIM card is inserted in GSM Module or not
  initModule("AT+CMGF=1","OK",1000);         //Set SMS mode to ASCII
  initModule("AT+CNMI=2,2,0,0,0","OK",1000); //Set device to read SMS if available and print to serial
  Serial.println("Initialized Successfully"); 
*/
    //resetGSM();
  } else {
    Serial.println("SMS sent successfully.");
    //String number, String msg
    }
}
