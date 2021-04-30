#include <fstream>
#include <iostream>
#include <HeightMap.h>

#define HEIGHTMAP_GRASS_TEXTURE "assets/grass.tga"
#define HEIGHTMAP_WATER_TEXTURE "assets/water.tga"
#define HEIGHTMAP_VERTEX_SHADER "shaders/vertexShader.glsl"
#define HEIGHTMAP_FRAGMENT_SHADER "shaders/fragmentShader.glsl"
#define HEIGHTMAP_TEXTURE "assets/heightmap.raw"
bool HeightMap::shaderLoaded;
Targa::Image HeightMap::grassTexture;
GLuint HeightMap::grassTexID;
Shader HeightMap::heightmapShader;

HeightMap::HeightMap(GameWorld* const gameWorld) :
    Entity(gameWorld)
{
    width = 0;
    vertexBuffer = 0;
    normalBuffer = 0;
    texCoordBuffer = 0;
    indexBuffer = 0;
    colorBuffer = 0;

    waterVertexBuffer = 0;
    waterIndexBuffer = 0;
    waterTexCoordBuffer = 0;

    shaderLoaded = false;
}

HeightMap::~HeightMap()
{
    if (vertexBuffer != 0) glDeleteBuffers(1, &vertexBuffer);
    if (texCoordBuffer != 0) glDeleteBuffers(1, &texCoordBuffer);
    if (colorBuffer != 0) glDeleteBuffers(1, &colorBuffer);
    if (normalBuffer != 0) glDeleteBuffers(1, &normalBuffer);
    if (indexBuffer != 0) glDeleteBuffers(1, &indexBuffer);

    if (waterVertexBuffer != 0) glDeleteBuffers(1, &waterVertexBuffer);
    if (waterIndexBuffer != 0) glDeleteBuffers(1, &waterIndexBuffer);
    if (waterTexCoordBuffer != 0) glDeleteBuffers(1, &waterTexCoordBuffer);
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
        //colors[i].x = 0.0f;
        //colors[i].y = ((float)fileBytes[i]) / 256.0f;
        //colors[i].z = 0.0f;
        // Let texture do the coloring
        colors[i].x = 1.0f;
        colors[i].y = 1.0f;
        colors[i].z = 1.0f;
    }

    GenerateVertices(sizeScale);
    GenerateTexCoords();
    GenerateIndices();
    GenerateNormals();

    GenerateWaterVertices();
    GenerateWaterTexCoords();
    GenerateWaterIndices();

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

void HeightMap::GenerateTexCoords()
{
    texCoords.clear();
    for (int z = 0; z < width; ++z) {
        for (int x = 0; x < width; ++x) {
            float s = (float(x) / float(width)) * 8.0f;
            float t = (float(z) / float(width)) * 8.0f;
            texCoords.push_back(Vec2(s,t));
        }
    }
}

void HeightMap::GenerateNormals()
{
    std::vector<Vec3> faceNormals; // tmp calculation
    std::vector<int> shareCount;

    normals.resize(positions.size());
    shareCount.resize(positions.size());

    for (size_t i = 0; i < shareCount.size(); ++i) {
        shareCount[i] = 0;
    }

    size_t numTriangles = indices.size() / 3;
    faceNormals.resize(numTriangles);
    
    for (unsigned int i = 0; i < numTriangles; ++i) {
        Vec3* v1 = &positions[indices[(i*3) + 0]];
        Vec3* v2 = &positions[indices[(i*3) + 1]];
        Vec3* v3 = &positions[indices[(i*3) + 2]];

        Vec3 vec1, vec2;

        vec1.x = v2->x - v1->x;
        vec1.y = v2->y - v1->y;
        vec1.z = v2->z - v1->z;

        vec2.x = v3->x - v1->x;
        vec2.y = v3->y - v1->y;
        vec2.z = v3->z - v1->z;

        Vec3* normal = &faceNormals[i];
        *normal = cross(vec1, vec2);
        normalize(*normal);

        for (int j = 0; j < 3; ++j) {
            int index = indices[(i*3) + j];
            normals[index].x += normal->x;
            normals[index].y += normal->y;
            normals[index].z += normal->z;
            shareCount[index]++;
        }
    }

    for (unsigned int i = 0; i < positions.size(); ++i) {
        normals[i].x = normals[i].x / shareCount[i];
        normals[i].y = normals[i].y / shareCount[i];
        normals[i].z = normals[i].z / shareCount[i];
        normalize(normals[i]);
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

void HeightMap::GenerateWaterVertices()
{
    size_t i = 0;
    waterPositions.resize(width * width);
    for (float z = float(-((int)width) / 2); z <= (int)width / 2; z++) {
        for (float x = float(-((int)width) / 2); x <= (int)width / 2; x++) {
            if (i >= waterPositions.size()) {
                std::cerr << "INVALID I!!!!" << std::endl;
                return;
            }
            waterPositions[i].x = x;
            waterPositions[i].y = 4.0f;
            waterPositions[i].z = z;
            i++;
        }
    }
}

void HeightMap::GenerateWaterIndices()
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
    waterIndices.clear();
    for (int z = 0; z < width - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            waterIndices.push_back(z     * width + x);
            waterIndices.push_back((z+1) * width + x);
            waterIndices.push_back(z     * width + x + 1);

            waterIndices.push_back((z+1) * width + x);
            waterIndices.push_back((z+1) * width + x + 1);
            waterIndices.push_back(z     * width + x + 1);
        }
    }
}

void HeightMap::GenerateWaterTexCoords()
{
    waterTexCoords.clear();
    for (int z = 0; z < width; ++z) {
        for (int x = 0; x < width; ++x) {
            float s = (float(x) / float(width)) * 8.0f;
            float t = (float(z) / float(width)) * 8.0f;
            waterTexCoords.push_back(Vec2(s,t));
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

    normalBuffer = 0;
    glCreateBuffers(1, &normalBuffer);
    if (!normalBuffer) {
        std::cerr << __func__ << " failed to gen normal buffer" << std::endl;
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 normals.size()*sizeof(Vec3),
                 (void*)normals.data(), 
                 GL_STATIC_DRAW);

    texCoordBuffer = 0;
    glCreateBuffers(1, &texCoordBuffer);
    if (!texCoordBuffer) {
        std::cerr << __func__ << " failed to gen tex coord buffer" << std::endl;
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 texCoords.size()*sizeof(Vec2),
                 (void*)texCoords.data(), 
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

    waterVertexBuffer = 0;
    glCreateBuffers(1, &waterVertexBuffer);
    if (!waterVertexBuffer) {
        std::cerr << __func__ << " failed to gen water vertex buffer" << std::endl;
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, waterVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 waterPositions.size()*sizeof(Vec3),
                 (void*)waterPositions.data(), 
                 GL_STATIC_DRAW);

    waterTexCoordBuffer = 0;
    glCreateBuffers(1, &waterTexCoordBuffer);
    if (!waterTexCoordBuffer) {
        std::cerr << __func__ << " failed to gen water tex coord buffer" << std::endl;
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, waterTexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 waterTexCoords.size()*sizeof(Vec2),
                 (void*)waterTexCoords.data(), 
                 GL_STATIC_DRAW);

    waterIndexBuffer = 0;
    glCreateBuffers(1, &waterIndexBuffer);
    if (!waterIndexBuffer) {
        std::cerr << __func__ << " failed to gen water index buffer" << std::endl;
        return;
    } 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 waterIndices.size()*sizeof(uint32_t), 
                 (void*)waterIndices.data(),
                 GL_STATIC_DRAW);
}

void HeightMap::Draw(Shader& shader)
{
    shader.Bind();
    GLuint aPosition = shader.GetAttribute("aPosition");
    GLuint aNormal = shader.GetAttribute("aNormal");
    GLuint aTexCoord0 = shader.GetAttribute("aTexCoord0");
    GLuint aColor = shader.GetAttribute("aColor");
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(aPosition,
                          3, // Vec3 x,y,z
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aPosition);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(aNormal,
                          3, // Vec3 x,y,z
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aNormal);

    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glVertexAttribPointer(aTexCoord0,
                          2, // Vec2 x,y
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aTexCoord0);

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

void HeightMap::DrawWater(Shader& shader)
{
    GLuint aPosition = shader.GetAttribute("aPosition");
    GLuint aTexCoord0 = shader.GetAttribute("aTexCoord0");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    glBindBuffer(GL_ARRAY_BUFFER, waterVertexBuffer);
    glVertexAttribPointer(aPosition,
                          3, // Vec3 x,y,z
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aPosition);

    glBindBuffer(GL_ARRAY_BUFFER, waterTexCoordBuffer);
    glVertexAttribPointer(aTexCoord0,
                          2, // Vec2 x,y
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aTexCoord0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterIndexBuffer);
    glDrawElements(GL_TRIANGLES, waterIndices.size(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(aPosition);
    glDisableVertexAttribArray(aTexCoord0);
    glDisable(GL_BLEND);
}


void HeightMap::OnPrepare(float dt)
{
}

void HeightMap::OnRender()
{
    static Vec3 lightColor(1.0f,1.0f,1.0f);
    static Vec3 lightPos(0.0f,18.0f,0.0f);
    static Vec3 ambientColor(0.2f,0.2f,0.2f);
    
    heightmapShader.Bind();
    Mat4 modelMat = transformToMat4(transform);
    Mat4 normalMat = inverse(transposed(modelMat));
    Mat4* viewMat = gameWorld->GetViewMatrix();
    Mat4* projMat = gameWorld->GetProjectionMatrix();
    heightmapShader.SetUniform("uModel", modelMat);
    heightmapShader.SetUniform("uNormal", normalMat);
    heightmapShader.SetUniform("uView", *viewMat);
    heightmapShader.SetUniform("uProjection", *projMat);
    heightmapShader.SetUniform("uLightColor", lightColor);
    heightmapShader.SetUniform("uLightPos", lightPos);
    heightmapShader.SetUniform("uAmbientColor", ambientColor);
    heightmapShader.SetUniform("uTexture0", 0); // gl texture 0
    glBindTexture(GL_TEXTURE_2D, grassTexID);
    Draw(heightmapShader);
    heightmapShader.UnBind();
}

void HeightMap::OnPostRender()
{
}

bool HeightMap::OnInitialize()
{
    if (!shaderLoaded)
    {
        std::cout << "Heightmap Initialize" << std::endl;
        heightmapShader.Load(HEIGHTMAP_VERTEX_SHADER, HEIGHTMAP_FRAGMENT_SHADER);
        heightmapShader.Bind();
        if (!Load(HEIGHTMAP_TEXTURE)) {
            return false;
        }
        if (!grassTexture.Load(HEIGHTMAP_GRASS_TEXTURE)) {
            return false;
        }
        grassTexID = Targa::Image::MakeTexture(grassTexture);
        heightmapShader.UnBind();
        shaderLoaded = true;
    }
    return true;
}

void HeightMap::OnShutdown()
{
}

void HeightMap::OnCollision(Entity* collider)
{
}

