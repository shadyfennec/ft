#ifndef __FONT_H__
#define __FONT_H__

#include <bits/stdint-intn.h>
#include <stdint.h>
#include <unistd.h>
#include <X11/Xft/Xft.h>

struct FtGlyph {
    uint32_t glyph_index;
    int32_t x_offset;
    int32_t y_offset;
    int32_t x_advance;
    int32_t y_advance;
};

void get_font_path(const char* name, char *path);
void load_font(XftFont* font);
int get_glyphs_shaped(const char* text, int len, struct FtGlyph* glyphs);
void clean_font();

#endif
