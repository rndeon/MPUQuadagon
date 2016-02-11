MPUQuadagon
==========

A simple game for the Arduino + 8x8 LED display + MPU6050 . I used it to get familiar with how to make those work together.

## Installation
Place the folder containing this file (and all its files!) into your arduino sketchbook folder. Mine is at '~/sketchbook'.
Copy the contents of the 'libraries' folder into your Arduino installation's 'libraries' folder. This will be something like '~/arduino-1.0.5/libraries' or '~/sketchbook/libraries'. Your choice!

Then just open the .ino file in your Arduino IDE.

## Super Quadagon
This is based on an earlier, very similar game I made without the MPU that used buttons to move the player's pixel around. Check that out at https://github.com/rndeon/SuperQuadagon. 

The electronics are nearly the same. I just added the MPU onto the existing set up by attaching it to ground, +5V, SCL, and SDA.

## Use and Playing the Game
You control the position of the player pixel through the acceleration measured by the MPU. As the wall closes in on you from outside, make sure your pixel doesn't get hit, by keeping it in the unlit 'hole' of the wall. 

Easy mode is to use the acceleration due to gravity and just tilt the MPU (I have mine affixed so I'm tilting the whole game contraption). Hard mode is to physically yank the thing back and forth at the right times to cause the acceleration by hand. I have never cleared a single wall using this method: I wish you luck.

Each level is faster than the one before it. 

If you mess up, game over, and your score (number of walls avoided) will display in flashing dots around the outside of the display.

Press the 'Reset' button to try again.
