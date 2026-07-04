#pragma once
#include "inc.h"

namespace core {
	struct glyph {
	    int width;
	    int height;
	
	    int bearingX;
	    int bearingY;
	
	    int advance; // pixels
	
	    float u0, v0, u1, v1; // atlas UVs
	};
	
	class font {
	public:
	    bool Load(const fs::path& path, int pixelSize = 48);
	
	    const glyph& GetGlyph(char c) const { return _glyphs.at(c); }
	
	    const std::vector<uint8_t>& GetAtlas() const { return _atlas; }
	    int GetAtlasWidth()  const { return _atlasWidth; }
	    int GetAtlasHeight() const { return _atlasHeight; }
	    int GetDescent() const { return _descent; }
	    int GetAscent() const { return _ascent; }
	    int GetLineHeight() const { return _lineHeight; }
	private:
	    std::unordered_map<char, glyph> _glyphs;
	
	    std::vector<uint8_t> _atlas; // 1 byte per pixel (alpha)
	    int _atlasWidth = 0;
	    int _atlasHeight = 0;
	    int _ascent = 0;
	    int _descent = 0;
	    int _lineHeight = 0;
	};
}
