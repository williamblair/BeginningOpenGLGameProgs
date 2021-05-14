#ifndef FRUSTUM_H_INCLUDED
#define FRUSTUM_H_INCLUDED

#include <Mat4.h>

struct Plane
{
    float a, b, c, d;
};

class Frustum
{
public:
    
    Frustum();
    ~Frustum();
    
    void UpdateFrustum(Mat4* viewMatrix, Mat4* projMatrix);
    bool SphereInFrustum(float x, float y, float z, float radius);
    bool PointInFrustum(float x, float y, float z);
    
private:
    Plane planes[6];
    
    enum
    {
        PLANE_LEFT = 0,
        PLANE_RIGHT = 1,
        PLANE_TOP = 2,
        PLANE_BOTTOM = 3,
        PLANE_FAR = 4,
        PLANE_NEAR = 5
    };
    Plane extractPlane(float a, float b, float c, float d);
};

#endif // FRUSTUM_H_INCLUDED