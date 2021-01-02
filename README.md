# garagedoor_mqtt_connector
Arduino ESP01 project to connect a dumb gateway or garagedoor controller to github's arachnetech/homebridge-mqttthing

This module enables a stupid garage door or gateway drive to be controlled by Homekit or other smart home solutions.
I used the following setup (but there's no limit to use other devices or projects)
* Homebridge on a raspberry pi
* mqttthing by arachnetech as aconnector to homebridge
* Platform IO plugin in Visual Studio code as development tool
* ESP01 and a small homemade pcb to hold a DC/DC converter and some interface transistors to connect to the door drive
* Schartec Swing gate opener (can be any other brand, just adapt your interface in case)

My state machine mirrors the internal state of the gate opener. As there are no built in limit switches or smart interface in the opener itself, I connected sensor inputs to the door switch and the remote control pcb, to be able to sample the manual commands. I added a reed contact as an end switch that signals a closed gate. This switch also resets the state machine, just in case it comes out of sync with the real state (due to power loss for example).

Additionally, the device is able to control the gate by homekit and siri.

