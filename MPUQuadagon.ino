

#include <Stepper.h>

#include <Wire.h>
#include "MPU6050.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 4; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 1;
int screensize=8;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

AccelReading MPU;

//#define fieldsize 32 //screensize*4
#define starscale 25 ///the stored values are divided by this number before translation to the 8x8 grid for displaying
//bool starfield[fieldsize][fieldsize];
//int star_pos_x=0;
//int star_pos_y=0;

int playerx=2;
int playery=2;
unsigned long prev_time=0;
//int v_x=0;
//int v_y=0;
//int max_vx=4*starscale;
//int max_vy=4*starscale;

///game vars
int holex=0;
int holey=0;
int playerPosition=0;
int buttonleft=6;
int buttonright=7;
int buttonReset=5;
long wallInterval = 500;
long wallIntervalLevelMod = 20;
long previousWallTime = 0;
long playerInterval = 100;
long playerIntervalLevelMod = 5;
long previousPlayerTime = 0;
int shell_num = 0;
boolean previousGameOver =false;
long gameOverTime = 0;
long gameOverFlashInterval = 100;
boolean gameOverFlash=HIGH;
uint8_t level = 0;
void randomize_hole(){
  holex=random(0,2) * 4;
  holey=random(0,2) * 4;
}

void setup()
{
  int error;
  uint8_t c;
  //pinMode(led1, OUTPUT);
  //pinMode(led2, OUTPUT);

  Serial.begin(9600);
  Serial.println(F("InvenSense MPU-6050"));
  Serial.println(F("June 2012"));

  // Initialize the 'Wire' class for the I2C-bus.
  Wire.begin();


  // default at power-up:
  //    Gyro at 250 degrees second
  //    Acceleration at 2g
  //    Clock source at internal 8MHz
  //    The device is in sleep mode.
  //

  error = MPU6050_read (MPU6050_WHO_AM_I, &c, 1);
  Serial.print(F("WHO_AM_I : "));
  Serial.print(c,HEX);
  Serial.print(F(", error = "));
  Serial.println(error,DEC);

  // According to the datasheet, the 'sleep' bit
  // should read a '1'.
  // That bit has to be cleared, since the sensor
  // is in sleep mode at power-up. 
  error = MPU6050_read (MPU6050_PWR_MGMT_1, &c, 1);
  Serial.print(F("PWR_MGMT_1 : "));
  Serial.print(c,HEX);
  Serial.print(F(", error = "));
  Serial.println(error,DEC);

  // initialize all the readings to 0:
  MPU.Initialize();
  // Clear the 'sleep' bit to start the sensor.
  MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);

///LED setup
   matrix.setIntensity(0);

// Adjust to your own needs
  matrix.setPosition(0, 0, 0); // The first display is at <0, 0>
//  matrix.setPosition(1, 1, 0); // The second display is at <1, 0>
//  matrix.setPosition(2, 2, 0); // The third display is at <2, 0>
//  matrix.setPosition(3, 3, 0); // And the last display is at <3, 0>
//  ...
  matrix.setRotation(0, 0);    // The first display is position upside down

  ///Starfield initialization
//  randomSeed(analogRead(0));
//  for(int xpos=0; xpos<fieldsize;xpos++)
//  {
//    for(int ypos=0;ypos<fieldsize;ypos++)
//    {
//      starfield[xpos][ypos] = random(10) > 6? HIGH : LOW;
//    }
//  }

  //game setup
  randomize_hole();
  ///buttons
  pinMode(buttonleft, INPUT);
  pinMode(buttonright, INPUT);    
  pinMode(buttonReset, INPUT);
  
  delay(1000);
  prev_time = millis();
}
void movePlayer(double valuex, double valuey)
{
  valuex = valuex / starscale ;
  playerx =  floor(valuex);
  playerx += 4;
  valuey = (100- valuey) / starscale  ;
  playery = floor(valuey);
 // playery += 4;

}
boolean playerIsInHole(){
  return (playerx >=holex && playerx < holex + matrix.width()/2) 
                 && (playery >=holey && playery < holey + matrix.height()/2) ;
}
void drawScore(){
  if (level <= 7) {
      matrix.drawFastHLine(0,0,level+1, !gameOverFlash);
    } else {      
      matrix.drawFastHLine(0,0,8, !gameOverFlash);
    }
    if (level > 7 && level <=14){
      matrix.drawFastVLine(7,1,level-7, !gameOverFlash);
    } else if ( level > 7){
      matrix.drawFastVLine(7,1,7, !gameOverFlash);
    }
    if (level > 14 && level <= 21){
      matrix.drawLine(21-level,7,6,7,!gameOverFlash);
    } else if (level > 14) {
      matrix.drawFastHLine(0,7,7, !gameOverFlash);
    }
    if (level > 21){
      matrix.drawLine(0,28-level,0,6,!gameOverFlash);
    }
}
unsigned frames_counter=0;
void drawToDisplay(boolean gameOverScreen){
  matrix.fillScreen(LOW);
  
  if(gameOverScreen && !previousGameOver){
 //  Serial.println("game over") ;
   gameOverTime = millis();
   previousGameOver = true;
  }
  if (previousGameOver){
    if (millis() >= gameOverTime + gameOverFlashInterval){
       gameOverFlash = ! gameOverFlash;
       gameOverTime = millis();
    }
    drawScore();
    matrix.drawRect(shell_num, shell_num,matrix.width() - 2*shell_num, matrix.height() - 2*shell_num , gameOverFlash);
  } else{
    //walls
    if(frames_counter ==1)
      {
        matrix.drawRect(shell_num, shell_num,matrix.width() - 2*shell_num, matrix.height() - 2*shell_num , HIGH);
        matrix.drawPixel(playerx,playery,HIGH);
        frames_counter =0;
      } else {
        // matrix.setIntensity(255);
        matrix.drawPixel(playerx,playery,HIGH);
        frames_counter++;
      }
    
    //hole
    matrix.fillRect(holex,holey,matrix.width()/2,matrix.height()/2,LOW);  
  }
  ///starfield
//  int xstartedge=floor(star_pos_x/starscale);
//  int ystartedge=floor(star_pos_y/starscale);
//  if(frames_counter ==1)
//  {
//    //matrix.setIntensity(100);
//    for(int xpos=0; xpos<screensize;xpos++)
//    {
//      for(int ypos=0; ypos<screensize;ypos++)
//      {
//          ///if screen's view goes off the edge, wrap around to beginning of starfield
//          if(xstartedge + xpos >= fieldsize){ xstartedge -= fieldsize; }
//          if(ystartedge + ypos >= fieldsize){ ystartedge -= fieldsize; }
//          matrix.drawPixel(xpos,ypos,starfield[xstartedge + xpos][ystartedge + ypos]);
//      }
//    }
//    matrix.drawPixel(playerx,playery,HIGH);
//    frames_counter =0;
//  } else {
//    // matrix.setIntensity(255);
//    matrix.drawPixel(playerx,playery,HIGH);
//    frames_counter++;
//  }
  
    //player
    matrix.drawPixel(playerx,playery,HIGH);
  matrix.write(); // Send bitmap to display 
  
}
///to get which 'shell' a point is in. For an 8x8 display, there are 4 concentric squares that can be drawn, and the innnermost one (comprised of 4 pixels) is labelled shell 3, the next one (12 pixels) is shell 2, etc until shell 0
int getShell(int x_coord, int y_coord){
  ///transform the coords so 0,1,2,3,4,5,6,7 now goes 0,1,2,3,3,2,1,0
  if (x_coord >= 4) {x_coord = 7 - x_coord;}
  if (y_coord >= 4) {y_coord = 7 - y_coord;}
  ///and the absolute value now equals shell number!
  ///so whichever coord has the lower value (outermost shell) decides the shell number
  return min( x_coord, y_coord); 
}
void loop()
{

  MPU.Read();
  long currentTime = millis();
  boolean gameOver = false;
  if (!previousGameOver){
    //Update walls position
    if ( currentTime - previousWallTime >= wallInterval - wallIntervalLevelMod*level)
    {
      previousWallTime = currentTime;
      if (shell_num <3 ){
        shell_num++;
      } 
      else if (level >= 24){
        gameOver = true;//you win!
      }else {
        shell_num = 0;
        randomize_hole();
        level++;
        //Serial.println(level);
      }
    }
    //update player position
//    if (currentTime - previousPlayerTime >=playerInterval - playerIntervalLevelMod*level)
//    {
       movePlayer(MPU.GetX(),MPU.GetY() );
       previousPlayerTime = currentTime;
//    }
    //check collision
    if(getShell(playerx,playery) == shell_num && !gameOver){
       gameOver = ! playerIsInHole();
    }
  
  }
  if (digitalRead(buttonReset) == HIGH){
    shell_num = 0;
    randomize_hole();
    previousGameOver = false;
    level=0;
  }
  drawToDisplay(gameOver);
//  unsigned long dt = millis() - prev_time;
  //update background position
//  int accel_x= MPU.GetX()/10;
//  int accel_y= ((100- MPU.GetY()) - 4 * starscale)/10;
//  if(MPU.GetX() >25) { v_x += accel_x;
//    }else if(MPU.GetX() < -25) { v_x += accel_x;
//    }//else{ v_x=0;}
//  if ( v_x > max_vx) v_x = max_vx;
//  if ( v_x < -max_vx) v_x = -max_vx;
//  star_pos_x += v_x/starscale;
//  if(star_pos_x >= fieldsize*starscale){ star_pos_x -=fieldsize*starscale; 
//    } else if (star_pos_x < 0){ star_pos_x +=fieldsize*starscale; }
//  if(MPU.GetY() >25) { v_y += accel_y;
//    }else if(MPU.GetY() <-25) { v_y += accel_y;
//    }//else{ v_y=0;};
//  if ( v_y > max_vy) v_y = max_vy;
//  if ( v_y < -max_vy) v_y = -max_vy;
//  star_pos_y += v_y/starscale;
//  if(star_pos_y >= fieldsize*starscale){ star_pos_y -=fieldsize*starscale; 
//    }else if(star_pos_y < 0){ star_pos_y +=fieldsize*starscale; }
//  prev_time = millis();
//  drawToDisplay(true);
  delay(10);
}
