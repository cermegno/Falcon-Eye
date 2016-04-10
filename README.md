# Falcon-Eye
## History
This was a personal project I undertook to get initiated in the IOT world as well as Python coding, mobile app development
and a few other things ... Happy for you to reuse what you can. I take no responsabilities for any damages.

## Purpose
I created a fictious story so that I have a specific need to solve. This helped me to define more specific behaviours in the code:
 - We are in the future. About to colonize Mars.
 - Mars environment is volatile and extreme due to thin atmosphere
 - This project will monitor the environment variables and act if thresholds are violated (lights, sound alarms, close the door)
 - The project will also keep track of vehicles leaving and returning to the base
 - There will be three bases (Alfa, Beta, Gamma) in the future. Parts of the code make that asumption

## Hardware
Four main hardware components:
 - 2 x Photon controllers (by Particle)
     + One with with
          o Sensors (temperature/humidity sensor, light sensor, and override switch)
          o Actuators/outputs (red LED, green LED, yellow LED, a passive buzzer, a servo, a 16x2 LCD with external potentiometer)
      + Another one with
          o Sensors (RFID)
          o Actuators (red LED, green LED, servo)
 - Raspberry Pi wireless enabled
 - A laptop running windows
 
## Software
 From a software component we have
 - Sketches for the Photon controllers reading all the sensors and acting on thresholds. They also publish data through MQTT
 - MQTT (Message Queuing for Telemetry Transport) for data transfer from controllers to laptop. In particular I run Mosquitto
 on the Raspberry Pi (mosquitto.org)
 - MQTT client running on the laptop listening to two topics (environment and vehicle activity) and creating two log files
 - Python code running on the laptop to:
    + create a web page with a map of Mars showing the colour-coded status of each base. This has some Javascript
    + create some other smaller, cleaner files to be used by the mobile app
    + creates a page with historical and another one with the vehicle access log
    + it sends alerts via Twitter (https://twitter.com/MarsAlerts)
 - Nginix is the web server I used on my laptop
 - Splunk light version is also checking the environment log file and using a dashboard with timelines and basic stats
 - A mobile app created with MIT app inventor

## Files 
 I have uploaded the following files:
  - Photon sketch for Environment monitoring
  - Photon sketch for vehicle tracking
  - Python script to process the log file
  - Screenshots of the AppInventor
  
 Enjoy !!!
 Alberto
 Monday 7th December 2015
