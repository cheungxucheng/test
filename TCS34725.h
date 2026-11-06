/**************************************************************
 * Class:: CSC-615-01 Fall 2026
 * Name::Maeve Mendelson-Reyes
 * Student ID::923376919
 * Github-Name::ssohbn
 * Project::Assignment 5 - rgb sensor
 *
 * File:: TCS34725.c
 *
 * Description:: driver for tcs34725 sensor
 *
 **************************************************************/
#include <stdint.h>

// I2C 7-bit address 0x29 for the TCS34725
#define TCS34725_ADDRESS          0x29

// All registers used in our lib implementation
#define TCS34725_CMD_BIT          0x80
#define TCS34725_CMD_AUTO_INC     0x20    // Allows for reading of multiple channels

#define TCS34725_ENABLE           0x00     
#define TCS34725_ENABLE_AEN       0x02    // Enables RGBC
#define TCS34725_ENABLE_PON       0x01    // Power ON. 2.4 ms before RGBC can be enabled after Power On

#define TCS34725_ATIME            0x01    // Integration time 

#define TCS34725_CONTROL          0x0F    // Set Gain

// Data for low and high bytes for Clear -> Red -> Green -> Blue channels
#define TCS34725_CDATAL           0x14    
#define TCS34725_CDATAH           0x15
#define TCS34725_RDATAL           0x16    
#define TCS34725_RDATAH           0x17
#define TCS34725_GDATAL           0x18    
#define TCS34725_GDATAH           0x19
#define TCS34725_BDATAL           0x1A    
#define TCS34725_BDATAH           0x1B

// Fixed integration times
typedef enum
{
  TCS34725_INTEGRATIONTIME_2_4MS  = 0xFF,   
  TCS34725_INTEGRATIONTIME_24MS   = 0xF6,   
  TCS34725_INTEGRATIONTIME_50MS   = 0xEB,   
  TCS34725_INTEGRATIONTIME_101MS  = 0xD5,  
  TCS34725_INTEGRATIONTIME_154MS  = 0xC0,   
  TCS34725_INTEGRATIONTIME_700MS  = 0x00    
}
TCS34725IntegrationTime_t;

typedef enum
{
  TCS34725_GAIN_1X                = 0x00,   
  TCS34725_GAIN_4X                = 0x01,   
  TCS34725_GAIN_16X               = 0x02,   
  TCS34725_GAIN_60X               = 0x03    
}
TCS34725Gain_t;

// Valid int for error handling on return
typedef struct RGBC {
  uint16_t r;
  uint16_t g;
  uint16_t b;
  uint16_t c;
  int valid;
} RGBC;

// Group all information specific to sensor in a struct
typedef struct {
  uint8_t add;
  int fd;
  TCS34725IntegrationTime_t integ_time;
  TCS34725Gain_t gain;
} Sensor;


int set_integration(TCS34725IntegrationTime_t new_time);

int set_gain(TCS34725Gain_t new_gain);

int enable(void);

// Sets up the RGB Sensor using i2c driver. Must be called before any other functions are called
int tcs_init(void);

// Returns RGBC struct with current 2 byte RGBC values. Check valid int for error handling.
RGBC get_RGBC(void);

// Closes the file descriptor for i2c driver. Must call before program end.
int tcs_close(void);
