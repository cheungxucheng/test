#include <TCS34725.h>
#include <gpiod.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (tcs_init() == -1) {
        printf("init err");
        return -1;
    }
    RGBC temp = get_RGBC();
    if (temp.valid == 0) {
        printf("get rgbc err");
        return -1;
    }
    printf("test %d %d %d %d", temp.r, temp.g, temp.b, temp.c);
    if (tcs_close() == -1) {
        printf("close err");
        return -1;
    }
    return 0;
}
