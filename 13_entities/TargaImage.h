#ifndef TARGA_IMAGE_H_INCLUDED
#define TARGA_IMAGE_H_INCLUDED

#include <vector>
#include <string>
#include <fstream>
#include <cstdint>

#include <GL/glew.h>

namespace Targa
{

// imageDesc bitmasks,
// bit 4 is left-to-right ordering,
// bit 5 is top-to-bottom ordering
enum IMAGE_ORIENTATIONS
{
    BOTTOM_LEFT  = 0x00,    // first px is bottom left
    BOTTOM_RIGHT = 0x10,    // first px is bottom right
    TOP_LEFT     = 0x20,    // first px is top left
    TOP_RIGHT    = 0x30     // first px is top right
};

enum FILE_TYPES
{
    TFT_NO_DATA = 0,
    TFT_INDEXED = 1,
    TFT_RGB = 2,
    TFT_GRAYSCALE = 3,
    TFT_RLE_INDEXED = 9,
    TFT_RLE_RGB = 10,
    TFT_RLE_GRAYSCALE = 11
};

struct Header
{
    uint8_t idLength;
    uint8_t colorMapType;
    uint8_t imageTypeCode;
    uint8_t colorMapSpec[5];
    uint16_t xOrigin;
    uint16_t yOrigin;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;        // bits per pixel
    uint8_t imageDesc;
};

class Image
{
public:
    Image();
    virtual ~Image();
    
    bool Load(const std::string& fileName);
    void Unload();
    
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    unsigned int GetBitsPerPixel() const;

    const uint8_t* GetImageData() const;
    
    static inline GLuint MakeTexture(Image& tgaImage)
    {
        GLuint texID;
    
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,
                    0, 
                    GL_RGB8, 
                    tgaImage.GetWidth(), 
                    tgaImage.GetHeight(), 
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    tgaImage.GetImageData());

        return texID;
    }

private:
    Header header;
    unsigned int width;
    unsigned int height;
    unsigned int bitsPerPixel;
    unsigned int bytesPerPixel;
    
    std::vector<uint8_t> imageData;
    
    bool LoadUncompressed(std::ifstream& inFile);
    bool LoadCompressed(std::ifstream& inFile);

    bool IsImageTypeSupported(const Header& header);
    bool IsCompressed(const Header& header);
    bool IsUncompressed(const Header& header);

    void FlipImageVertically();
};

} // end namespace Targa

#endif // TARGA_IMAGE_H_INCLUDED


