// This #include statement was automatically added by the Particle IDE.
#include "LiquidCrystal/LiquidCrystal.h"
// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"
// This #include statement was automatically added by the Particle IDE.
#include "PietteTech_DHT/PietteTech_DHT.h"

#define DHTTYPE  DHT11       // Sensor type DHT11/21/22/AM2301/AM2302
#define DHTPIN   3           // Digital pin for communications

// pinout on LCD [RS, EN, D4, D5, D6, D7];
LiquidCrystal lcd(A2, A3, A4, A5, DAC, WKP);

//declaration
void dht_wrapper(); // must be declared before the lib initialization
void callback(char* topic, byte* payload, unsigned int length);

// Lib instantiate
PietteTech_DHT DHT(DHTPIN, DHTTYPE, dht_wrapper);

//byte server[] = { 192,168,  0,100 };
byte server[] = { 192,168, 43,200 };
MQTT client(server, 1883, callback);
//MQTT client("test.mosquitto.org", 1883, callback);

Servo myservo;// create servo object using the built-in Particle Servo Library

int redLED = D0;
int servoPin = D1;
int buzzer = D2;
int greenLED = D4;
int button = D5; // Override button
int blueLED = D7;
int photoresistor = A0;
int yellowLED = A1;
//int power = A2; // Constant power supply for better photoresistor accuracy

int temperature;
int humidity;
int lightlevel;

String mypayload;
const char *p;

String lcdmessage;
const char *l;

bool override = 0;
bool overThreshold = 0;
bool alertMode = 0;
bool previousAlert = 0;
bool needToAct = 0;

int maxtemperature=27;
int maxhumidity=40;
int maxlightlevel=1000;

int AlertMelody[] = {659,220,698,220,587,220,659,220,523,220,440,220,659,220,698,220,587,220,659,220,523,220,440,220,698,349,784,349,659,349,698,294,587,294,659,415,523,415,587,220,494,220,523,220,440,220,494,208,415,330,440};
int SongSize = 51;
//int AlertMelody[] = {850,750,658,540,440};
//int SongSize = 5;
int AllgoodMelody[] = {784,262,659,262,587,262,523,262,587,262,659,262,784,262,659,262,784,262,831,349,698,349,831,349,784,262,659,262,587,262,523};
int GSongSize = 31;
//int AllgoodMelody[] = {440,540,659,750,850}; 
//int GSongSize = 5;

void callback(char* topic, byte* payload, unsigned int length) {
    delay(100);
}

void setup()
{
    Serial.begin(9600);
//    while (!Serial.available()) {
//        Serial.println("Press any key to start.");
//        delay (1000);
//    }

    lcd.begin(16,2);
    lcd.setCursor(0,1);
    lcd.print("  - ALL GOOD -  ");

    pinMode(photoresistor,INPUT);
//    pinMode(power,OUTPUT); // The pin powering the photoresistor is output (sending out consistent power)
//    digitalWrite(power,HIGH); // Power for the photoresistor
    pinMode(redLED,OUTPUT);
    pinMode(greenLED,OUTPUT);
    pinMode(D7,OUTPUT);
    pinMode(button, INPUT_PULLUP); 
    pinMode(buzzer, OUTPUT);
    pinMode(yellowLED,OUTPUT);

// Need to initialize all outputs to start GREEN
    digitalWrite(greenLED,HIGH);
    //open the door
    myservo.attach(servoPin);  //Initialize the servo attached to pin D0
    myservo.write(90);        //set servo to open the door
    delay(500);                //delay to give the servo time to move to its position
    myservo.detach();          //detach the servo to prevent it from jittering
    client.connect("sparkclient");

}


// This wrapper is in charge of calling must be defined like this for the lib work
void dht_wrapper() {
    DHT.isrCallback();
}

void loop()
{
    DHT.acquireAndWait();

    temperature = DHT.getCelsius();
    humidity = DHT.getHumidity();
    lightlevel = analogRead(photoresistor);
    delay(100);

//Build the string for MQTT publish
    Time.zone(+11);
    mypayload = "Alfa,";
    mypayload += temperature;
    mypayload += ",";
    mypayload += humidity;
    mypayload += ",";
    mypayload += lightlevel;
    mypayload += ",";
    mypayload += Time.now();
    mypayload += ",";
    mypayload += Time.format(Time.now(), "%H:%M:%S");

    char *p = new char[mypayload.length() + 1];
    strcpy(p, mypayload.c_str());
    Serial.print ("payload is: ");
    Serial.println(p);

///////////////
//    Send publish message to MQTT instead of the serial port
    digitalWrite(D7,HIGH); //Blue light indicates we are attempting trainsmission
    if (!client.isConnected()) {
        Serial.println("It wasn't connected");
        client.connect("sparkclient");
    }
    if (client.isConnected()) {
        Serial.println("It was connected");
        client.loop();
        client.publish("mars/alfa",p);
    }
    else
    {
        Serial.println("connection failed");
    }
//For longer periods it's better to disconnect on every cycle
    client.disconnect();
    digitalWrite(D7,LOW);

//Build the string to publish on the LCD screen
    lcdmessage = temperature;
    lcdmessage += ".0C ";
    lcdmessage += humidity;
    lcdmessage += ".0% ";
    lcdmessage += lightlevel;

    char *l = new char[lcdmessage.length() + 1];
    strcpy(l, lcdmessage.c_str());
    Serial.print ("Message for the screen is: ");
    Serial.println(l);
//Update first line of the LCD
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print(l);

//First condition - is the override button ON?
    if(digitalRead(button) == LOW){ // If we push down on the push button
        override = 1;
        digitalWrite(yellowLED,HIGH);
//        digitalWrite(D7,HIGH);
    } else {
        override = 0;
        digitalWrite(yellowLED,LOW);
//        digitalWrite(D7,LOW);
    }

//Second - are we over any of the thresholds?
    if (lightlevel > maxlightlevel || temperature > maxtemperature || humidity > maxhumidity){
        overThreshold = 1;
    } else {
        overThreshold = 0;
    }

//Third - do we declare Alert or not
//  Another way of saying:  if ((overThreshold == 1 && override == 0) || (overThreshold == 0 && override ==1)) {
    if (overThreshold != override) {
        alertMode = 1;
    } else {
        alertMode = 0;
    }

//Fourth - if state has changed we need to do something
    if (previousAlert != alertMode) {
        needToAct = 1;
    }

//Fifth - Do it
//##############ALERT MODE ROUTINE
    if (needToAct == 1 && alertMode == 1) { //I could nest this one into the previous if statement
    
        digitalWrite(redLED,HIGH);
        digitalWrite(greenLED,LOW);
//        update second line in the LCD
        lcd.setCursor(0,1);
        lcd.print(">>>ALERT MODE<<<");
//        play Asturias
        for (int i = 0; i < SongSize; i++) {
            tone(D2,AlertMelody[i],900);
            delay(100);
        }

//        close the door
        myservo.attach(servoPin);
        myservo.write(0);
        delay(500);           //debounce and give servo time to move
        myservo.detach();

//##############NORMAL MODE ROUTINE
    } else if (needToAct == 1 && alertMode == 0){

        digitalWrite(redLED,LOW);
        digitalWrite(greenLED,HIGH);
//        play Grieg
        for (int i = 0; i < GSongSize; i++) {
            tone(D2,AllgoodMelody[i],1800);
            delay(150);
        }
//        update second line in the LCD = all good!
        lcd.setCursor(0,1);
        lcd.print("  - ALL GOOD -  ");

//        open the door
        myservo.attach(servoPin);
        myservo.write(90);
        delay(500);           //debounce and give servo time to move
        myservo.detach();
    }
//I can even put in the previous condition (there are two parts to it) alertMode != alertMode
    previousAlert = alertMode;
    needToAct = 0;
    delay(60000);
}


