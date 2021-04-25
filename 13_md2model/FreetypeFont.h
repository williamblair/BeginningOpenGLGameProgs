#ifndef FREETYPE_FONT_H_INCLUDED
#define FREETYPE_FONT_H_INCLUDED

#include <string>
#include <map>

#include <GL/glew.h>

// Freetype headers
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <Shader.h>

class FreetypeFont
{
public:
    FreetypeFont(const std::string& fontName,
                 int screenWidth, int screenHeight,
                 int fontSize = 16);
    ~FreetypeFont();

    bool Init();
    void PrintString(const std::string& str, float x, float y);

private:
    GLuint textureIDs[128]; // a texture for each character

    int fontSize;
    int screenWidth;
    int screenHeight;
    std::string fontName;

    GLuint texCoordBufID;
    GLuint vertexBufID;
    
    void setOrthoMode();
    void unsetOrthoMode();
    bool genCharacterTexture(unsigned char ch, FT_Face fontInfo);

    Shader shader;

    std::map<char, std::pair<int, int>> glyphDimensions;
    std::map<char, std::pair<int, int>> glyphPositions;
    std::map<char, int> glyphAdvances;
};


#endif // FREETYPE_FONT_H_INCLUDED

