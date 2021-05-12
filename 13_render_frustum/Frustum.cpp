#include <cassert>
#include <Frustum.h>

#define DBG_ASSERT(cond) assert(cond)
//#define DBG_ASSERT(cond)

Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}

void Frustum::UpdateFrustum(Mat4* viewMatrix, Mat4* projMatrix)
{
    DBG_ASSERT(viewMatrix != nullptr);
    DBG_ASSERT(projMatrix != nullptr);
    Mat4 mvp = (*projMatrix) * (*viewMatrix);
    
    planes[PLANE_RIGHT] = extractPlane(mvp.v[ 3] - mvp.v[ 0],  // xw - xx
                                       mvp.v[ 7] - mvp.v[ 4],  // yw - yx
                                       mvp.v[11] - mvp.v[ 8],  // zw - zx
                                       mvp.v[15] - mvp.v[12]); // tw - tx
                                       
    planes[PLANE_LEFT] = extractPlane(mvp.v[ 3] + mvp.v[ 0],   // xw + xx
                                      mvp.v[ 7] + mvp.v[ 4],   // yw + yx
                                      mvp.v[11] + mvp.v[ 8],   // zw + zx
                                      mvp.v[15] + mvp.v[12]);  // tw + tx
                                      
    planes[PLANE_TOP] = extractPlane(mvp.v[ 3] - mvp.v[ 1],    // xw - xy
                                     mvp.v[ 7] - mvp.v[ 5],    // yw - yy
                                     mvp.v[11] - mvp.v[ 9],    // zw - zy
                                     mvp.v[15] - mvp.v[13]);   // tw - ty
                                     
    planes[PLANE_BOTTOM] = extractPlane(mvp.v[ 3] + mvp.v[ 1],  // xw + xy
                                        mvp.v[ 7] + mvp.v[ 5],  // yw + yy
                                        mvp.v[11] + mvp.v[ 9],  // zw + zy
                                        mvp.v[15] + mvp.v[13]); // tw + ty
                                        
    planes[PLANE_FAR] = extractPlane(mvp.v[ 3] - mvp.v[ 2],  // xw - xz
                                     mvp.v[ 7] - mvp.v[ 6],  // yw - yz
                                     mvp.v[11] - mvp.v[10],  // zw - zz
                                     mvp.v[15] - mvp.v[14]); // tw - tz
                                     
    planes[PLANE_NEAR] = extractPlane(mvp.v[ 3] + mvp.v[ 2],  // xw + xz
                                      mvp.v[ 7] + mvp.v[ 6],  // yw + yz
                                      mvp.v[11] + mvp.v[10],  // zw + zz
                                      mvp.v[15] + mvp.v[14]); // tw + tz
}

bool Frustum::SphereInFrustum(float x, float y, float z, float radius)
{
    for (int p = 0; p < 6; ++p)
    {
        if (planes[p].a * x + 
            planes[p].b * y +
            planes[p].c * z +
            planes[p].d <= -radius)
        {
            return false;
        }
    }
    return true;
}

bool Frustum::PointInFrustum(float x, float y, float z)
{
    for (int p = 0; p < 6; ++p)
    {
        if (planes[p].a * x +
            planes[p].b * y +
            planes[p].c * z +
            planes[p].d < 0)
        {
            return false;
        }
    }
    return true;
}

Plane Frustum::extractPlane(float a, float b, float c, float d)
{
    float t = sqrt(a*a + b*b + c*c);
    Plane p;
    p.a = a / t;
    p.b = b / t;
    p.c = c / t;
    p.d = d / t;
    return p;
}

#undef DBG_ASSERT
