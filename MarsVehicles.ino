// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"

/*
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 * Released into the public domain.
 *
 * Sample program showing how to read data from a PICC using a MFRC522 reader on the Arduino SPI interface.
 *----------------------------------------------------------------------------- empty_skull 
 * Aggiunti pin per arduino Mega
 * add pin configuration for arduino mega
 * http://mac86project.altervista.org/
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin			Pin
 *            Arduino Uno      Arduino Mega      SPARK			MFRC522 board
 * ---------------------------------------------------------------------------
 * Reset      9                5                 ANY (D2)		RST
 * SPI SS     10               53                ANY (A2)		SDA
 * SPI MOSI   11               51                A5				MOSI
 * SPI MISO   12               50                A4				MISO
 * SPI SCK    13               52                A3				SCK
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */


//#include <SPI.h>
#include "MFRC522/MFRC522.h"

#define SS_PIN SS
#define RST_PIN D2

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.

String CarID1 = "5c0affe5";
String CarID2 = "5c7e3200";
int Car1In = 1;
int Car2In = 1;
int RedLED = D4;
int GreenLED = D3;
int servoPin = D1;
// All the sensor pins are as per the library credits above
int DOit = 0;

Servo myservo;// create servo object using the built-in Particle Servo Library

char* mypayload = "";
byte server[] = { 192,168, 43,200 };
MQTT client(server, 1883, callback);
int blueLED = D7;


void setup() {
	Serial.begin(9600);	// Initialize serial communications with the PC
    delay(10000);
    Serial.print("Starting now ...");
	mfrc522.setSPIConfig();
    pinMode(RedLED, OUTPUT);
    pinMode(GreenLED, OUTPUT);
    digitalWrite(RedLED, HIGH);
    digitalWrite(GreenLED, LOW);

    myservo.attach(servoPin);  //Initialize the servo attached to pin D0
    myservo.write(0);        //set servo toclose  the door
    delay(300);                //delay to give the servo time to move to its position
    myservo.detach();          //detach the servo to prevent it from jittering

	mfrc522.PCD_Init();	// Init MFRC522 card
	Serial.println("Scan PICC to see UID and type...");

    pinMode(D7,OUTPUT);
    client.connect("sparkclient");

}

void loop() {
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
    	return;
    }

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
    	return;
	
    }

    String cardID = "";

    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
        cardID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        cardID += String(mfrc522.uid.uidByte[i], HEX);
    }

//    Serial.println(cardID);
    if (cardID.equals(CarID1)) {
    
        if (Car1In == 1) {
            mypayload = "Vehicle ONE left the building !!";
            Car1In = 0;

        } else {

            mypayload = "Vehicle ONE has returned from a trip !!";
            Car1In = 1;

        }
        DOit = 1;

    } else if (cardID.equals(CarID2)) {
        
        if (Car2In == 1) {

            mypayload = "Vehicle TWO left the building !!";
            Car2In = 0;

        } else {

            mypayload = "Vehicle TWO has returned from a trip !!";
            Car2In = 1;

        }

        DOit = 1;

    } else {
        
        mypayload = "ALERT!! A MARTIAN has attempted to break in with an unrecognised card!!!";

        for (int i = 0; i < 10; i++) 
        {
            digitalWrite(RedLED, LOW);
            delay (50);
            digitalWrite(RedLED, HIGH);
            delay (50);
        }

    }
   
    Serial.println(mypayload);

    if (DOit == 1 ) {
//        open the door
        digitalWrite(RedLED, LOW);
        digitalWrite(GreenLED, HIGH);
        myservo.attach(servoPin);
        myservo.write(90);
        delay(300);           //debounce and give servo time to move
        myservo.detach();
    	delay(2000);

//        close the door
        digitalWrite(RedLED, HIGH);
        digitalWrite(GreenLED, LOW);
        myservo.attach(servoPin);
        myservo.write(0);
        delay(300);           //debounce and give servo time to move
        myservo.detach();
 
        DOit = 0;
    }
 //    Send publish message to MQTT instead of the serial port
    digitalWrite(D7,HIGH); //Blue light indicates we are attempting trainsmission
    if (!client.isConnected()) {
//        Serial.println("It wasn't connected");
        client.connect("sparkclient");
    }
    if (client.isConnected()) {
//        Serial.println("It was connected");
        client.loop();
        client.publish("mars/vehicles",mypayload);
    }
    else
    {
        Serial.println("connection failed");
    }
//For longer periods it's better to disconnect on every cycle
    client.disconnect();
    digitalWrite(D7,LOW);

    mfrc522.PICC_HaltA();
}

void callback(char* topic, byte* payload, unsigned int length) {
    delay(100);
}
