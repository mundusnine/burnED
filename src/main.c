#include <stdio.h>
#include <string.h>
#include "fenster.h"
#include "renderer.h"
#include "application.h"

void transform_to_4x3(int width, int height, int *out_width, int *out_height) {
    
    double width_ratio = width / 4.0;
    int h = (int)(width_ratio * 3 + 0.5);

    if (h <= height) {
        *out_width = width;
        *out_height = height;
    } else {
        double height_ratio = height / 3.0;
        *out_width = (int)(height_ratio * 4 + 0.5);
        *out_height = height;
    }
}

int main(int argc,char** argv){
    int w,h;
    fenster_screen_size(&w,&h);
    w = w* 0.8f;
    h = h* 0.8f;
    transform_to_4x3(w,h,&w,&h);
    uint32_t* buf = (uint32_t *)malloc(sizeof(uint32_t) * w * h);
    memset(buf,0,sizeof(uint32_t) * w * h);
    fenster_window f = { .title = "burnED", .width = w, .height = h, .buf = buf };
    fenster_open(&f);
    ren_init(&f);
    application_init();
    while (fenster_loop(&f) == 0) {
        application_update(&f);
    }
    
    fenster_close(&f);
    return 0;
}