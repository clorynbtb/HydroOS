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
    description: 'Dubnium Docker Manager (Db) - Container structure, scheduling queue, and pseudo-virtual execution',
    content: `/**
 * @file db.c
 * @brief Dubnium Docker Engine (Db) - Container process virtualization inside HydroOS
 * Compiled with: clang -target x86_64-none-elf -fno-stack-protector
 */

#include <stdint.h>
#include <stddef.h>

#define MAX_CONTAINERS 16
#define CONTAINER_ID_LEN 12

typedef enum {
    STATE_CREATED,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_EXITED
} container_state_t;

struct ContainerProcess {
    char id[CONTAINER_ID_LEN];
    char image_name[32];
    container_state_t state;
    uint32_t ram_usage_mb;
    uint16_t virtual_port;
    uint64_t cpu_cycles;
};

// Global container list mimicking a running Docker engine memory registry
static struct ContainerProcess container_registry[MAX_CONTAINERS];
static int active_container_count = 0;

// Initialize the Dubnium Docker engine framework
void dubnium_docker_init(void) {
    active_container_count = 0;
    
    // Seed default system-critical containers
    dubnium_create_container("nginx-stable", 80, 45);
    dubnium_create_container("postgres-db", 5432, 128);
    
    // Start containers
    for (int i = 0; i < active_container_count; i++) {
        container_registry[i].state = STATE_RUNNING;
    }
}

// Spin up a new container from a simulated Docker Image
int dubnium_create_container(const char *image, uint16_t port, uint32_t ram) {
    if (active_container_count >= MAX_CONTAINERS) return -1;
    
    struct ContainerProcess *new_proc = &container_registry[active_container_count];
    
    // Generate pseudo-random Container ID hex sequence
    uint32_t salt = active_container_count * 0x7F31;
    for (int i = 0; i < 8; i++) {
        int val = (salt >> (i * 4)) & 0xF;
        new_proc->id[i] = (val < 10) ? ('0' + val) : ('a' + (val - 10));
    }
    new_proc->id[8] = '\\0';
    
    // Load metadata
    int idx = 0;
    while (image[idx] && idx < 31) {
        new_proc->image_name[idx] = image[idx];
        idx++;
    }
    new_proc->image_name[idx] = '\\0';
    
    new_proc->state = STATE_CREATED;
    new_proc->ram_usage_mb = ram;
    new_proc->virtual_port = port;
    new_proc->cpu_cycles = 0;
    
    active_container_count++;
    return active_container_count - 1;
}

// Simulate CPU tick execution of containers to generate realistic loads
void dubnium_tick_containers(void) {
    for (int i = 0; i < active_container_count; i++) {
        if (container_registry[i].state == STATE_RUNNING) {
            container_registry[i].cpu_cycles += 4210; // Accruing cycle count
        }
    }
}
`
  }
];
