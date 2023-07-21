#pragma once

#define FENSTER_HEADER
#include "fenster.h"

#include <stdint.h>

typedef struct fenster fenster_window;

typedef struct RenImage RenImage;
typedef struct RenFont RenFont;

typedef struct { uint8_t b, g, r, a; } RenColor;
typedef struct { int x, y, width, height; } RenRect;


void ren_init(fenster_window *win);
void ren_update_rects(RenRect *rects, int count);
void ren_set_clip_rect(RenRect rect);
void ren_get_size(int *x, int *y);

RenImage* ren_new_image(int width, int height);
void ren_free_image(RenImage *image);

RenFont* ren_load_font(const char *filename, float size);
void ren_free_font(RenFont *font);
void ren_set_font_tab_width(RenFont *font, int n);
int ren_get_font_tab_width(RenFont *font);
int ren_get_font_width(RenFont *font, const char *text);
int ren_get_font_height(RenFont *font);

void ren_fill_rect(RenRect rect, RenColor color);
void ren_draw_line(int x0, int y0, int x1, int y1,RenColor color);
void ren_draw_triangle( int x1, int y1, int x2, int y2, int x3, int y3, RenColor color);
void ren_draw_circle(int x, int y, int r, RenColor color);
void ren_draw_image(RenImage *image, RenRect *sub, int x, int y, RenColor color);
int ren_draw_text(RenFont *font, const char *text, int x, int y, RenColor color);
