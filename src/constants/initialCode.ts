import { CodeFile } from '../types';

export const INITIAL_CODE_FILES: CodeFile[] = [
  {
    name: 'kernel.c',
    path: 'kernel/kernel.c',
    language: 'c',
    description: 'Hydrogen Core (H) - Limine bootloader handoff, framebuffer query, splash screen logic',
    content: `/**
 * @file kernel.c
 * @brief Hydrogen Core (H) - Entry point of HydroOS
 * Compiled with: clang -target x86_64-none-elf -O2
 */

#include <stdint.h>
#include <stddef.h>
#include "limine.h"

// Request Limine's framebuffer feature
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Simple delay function for real loading simulation
void delay(uint64_t count) {
    for (volatile uint64_t i = 0; i < count * 50000; i++) {
        __asm__("nop");
    }
}

// Draw a single pixel on the screen (x86_64 framebuffer)
void draw_pixel(struct limine_framebuffer *fb, int x, int y, uint32_t color) {
    uint32_t *fb_ptr = (uint32_t *)fb->address;
    size_t index = y * (fb->pitch / 4) + x;
    if (x >= 0 && x < fb->width && y >= 0 && y < fb->height) {
        fb_ptr[index] = color;
    }
}

// Draw the Hydrogen Logo 'H' using pixel coordinates
void draw_h_logo(struct limine_framebuffer *fb, int cx, int cy, int size, uint32_t color) {
    int thickness = size / 4;
    // Left bar
    for (int y = cy - size/2; y < cy + size/2; y++) {
        for (int x = cx - size/2; x < cx - size/2 + thickness; x++) {
            draw_pixel(fb, x, y, color);
        }
    }
    // Right bar
    for (int y = cy - size/2; y < cy + size/2; y++) {
        for (int x = cx + size/2 - thickness; x < cx + size/2; x++) {
            draw_pixel(fb, x, y, color);
        }
    }
    // Middle bridge
    for (int y = cy - thickness/2; y < cy + thickness/2; y++) {
        for (int x = cx - size/2 + thickness; x < cx + size/2 - thickness; x++) {
            draw_pixel(fb, x, y, color);
        }
    }
}

// Draw splash screen progress bar
void draw_progress_bar(struct limine_framebuffer *fb, int pct) {
    int screen_w = fb->width;
    int screen_h = fb->height;
    
    int bar_w = 400;
    int bar_h = 10;
    int bar_x = (screen_w - bar_w) / 2;
    int bar_y = screen_h / 2 + 100;
    
    // Border
    for (int x = bar_x - 2; x < bar_x + bar_w + 2; x++) {
        draw_pixel(fb, x, bar_y - 2, 0x333333);
        draw_pixel(fb, x, bar_y + bar_h + 2, 0x333333);
    }
    for (int y = bar_y - 2; y < bar_y + bar_h + 2; y++) {
        draw_pixel(fb, bar_x - 2, y, 0x333333);
        draw_pixel(fb, bar_x + bar_w + 2, y, 0x333333);
    }
    
    // Filled bar
    int filled_w = (bar_w * pct) / 100;
    for (int y = bar_y; y < bar_y + bar_h; y++) {
        for (int x = bar_x; x < bar_x + filled_w; x++) {
            draw_pixel(fb, x, y, 0x10B981); // Emerald loading bar
        }
    }
}

// Kernel main entry point called by Limine
void _start(void) {
    // Ensure the bootloader provided a framebuffer
    if (framebuffer_request.response == NULL || framebuffer_request.response->count < 1) {
        // Halt CPU if no graphics support
        for (;;) { __asm__("hlt"); }
    }

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    
    // Clear screen to deep dark elegant background (0x0A0A0A)
    for (size_t y = 0; y < fb->height; y++) {
        for (size_t x = 0; x < fb->width; x++) {
            draw_pixel(fb, x, y, 0x0A0A0A);
        }
    }

    // Splash Screen Loop
    for (int pct = 0; pct <= 100; pct += 2) {
        draw_h_logo(fb, fb->width / 2, fb->height / 2 - 40, 120, 0xFFFFFF);
        draw_progress_bar(fb, pct);
        delay(15); // Dynamic delay
    }

    // Initialize UI Subsystem
    oxygen_ui_init(fb);

    // Load Docker Manager
    dubnium_docker_init();

    // Handoff to main system loop
    for (;;) {
        __asm__("hlt");
    }
}
`
  },
  {
    name: 'oxygen.c',
    path: 'ui/oxygen.c',
    language: 'c',
    description: 'Oxygen UI (O) - macOS Top Menu Bar, Dock container, 8x8 font engine, and window rendering',
    content: `/**
 * @file oxygen.c
 * @brief Oxygen UI (O) - Graphics rendering with a custom 8x8 bitmap font
 * Compiled with: clang -target x86_64-none-elf -ffreestanding
 */

#include <stdint.h>
#include "limine.h"

// Simplified 8x8 bitmap font representation (binary mask array for ASCII)
// Example glyph map for uppercase H and letters
static const uint8_t font_8x8[256][8] = {
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['A'] = {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42},
    ['D'] = {0x7C, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x7C},
    ['H'] = {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x66},
    ['O'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['S'] = {0x3E, 0x40, 0x40, 0x3C, 0x02, 0x02, 0x02, 0x7C},
    ['y'] = {0x00, 0x44, 0x44, 0x44, 0x3C, 0x04, 0x04, 0x38},
    ['d'] = {0x04, 0x04, 0x3C, 0x44, 0x44, 0x44, 0x44, 0x38},
    ['r'] = {0x00, 0x00, 0x2E, 0x32, 0x20, 0x20, 0x20, 0x20},
    ['o'] = {0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C}
    // Full mapping of 256 glyphs loaded in HydroOS memory...
};

struct DrawContext {
    struct limine_framebuffer *fb;
    uint32_t text_color;
    uint32_t bg_color;
};

// Render a character glyph to framebuffer at coordinates
void draw_char(struct limine_framebuffer *fb, int x, int y, char c, uint32_t color) {
    uint8_t glyph[8];
    // Fallback if character font isn't explicitly defined
    for (int i = 0; i < 8; i++) {
        glyph[i] = font_8x8[(uint8_t)c][i] ? font_8x8[(uint8_t)c][i] : 0x55;
    }
    
    for (int row = 0; row < 8; row++) {
        uint8_t data = font_8x8[(uint8_t)c][row];
        for (int col = 0; col < 8; col++) {
            if ((data >> (7 - col)) & 1) {
                draw_pixel(fb, x + col, y + row, color);
            }
        }
    }
}

// Print a string of text
void draw_string(struct limine_framebuffer *fb, int x, int y, const char *str, uint32_t color) {
    while (*str) {
        draw_char(fb, x, y, *str, color);
        x += 8; // Move cursor right by 8 pixels
        str++;
    }
}

// Draw a filled rounded rect container for Windows and Dock
void draw_rect_rounded(struct limine_framebuffer *fb, int x, int y, int w, int h, int r, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            // Check corners
            int dx = px - x;
            int dy = py - y;
            if (dx < r && dy < r && (r-dx)*(r-dx) + (r-dy)*(r-dy) > r*r) continue; // Top-left
            if (dx >= w-r && dy < r && (px-(x+w-r))*(px-(x+w-r)) + (r-dy)*(r-dy) > r*r) continue; // Top-right
            if (dx < r && dy >= h-r && (r-dx)*(r-dx) + (py-(y+h-r))*(py-(y+h-r)) > r*r) continue; // Bottom-left
            if (dx >= w-r && dy >= h-r && (px-(x+w-r))*(px-(x+w-r)) + (py-(y+h-r))*(py-(y+h-r)) > r*r) continue; // Bottom-right
            
            draw_pixel(fb, px, py, color);
        }
    }
}

// Render the macOS Style Top Menu Bar (height 28px)
void render_top_menu(struct limine_framebuffer *fb) {
    // Fill top bar with transparent carbon zinc
    draw_rect_rounded(fb, 0, 0, fb->width, 28, 0, 0x1E1E1E);
    
    // Draw logo, menu options, and right system stats
    draw_string(fb, 16, 10, "H", 0xFFFFFF);
    draw_string(fb, 36, 10, "HydroOS", 0xFFFFFF);
    draw_string(fb, 110, 10, "File", 0xA0A0A0);
    draw_string(fb, 160, 10, "Edit", 0xA0A0A0);
    draw_string(fb, 210, 10, "Docker", 0xFFFFFF);
    draw_string(fb, 280, 10, "Window", 0xA0A0A0);
    
    // Right side stats
    draw_string(fb, fb->width - 240, 10, "[LLD/CLANG]", 0x10B981);
    draw_string(fb, fb->width - 120, 10, "85% CPU", 0xD1D5DB);
}

// Render macOS Dock with app icons (He, Li, Db) at bottom
void render_dock(struct limine_framebuffer *fb) {
    int dock_w = 320;
    int dock_h = 60;
    int dock_x = (fb->width - dock_w) / 2;
    int dock_y = fb->height - dock_h - 16;
    
    // Blurred dock container
    draw_rect_rounded(fb, dock_x, dock_y, dock_w, dock_h, 16, 0x27272A);
    
    // Draw Icons (simplistic box layout mapping letters)
    draw_rect_rounded(fb, dock_x + 20, dock_y + 10, 40, 40, 8, 0x3F3F46);
    draw_string(fb, dock_x + 32, dock_y + 26, "He", 0xFFFFFF);
    
    draw_rect_rounded(fb, dock_x + 80, dock_y + 10, 40, 40, 8, 0x3F3F46);
    draw_string(fb, dock_x + 92, dock_y + 26, "Li", 0xFFFFFF);
    
    // Db Docker Manager Active Icon
    draw_rect_rounded(fb, dock_x + 140, dock_y + 10, 40, 40, 8, 0xF4F4F5);
    draw_string(fb, dock_x + 152, dock_y + 26, "Db", 0x000000);
}

void oxygen_ui_init(struct limine_framebuffer *fb) {
    render_top_menu(fb);
    render_dock(fb);
}
`
  },
  {
    name: 'db.c',
    path: 'docker/db.c',
    language: 'c',
    description: 'Dubnium App Repository (Db) - Periodic Table of HydroOS Applications with chemistry-themed naming',
    content: `/**
 * @file db.c
 * @brief Dubnium App Repository (Db) - Periodic Table of HydroOS Applications
 * Every app is an ELEMENT with atomic number, symbol, and chemical identity.
 * Compiled with: clang -target x86_64-none-elf -fno-stack-protector
 */

#include <stdint.h>
#include <stddef.h>

#define MAX_ELEMENTS 20
#define SYMBOL_LEN 4
#define NAME_LEN 16

/* =============================================================================
 * HYDROELEMENT APP STRUCTURE - Chemistry-themed Application Definition
 * Every application MUST be a valid chemical element from the Periodic Table.
 * ============================================================================= */
typedef struct {
    int atomic_number;      // Số hiệu nguyên tử (Ví dụ: 6, 5, 22, 79, 26)
    char symbol[SYMBOL_LEN]; // Ký hiệu nguyên tố (Ví dụ: "C", "B", "Ti", "Au")
    char full_name[NAME_LEN]; // Tên đầy đủ (Ví dụ: "Carbon", "Boron", "Titanium")
    uint32_t bg_color;      // Màu nền Vintage Minimalist của cửa sổ
    uint32_t text_color;    // Màu chữ Espresso nâu đậm
    int is_running;         // Trạng thái hoạt động (1 = running, 0 = stopped)
    uint64_t cpu_cycles;    // CPU cycles consumed
} HydroElementApp;

/* =============================================================================
 * VINTAGE COLOR PALETTE - Notion Warm Manuscript Theme
 * ============================================================================= */
#define COLOR_HE_BG      0xFFF5EFE6  /* Trắng kem ấm - Helium/Electron background */
#define COLOR_LI_BG      0xFFEFEBE9  /* Nâu Latte nhạt - Lithium panels */
#define COLOR_DB_BG      0xFFE0D4C3  /* Nâu gỗ sáng - Dubnium containers */
#define COLOR_TEXT       0xFF3E2723  /* Espresso nâu đậm */
#define COLOR_BORDER     0xFFD7CCC8  /* Nâu tro mảnh 1px */
#define COLOR_WHITE      0xFFFFFFFF  /* Trắng ngà */
#define COLOR_EMERALD    0xFF10B981  /* Emerald accent */

/* =============================================================================
 * PERIODIC TABLE OF HYDROOS APPLICATIONS
 * Mỗi nguyên tố là một ứng dụng với chức năng riêng biệt.
 * ============================================================================= */
static HydroElementApp periodic_table_apps[MAX_ELEMENTS] = {
    /* Element 2: [He] Helium - System Dashboard/Welcome Screen */
    { 2,  "He", "Helium",      COLOR_WHITE,   COLOR_TEXT, 1, 0 },

    /* Element 3: [Li] Lithium - Process Manager */
    { 3,  "Li", "Lithium",     COLOR_LI_BG,   COLOR_TEXT, 0, 0 },

    /* Element 5: [B] Boron - CALCULATOR (Máy tính xử lý số liệu) */
    { 5,  "B",  "Boron",       COLOR_LI_BG,   COLOR_TEXT, 0, 0 },

    /* Element 6: [C] Carbon - TEXT EDITOR / NOTES (Sổ tay ghi chú) */
    { 6,  "C",  "Carbon",      COLOR_HE_BG,   COLOR_TEXT, 1, 0 },

    /* Element 14: [Si] Silicon - System Configuration Panel */
    { 14, "Si", "Silicon",     COLOR_DB_BG,   COLOR_TEXT, 0, 0 },

    /* Element 22: [Ti] Titanium - SYSTEM CLOCK (Đồng hồ hệ thống) */
    { 22, "Ti", "Titanium",    COLOR_DB_BG,   COLOR_TEXT, 1, 0 },

    /* Element 26: [Fe] Iron - FILE MANAGER (Quản lý file hệ thống) */
    { 26, "Fe", "Iron",        COLOR_LI_BG,   COLOR_TEXT, 0, 0 },

    /* Element 29: [Cu] Copper - Network Browser */
    { 29, "Cu", "Copper",      COLOR_HE_BG,   COLOR_TEXT, 0, 0 },

    /* Element 47: [Ag] Silver - Terminal Console */
    { 47, "Ag", "Silver",     0xFFF5F5F5,    COLOR_TEXT, 1, 0 },

    /* Element 79: [Au] Gold - DIGITAL WALLET (Ví điện tử) */
    { 79, "Au", "Gold",        0xFFFFF8E1,    COLOR_TEXT, 0, 0 },

    /* Element 80: [Hg] Mercury - Message/Chat App */
    { 80, "Hg", "Mercury",     COLOR_HE_BG,   COLOR_TEXT, 0, 0 },

    /* Element 105: [Db] Dubnium - DOCKER APP REPOSITORY (This Manager!) */
    { 105, "Db", "Dubnium",    COLOR_DB_BG,   COLOR_TEXT, 1, 0 },
};

static int active_element_count = 12;

/* =============================================================================
 * DRAW PERIODIC TABLE APP STORE - Notion Vintage Minimalist Style
 * Hiển thị bảng tuần hoàn ứng dụng khi mở Dubnium [Db]
 * ============================================================================= */
void draw_element_box(struct limine_framebuffer *fb, int x, int y, HydroElementApp *app) {
    int box_w = 64;
    int box_h = 72;

    /* Vẽ khung ô ứng dụng màu nền Vintage */
    draw_rect_filled(fb, x, y, box_w, box_h, app->bg_color);
    draw_rect_outline(fb, x, y, box_w, box_h, COLOR_BORDER, app->bg_color);

    /* Số hiệu nguyên tử nhỏ góc trái trên (font 8x8) */
    char atomic_str[4];
    atomic_str[0] = (app->atomic_number / 100) + '0';
    atomic_str[1] = ((app->atomic_number % 100) / 10) + '0';
    atomic_str[2] = (app->atomic_number % 10) + '0';
    atomic_str[3] = '\\0';
    draw_string_small(fb, x + 3, y + 2, atomic_str, COLOR_TEXT);

    /* Ký hiệu nguyên tố viết HOA LỚN ở giữa ô */
    draw_string_large(fb, x + (box_w - (my_strlen(app->symbol) * 12)) / 2, y + 26, app->symbol, COLOR_TEXT);

    /* Tên đầy đủ nguyên tố bên dưới */
    draw_string_small(fb, x + (box_w - (my_strlen(app->full_name) * 4)) / 2, y + 52, app->full_name, COLOR_TEXT);

    /* Running indicator dot (emerald) */
    if (app->is_running) {
        draw_rect_filled(fb, x + box_w - 8, y + 2, 6, 6, COLOR_EMERALD);
    }
}

/* Vẽ toàn bộ bảng tuần hoàn ứng dụng Dubnium */
void draw_db_app_store(struct limine_framebuffer *fb) {
    int start_x = 80;
    int start_y = 60;
    int cols = 6;
    int gap = 8;
    int box_w = 64;
    int box_h = 72;

    /* Header: DUBNIUM PERIODIC TABLE APP STORE */
    draw_string(fb, start_x, start_y - 24, "[Db] ELEMENT APP REPOSITORY - PERIODIC TABLE", COLOR_TEXT);

    /* Render từng element box theo grid layout */
    int idx = 0;
    for (int row = 0; row < (active_element_count + cols - 1) / cols; row++) {
        for (int col = 0; col < cols && idx < active_element_count; col++) {
            int x = start_x + col * (box_w + gap);
            int y = start_y + row * (box_h + gap);
            draw_element_box(fb, x, y, &periodic_table_apps[idx]);
            idx++;
        }
    }
}

/* =============================================================================
 * DUBNIUM INITIALIZATION - Start with default running elements
 * ============================================================================= */
void dubnium_docker_init(void) {
    /* He (Helium) - Dashboard always running */
    periodic_table_apps[0].is_running = 1;
    /* C (Carbon) - Text Editor default */
    periodic_table_apps[3].is_running = 1;
    /* Ti (Titanium) - Clock always running */
    periodic_table_apps[5].is_running = 1;
    /* Ag (Silver) - Terminal always running */
    periodic_table_apps[8].is_running = 1;
    /* Db (Dubnium) - This manager always running */
    periodic_table_apps[11].is_running = 1;
}

/* Toggle element app state */
void toggle_element_app(int atomic_num) {
    for (int i = 0; i < active_element_count; i++) {
        if (periodic_table_apps[i].atomic_number == atomic_num) {
            periodic_table_apps[i].is_running = !periodic_table_apps[i].is_running;
            return;
        }
    }
}

/* Get running element count */
int get_running_element_count(void) {
    int count = 0;
    for (int i = 0; i < active_element_count; i++) {
        if (periodic_table_apps[i].is_running) count++;
    }
    return count;
}

/* Simulate CPU cycles for running elements */
void dubnium_tick_containers(void) {
    for (int i = 0; i < active_element_count; i++) {
        if (periodic_table_apps[i].is_running) {
            periodic_table_apps[i].cpu_cycles += 4210;
        }
    }
}

/* Helper: simple string length */
static int my_strlen(const char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}
`
  },
  {
    name: 'carbon_app.c',
    path: 'apps/carbon_app.c',
    language: 'c',
    description: 'Carbon Text Editor [C] - ELEMENT 6: Sổ tay ghi chú với giao diện Vintage Minimalist',
    content: `/**
 * @file carbon_app.c
 * @brief Carbon Text Editor [C] - ELEMENT 6
 * Sổ tay ghi chú (Text Editor/Notes) trong hệ sinh thái HydroOS
 * Compiled with: clang -target x86_64-none-elf -ffreestanding
 */

#include <stdint.h>
#include <stddef.h>

/* VINTAGE COLOR PALETTE */
#define COLOR_HE_BG      0xFFF5EFE6  /* Trắng kem ấm */
#define COLOR_TEXT       0xFF3E2723  /* Espresso nâu đậm */
#define COLOR_BORDER     0xFFD7CCC8  /* Nâu tro mảnh */
#define COLOR_WHITE      0xFFFFFFFF
#define COLOR_EMERALD    0xFF10B981

/* ELEMENT 6: CARBON - Properties */
#define CARBON_ATOMIC_NUMBER  6
#define CARBON_SYMBOL         "C"
#define CARBON_FULL_NAME      "Carbon"

/* Text buffer for notes */
#define MAX_TEXT_LINES 32
#define MAX_LINE_LENGTH 80
static char text_buffer[MAX_TEXT_LINES][MAX_LINE_LENGTH];
static int current_line = 0;
static int cursor_x = 0;
static int cursor_y = 0;

/* =============================================================================
 * SHOW CARBON APP - Text Editor Window
 * Tiêu đề: "ELEMENT 6: CARBON (TEXT EDITOR)"
 * ============================================================================= */
void show_carbon_app(struct limine_framebuffer *fb, int win_x, int win_y, int win_w, int win_h) {
    /* Window Background - COLOR_HE_BG (Trắng kem ấm) */
    draw_rect_filled(fb, win_x, win_y, win_w, win_h, COLOR_HE_BG);

    /* Window Border - 1px nâu tro mảnh */
    draw_rect_outline(fb, win_x, win_y, win_w, win_h, COLOR_BORDER, COLOR_HE_BG);

    /* ----- TITLE BAR ----- */
    int title_h = 24;
    draw_rect_filled(fb, win_x, win_y, win_w, title_h, COLOR_WHITE);
    draw_line(fb, win_x, win_y + title_h - 1, win_x + win_w, win_y + title_h - 1, COLOR_BORDER);

    /* Title Text: "ELEMENT 6: CARBON (TEXT EDITOR)" */
    draw_string(fb, win_x + 28, win_y + 8, "ELEMENT 6: CARBON (TEXT EDITOR)", COLOR_TEXT);

    /* Periodic Table Mini Box in Title */
    draw_rect_filled(fb, win_x + 6, win_y + 4, 18, 16, COLOR_HE_BG);
    draw_rect_outline(fb, win_x + 6, win_y + 4, 18, 16, COLOR_BORDER, COLOR_HE_BG);
    draw_char(fb, win_x + 10, win_y + 7, 'C', COLOR_TEXT);

    /* ----- TEXT EDITOR CANVAS ----- */
    int canvas_x = win_x + 8;
    int canvas_y = win_y + title_h + 8;
    int line_h = 10;

    /* Render each line of text buffer */
    for (int i = 0; i < MAX_TEXT_LINES && i < (win_h - title_h - 16) / line_h; i++) {
        if (text_buffer[i][0] != '\\0') {
            draw_string(fb, canvas_x, canvas_y + i * line_h, text_buffer[i], COLOR_TEXT);
        }
    }

    /* Blinking Cursor (underscore) */
    int cx = canvas_x + cursor_x * 8;
    int cy = canvas_y + cursor_y * line_h;
    draw_char(fb, cx, cy, '_', COLOR_EMERALD);

    /* ----- FOOTER INFO BAR ----- */
    int footer_y = win_y + win_h - 20;
    draw_rect_filled(fb, win_x, footer_y, win_w, 20, COLOR_WHITE);
    draw_line(fb, win_x, footer_y, win_x + win_w, footer_y, COLOR_BORDER);

    /* Footer: Atomic Number & Status */
    draw_string_small(fb, win_x + 8, footer_y + 6, "Z=6", COLOR_TEXT);
    draw_string_small(fb, win_x + 40, footer_y + 6, "[C] CARBON - TEXT NOTES", COLOR_TEXT);
}

/* Initialize Carbon Text Editor with default content */
void carbon_app_init(void) {
    current_line = 0;
    cursor_x = 0;
    cursor_y = 0;

    /* Default welcome note */
    const char *welcome = "Welcome to Carbon [C] Notes!";
    int i = 0;
    while (welcome[i] && i < MAX_LINE_LENGTH - 1) {
        text_buffer[0][i] = welcome[i];
        i++;
    }
    text_buffer[0][i] = '\\0';

    const char *hint = "Press keys to type...";
    i = 0;
    while (hint[i] && i < MAX_LINE_LENGTH - 1) {
        text_buffer[1][i] = hint[i];
        i++;
    }
    text_buffer[1][i] = '\\0';

    cursor_y = 2;
}

/* Insert character at current cursor position */
void carbon_insert_char(char c) {
    if (cursor_x < MAX_LINE_LENGTH - 1) {
        text_buffer[cursor_y][cursor_x] = c;
        text_buffer[cursor_y][cursor_x + 1] = '\\0';
        cursor_x++;
    }
}

/* New line - move cursor down */
void carbon_new_line(void) {
    if (cursor_y < MAX_TEXT_LINES - 1) {
        cursor_y++;
        cursor_x = 0;
        text_buffer[cursor_y][0] = '\\0';
    }
}

/* Backspace - delete character */
void carbon_backspace(void) {
    if (cursor_x > 0) {
        cursor_x--;
        text_buffer[cursor_y][cursor_x] = '\\0';
    } else if (cursor_y > 0) {
        cursor_y--;
        cursor_x = my_strlen(text_buffer[cursor_y]);
    }
}
`
  },
  {
    name: 'boron_app.c',
    path: 'apps/boron_app.c',
    language: 'c',
    description: 'Boron Calculator [B] - ELEMENT 5: Máy tính logic giao diện Vintage Minimalist',
    content: `/**
 * @file boron_app.c
 * @brief Boron Calculator [B] - ELEMENT 5
 * Máy tính logic (Calculator) trong hệ sinh thái HydroOS
 * Vintage Minimalist Design - Pixel Perfect Geometry
 * Compiled with: clang -target x86_64-none-elf -ffreestanding
 */

#include <stdint.h>
#include <stddef.h>

/* =============================================================================
 * VINTAGE COLOR PALETTE - Notion Warm Manuscript Theme
 * ============================================================================= */
#define COLOR_HE_BG      0xFFF5EFE6  /* Trắng kem ấm */
#define COLOR_LI_BG      0xFFEFEBE9  /* Nâu Latte nhạt */
#define COLOR_DB_BG      0xFFE0D4C3  /* Nâu gỗ sáng */
#define COLOR_TEXT       0xFF3E2723  /* Espresso nâu đậm */
#define COLOR_BORDER     0xFFD7CCC8  /* Nâu tro mảnh 1px */
#define COLOR_WHITE      0xFFFFFFFF  /* Trắng ngà */
#define COLOR_EMERALD    0xFF10B981  /* Emerald accent */

/* ELEMENT 5: BORON - Properties */
#define BORON_ATOMIC_NUMBER  5
#define BORON_SYMBOL         "B"
#define BORON_FULL_NAME      "Boron"

/* =============================================================================
 * WINDOW GEOMETRY - Exact Pixel Coordinates
 * Cửa sổ vuông gọn gàng: X=312, Y=184, W=400, H=400
 * ============================================================================= */
#define BORON_WIN_X      312
#define BORON_WIN_Y      184
#define BORON_WIN_W      400
#define BORON_WIN_H      400
#define BORON_TITLE_H    28

/* Calculator Display Screen */
#define DISPLAY_X        332
#define DISPLAY_Y        224
#define DISPLAY_W        360
#define DISPLAY_H        48

/* Button Grid Layout */
#define KEY_START_X      332
#define KEY_START_Y      288
#define KEY_W            75
#define KEY_H            40
#define KEY_SPACING      12

/* =============================================================================
 * CALCULATOR STATE
 * ============================================================================= */
static double current_value = 0.0;
static double stored_value = 0.0;
static char operator = '\\0';
static int has_operator = 0;
static char display_buffer[16] = "0";

/* =============================================================================
 * DRAW BORON WINDOW FRAME - With Retro Hard Shadow
 * ============================================================================= */
void draw_boron_frame(struct limine_framebuffer *fb) {
    /* Hard Shadow (2px offset down-right) */
    draw_rect_filled(fb, BORON_WIN_X + 2, BORON_WIN_Y + 2, BORON_WIN_W, BORON_WIN_H, COLOR_BORDER);

    /* Main Window Background - COLOR_WHITE */
    draw_rect_filled(fb, BORON_WIN_X, BORON_WIN_Y, BORON_WIN_W, BORON_WIN_H, COLOR_WHITE);

    /* Window Border - 1px nâu tro */
    draw_rect_outline(fb, BORON_WIN_X, BORON_WIN_Y, BORON_WIN_W, BORON_WIN_H, COLOR_BORDER, COLOR_WHITE);
}

/* =============================================================================
 * DRAW TITLE BAR
 * X=312, Y=184, W=400, H=28
 * ============================================================================= */
void draw_boron_title(struct limine_framebuffer *fb) {
    /* Title Bar Background - COLOR_HE_BG (Kem ấm) */
    draw_rect_filled(fb, BORON_WIN_X, BORON_WIN_Y, BORON_WIN_W, BORON_TITLE_H, COLOR_HE_BG);

    /* Title Bar Bottom Border - 1px */
    draw_line(fb, BORON_WIN_X, BORON_WIN_Y + BORON_TITLE_H - 1,
              BORON_WIN_X + BORON_WIN_W, BORON_WIN_Y + BORON_TITLE_H - 1, COLOR_BORDER);

    /* Periodic Table Mini Box: Element "B" */
    int box_x = BORON_WIN_X + 8;
    int box_y = BORON_WIN_Y + 5;
    draw_rect_filled(fb, box_x, box_y, 18, 18, COLOR_WHITE);
    draw_rect_outline(fb, box_x, box_y, 18, 18, COLOR_BORDER, COLOR_WHITE);
    draw_char(fb, box_x + 5, box_y + 5, 'B', COLOR_TEXT);

    /* Title Text: "ELEMENT 5: BORON (CALCULATOR)" at X=324, Y=192 */
    /* Title Y baseline = BORON_WIN_Y + 8 = 192 */
    draw_string(fb, BORON_WIN_X + 32, BORON_WIN_Y + 9, "ELEMENT 5: BORON (CALCULATOR)", COLOR_TEXT);
}

/* =============================================================================
 * DRAW CALCULATOR DISPLAY SCREEN
 * X=332, Y=224, W=360, H=48
 * ============================================================================= */
void draw_boron_display(struct limine_framebuffer *fb) {
    /* LCD Screen Background - COLOR_HE_BG (Kem ấm, classic LCD feel) */
    draw_rect_filled(fb, DISPLAY_X, DISPLAY_Y, DISPLAY_W, DISPLAY_H, COLOR_HE_BG);

    /* Display Border - 1px */
    draw_rect_outline(fb, DISPLAY_X, DISPLAY_Y, DISPLAY_W, DISPLAY_H, COLOR_BORDER, COLOR_HE_BG);

    /* Format number with commas and decimals */
    /* Example: "12,345.00" displayed right-aligned */
    int text_len = 0;
    while (display_buffer[text_len]) text_len++;

    /* Right-align: X=670 is right edge, subtract text width (8px per char) */
    /* DISPLAY_X + DISPLAY_W = 332 + 360 = 692, right margin ~22px */
    int text_x = DISPLAY_X + DISPLAY_W - 12 - (text_len * 8);
    int text_y = DISPLAY_Y + 16;

    /* Draw display value in Espresso brown */
    draw_string(fb, text_x, text_y, display_buffer, COLOR_TEXT);
}

/* =============================================================================
 * DRAW SINGLE BUTTON KEY
 * ============================================================================= */
void draw_boron_key(struct limine_framebuffer *fb, int x, int y, const char *label, int is_function) {
    uint32_t bg_color = is_function ? COLOR_LI_BG : COLOR_HE_BG;

    /* Button Background */
    draw_rect_filled(fb, x, y, KEY_W, KEY_H, bg_color);

    /* Button Border - 1px */
    draw_rect_outline(fb, x, y, KEY_W, KEY_H, COLOR_BORDER, bg_color);

    /* Button Label - centered horizontally and vertically */
    int label_len = 0;
    while (label[label_len]) label_len++;

    /* Center X: button_x + (KEY_W - label_width) / 2 */
    int label_x = x + (KEY_W - label_len * 8) / 2;
    /* Center Y: button_y + (KEY_H - 8) / 2 */
    int label_y = y + (KEY_H - 8) / 2;

    draw_string(fb, label_x, label_y, label, COLOR_TEXT);
}

/* =============================================================================
 * DRAW BUTTON GRID LAYOUT
 * 4 hàng x 4 cột, mỗi phím W=75, H=40, spacing=12px
 * Start: X=332, Y=288
 * ============================================================================= */
void draw_boron_keys(struct limine_framebuffer *fb) {
    /* Key labels: 4 rows x 4 cols */
    const char *key_labels[4][4] = {
        { "7", "8", "9", "/" },
        { "4", "5", "6", "*" },
        { "1", "2", "3", "-" },
        { "C", "0", "=", "+" }
    };

    /* is_function: 1 for operators and C/= */
    const int is_func[4][4] = {
        { 0, 0, 0, 1 },
        { 0, 0, 0, 1 },
        { 0, 0, 0, 1 },
        { 1, 0, 1, 1 }
    };

    /* Draw each key button */
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            /* Calculate position: start + (col * (KEY_W + spacing)), same for row */
            int key_x = KEY_START_X + col * (KEY_W + KEY_SPACING);
            int key_y = KEY_START_Y + row * (KEY_H + KEY_SPACING);

            draw_boron_key(fb, key_x, key_y, key_labels[row][col], is_func[row][col]);
        }
    }
}

/* =============================================================================
 * MAIN BORON APP RENDER FUNCTION
 * ============================================================================= */
void show_boron_app(struct limine_framebuffer *fb) {
    /* 1. Draw window frame with retro shadow */
    draw_boron_frame(fb);

    /* 2. Draw title bar with element info */
    draw_boron_title(fb);

    /* 3. Draw LCD display screen */
    draw_boron_display(fb);

    /* 4. Draw mechanical button grid */
    draw_boron_keys(fb);
}

/* =============================================================================
 * CALCULATOR LOGIC FUNCTIONS
 * ============================================================================= */

/* Initialize Boron Calculator */
void boron_app_init(void) {
    current_value = 0.0;
    stored_value = 0.0;
    operator = '\\0';
    has_operator = 0;
    display_buffer[0] = '0';
    display_buffer[1] = '\\0';
}

/* Handle digit input */
void boron_digit(int digit) {
    int len = 0;
    while (display_buffer[len]) len++;

    if (len < 15) {
        if (display_buffer[0] == '0' && len == 1) {
            display_buffer[0] = '0' + digit;
        } else {
            display_buffer[len] = '0' + digit;
            display_buffer[len + 1] = '\\0';
        }
    }
    current_value = boron_atoi(display_buffer);
}

/* Handle operator input */
void boron_operator(char op) {
    stored_value = current_value;
    operator = op;
    has_operator = 1;
    display_buffer[0] = '0';
    display_buffer[1] = '\\0';
}

/* Calculate result */
void boron_equals(void) {
    double result = 0.0;

    switch (operator) {
        case '+': result = stored_value + current_value; break;
        case '-': result = stored_value - current_value; break;
        case '*': result = stored_value * current_value; break;
        case '/':
            if (current_value != 0.0) {
                result = stored_value / current_value;
            } else {
                result = 0.0;
            }
            break;
        default: result = current_value; break;
    }

    current_value = result;
    boron_format_number(result, display_buffer);
    has_operator = 0;
}

/* Clear calculator */
void boron_clear(void) {
    current_value = 0.0;
    stored_value = 0.0;
    operator = '\\0';
    has_operator = 0;
    display_buffer[0] = '0';
    display_buffer[1] = '\\0';
}

/* Helper: Simple string to integer */
static int boron_atoi(const char *s) {
    int result = 0;
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return result;
}

/* Helper: Format number to display string */
static void boron_format_number(double val, char *buf) {
    int int_part = (int)val;
    int i = 0;

    if (int_part == 0) {
        buf[i++] = '0';
    } else {
        int temp = int_part;
        int digits[16];
        int d = 0;

        while (temp > 0) {
            digits[d++] = temp % 10;
            temp /= 10;
        }

        /* Add commas every 3 digits */
        int comma_counter = 0;
        for (int j = d - 1; j >= 0; j--) {
            buf[i++] = '0' + digits[j];
            comma_counter++;
            if (j > 0 && comma_counter == 3) {
                buf[i++] = ',';
                comma_counter = 0;
            }
        }
    }

    buf[i++] = '.';
    buf[i++] = '0';
    buf[i++] = '0';
    buf[i] = '\\0';
}
`
  },
  {
    name: 'titanium_app.c',
    path: 'apps/titanium_app.c',
    language: 'c',
    description: 'Titanium Time & Timeline [Ti] - ELEMENT 22: Đồng hồ & Lịch trình với giao diện Vintage Minimalist',
    content: `/**
 * @file titanium_app.c
 * @brief Titanium Time & Timeline [Ti] - ELEMENT 22
 * Đồng hồ & Lịch trình (Clock & Timeline) trong hệ sinh thái HydroOS
 * Vintage Minimalist Design - Split Panel Layout
 * Compiled with: clang -target x86_64-none-elf -ffreestanding
 */

#include <stdint.h>
#include <stddef.h>

/* =============================================================================
 * VINTAGE COLOR PALETTE - Notion Warm Manuscript Theme
 * ============================================================================= */
#define COLOR_HE_BG      0xFFF5EFE6  /* Trắng kem ấm */
#define COLOR_LI_BG      0xFFEFEBE9  /* Nâu Latte nhạt */
#define COLOR_DB_BG      0xFFE0D4C3  /* Nâu gỗ sáng */
#define COLOR_TEXT       0xFF3E2723  /* Espresso nâu đậm */
#define COLOR_BORDER     0xFFD7CCC8  /* Nâu tro mảnh 1px */
#define COLOR_WHITE      0xFFFFFFFF  /* Trắng ngà */
#define COLOR_EMERALD    0xFF10B981  /* Emerald accent */

/* ELEMENT 22: TITANIUM - Properties */
#define TITANIUM_ATOMIC_NUMBER  22
#define TITANIUM_SYMBOL         "Ti"
#define TITANIUM_FULL_NAME       "Titanium"

/* =============================================================================
 * WINDOW GEOMETRY - Exact Pixel Coordinates
 * Cửa sổ rộng nằm ngang: X=162, Y=154, W=700, H=460
 * ============================================================================= */
#define TITANIUM_WIN_X      162
#define TITANIUM_WIN_Y      154
#define TITANIUM_WIN_W      700
#define TITANIUM_WIN_H      460
#define TITANIUM_TITLE_H    28

/* Split Panel Divider */
#define SPLIT_X             432  /* Vertical divider line from Y=182 to Y=614 */
#define LEFT_PANEL_START    162
#define RIGHT_PANEL_START   432

/* =============================================================================
 * ANALOG CLOCK GEOMETRY - Left Panel
 * Tâm đồng hồ: (X=297, Y=380), Bán kính R=100 pixel
 * ============================================================================= */
#define CLOCK_CENTER_X      297
#define CLOCK_CENTER_Y      380
#define CLOCK_RADIUS        100

/* Clock Hand Lengths */
#define HOUR_HAND_LEN       50
#define MINUTE_HAND_LEN     75
#define SECOND_HAND_LEN     85

/* =============================================================================
 * TIMELINE GEOMETRY - Right Panel
 * Margin trái bắt đầu từ X=460
 * ============================================================================= */
#define TIMELINE_MARGIN_X   460
#define TIMELINE_START_Y    220

/* Event dot positions */
#define EVENT_DOT_X         460
#define EVENT_1_Y           260
#define EVENT_2_Y           300
#define EVENT_3_Y           340

/* =============================================================================
 * TIME STATE
 * ============================================================================= */
static int current_hour = 10;
static int current_minute = 10;
static int current_second = 0;

/* =============================================================================
 * DRAW TITANIUM WINDOW FRAME - With Retro Hard Shadow
 * ============================================================================= */
void draw_titanium_frame(struct limine_framebuffer *fb) {
    /* Hard Shadow (2px offset down-right) - COLOR_BORDER */
    draw_rect_filled(fb, TITANIUM_WIN_X + 2, TITANIUM_WIN_Y + 2, TITANIUM_WIN_W, TITANIUM_WIN_H, COLOR_BORDER);

    /* Main Window Background - COLOR_WHITE */
    draw_rect_filled(fb, TITANIUM_WIN_X, TITANIUM_WIN_Y, TITANIUM_WIN_W, TITANIUM_WIN_H, COLOR_WHITE);

    /* Window Border - 1px nâu tro */
    draw_rect_outline(fb, TITANIUM_WIN_X, TITANIUM_WIN_Y, TITANIUM_WIN_W, TITANIUM_WIN_H, COLOR_BORDER, COLOR_WHITE);
}

/* =============================================================================
 * DRAW TITLE BAR
 * X=162, Y=154, W=700, H=28
 * ============================================================================= */
void draw_titanium_title(struct limine_framebuffer *fb) {
    /* Title Bar Background - COLOR_HE_BG (Kem ấm) */
    draw_rect_filled(fb, TITANIUM_WIN_X, TITANIUM_WIN_Y, TITANIUM_WIN_W, TITANIUM_TITLE_H, COLOR_HE_BG);

    /* Title Bar Bottom Border - 1px */
    draw_line(fb, TITANIUM_WIN_X, TITANIUM_WIN_Y + TITANIUM_TITLE_H - 1,
              TITANIUM_WIN_X + TITANIUM_WIN_W, TITANIUM_WIN_Y + TITANIUM_TITLE_H - 1, COLOR_BORDER);

    /* Periodic Table Mini Box: Element "Ti" */
    int box_x = TITANIUM_WIN_X + 8;
    int box_y = TITANIUM_WIN_Y + 5;
    draw_rect_filled(fb, box_x, box_y, 20, 18, COLOR_WHITE);
    draw_rect_outline(fb, box_x, box_y, 20, 18, COLOR_BORDER, COLOR_WHITE);
    draw_string_small(fb, box_x + 3, box_y + 5, "Ti", COLOR_TEXT);

    /* Title Text: "ELEMENT 22: TITANIUM (TIME & TIMELINE)" at X=174, Y=162 */
    draw_string(fb, TITANIUM_WIN_X + 34, TITANIUM_WIN_Y + 9, "ELEMENT 22: TITANIUM (TIME & TIMELINE)", COLOR_TEXT);
}

/* =============================================================================
 * DRAW SPLIT PANEL DIVIDER
 * Vertical line at X=432 from Y=182 to Y=614
 * ============================================================================= */
void draw_split_divider(struct limine_framebuffer *fb) {
    /* Vertical line from below title bar to bottom */
    int divider_start_y = TITANIUM_WIN_Y + TITANIUM_TITLE_H;
    int divider_end_y = TITANIUM_WIN_Y + TITANIUM_WIN_H;

    draw_line(fb, SPLIT_X, divider_start_y, SPLIT_X, divider_end_y, COLOR_BORDER);
}

/* =============================================================================
 * DRAW ANALOG CLOCK FACE - Left Panel
 * Tâm (297, 380), R=100
 * ============================================================================= */
void draw_analog_clock(struct limine_framebuffer *fb) {
    int cx = CLOCK_CENTER_X;
    int cy = CLOCK_CENTER_Y;
    int r = CLOCK_RADIUS;

    /* Draw clock face circle - using Bresenham-like circle algorithm */
    draw_circle_outline(fb, cx, cy, r, COLOR_BORDER);

    /* Draw hour markers (12 marks around the clock) */
    for (int h = 0; h < 12; h++) {
        double angle = (h * 30.0 - 90.0) * 3.14159 / 180.0;
        int inner_r = r - 12;
        int outer_r = r - 4;

        int x1 = cx + (int)(inner_r * cos_approx(angle));
        int y1 = cy + (int)(inner_r * sin_approx(angle));
        int x2 = cx + (int)(outer_r * cos_approx(angle));
        int y2 = cy + (int)(outer_r * sin_approx(angle));

        draw_line(fb, x1, y1, x2, y2, COLOR_TEXT);
    }

    /* Draw hour marks for 12, 3, 6, 9 (longer) */
    const int major_hours[] = {0, 3, 6, 9};
    for (int i = 0; i < 4; i++) {
        int h = major_hours[i];
        double angle = (h * 30.0 - 90.0) * 3.14159 / 180.0;
        int inner_r = r - 18;
        int outer_r = r - 4;

        int x1 = cx + (int)(inner_r * cos_approx(angle));
        int y1 = cy + (int)(inner_r * sin_approx(angle));
        int x2 = cx + (int)(outer_r * cos_approx(angle));
        int y2 = cy + (int)(outer_r * sin_approx(angle));

        draw_line_thick(fb, x1, y1, x2, y2, COLOR_TEXT, 2);
    }

    /* Draw clock hands */
    /* Time: 10:10:00 (classic watch display time) */
    /* Hour hand: 50px, pointing to ~10 o'clock position */
    /* Hour angle: (hour % 12) * 30 + minute * 0.5 degrees from 12 o'clock */
    double hour_angle = ((10 % 12) * 30.0 + 10 * 0.5 - 90.0) * 3.14159 / 180.0;
    int hour_end_x = cx + (int)(HOUR_HAND_LEN * cos_approx(hour_angle));
    int hour_end_y = cy + (int)(HOUR_HAND_LEN * sin_approx(hour_angle));
    draw_line_thick(fb, cx, cy, hour_end_x, hour_end_y, COLOR_TEXT, 3);

    /* Minute hand: 75px, pointing to ~10 minute position */
    double minute_angle = (10 * 6.0 - 90.0) * 3.14159 / 180.0;
    int minute_end_x = cx + (int)(MINUTE_HAND_LEN * cos_approx(minute_angle));
    int minute_end_y = cy + (int)(MINUTE_HAND_LEN * sin_approx(minute_angle));
    draw_line_thick(fb, cx, cy, minute_end_x, minute_end_y, COLOR_TEXT, 2);

    /* Second hand: 85px, pointing to 12 o'clock (0 seconds) */
    double second_angle = (0 * 6.0 - 90.0) * 3.14159 / 180.0;
    int second_end_x = cx + (int)(SECOND_HAND_LEN * cos_approx(second_angle));
    int second_end_y = cy + (int)(SECOND_HAND_LEN * sin_approx(second_angle));
    draw_line(fb, cx, cy, second_end_x, second_end_y, COLOR_DB_BG);

    /* Center dot */
    draw_circle_filled(fb, cx, cy, 4, COLOR_TEXT);

    /* Draw digital time below clock */
    char time_str[12] = "10:10:00";
    int time_x = cx - 32;  /* Center: 8 chars * 8px / 2 = 32 */
    int time_y = cy + r + 20;
    draw_string(fb, time_x, time_y, time_str, COLOR_TEXT);
}

/* =============================================================================
 * DRAW TIMELINE NOTION VIEW - Right Panel
 * Margin: X=460
 * ============================================================================= */
void draw_timeline_view(struct limine_framebuffer *fb) {
    /* Header: "TODAY'S CHRONICLE" at Y=220 */
    draw_string(fb, TIMELINE_MARGIN_X, TIMELINE_START_Y, "TODAY'S CHRONICLE", COLOR_TEXT);

    /* Vertical timeline line connecting all dots */
    /* Line from Event 1 top to Event 3 bottom */
    draw_line(fb, EVENT_DOT_X, EVENT_1_Y - 5, EVENT_DOT_X, EVENT_3_Y + 5, COLOR_BORDER);

    /* Event 1: 09:00 AM | Boot HydroOS Kernel */
    draw_circle_filled(fb, EVENT_DOT_X, EVENT_1_Y, 4, COLOR_DB_BG);
    draw_string(fb, EVENT_DOT_X + 16, EVENT_1_Y - 4, "09:00 AM | Boot HydroOS Kernel", COLOR_TEXT);

    /* Event 2: 11:30 AM | Compile Dubnium Engine */
    draw_circle_filled(fb, EVENT_DOT_X, EVENT_2_Y, 4, COLOR_DB_BG);
    draw_string(fb, EVENT_DOT_X + 16, EVENT_2_Y - 4, "11:30 AM | Compile Dubnium Engine", COLOR_TEXT);

    /* Event 3: 03:00 PM | Refactor Vintage UI with AI */
    draw_circle_filled(fb, EVENT_DOT_X, EVENT_3_Y, 4, COLOR_DB_BG);
    draw_string(fb, EVENT_DOT_X + 16, EVENT_3_Y - 4, "03:00 PM | Refactor Vintage UI with AI", COLOR_TEXT);

    /* Additional timeline entries */
    int event_4_y = 380;
    draw_line(fb, EVENT_DOT_X, EVENT_3_Y + 5, EVENT_DOT_X, event_4_y - 5, COLOR_BORDER);
    draw_circle_outline(fb, EVENT_DOT_X, event_4_y, 4, COLOR_BORDER);
    draw_string(fb, EVENT_DOT_X + 16, event_4_y - 4, "05:00 PM | Kernel Shutdown Sequence", COLOR_TEXT);

    /* Footer time zone info */
    int footer_y = TITANIUM_WIN_Y + TITANIUM_WIN_H - 30;
    draw_string_small(fb, TIMELINE_MARGIN_X, footer_y, "TIMEZONE: UTC+7 (HydroOS Standard)", COLOR_TEXT);
}

/* =============================================================================
 * MAIN TITANIUM APP RENDER FUNCTION
 * ============================================================================= */
void show_titanium_app(struct limine_framebuffer *fb) {
    /* 1. Draw window frame with retro shadow */
    draw_titanium_frame(fb);

    /* 2. Draw title bar with element info */
    draw_titanium_title(fb);

    /* 3. Draw split panel divider */
    draw_split_divider(fb);

    /* 4. Draw analog clock in left panel */
    draw_analog_clock(fb);

    /* 5. Draw timeline schedule in right panel */
    draw_timeline_view(fb);
}

/* =============================================================================
 * HELPER MATH FUNCTIONS - Trigonometry for Clock
 * ============================================================================= */

/* Approximate cosine using Taylor series (reduced precision) */
static double cos_approx(double x) {
    /* Normalize x to [-pi, pi] range */
    while (x > 3.14159) x -= 2.0 * 3.14159;
    while (x < -3.14159) x += 2.0 * 3.14159;

    double x2 = x * x;
    double x4 = x2 * x2;
    double x6 = x4 * x2;

    /* cos(x) ≈ 1 - x²/2! + x⁴/4! - x⁶/6! */
    return 1.0 - x2/2.0 + x4/24.0 - x6/720.0;
}

/* Approximate sine using Taylor series */
static double sin_approx(double x) {
    /* sin(x) = cos(x - π/2) */
    return cos_approx(x - 3.14159/2.0);
}

/* =============================================================================
 * DRAWING PRIMITIVE HELPERS
 * ============================================================================= */

/* Draw circle outline (Bresenham-like midpoint algorithm) */
void draw_circle_outline(struct limine_framebuffer *fb, int cx, int cy, int r, uint32_t color) {
    int x = r;
    int y = 0;
    int err = 1 - r;

    while (x >= y) {
        draw_pixel(fb, cx + x, cy + y, color);
        draw_pixel(fb, cx + y, cy + x, color);
        draw_pixel(fb, cx - y, cy + x, color);
        draw_pixel(fb, cx - x, cy + y, color);
        draw_pixel(fb, cx - x, cy - y, color);
        draw_pixel(fb, cx - y, cy - x, color);
        draw_pixel(fb, cx + y, cy - x, color);
        draw_pixel(fb, cx + x, cy - y, color);

        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

/* Draw filled circle */
void draw_circle_filled(struct limine_framebuffer *fb, int cx, int cy, int r, uint32_t color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                draw_pixel(fb, cx + x, cy + y, color);
            }
        }
    }
}

/* Draw thicker line (width > 1) */
void draw_line_thick(struct limine_framebuffer *fb, int x1, int y1, int x2, int y2, uint32_t color, int thickness) {
    /* Draw main line */
    draw_line(fb, x1, y1, x2, y2, color);

    /* Draw parallel lines for thickness */
    for (int t = 1; t < thickness; t++) {
        draw_line(fb, x1 + t, y1, x2 + t, y2, color);
        draw_line(fb, x1 - t, y1, x2 - t, y2, color);
    }
}

/* =============================================================================
 * TIME FUNCTIONS
 * ============================================================================= */

/* Initialize Titanium Clock with default time */
void titanium_app_init(void) {
    current_hour = 10;
    current_minute = 10;
    current_second = 0;
}

/* Update time (tick) */
void titanium_tick(void) {
    current_second++;
    if (current_second >= 60) {
        current_second = 0;
        current_minute++;
        if (current_minute >= 60) {
            current_minute = 0;
            current_hour++;
            if (current_hour >= 24) {
                current_hour = 0;
            }
        }
    }
}
`
  }
];
