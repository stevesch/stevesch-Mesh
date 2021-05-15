#include "Frustum.h"

// extern void printPlane(const char* label, const stevesch::Plane& plane);
// extern void printVec3(const char* label, const stevesch::vector3& v);

namespace stevesch
{
  ///////////////////////////////////////////////////////////////////////////
  // class Frustum holds the six planes that determine the frustum and can be used for visibility checks
  ///////////////////////////////////////////////////////////////////////////
  Frustum::Frustum(float fZNear, float fZFar, const matrix4 &crProjection, const matrix4 *pWorldToView)
  {
    set(fZNear, fZFar, crProjection, pWorldToView);
  }

  Frustum::Frustum()
  {
    set(1.0f, 100.0f, matrix4::I, &matrix4::I);
  }

  static void sphereFromVectors(Sphere &rSphere, const vector4 *pPointArray, int nPoints);

  void sphereFromVectors(Sphere &rSphere, const vector4 *pPointArray, int nPoints)
  {
    vector4 vMin, vMax;
    vector4 vCenter;

    SASSERT(nPoints > 0);

    const vector4 *p = pPointArray;
    vMin = *p;
    vMax = *p;

    p++;
    int n = nPoints;
    while (--n > 0) // skip first point (n > 0)
    {
      vector4::min3(vMin, vMin, *p);
      vector4::max3(vMax, vMax, *p);
      p++;
    }

    vector4::add3(vCenter, vMin, vMax);
    vCenter.mul3(0.5f);

    float fRadius = 0.0f;
    float fDist2;
    n = nPoints;
    p = pPointArray;
    while (--n >= 0) // examine all points again (n >= 0)
    {
      fDist2 = vector4::distanceSquared3(*p, vCenter);
      fRadius = stevesch::maxf(fDist2, fRadius);
      p++;
    }

    fRadius = sqrtf(fRadius);

    rSphere.set(vCenter, fRadius);
  }

  bool Frustum::isVisible(const Sphere &sBoundingSphere) const
  {
    if (!sBoundingSphere.intersects(getBoundingSphere()))
    {
      return false;
    }

    for (uint i = 0; i < SFRUSTUM_PLANECOUNT; i++)
    {
      if (!sBoundingSphere.extendsAbovePlane(getPlane(i)))
        return false;
    }
    return true;
  }

  bool Frustum::isVisibleFar(const Sphere &sBoundingSphere) const
  {
    for (uint i = 0; i < SFRUSTUM_PLANECOUNT - 1; i++)
    {
      if (!sBoundingSphere.extendsAbovePlane(getPlane(i)))
        return false;
    }
    return true;
  }

  SINTERSECTION Frustum::intersection(const Sphere &s) const
  {
    SINTERSECTION nIntersection = SINTERSECT_OUT; // no intersection
    uint32_t i;
    float fRadius, fDistance;

    if (s.intersects(getBoundingSphere()))
    {
      fRadius = s.getRadius();
      nIntersection = SINTERSECT_DONE; // assume completely within frustum

      for (i = 0; i < SFRUSTUM_PLANECOUNT; i++)
      {
        const Plane &plane = getPlane(i);
        fDistance = s.centerAbovePlane(plane); // plane.dotNormal(s.m_v) - plane.getDistance()
        if (fDistance < -fRadius)
        {
          // sphere does not extend above the plane-- no intersection with frustum
          return SINTERSECT_OUT;
        }

        if (fDistance < fRadius)
        {
          // sphere is not completely above this plane-- partial intersection with frustum
          nIntersection = SINTERSECT_IN;
          i++;
          break;
          // (keep checking other planes)
        }
      }

      while (i < SFRUSTUM_PLANECOUNT)
      {
        const Plane &plane = getPlane(i);
        fDistance = s.centerAbovePlane(plane); // plane.dotNormal(s.m_v) - plane.getDistance()
        if (fDistance < -fRadius)
        {
          // sphere does not extend above the plane-- no intersection with frustum
          return SINTERSECT_OUT;
        }

        i++;
      }
    }

    return nIntersection;
  }

  void Frustum::set(float fZNear, float fZFar, const matrix4 &crProjection, const matrix4 *pViewToWorld)
  {
    matrix4 mtxScreenToView;
    matrix4::invert4x4(mtxScreenToView, crProjection);
    set(fZNear, fZFar, crProjection, mtxScreenToView, pViewToWorld);
  }

  void Frustum::set(float fZNear, float fZFar,
                    const matrix4 &crViewToScreen, // proj
                    const matrix4 &crScreenToView, // proj^-1
                    const matrix4 *pViewToWorld)   // view^-1
  {
    if (NULL == pViewToWorld)
    {
      pViewToWorld = &matrix4::I;
    }
    vector4 v[8];
    vector4 vViewpoint(0.0f, 0.0f, 0.0f);
    vector4::transform(m_vViewpoint, *pViewToWorld, vViewpoint);

    bool rh = (crViewToScreen.m32 < 0.0f);
    // float zScreenNear = rh ? -fZNear : fZNear;
    // float zScreenFar = rh ? -fZFar : fZFar;
    float zScreenNear = fZNear;
    float zScreenFar = fZFar;
    v[0] = vector4(-1.0f, -1.0f, zScreenNear); // bottom-left (near)
    v[1] = vector4(1.0f, -1.0f, zScreenNear);  // bottom-right
    v[2] = vector4(-1.0f, 1.0f, zScreenNear);  // top-left
    v[3] = vector4(1.0f, 1.0f, zScreenNear);   // top-right
    v[4] = vector4(-1.0f, -1.0f, zScreenFar);  // bottom-left (far)
    v[5] = vector4(1.0f, -1.0f, zScreenFar);   // . . .
    v[6] = vector4(-1.0f, 1.0f, zScreenFar);
    v[7] = vector4(1.0f, 1.0f, zScreenFar);
    SScreenToWorld(v, v, 8, crViewToScreen, crScreenToView, pViewToWorld);

    // printVec3("Frustum v[0]", v[0]);
    // printVec3("Frustum v[1]", v[1]);
    // printVec3("Frustum v[2]", v[2]);
    // printVec3("Frustum v[3]", v[3]);
    // printVec3("Frustum v[4]", v[4]);
    // printVec3("Frustum v[5]", v[5]);
    // printVec3("Frustum v[6]", v[6]);
    // printVec3("Frustum v[7]", v[7]);

    sphereFromVectors(m_BoundingSphere, v, 8);

    // planes w/normals that point into frustum interior:
    getPlane(SFRUSTUM_NEAR) = Plane(v[3], v[2], v[0], rh);   // front (near camera)
    getPlane(SFRUSTUM_FAR) = Plane(v[7], v[4], v[6], rh);    // back (distant from camera)
    getPlane(SFRUSTUM_TOP) = Plane(v[7], v[6], v[2], rh);    // top
    getPlane(SFRUSTUM_BOTTOM) = Plane(v[5], v[1], v[0], rh); // bottom
    getPlane(SFRUSTUM_LEFT) = Plane(v[2], v[6], v[4], rh);   // left
    getPlane(SFRUSTUM_RIGHT) = Plane(v[7], v[3], v[1], rh);  // right

    // printPlane("NEAR", getPlane(SFRUSTUM_NEAR));
    // printPlane("FAR", getPlane(SFRUSTUM_FAR));
    // printPlane("TOP", getPlane(SFRUSTUM_TOP));
    // printPlane("BOTTOM", getPlane(SFRUSTUM_BOTTOM));
    // printPlane("LEFT", getPlane(SFRUSTUM_LEFT));
    // printPlane("RIGHT", getPlane(SFRUSTUM_RIGHT));
  }

} // namespace stevesch
