#include <FreetypeFont.h>
#include <iostream>
#include <vector>

#define FONT_VERTEX_SHADER "shaders/fontVertex.glsl"
#define FONT_FRAGMENT_SHADER "shaders/fontFragment.glsl"

// screen pixels to OpenGL NDC coordinates
static inline float xPixelToNDCCoord(int pxVal, int sWidth)
{
    return -1.0f + (2.0f * float(pxVal) / float(sWidth));
}
static inline float yPixelToNDCCoord(int pxVal, int sHeight)
{
    return -1.0f + (2.0f * float(sHeight - pxVal) / float(sHeight));
}
static inline float pxSizeToNDCSize(int pxSize, int sSize)
{
    return 2.0f * float(pxSize) / float(sSize);
}

FreetypeFont::FreetypeFont(const std::string& fontName,
                           int screenWidth, int screenHeight,
                           int fontSize) :
    fontSize(fontSize),
    screenWidth(screenWidth),
    screenHeight(screenHeight),
    fontName(fontName)
{
}

FreetypeFont::~FreetypeFont()
{
    glDeleteTextures(128, textureIDs);
}

bool FreetypeFont::Init()
{
    shader.Load(FONT_VERTEX_SHADER, FONT_FRAGMENT_SHADER);
    
    FT_Library library; // freetype library instance

    if (FT_Init_FreeType(&library))
    {
        std::cerr << __func__ << ": failed to init freetype" << std::endl;
        return false;
    }

    FT_Face fontInfo;
    if (FT_New_Face(library, fontName.c_str(), 0, &fontInfo))
    {
        std::cerr << __func__ << ": failed to load font " << fontName
                  << std::endl;
        return false;
    }

    // Freetype heights are 1/64th pixel size
    // 96x96 is dots per inch (dpi)
    FT_Set_Char_Size(fontInfo,
                     fontSize * 64, fontSize * 64,
                     96, 96);

    glGenTextures(128, textureIDs);

    for (unsigned char ch = 0; ch < 128; ++ch)
    {
        if (!genCharacterTexture(ch, fontInfo))
        {
            std::cerr << __func__ << ": failed to gen texture for char "
                      << ch << std::endl;
            return false;
        }
    }

    FT_Done_Face(fontInfo);
    FT_Done_FreeType(library);

    // will be updated for each character anyways
    float screenFontSize = pxSizeToNDCSize(fontSize, screenWidth);
    float vertices[] = 
    {
        0.0f, 0.0f, // bottom left
        screenFontSize, 0.0f, // bottom right
        screenFontSize, screenFontSize, // top right
        
        0.0f, 0.0f, // bottom left
        screenFontSize, screenFontSize, // top right
        0.0f, screenFontSize // top left
    };

    glGenBuffers(1, &vertexBufID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
    glBufferData(GL_ARRAY_BUFFER,
                 6*2 * sizeof(GLfloat),
                 &vertices[0],
                 GL_STATIC_DRAW);

    float texCoords[] =
    {
        0.0f, 1.0f, // bottom left
        1.0f, 1.0f, // bottom right
        1.0f, 0.0f, // top right
        
        0.0f, 1.0f, // bottom left
        1.0f, 0.0f, // top right
        0.0f, 0.0f // top left
    };

    glGenBuffers(1, &texCoordBufID);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
    glBufferData(GL_ARRAY_BUFFER,
                 6*2 * sizeof(GLfloat),
                 &texCoords[0],
                 GL_DYNAMIC_DRAW);
                 
    return true;
}

void FreetypeFont::PrintString(const std::string& str, float x, float y)
{
    shader.Bind();
    
    GLuint aPosition = shader.GetAttribute("aPosition");
    GLuint aTexCoord0 = shader.GetAttribute("aTexCoord0");
    
    glEnableVertexAttribArray(aPosition);
    glEnableVertexAttribArray(aTexCoord0);    
    
    shader.SetUniform("uTexture0", 0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    float normX = xPixelToNDCCoord(x, screenWidth);
    float normY = yPixelToNDCCoord(y, screenHeight);

    Vec2 charTransform(normX, normY);
    for (size_t i = 0; i < str.size(); ++i)
    {
        int ch = int(str[i]);
        
        float normWidth = pxSizeToNDCSize(glyphDimensions[ch].first, screenWidth);
        float normHeight = pxSizeToNDCSize(glyphDimensions[ch].second, screenHeight);
        float vertices[] =
        {
            0.0f, 0.0f, // bottom left
            normWidth, 0.0f, // bottom right
            normWidth, normHeight, // top right
            
            0.0f, 0.0f, // bottom left
            normWidth, normHeight, // top right
            0.0f, normHeight // top left
        };
        
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        6 * 2 * sizeof(float),
                        &vertices[0]);
        glVertexAttribPointer(aPosition,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              0);
        
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
        glVertexAttribPointer(aTexCoord0,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              0);
                              
        
                              
        glBindTexture(GL_TEXTURE_2D, textureIDs[ch]);
        
        shader.SetUniform("uTranslate", charTransform);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        float advance = pxSizeToNDCSize(glyphAdvances[ch], screenWidth);
        charTransform.x += advance;
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    glDisableVertexAttribArray(aPosition);
    glDisableVertexAttribArray(aTexCoord0);
}

bool FreetypeFont::genCharacterTexture(unsigned char ch, FT_Face fontInfo)
{
    if (FT_Load_Glyph(fontInfo,
                      FT_Get_Char_Index(fontInfo, ch),
                      FT_LOAD_DEFAULT))
    {
        return false;
    }

    FT_Glyph glyph;
    if (FT_Get_Glyph(fontInfo->glyph, &glyph))
    {
        return false;
    }

    if (FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1))
    {
        return false;
    }

    FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph) glyph;

    int width = bitmapGlyph->bitmap.width ? bitmapGlyph->bitmap.width : 16;
    int rows = bitmapGlyph->bitmap.rows ? bitmapGlyph->bitmap.rows : 16;

    // allocate space for font texture (r,g,b,a)
    int imageSize = width * rows * 4;

    std::vector<unsigned char> imageData(imageSize);
    for (int i = 0; i < imageSize / 4; ++i)
    {
        unsigned char gray = 0;
        if (bitmapGlyph->bitmap.buffer)
        {
            gray = bitmapGlyph->bitmap.buffer[i];
        }
        
        imageData[i*4] = gray;
        imageData[i*4 + 1] = gray;
        imageData[i*4 + 2] = gray;
        imageData[i*4 + 3] = gray;
    }

    glBindTexture(GL_TEXTURE_2D, textureIDs[ch]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width, rows,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 imageData.data());

    glyphDimensions[ch] = std::make_pair(width, rows);
    glyphPositions[ch] = std::make_pair(bitmapGlyph->left, bitmapGlyph->top);
    glyphAdvances[ch] = fontInfo->glyph->advance.x / 64;

    return true;
}

