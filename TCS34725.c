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
//
#include "assert.h"
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "TCS34725.h"
#include <gpiod.h>
#include <stdlib.h>

Sensor tcs = {0x29, 0, TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X};

int set_integration(TCS34725IntegrationTime_t new_time) {
  uint8_t add = TCS34725_CMD_BIT | TCS34725_ATIME | newTime;
  if (write(tcs.fd, &add, 1) == -1) {
    perror("Write err: ");
    return -1;
  }
  return 0;
}

int set_gain(TCS34725Gain_t new_gain) {
  uint8_t add = TCS34725_CMD_BIT | TCS34725_AGAIN | newGain;
  if (write(tcs.fd, &add, 1) == -1) {
    perror("Write err: ");
    return -1;
  }
  return 0;
}

int enable() {
  uint8_t add = TCS34725_CMD_BIT | TCS34725_ENABLE | TCS34725_ENABLE_PON;
  if (write(tcs.fd, &add, 1) == -1) {
    perror("Write err: ");
    return -1;
  }
  
  if (usleep(3e3) == -1) {
    perror("sleep err: ");
    return -1;
  }

  add = TCS34725_CMD_BIT | TCS34725_ENABLE | TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
  if (write(tcs.fd, &add, 1) == -1) {
    perror("Write err: ");
    return -1;
  }

  return 0;
}

int tcs_init() {
  int file;
  int adap_no = 1;
  char f_name[20];
  snprintf(f_name, 19, "/dev/i2c-%d", adap_no);
  file = open(f_name, O_RDWR);
  if (file == -1) {
    perror("file open err: ");
    return -1;
  }
  tcs.fd = file;

  if (ioctl(file, I2C_SLAVE, tcs.add) == -1) {
    perror("ioctl err: ");
    return -1;
  }

  if (enable() == -1) {
    printf("enable err");
    return -1;
  }

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

RGBC * get_RGBC() {
  uint8_t = TCS34725_CMD_BIT | TCS34725_CMD_AUTO_INC | TCS34725_CDATAL;
  uint8_t data[8];
  RGBC * some = malloc(sizeof(RGBC));
  if (write(tcs.fd, &add, 1) == -1) {
    perror("Write err: ");
    return NULL;
  }
  
  if (read(tcs.fd, data, 8) == -1) {
    perror("read err:");
    return NULL;
  }

  some->c = (uint16_t)data[1] | data[0];
  some->r = (uint16_t)data[3] | data[2];
  some->g = (uint16_t)data[5] | data[4];
  some->b = (uint16_t)data[7] | data[6];

  switch(tcs.integ_time) {
    case TCS34725_INTEGRATIONTIME_2_4MS:
      if (usleep(3e3) == -1) {
        perror("sleep err: ");
        return NULL;
      }
      break;
    case TCS34725_INTEGRATIONTIME_24MS:
      if (usleep(3e4) == -1) {
        perror("sleep err: ");
        return NULL;
      }
      break;
    case TCS34725_INTEGRATIONTIME_50MS:
      if (usleep(5e4) == -1) {
        perror("sleep err: ");
        return NULL;
      }
      break;
    case TCS34725_INTEGRATIONTIME_101MS:
      if (usleep(101e3) == -1) {
        perror("sleep err: ");
        return NULL;
      }
      break;
    case TCS34725_INTEGRATIONTIME_154MS:
      if (usleep(154e3) == -1) {
        perror("sleep err: ");
        return NULL;
      }
      break;
    case TCS34725_INTEGRATIONTIME_700MS:
      if (usleep(7e5) == -1) {
        perror("sleep err: ");
        return NULL;
      }
      break;
  }

  return some;
}

int tcs_close() {
  if (close(tcs.fd) == -1) {
    perror("close err");
    return -1;
  }
  return 0;
}

