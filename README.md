
Automatic Headlight Control System
Team Members: Niju Khanal and Ellie Nguyen  

This project implements a car ignition and headlight control system that improves safety and ease of use for the driver. 
The ignition subsystem checks whether both the driver and passenger seats are occupied and whether both seatbelts are fastened before allowing the engine to start. 
If any of these conditions are not met, the system prevents ignition and displays clear messages explaining why the start was blocked.

Once the engine is running, it remains on even if a seatbelt is unfastened or a passenger leaves the vehicle, and the engine can be stopped by pressing the ignition button again. 
In addition, the system includes a headlight control subsystem that allows the driver to manually turn the headlights ON or OFF or select an AUTO mode. 
In AUTO mode, a light sensor is used to automatically turn the headlights on in low-light conditions and off in bright conditions, with built-in time delays to prevent rapid switching. 
This behavior is similar to automatic headlight systems used in modern vehicles.

This project was developed starting from our Project 1 repository. https://github.com/ellienguyen113/Car-Alarm-Upgrade
The ignition subsystem was modified to allow multiple start attempts, keep the engine running even if seatbelts are unfastened or passengers exit, 
and allow the engine to be stopped by pressing the ignition button again. 
Automatic/Manual headlight control was then added as part of Project 2.

Ignition (table 1) and Headlight (table 2) Subsystem

|                                    Specification                                   |                         Test Process                          |                            Results                             |
|:----------------------------------------------------------------------------------:|:-------------------------------------------------------------:|:--------------------------------------------------------------:|
| Display welcome message when driver sits down                                      | Turned ON the driver seat slide switch                        | Welcome message displayed once on the serial monitor           |
| Enable ignition only when both seats are occupied and both seatbelts are fastened  | Turned ON all seat and seatbelt slide switches                | Red ignition-enabled LED turned on                             |
| Inhibit ignition when safety conditions are not met                                | Pressed ignition button with one or more switches OFF         | Buzzer sounded and correct error messages were displayed       |
| Start engine when ignition is enabled                                              | Pressed ignition button when all conditions were met          | Green engine LED turned on and “Engine started” was printed    |
| Allow multiple ignition attempts after failure                                     | Pressed ignition button multiple times with unmet conditions  | System allowed repeated attempts and printed errors each time  |
| Keep engine running if passenger unfastens seatbelt or exits                       | Turned OFF passenger seat and seatbelt while engine running   | Engine continued running as expected                           |
| Stop engine when ignition button is pressed again                                  | Pressed ignition button while engine was running              | Engine stopped and green LED turned off                        |

|                        Specification                        |                      Test Process                       |                   Results                    |
|:-----------------------------------------------------------:|:-------------------------------------------------------:|:--------------------------------------------:|
| Turn headlights ON in ON mode                               | Rotated potentiometer to ON position                    | Both yellow low-beam LEDs turned on          |
| Turn headlights OFF in OFF mode                             | Rotated potentiometer to OFF position                   | Both yellow low-beam LEDs turned off         |
| Automatically turn headlights ON in low-light conditions    | Covered light sensor and waited 1 second                | Headlights turned on after the delay         |
| Automatically turn headlights OFF in bright conditions      | Shined phone flashlight on sensor and waited 2 seconds  | Headlights turned off after the delay        |
| Maintain previous headlight state between light thresholds  | Adjusted light level between dusk and daylight values   | Headlights remained in their previous state  |
| Turn off all headlights when engine is off                  | Turned engine off using ignition button                 | All headlights turned off automatically      |
| Stop engine when ignition button is pressed again           | Pressed ignition button while engine was running        | Engine stopped and green LED turned off      |
