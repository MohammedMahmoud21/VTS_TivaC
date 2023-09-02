# VTS_TivaC
Real-Time Vehicle Tracking System
This project is a real-time vehicle tracking system that uses a GPS module to monitor the location of the car and a GSM module to send the data to a user-friendly web server. The system can also detect if the car is shut down and is moving, which means someone is trying to steal the car. The system also validates the GPS frame to ensure that the data inside the frame is valid with the last 1 second previous frame location. If the car is taking a turn, the rate of sending the data will increase. If the car is being stolen, an SMS will be sent to the user. The TM4c (Tiva C) microcontroller was used as the main MCU.

## Features
1.Tracks the location of the car in real time. <br />
2.Detect turns to draw smooth lines and curves on the web server. <br />
3.Detects if the car is being stolen.<br />
4.Sends an SMS message to the user if the car is being stolen.<br />
5.Easy to use and can be implemented with a variety of components.<br />
## Requirements
a)GPS module: The GPS module must be able to determine the latitude and longitude of the car with an accuracy of 10 meters.<br />
b)GSM module: The GSM module must be able to send and receive SMS messages and data packets.<br />
c)MCU: The MCU must be able to process the data from the GPS module and the GSM module, and send and receive data packets.<br />
d)Web server: The web server must be able to display the location of the car on a map.<br />
## Usage
1.Install the necessary components.<br />
2.Open the project on any IDE like CCS version 12.4.<br />
3.Load Tiva ware Libraries.<br />
4.Load the code on MCU.<br />
5.Open your terminal to watch system status as well as web server.<br />
