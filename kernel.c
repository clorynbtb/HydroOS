/**
 * @file kernel.c
 * @brief HydroOS Kernel Core (Hydrogen Core - H) - Vintage Editorial & Notion Minimalist Theme Edition
 *
 * Architecture: 64-bit Freestanding Kernel (x86_64-none-elf)
 * Bootloader: Limine Protocol v5.x
 * Design System: Vintage Minimalist (Warm Manuscript Cream, Espresso Brown, Ash-Brown & Ivory Panels)
 */

#include <stdint.h>
#include <stddef.h>

/* Specifying framebuffer structure according to the Limine specification */
struct limine_framebuffer {
    void *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused[7];
};

struct limine_framebuffer_request {
    uint64_t id[4];
    uint64_t revision;
    struct {
        uint64_t count;
        struct limine_framebuffer **framebuffers;
    } *response;
};

#define LIMINE_FRAMEBUFFER_REQUEST { \
    0xc7b1dd30df4c8b88, 0x0a82e7ee10ab4ee6, \
    0x97a3efd2039120af, 0xf6be6cfb1ef96d66  \
    }

static volatile struct limine_framebuffer_request fb_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

/* =============================================================================
 * BẢNG MÀU MAC CLASSIC × NOTION VINTAGE (ARGB format)
 * Tone: Nâu Espresso · Kem ấm · Trắng ngà · Mac System 7
 * ============================================================================= */
#define COLOR_DESKTOP        0xFFF5EFE6  /* Nền kem ấm toàn màn hình (Desktop) */
#define COLOR_TEXT           0xFF3E2723  /* Chữ nâu Espresso đậm */
#define COLOR_BORDER         0xFFD7CCC8  /* Viền nâu tro mảnh 1px */
#define COLOR_WHITE          0xFFFFFFFF  /* Trắng ngà */
#define COLOR_HE_BG          0xFFF9F6F0  /* Trắng sữa nhạt [HE] */
#define COLOR_LI_BG          0xFFEFEBE9  /* Nâu Latte nhạt [LI] */
#define COLOR_DB_BG          0xFFE0D4C3  /* Nâu gỗ sáng [DB] */

/* Mac Classic bevel palette */
#define COLOR_BEVEL_LIGHT    0xFFFFFFFF  /* Cạnh trên/trái: trắng tinh (highlight ánh sáng) */
#define COLOR_BEVEL_DARK     0xFF5D4037  /* Cạnh dưới/phải: nâu sẫm (bóng đổ dày) */
#define COLOR_BEVEL_MID      0xFF8D6E63  /* Nâu trung tính (viền chữ muted) */
#define COLOR_TITLE_STRIPE   0xFFD7CCC8  /* Đường sọc tiêu đề Mac OS 7 */
#define COLOR_MENU_BAR       COLOR_WHITE

/* Aliases tương thích ngược */
#define COLOR_BACKGROUND     COLOR_DESKTOP
#define COLOR_TEXT_MAIN      COLOR_TEXT
#define NOTION_COLOR_BG      COLOR_DESKTOP
#define NOTION_COLOR_TEXT    COLOR_TEXT
#define NOTION_COLOR_BORDER  COLOR_BORDER
#define NOTION_COLOR_TEXT_MUTED 0xFF8D6E63

/* Biến con trỏ framebuffer toàn cục nhận diện từ Limine */
static uint32_t *g_fb_address = NULL;
static uint64_t g_fb_width = 1024;
static uint64_t g_fb_height = 768;
static uint64_t g_fb_pitch = 4096;

/* Khai báo nguyên mẫu các hàm đồ họa cơ bản */
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void draw_rect_filled(int x, int y, int w, int h, uint32_t color);
void draw_rect_outline(int x, int y, int w, int h, uint32_t border_color, uint32_t bg_color);
void draw_char(int x, int y, char c, uint32_t color);
void draw_string(int x, int y, const char *str, uint32_t color);
void delay(uint64_t count);

/* Mac Classic bevel primitives */
void draw_retro_button(int x, int y, int w, int h, uint32_t bg_color);
void draw_retro_button_pressed(int x, int y, int w, int h, uint32_t bg_color);
void draw_mac_titlebar(int x, int y, int w, int h, const char *title);
void draw_mac_close_box(int x, int y);

/* 3. BỘ FONT CHỮ PIXEL-ART TỐI GIẢN SẮC NÉT (Monospace 8x8) */
static const uint8_t font_8x8[128][8] = {
    [32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* ' ' */
    [33] = {0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00}, /* '!' */
    [34] = {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* '"' */
    [35] = {0x36, 0x36, 0x7f, 0x36, 0x7f, 0x36, 0x36, 0x00}, /* '#' */
    [36] = {0x18, 0x3e, 0x60, 0x3c, 0x06, 0x7c, 0x18, 0x00}, /* '$' */
    [37] = {0x00, 0x66, 0x66, 0x10, 0x20, 0x66, 0x66, 0x00}, /* '%' */
    [38] = {0x1c, 0x36, 0x1c, 0x3b, 0x6e, 0x3e, 0x00, 0x00}, /* '&' */
    [39] = {0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* '\'' */
    [40] = {0x0c, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x00}, /* '(' */
    [41] = {0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00}, /* ')' */
    [42] = {0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00}, /* '*' */
    [43] = {0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00}, /* '+' */
    [44] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30}, /* ',' */
    [45] = {0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00}, /* '-' */
    [46] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00}, /* '.' */
    [47] = {0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x00, 0x00}, /* '/' */
    [48] = {0x3e, 0x63, 0x67, 0x6f, 0x7b, 0x63, 0x3e, 0x00}, /* '0' */
    [49] = {0x0c, 0x1e, 0x0c, 0x0c, 0x0c, 0x0c, 0x3e, 0x00}, /* '1' */
    [50] = {0x3e, 0x63, 0x06, 0x1c, 0x30, 0x63, 0x7f, 0x00}, /* '2' */
    [51] = {0x7f, 0x06, 0x0c, 0x1c, 0x06, 0x63, 0x3e, 0x00}, /* '3' */
    [52] = {0x0c, 0x1c, 0x3c, 0x6c, 0x7f, 0x0c, 0x0c, 0x00}, /* '4' */
    [53] = {0x7f, 0x60, 0x7e, 0x03, 0x03, 0x63, 0x3e, 0x00}, /* '5' */
    [54] = {0x1c, 0x30, 0x60, 0x7e, 0x63, 0x63, 0x3e, 0x00}, /* '6' */
    [55] = {0x7f, 0x03, 0x06, 0x0c, 0x18, 0x18, 0x18, 0x00}, /* '7' */
    [56] = {0x3e, 0x63, 0x63, 0x3e, 0x63, 0x63, 0x3e, 0x00}, /* '8' */
    [57] = {0x3e, 0x63, 0x63, 0x7f, 0x03, 0x06, 0x3c, 0x00}, /* '9' */
    [58] = {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00}, /* ':' */
    [59] = {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x30, 0x00}, /* ';' */
    [60] = {0x06, 0x0c, 0x18, 0x30, 0x18, 0x0c, 0x06, 0x00}, /* '<' */
    [61] = {0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00}, /* '=' */
    [62] = {0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00}, /* '>' */
    [63] = {0x3e, 0x63, 0x06, 0x0c, 0x18, 0x00, 0x18, 0x00}, /* '?' */
    [64] = {0x3e, 0x63, 0x6f, 0x6f, 0x6e, 0x60, 0x3e, 0x00}, /* '@' */
    [65] = {0x1c, 0x36, 0x63, 0x7f, 0x63, 0x63, 0x63, 0x00}, /* 'A' */
    [66] = {0x7e, 0x63, 0x63, 0x7e, 0x63, 0x63, 0x7e, 0x00}, /* 'B' */
    [67] = {0x3e, 0x63, 0x60, 0x60, 0x60, 0x63, 0x3e, 0x00}, /* 'C' */
    [68] = {0x7c, 0x66, 0x63, 0x63, 0x63, 0x66, 0x7c, 0x00}, /* 'D' */
    [69] = {0x7f, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x7f, 0x00}, /* 'E' */
    [70] = {0x7f, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x60, 0x00}, /* 'F' */
    [71] = {0x3e, 0x63, 0x60, 0x6f, 0x63, 0x63, 0x3e, 0x00}, /* 'G' */
    [72] = {0x63, 0x63, 0x63, 0x7f, 0x63, 0x63, 0x63, 0x00}, /* 'H' */
    [73] = {0x3e, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3e, 0x00}, /* 'I' */
    [74] = {0x07, 0x03, 0x03, 0x03, 0x03, 0x63, 0x3e, 0x00}, /* 'J' */
    [75] = {0x63, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0x63, 0x00}, /* 'K' */
    [76] = {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7f, 0x00}, /* 'L' */
    [77] = {0x63, 0x77, 0x7f, 0x6b, 0x63, 0x63, 0x63, 0x00}, /* 'M' */
    [78] = {0x63, 0x63, 0x73, 0x7b, 0x6f, 0x67, 0x63, 0x00}, /* 'N' */
    [79] = {0x3e, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3e, 0x00}, /* 'O' */
    [80] = {0x7e, 0x63, 0x63, 0x7e, 0x60, 0x60, 0x60, 0x00}, /* 'P' */
    [81] = {0x3e, 0x63, 0x63, 0x63, 0x6b, 0x66, 0x3d, 0x00}, /* 'Q' */
    [82] = {0x7e, 0x63, 0x63, 0x7e, 0x6c, 0x66, 0x63, 0x00}, /* 'R' */
    [83] = {0x3e, 0x63, 0x60, 0x3e, 0x03, 0x63, 0x3e, 0x00}, /* 'S' */
    [84] = {0x7f, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00}, /* 'T' */
    [85] = {0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3e, 0x00}, /* 'U' */
    [86] = {0x63, 0x63, 0x63, 0x63, 0x63, 0x34, 0x1c, 0x00}, /* 'V' */
    [87] = {0x63, 0x63, 0x63, 0x6b, 0x7f, 0x77, 0x63, 0x00}, /* 'W' */
    [88] = {0x63, 0x63, 0x34, 0x1c, 0x34, 0x63, 0x63, 0x00}, /* 'X' */
    [89] = {0x63, 0x63, 0x63, 0x3e, 0x0c, 0x0c, 0x0c, 0x00}, /* 'Y' */
    [90] = {0x7f, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x7f, 0x00}, /* 'Z' */
    [91] = {0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00}, /* '[' */
    [92] = {0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x00, 0x00}, /* '\\' */
    [93] = {0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00}, /* ']' */
    [94] = {0x08, 0x1c, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, /* '^' */
    [95] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00}, /* '_' */
    [96] = {0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* '`' */
    [97] = {0x00, 0x00, 0x3c, 0x03, 0x3d, 0x63, 0x3d, 0x00}, /* 'a' */
    [98] = {0x60, 0x60, 0x7c, 0x63, 0x63, 0x63, 0x7c, 0x00}, /* 'b' */
    [99] = {0x00, 0x00, 0x3e, 0x60, 0x60, 0x63, 0x3e, 0x00}, /* 'c' */
    [100] = {0x03, 0x03, 0x3f, 0x63, 0x63, 0x63, 0x3f, 0x00}, /* 'd' */
    [101] = {0x00, 0x00, 0x3e, 0x63, 0x7f, 0x60, 0x3e, 0x00}, /* 'e' */
    [102] = {0x1c, 0x30, 0x7c, 0x30, 0x30, 0x30, 0x30, 0x00}, /* 'f' */
    [103] = {0x00, 0x00, 0x3e, 0x63, 0x63, 0x3e, 0x03, 0x3e}, /* 'g' */
    [104] = {0x60, 0x60, 0x7c, 0x63, 0x63, 0x63, 0x63, 0x00}, /* 'h' */
    [105] = {0x0c, 0x00, 0x1c, 0x0c, 0x0c, 0x0c, 0x1e, 0x00}, /* 'i' */
    [106] = {0x03, 0x00, 0x07, 0x03, 0x03, 0x03, 0x03, 0x1e}, /* 'j' */
    [107] = {0x60, 0x60, 0x63, 0x66, 0x7c, 0x66, 0x63, 0x00}, /* 'k' */
    [108] = {0x1c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x1e, 0x00}, /* 'l' */
    [109] = {0x00, 0x00, 0x7e, 0x6b, 0x6b, 0x6b, 0x6b, 0x00}, /* 'm' */
    [110] = {0x00, 0x00, 0x7c, 0x63, 0x63, 0x63, 0x63, 0x00}, /* 'n' */
    [111] = {0x00, 0x00, 0x3e, 0x63, 0x63, 0x63, 0x3e, 0x00}, /* 'o' */
    [112] = {0x00, 0x00, 0x7c, 0x63, 0x63, 0x7c, 0x60, 0x60}, /* 'p' */
    [113] = {0x00, 0x00, 0x3f, 0x63, 0x63, 0x3f, 0x03, 0x03}, /* 'q' */
    [114] = {0x00, 0x00, 0x5e, 0x30, 0x30, 0x30, 0x30, 0x00}, /* 'r' */
    [115] = {0x00, 0x00, 0x3e, 0x60, 0x3c, 0x03, 0x3e, 0x00}, /* 's' */
    [116] = {0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x0c, 0x00}, /* 't' */
    [117] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x63, 0x3d, 0x00}, /* 'u' */
    [118] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x34, 0x1c, 0x00}, /* 'v' */
    [119] = {0x00, 0x00, 0x63, 0x63, 0x6b, 0x7f, 0x34, 0x00}, /* 'w' */
    [120] = {0x00, 0x00, 0x63, 0x34, 0x1c, 0x34, 0x63, 0x00}, /* 'x' */
    [121] = {0x00, 0x00, 0x63, 0x63, 0x63, 0x3f, 0x03, 0x3e}, /* 'y' */
    [122] = {0x00, 0x00, 0x7f, 0x06, 0x0c, 0x18, 0x7f, 0x00}, /* 'z' */
    [123] = {0x0e, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0e, 0x00}, /* '{' */
    [124] = {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, /* '|' */
    [125] = {0x70, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x70, 0x00}, /* '}' */
    [126] = {0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* '~' */
};

/* Hàm trì hoãn phần cứng thực tế */
void delay(uint64_t count) {
    for (volatile uint64_t i = 0; i < count * 35000; i++) {
        __asm__("nop");
    }
}

/* 2. HÀM ĐỒ HỌA GỐC (Xử lý vùng nhớ framebuffer đồ họa thuần C) */

/* Tô màu pixel tại tọa độ màn hình */
void draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= (int)g_fb_width || y < 0 || y >= (int)g_fb_height) {
        return;
    }
    if (g_fb_address == NULL) return;

    size_t index = y * (g_fb_pitch / 4) + x;
    g_fb_address[index] = color;
}

/* Vẽ đường thẳng mảnh 1px bằng thuật toán Bresenham */
void draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = (x2 - x1 >= 0) ? (x2 - x1) : (x1 - x2);
    int dy = (y2 - y1 >= 0) ? (y2 - y1) : (y1 - y2);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        draw_pixel(x1, y1, color);
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

/* Vẽ khối hình chữ nhật đặc màu sắc bất kỳ */
void draw_rect_filled(int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            draw_pixel(px, py, color);
        }
    }
}

/* Tương thích ngược */
void draw_rect(int x, int y, int w, int h, uint32_t color) {
    draw_rect_filled(x, y, w, h, color);
}

/* Vẽ khối hình chữ nhật có đường viền mảnh 1px */
void draw_rect_outline(int x, int y, int w, int h, uint32_t border_color, uint32_t bg_color) {
    draw_rect_filled(x, y, w, h, bg_color);
    draw_line(x, y, x + w - 1, y, border_color);                 /* Cạnh trên */
    draw_line(x, y + h - 1, x + w - 1, y + h - 1, border_color); /* Cạnh dưới */
    draw_line(x, y, x, y + h - 1, border_color);                 /* Cạnh trái */
    draw_line(x + w - 1, y, x + w - 1, y + h - 1, border_color); /* Cạnh phải */
}

/* Vẽ một ký tự đơn từ bảng font Pixel-Art */
void draw_char(int x, int y, char c, uint32_t color) {
    uint8_t glyph_idx = (uint8_t)c;
    if (glyph_idx >= 128) glyph_idx = '?';

    for (int row = 0; row < 8; row++) {
        uint8_t data = font_8x8[glyph_idx][row];
        for (int col = 0; col < 8; col++) {
            if ((data >> (7 - col)) & 1) {
                draw_pixel(x + col, y + row, color);
            }
        }
    }
}

/* Vẽ chuỗi ký tự bằng bộ font Pixel-Art */
void draw_string(int x, int y, const char *str, uint32_t color) {
    while (*str) {
        draw_char(x, y, *str, color);
        x += 8; /* Monospace gap tinh tế */
        str++;
    }
}

/* Hàm tương thích ngược */
void draw_vector_char(int x, int y, char c, uint32_t color) {
    draw_char(x, y, c, color);
}
void draw_vector_string(int x, int y, const char *str, uint32_t color) {
    draw_string(x, y, str, color);
}

/* =============================================================================
 * 4. MAC CLASSIC BEVEL RENDERING SYSTEM
 * Tái hiện hiệu ứng khối 3D nổi của Macintosh System 7
 * ============================================================================= */

/**
 * draw_retro_button - Vẽ khối nút bấm cơ học 3D nổi kiểu Mac Classic
 *
 * Cấu trúc bevel 4 lớp:
 *   - Lớp 0 (ngoài cùng): Viền nâu tro COLOR_BORDER bao quanh
 *   - Lớp 1 (highlight):  Cạnh trên + trái = TRẮNG TINH (ánh sáng chiếu vào)
 *   - Lớp 2 (shadow):     Cạnh dưới + phải = NÂU SẪM (bóng đổ vật lý)
 *   - Lớp 3 (fill):       Lòng khối màu bg_color
 *
 * Kết quả: Khối UI nổi lên 3D như phím bấm cơ học trên máy Mac thập niên 90.
 */
void draw_retro_button(int x, int y, int w, int h, uint32_t bg_color) {
    /* Lớp 0: Viền ngoài cùng COLOR_BORDER */
    draw_rect_outline(x, y, w, h, COLOR_BORDER, bg_color);

    /* Lớp 1: Highlight – cạnh trên và cạnh trái = TRẮNG TINH */
    draw_line(x + 1, y + 1, x + w - 2, y + 1, COLOR_BEVEL_LIGHT); /* Trên */
    draw_line(x + 1, y + 1, x + 1, y + h - 2, COLOR_BEVEL_LIGHT); /* Trái */

    /* Lớp 2: Shadow – cạnh dưới và cạnh phải = NÂU SẪM */
    draw_line(x + 1, y + h - 2, x + w - 2, y + h - 2, COLOR_BEVEL_DARK); /* Dưới */
    draw_line(x + w - 2, y + 1, x + w - 2, y + h - 2, COLOR_BEVEL_DARK); /* Phải */
}

/**
 * draw_retro_button_pressed - Trạng thái nút lún xuống (pressed/active)
 * Đảo ngược highlight ↔ shadow để tạo cảm giác lún vào.
 */
void draw_retro_button_pressed(int x, int y, int w, int h, uint32_t bg_color) {
    draw_rect_outline(x, y, w, h, COLOR_BORDER, bg_color);

    /* Shadow trên/trái (lún vào) */
    draw_line(x + 1, y + 1, x + w - 2, y + 1, COLOR_BEVEL_DARK);
    draw_line(x + 1, y + 1, x + 1, y + h - 2, COLOR_BEVEL_DARK);

    /* Highlight dưới/phải */
    draw_line(x + 1, y + h - 2, x + w - 2, y + h - 2, COLOR_BEVEL_LIGHT);
    draw_line(x + w - 2, y + 1, x + w - 2, y + h - 2, COLOR_BEVEL_LIGHT);
}

/**
 * draw_mac_close_box - Ô vuông Close kiểu Mac Classic
 * Hình vuông 12×12px nâu tro viền, nền trắng — không tròn.
 */
void draw_mac_close_box(int x, int y) {
    draw_retro_button(x, y, 12, 12, COLOR_WHITE);
}

/**
 * draw_mac_titlebar - Thanh tiêu đề cửa sổ kiểu Mac System 7
 *
 * Cấu trúc:
 *   - Nền: COLOR_LI_BG (Kem sẫm 0xFFEFEBE9)
 *   - HỌA TIẾT SỌC MAC OS: 4 đường ngang mảnh COLOR_TITLE_STRIPE
 *     chạy song song tại Y+6, Y+9, Y+12, Y+15 (khoảng cách 3px)
 *   - NÚT CLOSE BOX: Góc trái tại (x+6, y+6), vuông 12×12px
 *   - Chữ tiêu đề: Căn giữa thanh, màu COLOR_TEXT, đè lên sọc
 *   - Viền dưới: 1px COLOR_BEVEL_DARK
 */
void draw_mac_titlebar(int x, int y, int w, int h, const char *title) {
    /* Nền thanh tiêu đề */
    draw_rect_filled(x, y, w, h, COLOR_LI_BG);

    /* Họa tiết sọc ngang Mac OS 7 — 4 đường kẻ song song */
    draw_line(x + 26, y + 5, x + w - 8, y + 5, COLOR_TITLE_STRIPE);
    draw_line(x + 26, y + 8, x + w - 8, y + 8, COLOR_TITLE_STRIPE);
    draw_line(x + 26, y + 11, x + w - 8, y + 11, COLOR_TITLE_STRIPE);
    draw_line(x + 26, y + 14, x + w - 8, y + 14, COLOR_TITLE_STRIPE);

    /* Close Box: góc trái (x+6, y+6) */
    draw_mac_close_box(x + 6, y + 6);

    /* Chữ tiêu đề căn giữa, đè lên sọc */
    int title_len = 0;
    while (title[title_len]) title_len++;
    int title_px = title_len * 8;
    int title_x = x + (w - title_px) / 2;
    int title_y = y + (h - 8) / 2;
    /* Đệm nền trắng nhỏ sau chữ để dễ đọc trên sọc */
    draw_rect_filled(title_x - 2, title_y - 1, title_px + 4, 10, COLOR_LI_BG);
    draw_string(title_x, title_y, title, COLOR_TEXT);

    /* Viền dưới thanh tiêu đề */
    draw_line(x, y + h - 1, x + w - 1, y + h - 1, COLOR_BEVEL_DARK);
}

/* =============================================================================
 * 5. LOGIC HIỂN THỊ UI – MAC CLASSIC × NOTION VINTAGE
 * ============================================================================= */

/**
 * hydro_splash_screen - Màn hình khởi động Mac Classic
 * Nền trắng ngà, logo [H] dạng khối nổi bevel, thanh tải dạng retro bar.
 */
void hydro_splash_screen(void) {
    /* Nền trắng ngà toàn màn hình */
    draw_rect_filled(0, 0, g_fb_width, g_fb_height, COLOR_WHITE);

    /* Logo [H] — khối nguyên tố nổi 3D, kích thước 64×72px tại trung tâm */
    int lx = 480, ly = 300, lw = 64, lh = 72;
    draw_retro_button(lx, ly, lw, lh, COLOR_LI_BG);

    /* Số hiệu nguyên tử "1" góc trên trái */
    draw_string(lx + 4, ly + 4, "1", COLOR_BEVEL_MID);

    /* Ký hiệu "H" lớn ở giữa */
    /* Trụ dọc trái */
    draw_line(lx + 16, ly + 20, lx + 16, ly + 50, COLOR_TEXT);
    draw_line(lx + 17, ly + 20, lx + 17, ly + 50, COLOR_TEXT);
    draw_line(lx + 18, ly + 20, lx + 18, ly + 50, COLOR_TEXT);
    /* Trụ dọc phải */
    draw_line(lx + 46, ly + 20, lx + 46, ly + 50, COLOR_TEXT);
    draw_line(lx + 47, ly + 20, lx + 47, ly + 50, COLOR_TEXT);
    draw_line(lx + 48, ly + 20, lx + 48, ly + 50, COLOR_TEXT);
    /* Thanh ngang giữa */
    draw_line(lx + 16, ly + 35, lx + 48, ly + 35, COLOR_TEXT);
    draw_line(lx + 16, ly + 36, lx + 48, ly + 36, COLOR_TEXT);

    /* Tên nguyên tố "Hydrogen" bên dưới logo */
    draw_string(lx + 4, ly + 56, "Hydrogen", COLOR_BEVEL_MID);

    /* Thanh Loading kiểu Mac Classic — khung retro_button */
    int bx = 400, by = 420, bw = 224, bh = 18;
    draw_retro_button_pressed(bx, by, bw, bh, COLOR_DESKTOP);

    /* Điền thanh tải từ trái sang phải */
    for (int pct = 0; pct <= 100; pct += 4) {
        int fill_w = ((bw - 4) * pct) / 100;
        if (fill_w > 0) {
            draw_rect_filled(bx + 2, by + 2, fill_w, bh - 4, COLOR_BEVEL_DARK);
        }
        delay(35);
    }
    delay(300);
}

/**
 * hydro_notion_desktop - Giao diện Desktop Mac Classic × Notion Vintage
 *
 * Layout:
 *   - Desktop: Nền kem ấm COLOR_DESKTOP
 *   - Top Menu Bar (Y=0, H=24): Trắng ngà + viền dưới + logo [H] bevel + menu items
 *   - Dock Control Tray (trung tâm, Y=700): Khay nổi retro bevel + 3 phím bấm 3D
 */
void hydro_notion_desktop(void) {
    /* Nền Desktop kem ấm */
    draw_rect_filled(0, 0, g_fb_width, g_fb_height, COLOR_DESKTOP);

    /* === TOP MENU BAR (Y=0, H=24) === */
    draw_rect_filled(0, 0, g_fb_width, 24, COLOR_WHITE);

    /* Viền dưới menu bar: 1px COLOR_BEVEL_DARK */
    draw_line(0, 23, g_fb_width, 23, COLOR_BEVEL_DARK);
    /* Inner highlight (bevel effect on menu bar bottom) */
    draw_line(0, 24, g_fb_width, 24, COLOR_BEVEL_LIGHT);

    /* Logo [H] element — hình chữ nhật 22×18px bevel, thay thế logo Táo Khuyết */
    draw_retro_button(8, 3, 22, 18, COLOR_LI_BG);
    draw_string(11, 7, "H", COLOR_TEXT);

    /* Menu items: HydroOS | File | Edit | Docker */
    draw_string(38, 8, "HydroOS", COLOR_TEXT);
    draw_string(108, 8, "File", COLOR_BEVEL_MID);
    draw_string(148, 8, "Edit", COLOR_BEVEL_MID);
    draw_string(188, 8, "Docker", COLOR_TEXT);
    draw_string(248, 8, "Window", COLOR_BEVEL_MID);

    /* Right: system status */
    draw_string(820, 8, "[LLD/CLANG]", COLOR_TEXT);

    /* === DOCK CONTROL TRAY === */
    /* Khay điều khiển nổi retro: X=350, Y=690, W=324, H=60 */
    int dock_x = 350, dock_y = 690, dock_w = 324, dock_h = 60;
    draw_retro_button(dock_x, dock_y, dock_w, dock_h, COLOR_LI_BG);

    /* Hard shadow offset 2px xuống-phải */
    draw_line(dock_x + 2, dock_y + dock_h, dock_x + dock_w + 2, dock_y + dock_h, COLOR_BORDER);
    draw_line(dock_x + dock_w, dock_y + 2, dock_x + dock_w, dock_y + dock_h + 2, COLOR_BORDER);

    /* [HE] (Helium) — Phím bấm 3D, X=362, Y=699, W=84, H=42 */
    draw_retro_button(dock_x + 12, dock_y + 10, 84, 42, COLOR_HE_BG);
    draw_string(dock_x + 42, dock_y + 27, "He", COLOR_TEXT);

    /* Ký hiệu nguyên tử nhỏ */
    draw_string(dock_x + 14, dock_y + 12, "2", COLOR_BEVEL_MID);

    /* [LI] (Lithium) — Phím bấm 3D, kế tiếp */
    draw_retro_button(dock_x + 108, dock_y + 10, 84, 42, COLOR_LI_BG);
    draw_string(dock_x + 138, dock_y + 27, "Li", COLOR_TEXT);
    draw_string(dock_x + 110, dock_y + 12, "3", COLOR_BEVEL_MID);

    /* [DB] (Dubnium) — Phím bấm 3D, màu nâu gỗ */
    draw_retro_button(dock_x + 204, dock_y + 10, 84, 42, COLOR_DB_BG);
    draw_string(dock_x + 232, dock_y + 27, "Db", COLOR_TEXT);
    draw_string(dock_x + 206, dock_y + 12, "105", COLOR_BEVEL_MID);

    /* Separator engraved line giữa các phím */
    draw_line(dock_x + 100, dock_y + 14, dock_x + 100, dock_y + dock_h - 14, COLOR_BEVEL_DARK);
    draw_line(dock_x + 101, dock_y + 14, dock_x + 101, dock_y + dock_h - 14, COLOR_BEVEL_LIGHT);
    draw_line(dock_x + 196, dock_y + 14, dock_x + 196, dock_y + dock_h - 14, COLOR_BEVEL_DARK);
    draw_line(dock_x + 197, dock_y + 14, dock_x + 197, dock_y + dock_h - 14, COLOR_BEVEL_LIGHT);
}

/* 5. Hàm kernel_main nhận địa chỉ framebuffer trực tiếp từ Limine hoặc đối số */
void kernel_main(uint32_t *framebuffer_addr) {
    if (framebuffer_addr != NULL) {
        g_fb_address = framebuffer_addr;
    }

    /* Chạy hoạt cảnh khởi động */
    hydro_splash_screen();

    /* Khởi động giao diện Desktop Vintage hoài cổ tối giản */
    hydro_notion_desktop();

    /* Giữ CPU nhàn rỗi trong vòng lặp vô hạn */
    for (;;) {
        __asm__("hlt");
    }
}

/* Điểm vào bootloader truyền thống */
void _start(void) {
    if (fb_request.response != NULL && fb_request.response->count > 0) {
        struct limine_framebuffer *fb = fb_request.response->framebuffers[0];
        g_fb_address = (uint32_t *)fb->address;
        g_fb_width = fb->width;
        g_fb_height = fb->height;
        g_fb_pitch = fb->pitch;
    }
    kernel_main(g_fb_address);
}
