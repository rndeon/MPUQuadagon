

#include <Stepper.h>

#include <Wire.h>
#include "MPU6050.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 4; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

AccelReading MPU;

int playerx=2;
int playery=2;

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
  delay(1000);
}
void movePlayer(double valuex, double valuey)
{
  valuex = valuex / 25 ;
  playerx =  floor(valuex);
  playerx += 4;
  valuey = (100- valuey) / 25  ;
  playery = floor(valuey);
 // playery += 4;
  Serial.print("x,y: ");
  Serial.print(valuex);
  Serial.print(" (");
  Serial.print(playerx);
  Serial.print(") , ");
  Serial.print(valuey);
  Serial.print(" (");
  Serial.print(playery);
  Serial.println(")");
}
void drawToDisplay(boolean gameOverScreen){
  matrix.fillScreen(LOW);
//  if(gameOverScreen && !previousGameOver){
// //  Serial.println("game over") ;
//   gameOverTime = millis();
//   previousGameOver = true;
//  }
//  if (previousGameOver){
//    if (millis() >= gameOverTime + gameOverFlashInterval){
//       gameOverFlash = ! gameOverFlash;
//       gameOverTime = millis();
//    }
//    drawScore();
//    matrix.drawRect(shell_num, shell_num,matrix.width() - 2*shell_num, matrix.height() - 2*shell_num , gameOverFlash);
//  } else{
//    //walls
//    matrix.drawRect(shell_num, shell_num,matrix.width() - 2*shell_num, matrix.height() - 2*shell_num , HIGH);
//    //hole
//    matrix.fillRect(holex,holey,matrix.width()/2,matrix.height()/2,LOW);  
//  }
    //player
    matrix.drawPixel(playerx,playery,HIGH);
  matrix.write(); // Send bitmap to display 
  
}
void loop()
{

  MPU.Read();
  // Print the raw acceleration values
  Serial.print(F(" accel x,y,z: "));
  Serial.print(MPU.GetX(), DEC);
//  Serial.print(accel_t_gyro.value.x_accel, DEC);
  Serial.print(F(", "));
  Serial.print(MPU.GetX(), DEC);
//  Serial.print(accel_t_gyro.value.y_accel, DEC);
  Serial.print(F(", "));
  Serial.print(MPU.GetX(), DEC);
  //  Serial.print(F(". "));
//  Serial.print(accel_t_gyro.value.z_accel, DEC);
  Serial.print(F(". "));
  
  ///controling lights
//  if(totaly > 500)
//  {
//    digitalWrite(led1, LOW);  
//    digitalWrite(led2, HIGH);
//  }else if (totaly < -500) {
//    digitalWrite(led1, HIGH);  
//    digitalWrite(led2, LOW);
//  } else {
//    digitalWrite(led1, HIGH);  
//    digitalWrite(led2, HIGH);
//  }
  Serial.print( MPU.GetTotalAccel());
  // The temperature sensor is -40 to +85 degrees Celsius.
  // It is a signed integer.
  // According to the datasheet: 
  //   340 per degrees Celsius, -512 at 35 degrees.
  // At 0 degrees: -512 - (340 * 35) = -12412
/*
  Serial.print(F("temperature: "));
  dT = ( (double) accel_t_gyro.value.temperature + 12412.0) / 340.0;
  Serial.print(dT, 3);
  Serial.print(F(" degrees Celsius"));
  Serial.print(F(". "));


  // Print the raw gyro values.

  Serial.print(F("gyro x,y,z : "));
  Serial.print(accel_t_gyro.value.x_gyro, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.y_gyro, DEC);
  Serial.print(F(", "));
  Serial.print(accel_t_gyro.value.z_gyro, DEC);
  Serial.print(F(", "));
*/  Serial.println(F(""));
  movePlayer(MPU.GetX(),MPU.GetY() );
  drawToDisplay(true);
  delay(10);
}
