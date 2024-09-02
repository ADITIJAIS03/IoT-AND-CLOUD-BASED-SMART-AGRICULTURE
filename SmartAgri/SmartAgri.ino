#include <LiquidCrystal.h>
LiquidCrystal lcd(13,12,14,27,26,25);
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REPORTING_PERIOD_MS     1000
#include <DHT.h>
#define dht_pin 4
#define Type DHT11
DHT HT(dht_pin,Type);
int wet=36;

int moistvalue;
float tempc,Humidity;

#include <WiFi.h>
#include "ThingSpeak.h"

const char* ssid = "smile123";   // your network SSID (name) 
const char* password = "123456789";   // your network password
WiFiClient  client;

unsigned long myChannelNumber =  2478521;
const char * myWriteAPIKey = "SBQJJLY8Q8NUEP7N";


// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

void Connect_wifi();

uint32_t tsLastReport = 0;

char MOISTURE_buff[20],CO_buff[20],PH_buff[20];
int MOISTURE_Value,PH_Value,CO_Value;


int phsensor=39;

unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;


int nValue;
int pValue;
int kValue;
int PhValue;

int ir=2;
int relay=15;

void setup() 
{
  lcd.begin(16,2);
  Serial.begin(9600);  //Initialize serial
  pinMode(phsensor,INPUT);
  pinMode(wet,INPUT);
  pinMode(ir,INPUT);
  pinMode(relay,OUTPUT);
  pinMode(dht_pin,INPUT);
  digitalWrite(relay,LOW);
  HT.begin();
  WiFi.mode(WIFI_STA);   
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
//  Connect_wifi();

  Connect_wifi();
}

void Connect_wifi()
{
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  pinMode(wet,INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    lcd.clear();
    lcd.print("wifi conneting");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
//  Init_spo2();
}

void loop() {
  ir_check();
  humidity_check();
  moisture_check();
  N_Check();
  P_Check();
  K_Check();
  lcd.clear();
  lcd.print("T="+String(tempc)+"H="+String(Humidity));
  lcd.setCursor(0,1);
  lcd.print("N="+String(nValue)+"P="+String(pValue)+"K="+String(kValue));
  delay(1000);
    // set the fields with the values
//    ThingSpeak.setField(1, count);
    ThingSpeak.setField(1, Humidity);
    ThingSpeak.setField(2, tempc);
    ThingSpeak.setField(3, moistvalue);
    ThingSpeak.setField(4, nValue);
    ThingSpeak.setField(5, pValue);
    ThingSpeak.setField(6, kValue);
   
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store  to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
      lcd.clear();
      lcd.print("Channel update");
      lcd.setCursor(0,1);
      lcd.print("successful.");
      Connect_wifi();
//      Init_spo2();
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
//  }
}
void ir_check()
{
  int irvalue=digitalRead(ir);
  Serial.println("irvalue="+String(irvalue));
  if(irvalue==LOW)
  {
   lcd.clear();
   lcd.print("ANIMAL DETECTED");
   delay(1000); 
  }
}
void moisture_check()
{
 moistvalue=analogRead(wet);
 moistvalue=map(moistvalue,0,4096,0,1024);
 Serial.println("moistvalue="+String(moistvalue));
 delay(1000);
 if(moistvalue<850)
 {
  digitalWrite(relay,LOW);
  lcd.clear();
  lcd.print("SOIL IS WET");
  delay(1000);
 }
 else
 {
  digitalWrite(relay,HIGH);
  lcd.clear();
  lcd.print("SOIL IS DRY");
  delay(1000);
 }
}

void humidity_check()
{
     tempc=HT.readTemperature();
     Humidity=HT.readHumidity();
     Serial.println("temp="+String(tempc));
     Serial.println("Humidity="+String(Humidity));
     delay(1000);
     
}
void N_Check(){
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(phsensor);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  nValue=3.5*phValue*35;  
  
  Serial.print("NValue:");  
  Serial.print(nValue);
  Serial.println("");
  
  delay(1000);
}

 void P_Check(){
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(phsensor);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  pValue=3.5*phValue*10;  

  Serial.print("PValue:");  
  Serial.print(pValue);
  Serial.println("");
  delay(1000);

    }
 
 void K_Check(){
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(phsensor);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  kValue=3.5*phValue*12;  

  Serial.print("kValue:");  
  Serial.print(kValue);
  Serial.println("");
  delay(1000);

    }