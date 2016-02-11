#include "Arduino.h"
#include "MPU6050.h"
#include <Wire.h>
AccelReading::AccelReading(){
     
}
void AccelReading::Initialize(){
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
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
      {
        accelx[thisReading] = 0;
        accely[thisReading] = 0;
        accelz[thisReading] = 0;
      }
  // Clear the 'sleep' bit to start the sensor.
  MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);
    
}
void AccelReading::Read(){
    int error;
    double dT;
    accel_t_gyro_union accel_t_gyro;
    error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));
    uint8_t swap;
    #define SWAP(x,y) swap = x; x = y; y = swap

    SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
    SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
    SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
    SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
    SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
    SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
    SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);

    UpdateAverageX(accel_t_gyro.value.x_accel);
    UpdateAverageY(accel_t_gyro.value.y_accel);
    UpdateAverageZ(accel_t_gyro.value.z_accel);

}
double AccelReading::UpdateAverageX (double newvalue)
{
  // subtract the last reading:
  totalx = totalx - accelx[indexx];
  // read from the sensor:
  accelx[indexx] = newvalue/ numReadings;
  // add the reading to the total:
  totalx = totalx + accelx[indexx];
  // advance to the next position in the array:
  indexx = indexx + 1;

  // if we're at the end of the array...
  if (indexx >= numReadings)
    // ...wrap around to the beginning:
    indexx = 0;

  // calculate the average:
  return totalx;
}
double AccelReading::UpdateAverageY (double newvalue)
{
  // subtract the last reading:
  totaly = totaly - accely[indexy];
  // read from the sensor:
  accely[indexy] = newvalue/ numReadings;
  // add the reading to the total:
  totaly = totaly + accely[indexy];
  // advance to the next position in the array:
  indexy++;

  // if we're at the end of the array...
  if (indexy >= numReadings)
    // ...wrap around to the beginning:
    indexy = 0;

  // calculate the average:
  return totaly ;
}
double AccelReading::UpdateAverageZ (double newvalue)
{
  // subtract the last reading:
  totalz -= accelz[indexz];
  // read from the sensor:
  accelz[indexz] = newvalue/ numReadings;
  // add the reading to the total:
  totalz += accelz[indexz];
  // advance to the next position in the array:
  indexz++;

  // if we're at the end of the array...
  if (indexz >= numReadings)
   { // ...wrap around to the beginning:
    indexz = 0;
   }
  // calculate the average:
  return totalz ;
}
double AccelReading::GetX(){
    return totalx/maxx * 100;
}
double AccelReading::GetY(){
    return totaly/maxy * 100;
}
double AccelReading::GetZ(){
    return totalz/maxz * 100;
}
double AccelReading::GetTotalAccel(){
    return sqrt(pow(totalx/maxx,2) + pow(totaly/maxy,2) + pow(totalz/maxz,2))*100;
}
// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes 
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus. 
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read. 
// There is no function for a single byte.
//
int MPU6050_read(int start, uint8_t *buffer, int size)
{
  int i, n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
    return (-11);

  return (0);  // return : no error
}


// --------------------------------------------------------
// MPU6050_write
//
// This is a common function to write multiple bytes to an I2C device.
//
// If only a single register is written,
// use the function MPU_6050_write_reg().
//
// Parameters:
//   start : Start address, use a define for the register
//   pData : A pointer to the data to write.
//   size  : The number of bytes to write.
//
// If only a single register is written, a pointer
// to the data has to be used, and the size is
// a single byte:
//   int data = 0;        // the data to write
//   MPU6050_write (MPU6050_PWR_MGMT_1, &c, 1);
//
int MPU6050_write(int start, const uint8_t *pData, int size)
{
  int n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);

  return (0);         // return : no error
}

// --------------------------------------------------------
// MPU6050_write_reg
//
// An extra function to write a single register.
// It is just a wrapper around the MPU_6050_write()
// function, and it is only a convenient function
// to make it easier to write a single register.
//
int MPU6050_write_reg(int reg, uint8_t data)
{
  int error;

  error = MPU6050_write(reg, &data, 1);

  return (error);
}
