#include <fstream>
#include <iostream>
#include <HeightMap.h>

HeightMap::HeightMap()
{
    width = 0;
    vertexBuffer = 0;
    indexBuffer = 0;
    colorBuffer = 0;
}

HeightMap::~HeightMap()
{
    if (vertexBuffer != 0) glDeleteBuffers(1, &vertexBuffer);
    if (colorBuffer != 0) glDeleteBuffers(1, &colorBuffer);
    if (indexBuffer != 0) glDeleteBuffers(1, &indexBuffer);
}

bool HeightMap::Load(const std::string& rawFileName,
                     const unsigned int width,
                     const float heightScale,
                     const float sizeScale)
{
    this->width = width;

    std::ifstream rawFile(rawFileName, std::ios::binary);
    if (!rawFile.is_open()) {
        std::cerr << __func__ << ": failed to open " << rawFileName << std::endl;
        return false;
    }
    std::vector<uint8_t> fileBytes;
    rawFile.seekg(0, std::ios::end);
    size_t fileSize = rawFile.tellg();
    rawFile.seekg(0, std::ios::beg);
    fileBytes.resize(fileSize);
    rawFile.read((char*)fileBytes.data(), fileBytes.size());
    if (fileBytes.size() != width * width) {
        std::cerr << "File size bytes != width*width" << std::endl;
        return false;
    }

    heights.resize(width * width);
    colors.resize(width * width);
    for (size_t i = 0; i < heights.size(); ++i) {
        heights[i] = (((float)fileBytes[i]) / 256.0f) * heightScale;
        colors[i].x = 0.0f;
        colors[i].y = ((float)fileBytes[i]) / 256.0f;
        colors[i].z = 0.0f;
    }

    GenerateVertices(sizeScale);
    GenerateIndices();
    GenerateBuffers();

    return true;
}

void HeightMap::GenerateVertices(const float sizeScale)
{
    size_t i = 0;
    positions.resize(width * width);
    for (float z = float(-((int)width) / 2); z <= (int)width / 2; z++) {
        for (float x = float(-((int)width) / 2); x <= (int)width / 2; x++) {
            if (i >= positions.size()) {
                std::cerr << "INVALID I!!!!" << std::endl;
                return;
            }
            positions[i].x = x;
            positions[i].y = heights[i];
            positions[i].z = z;
            i++;
        }
    }
}

void HeightMap::GenerateIndices()
{
    /*
        We loop through building the triangles that
        make up each grid square in the heightmap

        (z*w+x) *----* (z*w+x+1)
                |   /| 
                |  / | 
                | /  |
        ((z+1)*w+x)*----* ((z+1)*w+x+1)
    */
    indices.clear();
    for (int z = 0; z < width - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            indices.push_back(z     * width + x);
            indices.push_back((z+1) * width + x);
            indices.push_back(z     * width + x + 1);

            indices.push_back((z+1) * width + x);
            indices.push_back((z+1) * width + x + 1);
            indices.push_back(z     * width + x + 1);
        }
    }
}

void HeightMap::GenerateBuffers()
{
    vertexBuffer = 0;
    glCreateBuffers(1, &vertexBuffer);
    if (!vertexBuffer) {
        std::cerr << __func__ << " failed to gen vertex buffer" << std::endl;
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 positions.size()*sizeof(Vec3),
                 (void*)positions.data(), 
                 GL_STATIC_DRAW);

    colorBuffer = 0;
    glCreateBuffers(1, &colorBuffer);
    if (!colorBuffer) {
        std::cerr << __func__ << " failed to gen vertex buffer" << std::endl;
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 colors.size()*sizeof(Vec3),
                 (void*)colors.data(), 
                 GL_STATIC_DRAW);

    indexBuffer = 0;
    glCreateBuffers(1, &indexBuffer);
    if (!indexBuffer) {
        std::cerr << __func__ << " failed to gen index buffer" << std::endl;
        return;
    } 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size()*sizeof(uint32_t), 
                 (void*)indices.data(),
                 GL_STATIC_DRAW);
}

void HeightMap::Draw(Shader& shader)
{
    shader.Bind();
    GLuint aPosition = shader.GetAttribute("aPosition");
    GLuint aColor = shader.GetAttribute("aColor");
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(aPosition,
                          3, // Vec3 x,y,z
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aPosition);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(aColor,
                          3, // Vec3 x,y,z
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aColor);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

