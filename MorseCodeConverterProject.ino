#include <WiFi.h>
#include <WebServer.h>
#include<LiquidCrystal_I2C.h>
#include<Wire.h>
const char* ssid = "YourWiFiName";//add your local Wi-Fi name
const char* password = "YourPassword";//add your Wi-Fi password

WebServer server(80);
LiquidCrystal_I2C lcd(0x27,16,2);
String receivedMessage = "";
String msg="";
String last="";
struct morse{
  char letter;
  const char*code;
};
struct morse Morse[26]={
  {'A',".-"},{'B',"-..."},{'C',"-.-."},{'D',"-.."},{'E',"."},
  {'F',"..-."},{'G',"--."},{'H',"...."},{'I',".."},{'J',".---"},
  {'K',"-.-"},{'L',".-.."},{'M',"--"},{'N',"-."},{'O',"---"},{'P',".--."},{'Q',"--.-"},
  {'R',".-."},{'S',"..."},{'T',"-"},{'U',"..-"},{'V',"...-"},{'W',".--"},
  {'X',"-..-"},{'Y',"-.--"},{'Z',"--.."}
};//Morse code for each alphabet
bool check=false;
static int l=0;
const int dot=100;
const int dash=300;
const int gap=300;
const int lettergap=300;
bool flag;
#define led 2
#define buzzer 4

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>MorseCode Messenger</title></head><body>";
  html += "<h2>Convert Message to MorseCode</h2>";
  html += "<form action='/send' method='POST'>";
  html += "Message: <input type='text' name='msg'><br><br>";
  html += "<input type='submit' value='Send'>";
  html += "</form>";
  html += "<p>Last message: " + receivedMessage + "</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSend() {
  if (server.hasArg("msg")) {
    receivedMessage = server.arg("msg");
    Serial.println("Received: " + receivedMessage);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}
void pause(int value){
  unsigned long current=millis();
  while(millis()-current<value){}
}
void blinkMorse(char symbol){
  if(l>=16&&check){
    lcd.setCursor(0,1);
    check=false;
  }
  if(l>32){
    lcd.clear();
    lcd.setCursor(0,0);
    l=0;
    check=true;
  }
  if(symbol=='.'){
    digitalWrite(led,HIGH);
    digitalWrite(buzzer,HIGH);
    Serial.print(".");
    lcd.print(".");
    pause(dot);
  }
  else if(symbol=='-'){
    digitalWrite(led,HIGH);
    digitalWrite(buzzer,HIGH);
    Serial.print("-");
    lcd.print("-");
    pause(dash);
  }
  l++;
  digitalWrite(led,LOW);
  digitalWrite(buzzer,LOW);
  pause(gap);
}
void findMorse(char letter){
    letter=toupper(letter);
    const char*code=Morse[letter-'A'].code;//get index of morse of letter
    for(int j=0;code[j]!='\0';j++){
      blinkMorse(code[j]);
    }
}
void setup() {
  Wire.begin(21,22);
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  pinMode(buzzer,OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    pause(500);
    Serial.print(".");
  }
  lcd.setCursor(0,0);
  lcd.print("CONNECTED VISIT");
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP().toString());
  server.on("/", handleRoot);
  server.on("/send", HTTP_POST, handleSend);
  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
  msg=receivedMessage;
  if(msg!=last){
    flag=true;
    l=0;
    check=true;
    lcd.clear();
    lcd.setCursor(0,0);
    last=msg;
    for(int i=0;msg[i]!='\0';i++){
      if(msg[i]==' '){
        l++;
      }
      else{
        findMorse(msg[i]);
        Serial.print(" ");
        lcd.print(" ");
        l++;
      }
      pause(lettergap);
    }
  }
  Serial.println();
  pause(2000);
  if(flag){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SEND MESSAGE TO");
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP().toString());
    flag=false;
  }
}
