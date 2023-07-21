#pragma once

#include <stdbool.h>
#include "renderer.h"

void rencache_show_debug(bool enable);
void rencache_free_font(RenFont *font);
void rencache_set_clip_rect(RenRect rect);
void rencache_draw_rect(RenRect rect, RenColor color);
void rencache_draw_line(int x0, int y0, int x1, int y1,RenColor color);
void rencache_draw_circle(int x, int y,int r, RenColor color);
void rencache_draw_triangle( int x1, int y1, int x2, int y2, int x3, int y3, RenColor color);
int  rencache_draw_text(RenFont *font, const char *text, int x, int y, RenColor color);
void rencache_invalidate(void);
void rencache_begin_frame(void);
void rencache_end_frame(void);

