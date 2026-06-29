#ifndef HYDROOS_GRAPHICS_H
#define HYDROOS_GRAPHICS_H

#include <stdint.h>

void graphics_init(uint32_t *fb_addr, uint64_t width, uint64_t height, uint64_t pitch);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect_filled(int x, int y, int w, int h, uint32_t color);
void draw_retro_button(int x, int y, int w, int h, uint32_t bg_color);
void draw_mac_window(int x, int y, int w, int h, const char *title);
void draw_mac_string(int x, int y, const char *str, uint32_t color);

#endif /* HYDROOS_GRAPHICS_H */
