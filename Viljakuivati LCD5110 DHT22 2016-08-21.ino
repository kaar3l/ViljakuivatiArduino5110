#include "DHT.h"
#include <LCD5110_Basic.h>

LCD5110 myGLCD(2,3,4,6,5);
#define DHTPIN 10
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
extern uint8_t SmallFont[];
int ledpin=13;

//Niiskuse ja Tempi limiidid
int RHlimit=70; //<50 peaks olema niiskus
int templimit=24; //<22 peaks olema temp
int k=0; //Arv millele lisatakse juurde +1 kui temp korras ja veel +1, kui RH korras
//Kontrollitakse, kas arv on k>2 siis võib tööle panna

//Relee pinnid
int relay1=8;
int relay2=9;

void setup()
{
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  myGLCD.setContrast(65);
  dht.begin();
}

void animation(){
  for (int i=0; i <= 1; i++){
  myGLCD.print("-------", CENTER, 24);
  delay(300);
  myGLCD.print("|------", CENTER, 24);
  delay(300);
  myGLCD.print("||-----", CENTER, 24);
  delay(300);
  myGLCD.print("|||----", CENTER, 24);
  delay(300);
  myGLCD.print("||||---", CENTER, 24);
  delay(300);
  myGLCD.print("|||||--", CENTER, 24);
  delay(300);
  myGLCD.print("||||||-", CENTER, 24);
  delay(300);
  myGLCD.print("|||||||", CENTER, 24);
  delay(300);
  }
}

void loop()
{
  char hchar[10];
  char tchar[10];
  myGLCD.clrScr();
  myGLCD.print("Kontrollin", CENTER, 16);
  animation();
  delay(2000);
  digitalWrite(ledpin, HIGH);
  delay(50);
  float h = dht.readHumidity();
  delay(50);
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  digitalWrite(ledpin, LOW);

  dtostrf(h, 5, 2, hchar);
  delay (200);
  myGLCD.clrScr();
  myGLCD.print("RH:", LEFT, 0);
  delay (500);
  myGLCD.printNumF(h, 2, RIGHT, 0);
  delay (500);
  myGLCD.print("Temp:", LEFT, 8);
  delay (500);
  myGLCD.printNumF(t, 2, RIGHT, 8);
  delay (2000);
  myGLCD.print("OFF", LEFT, 42);
  k=0;
//NIISKUSE KONTROLL:
  if(h<RHlimit){
    myGLCD.print("RHlim", LEFT, 16);
    delay (500);
    myGLCD.print("<", 30, 16);
    delay (500);
    myGLCD.printNumI(RHlimit, 36, 16);
    delay (500);
    myGLCD.print("-->ON", RIGHT, 16);
    k=k+1;
  }else{
    myGLCD.print("RHlim", LEFT, 16);
    delay (500);
    myGLCD.print(">", 30, 16);
    delay (500);
    myGLCD.printNumI(RHlimit, 36, 16);
    delay (500);
    myGLCD.print("-->OFF", RIGHT, 16);
  }
//TEMPERATUURI KONTROLL:
  if(t<templimit){
    myGLCD.print("Tlim", LEFT, 24);
    delay (500);
    myGLCD.print("<", 30, 24);
    delay (500);
    myGLCD.printNumI(templimit, 36, 24);
    delay (500);
    myGLCD.print("-->ON", RIGHT, 24);
    k=k+1;
  }else{
    myGLCD.print("Tlim", LEFT, 24);
    delay (500);
    myGLCD.print(">", 30, 24);
    delay (500);
    myGLCD.printNumI(templimit, 36, 24);
    delay (500);
    myGLCD.print("-->OFF", RIGHT, 24);
  }
//KONTROLL KAS MÕLEMAD ON PAIGAS:
  if(k>1){
    delay (500);
    myGLCD.print("ON ", LEFT, 42);
    //Siia ka Relee sisse lülitamine
    delay (500);
    digitalWrite(relay1, HIGH);
  }else{
    delay (500);
    myGLCD.print("OFF", LEFT, 42);
    //Siia ka Relee välja lülitamine
    delay (500);
    digitalWrite(relay1, LOW);
  }
  
///////////////////////////////////////////////////////
//Countdown timer
//Timer=600*1000ms=600sec=10min
  int measuretime=10; //Mitme seci tagant teeb uue mõõtmise
  int countDownTimer=60; //Mitme seci tagant lülitab releesid

  int measuretimevariable=measuretime;
  while(countDownTimer>1){
    countDownTimer--;
    measuretimevariable--;
    myGLCD.print("    ", RIGHT, 42);
    myGLCD.printNumI(countDownTimer, RIGHT, 42);
    delay(1000);
    if (measuretimevariable<1){
      
      digitalWrite(ledpin, HIGH);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float f = dht.readTemperature(true);
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        myGLCD.print("     ", RIGHT, 0);
        myGLCD.print("     ", RIGHT, 8);
        myGLCD.printNumF(h, 2, RIGHT, 0);
        myGLCD.printNumF(t, 2, RIGHT, 8);
      }
      myGLCD.printNumF(h, 2, RIGHT, 0);
      myGLCD.printNumF(t, 2, RIGHT, 8);
      digitalWrite(ledpin, LOW);
      measuretimevariable=measuretime;
    }
  }
}