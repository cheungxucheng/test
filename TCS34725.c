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

#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "TCS34725.h"

Sensor tcs = {TCS34725_ADDRESS, 0, TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X};

int set_integration(TCS34725IntegrationTime_t new_time) {
  // Write two bytes, the first specifying the address for the write and second being the new data
  uint8_t add[2] = {
    TCS34725_CMD_BIT | TCS34725_ATIME,
    new_time
  };

  if (write(tcs.fd, add, 2) == -1) {
    perror("Write err: ");
    return -1;
  }

  tcs.integ_time = new_time;
  return 0;
}

int set_gain(TCS34725Gain_t new_gain) {
  // Write two bytes, the first specifying the address for the write and second being the new data
  uint8_t add[2] = {
    TCS34725_CMD_BIT | TCS34725_CONTROL,
    new_gain
  };

  if (write(tcs.fd, add, 2) == -1) {
    perror("Write err: ");
    return -1;
  }

  tcs.gain = new_gain;
  return 0;
}

int enable(void) {
  // Write two bytes, the first specifying the address for the write and second setting 
  // the PON bit of the Enable Register
  uint8_t add[2] = {
    TCS34725_CMD_BIT | TCS34725_ENABLE,
    TCS34725_ENABLE_PON
  };

  if (write(tcs.fd, add, 2) == -1) {
    perror("Write err: ");
    return -1;
  }
  
  // Datasheet specifies at least 2.4 ms between PON and AEN, so we sleep for 3 ms
  if (usleep(3e3) == -1) {
    perror("sleep err: ");
    return -1;
  }

  // Since we already wrote the address earlier, just write to Enable Register
  // again enabling RGBC
  add[1] = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
  if (write(tcs.fd, add, 2) == -1) {
    perror("Write err: ");
    return -1;
  }

  return 0;
}

// sets up RGB sensor for use
int tcs_init(void) {
  int file;
  int adap_no = 1;
  char f_name[20];
  snprintf(f_name, 19, "/dev/i2c-%d", adap_no);

  // we open a fd to the i2c dev driver for i2c communication
  file = open(f_name, O_RDWR);
  if (file == -1) {
    perror("file open err: ");
    return -1;
  }
  tcs.fd = file;

  // Before anything, we need to set the receiving address
  if (ioctl(file, I2C_SLAVE, tcs.add) == -1) {
    perror("ioctl err: ");
    return -1;
  }

  if (enable() == -1) {
    printf("enable err");
    return -1;
  }
  // set integration and gain to default settings
  if (set_integration(TCS34725_INTEGRATIONTIME_50MS) == -1) {
    printf("integ err");
    return -1;
  }

  if (set_gain(TCS34725_GAIN_16X) == -1) {
    printf("set gain err");
    return -1;
  }
  
  return 0;
}

RGBC get_RGBC(void) {
  // In this instance, we specify AUTO_INC so that we can read all channels instead of one by one
  // Specify CDATAL since thats the first register from (0x14-0x1B, clear->r->g->b->c)
  uint8_t add = TCS34725_CMD_BIT | TCS34725_CMD_AUTO_INC | TCS34725_CDATAL;
  uint8_t data[8];
  RGBC some;
  // Set valid to 1 for error handling and set 0 when error handling
  some.valid = 1;
  if (write(tcs.fd, &add, 1) == -1) {
    perror("Write err: ");
    some.valid = 0;
    return some;
  }
  // read 8 bytes after specifying address
  // each color has a low and high byte, with RGBC that means 8 bytes
  if (read(tcs.fd, data, 8) == -1) {
    perror("read err:");
    some.valid = 0;
    return some;
  }

  // Because rgbc is each separated into a high and low byte
  // we need to cast and lshift the high byte 8 bits and then bitwise OR with the low byte
  // in order to get the full two bytes for each channel
  some.c = (uint16_t)data[1] << 8 | data[0];
  some.r = (uint16_t)data[3] << 8 | data[2];
  some.g = (uint16_t)data[5] << 8 | data[4];
  some.b = (uint16_t)data[7] << 8 | data[6];

  // based on the set integration time, we wait for a certain period of time.
  // this prevents us from checking old data while still integrating
  switch(tcs.integ_time) {
    case TCS34725_INTEGRATIONTIME_2_4MS:
      if (usleep(3e3) == -1) {
        perror("sleep err: ");
        some.valid = 0;
        return some;
      }
      break;
    case TCS34725_INTEGRATIONTIME_24MS:
      if (usleep(3e4) == -1) {
        perror("sleep err: ");
        some.valid = 0;
        return some;
      }
      break;
    case TCS34725_INTEGRATIONTIME_50MS:
      if (usleep(5e4) == -1) {
        perror("sleep err: ");
        some.valid = 0;
        return some;
      }
      break;
    case TCS34725_INTEGRATIONTIME_101MS:
      if (usleep(101e3) == -1) {
        perror("sleep err: ");
        some.valid = 0;
        return some;
      }
      break;
    case TCS34725_INTEGRATIONTIME_154MS:
      if (usleep(154e3) == -1) {
        perror("sleep err: ");
        some.valid = 0;
        return some;
      }
      break;
    case TCS34725_INTEGRATIONTIME_700MS:
      if (usleep(7e5) == -1) {
        perror("sleep err: ");
        some.valid = 0;
        return some;
      }
      break;
  }

  return some;
}

// closes i2c dev file descriptor
int tcs_close(void) {
  if (close(tcs.fd) == -1) {
    perror("close err");
    return -1;
  }
  return 0;
}

