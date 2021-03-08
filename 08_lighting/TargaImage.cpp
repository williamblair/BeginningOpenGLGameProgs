#include <TargaImage.h>
#include <iostream>

namespace Targa
{

Image::Image() :
    width(0),
    height(0),
    bitsPerPixel(0),
    bytesPerPixel(0)
{}
Image::~Image()
{
    Unload();
}    

bool Image::Load(const std::string& fileName)
{
    std::ifstream inFile(fileName, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << __func__ << " failed to open targa image file: " 
                  << fileName << std::endl;
        return false;
    }

    inFile.read((char*)&header, sizeof(Header));
    if (!IsImageTypeSupported(header)) {
        std::cerr << __func__ << " unsupported targa type" << std::endl;
        return false;
    }

    width = header.width;
    height = header.height;

    bitsPerPixel = header.bpp;
    bytesPerPixel = header.bpp / 8;

    if (bytesPerPixel < 3) {
        std::cerr << __func__ << " unsupported bytesPerPixel: " 
                  << bytesPerPixel << std::endl;
        return false;
    }

    unsigned int imageSize = width * height * bytesPerPixel;
    imageData.resize(imageSize);

    // skip past the id if there is one
    if (header.idLength > 0) {
        inFile.ignore(header.idLength);
    }

    bool result = false;

    if (IsUncompressed(header)) {
        result = LoadUncompressed(inFile);
    } else {
        result = LoadCompressed(inFile);
    }

    if ((header.imageDesc & TOP_LEFT) == TOP_LEFT) {
        FlipImageVertically();
    }

    return result;
}

void Image::Unload()
{
    imageData.clear();
}
    
unsigned int Image::GetWidth() const        { return width;             }
unsigned int Image::GetHeight() const       { return height;            }
unsigned int Image::GetBitsPerPixel() const { return bitsPerPixel;      }
const uint8_t* Image::GetImageData() const  { return imageData.data();  }

bool Image::LoadUncompressed(std::ifstream& inFile)
{
    unsigned int imageSize = imageData.size();
    inFile.read((char*)imageData.data(), imageSize);

    // swap R and B components (BGR --> RGB)
    for (unsigned int swap = 0; swap < imageSize; swap += bytesPerPixel)
    {
        uint8_t cswap = imageData[swap];
        imageData[swap] = imageData[swap+2];
        imageData[swap+2] = cswap;
    }

    return true;
}

bool Image::LoadCompressed(std::ifstream& inFile)
{
    unsigned int pixelCount = height * width;
    unsigned int curPixel = 0;
    unsigned int curByte = 0;

    std::vector<uint8_t> colorBuffer(bytesPerPixel);

    do
    {
        uint8_t chunkHeader = 0;
        inFile.read((char*)&chunkHeader, sizeof(uint8_t)); // read in a single byte

        if (chunkHeader < 128)
        {
            chunkHeader++;

            for (short counter = 0; counter < chunkHeader; counter++)
            {
                // read a color chunk
                inFile.read((char*)colorBuffer.data(), bytesPerPixel);

                imageData[curByte+0] = colorBuffer[2];
                imageData[curByte+1] = colorBuffer[1];
                imageData[curByte+2] = colorBuffer[0];

                if (bytesPerPixel == 4) {
                    imageData[curByte + 3] = colorBuffer[3];
                }

                curByte += bytesPerPixel;
                curPixel++;

                // mismatch between sizes
                if (curPixel > pixelCount) {
                    return false;
                }
            }
        }
        // chunkHeader >= 128
        else
        {
            chunkHeader -= 127;

            // read a color chunk
            inFile.read((char*)colorBuffer.data(), bytesPerPixel);

            for (short counter = 0; counter < chunkHeader; counter++)
            {
                imageData[curByte+0] = colorBuffer[2];
                imageData[curByte+1] = colorBuffer[1];
                imageData[curByte+2] = colorBuffer[0];
                
                if (bytesPerPixel == 4) {
                    imageData[curByte + 3] = colorBuffer[3];
                }

                curByte += bytesPerPixel;
                curPixel++;

                // size mismatch
                if (curPixel > pixelCount) {
                    return false;
                }
            }
        }

    } while (curPixel < pixelCount);

    return true;
}

bool Image::IsImageTypeSupported(const Header& header)
{
    // only support color images currently
    return ((header.imageTypeCode == TFT_RGB ||
            header.imageTypeCode == TFT_RLE_RGB) &&
            header.colorMapType == 0);
}

bool Image::IsCompressed(const Header& header)
{
    return (header.imageTypeCode == TFT_RLE_RGB ||
            header.imageTypeCode == TFT_RLE_GRAYSCALE);
}

bool Image::IsUncompressed(const Header& header)
{
    return (header.imageTypeCode == TFT_RGB ||
            header.imageTypeCode == TFT_GRAYSCALE);
}

void Image::FlipImageVertically()
{
    std::vector<uint8_t> flippedData(imageData.size());
    size_t flippedIndex = 0;
    int step = bytesPerPixel;
    for (int row = height - 1; row >= 0; --row) {
        for (int col = 0; col < width * step; ++col) {
            uint8_t* rowData = &imageData[row * width * step];

            for (unsigned int i = 0; i < width * step; ++i) {
                flippedData[flippedIndex] = *rowData;
                flippedIndex++;
                rowData++;
            } 
        }
    }

    imageData.assign(flippedData.begin(), flippedData.end());
}

} // end namespace Targa

