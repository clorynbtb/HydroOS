#include <stdint.h>
#include <stddef.h>
#include "src/include/colors.h"
#include "src/include/graphics.h"
#include "src/include/elements.h"

static HydroDockerContainer docker_pool[5] = {
    {11, "Na", "alpine", 0, 0},
    {14, "Si", "python:alpine", 0, 0},
    {7, "N", "nginx:alpine", 0, 0},
    {0, "", "", 0, 0},
    {0, "", "", 0, 0},
};

static uint32_t *g_db_fb_address = NULL;
static uint64_t g_db_fb_width = 1024;
static uint64_t g_db_fb_height = 768;
static uint64_t g_db_fb_pitch = 4096;

static const uint8_t db_font_8x8[128][8] = {
    [32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    [65] = {0x1c, 0x36, 0x63, 0x7f, 0x63, 0x63, 0x63, 0x00},
    [66] = {0x7e, 0x63, 0x63, 0x7e, 0x63, 0x63, 0x7e, 0x00},
    [67] = {0x3e, 0x63, 0x60, 0x60, 0x60, 0x63, 0x3e, 0x00},
    [68] = {0x7c, 0x66, 0x63, 0x63, 0x63, 0x66, 0x7c, 0x00},
    [69] = {0x7f, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x7f, 0x00},
    [70] = {0x7f, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x60, 0x00},
    [71] = {0x3e, 0x63, 0x60, 0x6f, 0x63, 0x63, 0x3e, 0x00},
    [72] = {0x63, 0x63, 0x63, 0x7f, 0x63, 0x63, 0x63, 0x00},
    [73] = {0x3e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3e, 0x00},
    [76] = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7f, 0x00},
    [77] = {0x63, 0x77, 0x7f, 0x6b, 0x63, 0x63, 0x63, 0x00},
    [78] = {0x63, 0x63, 0x73, 0x7b, 0x6f, 0x67, 0x63, 0x00},
    [79] = {0x3e, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3e, 0x00},
    [83] = {0x3e, 0x63, 0x60, 0x3e, 0x03, 0x63, 0x3e, 0x00},
    [97] = {0x00, 0x00, 0x3c, 0x03, 0x3d, 0x63, 0x3d, 0x00},
    [98] = {0x60, 0x60, 0x7c, 0x63, 0x63, 0x63, 0x7c, 0x00},
    [99] = {0x00, 0x00, 0x3e, 0x60, 0x60, 0x63, 0x3e, 0x00},
    [100] = {0x03, 0x03, 0x3f, 0x63, 0x63, 0x63, 0x3f, 0x00},
    [101] = {0x00, 0x00, 0x3e, 0x63, 0x7f, 0x60, 0x3e, 0x00},
    [104] = {0x60, 0x60, 0x7c, 0x63, 0x63, 0x63, 0x63, 0x00},
    [105] = {0x0c, 0x00, 0x1c, 0x0c, 0x0c, 0x0c, 0x1e, 0x00},
    [110] = {0x00, 0x00, 0x7c, 0x63, 0x63, 0x63, 0x63, 0x00},
    [111] = {0x00, 0x00, 0x3e, 0x63, 0x63, 0x63, 0x3e, 0x00},
    [114] = {0x00, 0x00, 0x5e, 0x30, 0x30, 0x30, 0x30, 0x00},
    [116] = {0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x0c, 0x00},
    [117] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x63, 0x3d, 0x00},
    [118] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x34, 0x1c, 0x00},
    [121] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x3f, 0x03, 0x3e},
    [122] = {0x00, 0x00, 0x7f, 0x06, 0x0c, 0x18, 0x7f, 0x00},
};

static void db_draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= (int)g_db_fb_width || y < 0 || y >= (int)g_db_fb_height || g_db_fb_address == NULL) {
        return;
    }
    size_t index = y * (g_db_fb_pitch / 4) + x;
    g_db_fb_address[index] = color;
}

static void db_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 - x1 >= 0) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 - y1 >= 0) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        db_draw_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

static void db_draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            db_draw_pixel(px, py, color);
        }
    }
}

static void db_draw_rect_outline(int x, int y, int w, int h, uint32_t border_color, uint32_t bg_color) {
    db_draw_rect_filled(x, y, w, h, bg_color);
    db_draw_line(x, y, x + w - 1, y, border_color);
    db_draw_line(x + w - 1, y, x + w - 1, y + h - 1, border_color);
    db_draw_line(x, y, x, y + h - 1, border_color);
    db_draw_line(x, y + h - 1, x + w - 1, y + h - 1, border_color);
}

static void db_draw_char(int x, int y, char c, uint32_t color) {
    uint8_t glyph_idx = (uint8_t)c;
    if (glyph_idx >= 128) glyph_idx = '?';
    for (int row = 0; row < 8; row++) {
        uint8_t data = db_font_8x8[glyph_idx][row];
        for (int col = 0; col < 8; col++) {
            if ((data >> (7 - col)) & 1) {
                db_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

static void db_draw_string(int x, int y, const char *str, uint32_t color) {
    while (*str) {
        db_draw_char(x, y, *str, color);
        x += 8;
        str++;
    }
}

static void db_draw_retro_button(int x, int y, int w, int h, uint32_t bg_color) {
    db_draw_rect_filled(x, y, w, h, bg_color);
    db_draw_line(x, y, x + w - 1, y, COLOR_BORDER);
    db_draw_line(x, y + h - 1, x + w - 1, y + h - 1, COLOR_BEVEL_DARK);
    db_draw_line(x, y, x, y + h - 1, COLOR_BORDER);
    db_draw_line(x + w - 1, y, x + w - 1, y + h - 1, COLOR_BEVEL_DARK);
    db_draw_line(x + 1, y + 1, x + w - 2, y + 1, COLOR_BEVEL_LIGHT);
    db_draw_line(x + 1, y + 1, x + 1, y + h - 2, COLOR_BEVEL_LIGHT);
    db_draw_line(x + 1, y + h - 2, x + w - 2, y + h - 2, COLOR_BEVEL_DARK);
    db_draw_line(x + w - 2, y + 1, x + w - 2, y + h - 2, COLOR_BEVEL_DARK);
}

static void db_draw_mac_titlebar(int x, int y, int w, int h, const char *title) {
    db_draw_rect_filled(x, y, w, h, COLOR_LI_BG);
    for (int stripe = 0; stripe < 4; stripe++) {
        int sy = y + 5 + stripe * 3;
        db_draw_line(x + 26, sy, x + w - 8, sy, COLOR_TITLE_STRIPE);
    }

    db_draw_retro_button(x + 6, y + 6, 12, 12, COLOR_WHITE);

    int title_len = 0;
    while (title[title_len]) title_len++;
    int title_px = title_len * 8;
    int title_x = x + (w - title_px) / 2;
    int title_y = y + (h - 8) / 2;
    db_draw_rect_filled(title_x - 2, title_y - 1, title_px + 4, 10, COLOR_LI_BG);
    db_draw_string(title_x, title_y, title, COLOR_TEXT);
    db_draw_line(x, y + h - 1, x + w - 1, y + h - 1, COLOR_BEVEL_DARK);
}

static void db_draw_container_card(int x, int y, int w, int h, const HydroDockerContainer *container, uint32_t bg_color) {
    db_draw_retro_button(x, y, w, h, bg_color);
    db_draw_string(x + 16, y + 16, container->symbol, COLOR_TEXT);
    db_draw_string(x + 16, y + 42, container->docker_image, COLOR_TEXT);
    db_draw_string(x + 16, y + 60, "[ACTIVE]", COLOR_BEVEL_DARK);
}

static void db_draw_console_window(int x, int y, int w, int h, const char *title, const char *output) {
    db_draw_retro_button(x, y, w, h, COLOR_WHITE);
    db_draw_mac_titlebar(x + 2, y + 2, w - 4, 24, title);
    db_draw_rect_filled(x + 2, y + 28, w - 4, h - 30, COLOR_DESKTOP);
    db_draw_string(x + 12, y + 42, output, COLOR_TEXT);
}

void draw_classic_docker_manager(void) {
    if (g_db_fb_address == NULL) {
        return;
    }

    int x = 192, y = 144, w = 640, h = 480;
    db_draw_retro_button(x, y, w, h, COLOR_WHITE);
    db_draw_mac_titlebar(x + 2, y + 2, w - 4, 24, "🐳 ELEMENT 105: DUBNIUM (DOCKER CONTAINER MANAGER)");
    db_draw_rect_filled(x + 2, y + 28, w - 4, h - 30, COLOR_DESKTOP);

    int card_w = 176, card_h = 140;
    int card_x = x + 44;
    int card_y = y + 72;

    db_draw_container_card(card_x, card_y, card_w, card_h, &docker_pool[0], COLOR_HE_BG);
    db_draw_container_card(card_x + 212, card_y, card_w, card_h, &docker_pool[1], COLOR_LI_BG);
    db_draw_container_card(card_x + 424, card_y, card_w, card_h, &docker_pool[2], COLOR_DB_BG);

    db_draw_string(x + 48, y + 236, "Click a card to launch a vintage container.", COLOR_TEXT);
}

void launch_docker_element(int index) {
    if (g_db_fb_address == NULL || index < 0 || index >= 5) {
        return;
    }

    HydroDockerContainer *container = &docker_pool[index];
    if (container->atomic_number <= 0) {
        return;
    }

    container->is_active = 1;
    container->container_id = 0x105000u + (uint32_t)index;

    int x = 180, y = 140, w = 420, h = 260;
    db_draw_console_window(x, y, w, h, "CONTAINER: alpine [ACTIVE]", "[dubnium] pulling alpine:latest\n[dubnium] booting shell\n[dubnium] ready");

    db_draw_string(x + 12, y + 90, container->docker_image, COLOR_TEXT);
    db_draw_string(x + 12, y + 116, "atomic #", COLOR_TEXT);
    db_draw_string(x + 92, y + 116, container->symbol, COLOR_TEXT);
}

void db_init(uint32_t *framebuffer_addr, uint64_t width, uint64_t height, uint64_t pitch) {
    g_db_fb_address = framebuffer_addr;
    g_db_fb_width = width;
    g_db_fb_height = height;
    g_db_fb_pitch = pitch;
}
