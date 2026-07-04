#include "font.h"

#include <freetype2/ft2build.h>
#pragma comment(lib, "freetype.lib")
#include FT_FREETYPE_H

namespace core {
	bool font::Load(const fs::path& path, int pixelSize)
	{
	    FT_Library ft;
	    if (FT_Init_FreeType(&ft))
	        return false;
	
	    FT_Face face;
	    if (FT_New_Face(ft, path.string().c_str(), 0, &face))
	        return false;
	
	    FT_Set_Pixel_Sizes(face, 0, pixelSize);
	
	    // --- Build simple horizontal atlas (ASCII 32–126) ---
	    const int firstChar = 32;
	    const int lastChar = 126;
	
	    int rowHeight = 0;
	    int atlasWidth = 0;
	
	    // First pass: compute atlas width
	    for (int c = firstChar; c <= lastChar; c++)
	    {
	        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
	            continue;
	
	        atlasWidth += face->glyph->bitmap.width + 1;
	        rowHeight = MAX(rowHeight, (int)face->glyph->bitmap.rows);
	    }
	
	    _atlasWidth = atlasWidth;
	    _atlasHeight = rowHeight;
	    _descent = face->size->metrics.descender >> 6;
	    _ascent = face->size->metrics.ascender >> 6;
	    _lineHeight = face->size->metrics.height >> 6;
	
	    _atlas.resize(_atlasWidth * _atlasHeight);
	    std::fill(_atlas.begin(), _atlas.end(), 0);
	
	    // Second pass: copy glyphs
	    int xOffset = 0;
	
	    for (int c = firstChar; c <= lastChar; c++)
	    {
	        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
	            continue;
	
	        FT_GlyphSlot g = face->glyph;
	
	        int w = g->bitmap.width;
	        int h = g->bitmap.rows;
	
	        for (int y = 0; y < h; y++)
	        {
	            memcpy(
	                &_atlas[(y * _atlasWidth) + xOffset],
	                &g->bitmap.buffer[y * w],
	                w
	            );
	        }
	
	        glyph glyphData;
	        glyphData.width = w;
	        glyphData.height = h;
	        glyphData.bearingX = g->bitmap_left;
	        glyphData.bearingY = g->bitmap_top;
	        glyphData.advance = g->advance.x >> 6;
	
	        glyphData.u0 = (float)xOffset / _atlasWidth;
	        glyphData.v0 = 0.0f;
	        glyphData.u1 = (float)(xOffset + w) / _atlasWidth;
	        glyphData.v1 = (float)h / _atlasHeight;
	
	        _glyphs[(char)c] = glyphData;
	
	        xOffset += w + 1;
	    }
	
	    FT_Done_Face(face);
	    FT_Done_FreeType(ft);
	
	    return true;
	}
}
