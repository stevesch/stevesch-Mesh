#ifndef STEVESCH_RENDER_GEOM_SPLANE_INL_
#define STEVESCH_RENDER_GEOM_SPLANE_INL_
// Copyright © 2002, PulseCode Interactive LLC, All Rights Reserved
// inline header for plane

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace stevesch
{

  // construct from normal and point
  SPLANEINLINE Plane::Plane(const vector3 &vNormal, const vector3 &vPoint)
  {
    m_v.set(vNormal.x, vNormal.y, vNormal.z, vNormal.dot(vPoint));
  }

  // construct from 3 points (clockwise-facing left-handed generates normal towards viewer)
  // fast-- assumes well-formed triangles (non-degenerate, reasonable edge ratios)
  SPLANEINLINE Plane::Plane(vector4 &v0, vector4 &v1, vector4 &v2, bool rightHanded)
  {
    vector4 ea, eb; // edges

    if (rightHanded)
    {
      vector4::sub(ea, v2, v0);
      vector4::sub(eb, v1, v0);
    }
    else
    {
      vector4::sub(ea, v1, v0);
      vector4::sub(eb, v2, v0);
    }
    ea.cross(eb);
    ea.normalize();

    m_v = ea;
    m_v.w = ea.dot(v0);
  }

  SPLANEINLINE const Plane &Plane::set(const vector3 &vNormal, float fDistance)
  {
    m_v.x = vNormal.x;
    m_v.y = vNormal.y;
    m_v.z = vNormal.z;
    m_v.w = fDistance;
    return *this;
  }

  SPLANEINLINE const Plane &Plane::set(float nx, float ny, float nz, float fDistance)
  {
    m_v.set(nx, ny, nz, fDistance);
    return *this;
  }

  SPLANEINLINE float Plane::dotNormal(const vector4 &v) const
  {
    return m_v.dot(v);
  }

  SPLANEINLINE bool Plane::isVectorAbove(const vector4 &v) const
  {
    return dotNormal(v) > getDistance();
  }

  // excludes points on plane
  SPLANEINLINE bool Plane::isTriangleAbove(const vector4 *v3) const
  {
    const float fDistance = getDistance();
    if (v3[0].dot(m_v) <= fDistance)
      return false;
    if (v3[1].dot(m_v) <= fDistance)
      return false;
    if (v3[2].dot(m_v) <= fDistance)
      return false;

    return true;
  }

  // excludes points on plane
  SPLANEINLINE bool Plane::isTriangleBelow(const vector4 *v3) const
  {
    const float fDistance = getDistance();
    if (dotNormal(v3[0]) >= fDistance)
      return false;
    if (dotNormal(v3[1]) >= fDistance)
      return false;
    if (dotNormal(v3[2]) >= fDistance)
      return false;

    return true;
  }

}

#endif // _STEVESCH_RENDER_GEOM_SPLANE_INL__
