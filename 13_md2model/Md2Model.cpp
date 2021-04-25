#include <Md2Model.h>
#include <fstream>
#include <iostream>

#define DECL_ANIM(name, startFrame, endFrame) \
    const Md2Model::Animation Md2Model::Animation::name = Md2Model::Animation(startFrame, endFrame)
#define DECL_ANIM_NOLOOP(name, startFrame, endFrame) \
    const Md2Model::Animation Md2Model::Animation::name = Md2Model::Animation(startFrame, endFrame, false)

// static member, start frame, end frame    
DECL_ANIM(Idle, 0, 39);
DECL_ANIM(Run, 40, 45);
DECL_ANIM(Attack, 46, 53);
DECL_ANIM(Pain1, 54, 57);
DECL_ANIM(Pain2, 58, 61);
DECL_ANIM(Pain3, 62, 65);
DECL_ANIM(Jump, 66, 71);
DECL_ANIM(FlipOff, 72, 83);
DECL_ANIM(Salute, 84, 94);
DECL_ANIM(Taunt, 95, 111);
DECL_ANIM(Wave, 112, 122);
DECL_ANIM(Point, 123, 134);
DECL_ANIM(CrouchIdle, 135, 153);
DECL_ANIM(CrouchWalk, 154, 159);
DECL_ANIM(CrouchAttack, 160, 168);
DECL_ANIM(CrouchPain, 169, 172);
DECL_ANIM(CrouchDeath, 173, 177);
DECL_ANIM_NOLOOP(Death1, 178, 183);
DECL_ANIM_NOLOOP(Death2, 184, 189);
DECL_ANIM_NOLOOP(Death3, 190, 197);

#undef DECL_ANIM
#undef DECL_ANIM_NOLOOP

Md2Model::Md2Model() :
    currentFrame(0),
    nextFrame(1),
    startFrame(0),
    endFrame(0),
    interpolation(0.0f)
{}
Md2Model::~Md2Model()
{}
    
bool Md2Model::Load(const std::string& fileName)
{
    std::ifstream inFile(fileName, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << __func__ << ": failed to open Md2 File: " << fileName << std::endl;
        return false;
    }
    
    Header header;
    inFile.read((char*)&header, sizeof(Header));
    
    // Verify file header correctness
    if (header.magic[0] != 'I' ||
        header.magic[1] != 'D' ||
        header.magic[2] != 'P' ||
        header.magic[3] != '2')
    {
        std::cerr << __func__ << ": " << fileName << ": Invalid header: magic != IDP2" << std::endl;
        return false;
    }
    
    if (header.version != 8)
    {
        std::cerr << __func__ << ": " << fileName << ": Invalid header: version != 8" << std::endl;
        return false;
    }
    
    if (!inFile) {
        std::cout << "Bad file after header read" << std::endl;
        return false;
    }
    
    // reserve space for MD2 data
    skins.resize(header.numSkins);
    texCoords.resize(header.numTexCoords);
    md2TexCoords.resize(header.numTexCoords);
    triangles.resize(header.numTriangles);
    keyFrames.resize(header.numFrames);
    
    //for (KeyFrame& frame : keyFrames)
    for (size_t i = 0; i < keyFrames.size(); ++i)
    {
        keyFrames[i].vertices.resize(header.numVertices);
        keyFrames[i].md2Vertices.resize(header.numVertices);
    }
    
    std::cout << "Keyframes size setvertices: " << keyFrames.size() << std::endl;
    
    // read MD2 components
#define READ_DATA(offset, vec, number, type) \
    inFile.seekg(offset);                    \
    inFile.read((char*)vec.data(), number * sizeof(type))
    
    READ_DATA(header.skinOffset, skins, header.numSkins, Skin);    
    READ_DATA(header.texCoordOffset, md2TexCoords, header.numTexCoords, TexCoord);
    READ_DATA(header.triangleOffset, triangles, header.numTriangles, Triangle);

#undef READ_DATA
    
    inFile.seekg(header.frameOffset);
    for (int i = 0; i < header.numFrames; ++i)
    {
        KeyFrame* f = &keyFrames[i];
        inFile.read((char*)(f->scale), 3 * sizeof(float));
        if (!inFile) { std::cout << "Failed to read\n"; return false;}
        inFile.read((char*)(f->translate), 3 * sizeof(float));
        if (!inFile) { std::cout << "Failed to read\n"; return false;}
        inFile.read((char*)(f->name), 16 * sizeof(char));
        if (!inFile) { std::cout << "Failed to read\n"; return false;}
        inFile.read((char*)(f->md2Vertices.data()), header.numVertices * sizeof(Vertex));
        if (!inFile) { std::cout << "Failed to read\n"; return false;}
    }
    
    // scale MD2 vertices into regular OpenGL vertices
    radii.clear();
    for (KeyFrame& frame : keyFrames)
    {
        float min = 10000.0f;
        float max = -10000.0f;
        
        int k = 0;
        for (Vec3& vertex : frame.vertices)
        {
            vertex.x = frame.scale[0] * frame.md2Vertices[k].v[0] + frame.translate[0];
            vertex.z = frame.scale[1] * frame.md2Vertices[k].v[1] + frame.translate[1];
            vertex.y = frame.scale[2] * frame.md2Vertices[k].v[2] + frame.translate[2];
            
            vertex.x /= 64.0f;
            vertex.y /= 64.0f;
            vertex.z /= 64.0f;
                       
            ++k;
            
            if (vertex.y < min) min = vertex.y;
            if (vertex.y > max) max = vertex.y;
        }
        
        float frameRadius = (max - min) / 2.0f;
        radii.push_back(frameRadius);
    }
        
    // scale tex coords into regular OpenGL tex coords
    int i = 0;
    for (Vec2& texCoord : texCoords)
    {
        texCoord.x = float(md2TexCoords[i].s) / float(header.skinWidth);
        texCoord.y = 1.0f - (float(md2TexCoords[i].t) / float(header.skinHeight));
        ++i;
    }
    
    inFile.close();
    
    reorganizeVertices();
    stripTextureNames();
    
    interpolatedFrame.vertices = keyFrames[0].vertices;
    currentAnimName = std::string(keyFrames[0].name);
    std::cout << "interpolated frame vertices size: " << interpolatedFrame.vertices.size() << std::endl;
        
    genBuffers();

    return true;
}

void Md2Model::Update(const float dt)
{
    const float FRAMES_PER_SECOND = 8.0f;
    interpolation += dt * FRAMES_PER_SECOND;
    if (interpolation >= 1.0f)
    {
        currentFrame = nextFrame++;
        if (nextFrame > endFrame)
        {
            if (loopAnim)
            {
                nextFrame = startFrame;
            }
            else
            {
                nextFrame = endFrame;
                startFrame = endFrame;
            }
        }
        
        interpolation = 0.0f;
    }
    
    float t = interpolation;
    int i = 0;
    for (Vec3& vertex : interpolatedFrame.vertices)
    {
#define LERP(a, b, t) (a) + ((t) * ((b) - (a)))
        
        float x1 = keyFrames[currentFrame].vertices[i].x;
        float x2 = keyFrames[nextFrame].vertices[i].x;
        vertex.x = LERP(x1, x2, t);
        
        float y1 = keyFrames[currentFrame].vertices[i].y;
        float y2 = keyFrames[nextFrame].vertices[i].y;
        vertex.y = LERP(y1, y2, t);
        
        float z1 = keyFrames[currentFrame].vertices[i].z;
        float z2 = keyFrames[nextFrame].vertices[i].z;
        vertex.z = LERP(z1, z2, t);
        
#undef LERP
        ++i;
    }
    
    // send the interpolated vertices to OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    3 * sizeof(float) * interpolatedFrame.vertices.size(),
                    interpolatedFrame.vertices.data());
}

void Md2Model::Render(Shader& shader)
{
    GLuint aPosition = shader.GetAttribute("aPosition");
    GLuint aTexCoord = shader.GetAttribute("aTexCoord0");
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
    glVertexAttribPointer(aPosition,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aPosition);
    
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
    glVertexAttribPointer(aTexCoord,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);
    glEnableVertexAttribArray(aTexCoord);
    
    glDrawArrays(GL_TRIANGLES, 0, interpolatedFrame.vertices.size());
    
    glDisableVertexAttribArray(aPosition);
    glDisableVertexAttribArray(aTexCoord);
}

// Because some vertices are reused, different texture indices
// by default can refer to the same vertex indices, so we copy these
// vertices so there are no conflictions
void Md2Model::reorganizeVertices()
{
    std::vector<Vec3> tmpVertices;
    std::vector<Vec2> tmpTexCoords;
    
    bool texCoordsDone = false;
    
    for (KeyFrame& frame : keyFrames)
    {
        tmpVertices.clear();
        
        for (uint32_t i = 0; i < triangles.size(); ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                tmpVertices.push_back(frame.vertices[triangles[i].vertIndex[j]]);
                
                // only copy the texture coords once
                if (!texCoordsDone)
                {
                    tmpTexCoords.push_back(texCoords[triangles[i].texCoordIndex[j]]);
                }
            }
        }
        
        // tex coords shared between frames; only copy once
        texCoordsDone = true;
        
        // copy the reassigned vertex data
        frame.vertices = tmpVertices;
    }
    
    // copy the reassigned tex coord
    texCoords = tmpTexCoords;
}

// remove any folder prefixes from the model texture file names
void Md2Model::stripTextureNames()
{
    for (Skin& skin : skins)
    {
        std::string texture = skin.name;
        
        size_t fileNameStart = texture.find_last_of("/") + 1;
        size_t lastDot = texture.find_last_of(".");
        
        std::string textureName = texture.substr(fileNameStart, lastDot - fileNameStart);
        texNames.push_back(textureName);
    }
}

void Md2Model::genBuffers()
{
    glGenBuffers(1, &vertexBufID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
    // use dynamic since the data will be frequently updated
    glBufferData(GL_ARRAY_BUFFER, 
                 3 * sizeof(float) * interpolatedFrame.vertices.size(),
                 interpolatedFrame.vertices.data(),
                 GL_DYNAMIC_DRAW);
    
    glGenBuffers(1, &texCoordBufID);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufID);
    glBufferData(GL_ARRAY_BUFFER,
                 2 * sizeof(float) * texCoords.size(),
                 texCoords.data(),
                 GL_STATIC_DRAW);
}
