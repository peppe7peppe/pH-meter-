/*******************************
* SKETCH 1 - Wifi: DEFINE, VARIABLES...
********************************/
#include <SoftwareSerial.h>
#define RX 8
#define TX 9
String AP = "";       // AP NAME 
String PASS = ""; // AP PASSWORD
String API =  "";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial my_esp8266(RX,TX); 

/*******************************
* SKETCH 2 - pH: DEFINE, VARIABLES...
********************************/
int sensorPh =A0; //potentiometer ansluten till A0
int ledAc =11; //röd led (syrkontroll) ansluten till stift 11
int ledBs =12; //blå led (grundkontroll) ansluten till stift 12
int potValue;
int pH;


void setup() {
  /*****************************
   * SKETCH 1 - Wifi: SETUP CODE
   ****************************/
  Serial.begin(9600);
  my_esp8266.begin(9600);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  /*****************************
   * SKETCH 2 - pH: SETUP CODE
   ****************************/ 
  pinMode(sensorPh, INPUT); //Inmatning av pH-sensor
  pinMode(ledAc, OUTPUT); //ledd utgång
  pinMode(ledBs, OUTPUT);
  Serial.begin (9600);
}

void loop() {
  /*****************************
   * SKETCH 1 - Wifi: LOOP CODE
   ****************************/
 valSensor = getSensorData();
 String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 my_esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
   /*****************************
   * SKETCH 2 - pH: LOOP CODE
   ****************************/
  Serial.println (pH); // skriver ut pH-värdet med monitorn
  potValue = analogRead (sensorPh); // värdet lagras i sensorValue
  digitalWrite(ledAc, LOW);
  digitalWrite(ledBs, LOW);
  
  // pH-värde-korrigering för att erhålla ett intervall från 0 till 14
  pH = (14 * potValue / 1023);
  
  //om pH är mycket surt tänds den röda lysdioden
  if (pH < 5.5) 
  {
    digitalWrite(ledAc, HIGH);
    delay(1000); 
  }
  
  //om pH är mycket grundläggande tänds den blå lysdioden
  if (pH > 7.5)
  {
    digitalWrite(ledBs, HIGH);
    delay(1000);
  }  
}

///other code 1

int getSensorData(){
  return (pH); // skicka ut pH värdet till api 
}
// Jag har kommenterat bort utskrift på wifi inloggning, eftersom jag vill demonstrera endast pH-värde på serial monitor,
void sendCommand(String command, int maxTime, char readReplay[]) {
//  Serial.print(countTrueCommand);
//  Serial.print(". at command => ");
//  Serial.print(command);
//  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    my_esp8266.println(command);//at+cipsend
    if(my_esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
//    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail"); // fel meddelande skrivs ut om fel uppstår
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
