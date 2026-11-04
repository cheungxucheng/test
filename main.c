#include <TCS34725.h>
#include <gpiod.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    tcs_init();
    RGBC * temp = get_RGBC();
    printf(temp.r);
    free(temp);
    tcs_close();
    return 0;
}