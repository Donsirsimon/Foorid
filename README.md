# Foorid
School project for a internet of things  subject.

Project has 4 sets of traffic lights. 
Each traffic light set has one traffic light for cars and one for pedestrians.
Also each traffic light set has a button for pedestrians to request  green light for them.

Foor1 and foor2 are physical, made on a prototyping board and running on Nodemcu board and are connected to wifi. For them you need Arduino IDE software to make changes to code and upload to Nodemcu.
Foor3 and foor4 are virtual websites that contain a button for pedestrians and 2 traffic lights.

Seadistus.html is configuration page, where you can configure the following:
For each individual traffic light set you can choose between 3 modes: blinking yellow, timed switching and switching green for pedestrians with a button.
Also each trafficlight set has its own duration setting where you can choos for how long traffic light stays green. 

Addition to that configuration page has a green wave configuration below individual ones.
You can choose  delay time between traffic lights in seconds and mode.
Delay can be used 2 ways:
if delay is a positive number, green wave starts from  foor1>foor2>foor3>foor4
if delay is a negative number, green wave starts from the oppostite side...  foor4>foor3>foor2>foor1
Green wave modes:
1.you can set green wave to off, then each traffic light uses its own individual settings... mode and duration.
2.timed green wave: Trafficlights use delay to change state. If delay is positive..  foor 1 is master and changes its state as usual and upload it to database.
foor2 checks foor 1 state and if it has changed counts down delay time and matches its state to foor1.
Foor3 and foor 4 work the same way as foor 2. If delay is negative  foor4 is master and changes  its state using its duration, writes its state to database... and next trafficlight set checks it, if foor4 state has change it counts down set delay and matches foor4 state and so on.

To enable  traffic lights to communicate and get their configurational settings you need a database. 
In this project Firebase realtime database was used.

Database architecture is like this:

"foor": {
  "duration":800,
  "mode": 0,
  "state":0 
},
"foor2": {
  "duration":800,
  "mode": 0,
  "state":0 
},
"foor3": {
  "duration":800,
  "mode": 0,
  "state":0 
},
"foor4": {
  "duration":800,
  "mode": 0,
  "state":0 
},
"wave":{
  "button1state":0,
  "button2state":0,
  "button3state":0,
  "button4state":0,
  "delay": 300,
  "state": 1
}




