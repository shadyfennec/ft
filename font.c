#include <X11/Xft/Xft.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <fontconfig/fontconfig.h>
#include "font.h"
#include "util.h"

hb_font_t* font;

void get_font_path(const char *name, char *path) {
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcNameParse((const FcChar8*)name);

    FcConfigSubstitute(config, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    char* font_file;
    FcResult result;

    FcPattern* font = FcFontMatch(config, pat, &result);

    if (font) {
	FcChar8* file = NULL;

	if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
	    font_file = (char*) file;
	    strcpy(path, font_file);
	} else {
	    fprintf(stderr, "error: font not found: %s\n", name);
	    exit(1);
	}
    } else {
	fprintf(stderr, "error: font not found: %s\n", name);
	exit(1);
    }

    FcPatternDestroy(font);
    FcPatternDestroy(pat);
    FcConfigDestroy(config);
}

void load_font(XftFont* f) {
    FT_Face face = XftLockFace(f);
    font = hb_ft_font_create(face, NULL);
}

int get_glyphs_shaped(const char *text, int len, struct FtGlyph *glyphs) {
    hb_buffer_t* buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, len, 0, -1);
    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buf, hb_language_from_string("en", -1));

    hb_shape(font, buf, NULL, 0);

    unsigned int glyph_count;
    hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

    struct FtGlyph* g;
    for (int i = 0; i < glyph_count; i++) {
	g = &glyphs[i];
	g->glyph_index = glyph_info[i].codepoint;
	g->x_offset = glyph_pos[i].x_offset/64;
	g->y_offset = glyph_pos[i].y_offset/64;
	g->x_advance = glyph_pos[i].x_advance/64;
	g->y_advance = glyph_pos[i].y_advance/64;
    }

    hb_buffer_destroy(buf);

    return glyph_count;
}

void clean_font() {
    hb_font_destroy(font);
}
