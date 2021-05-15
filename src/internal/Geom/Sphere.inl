#ifndef STEVESCH_RENDER_GEOM_SSPHERE_INL_
#define STEVESCH_RENDER_GEOM_SSPHERE_INL_
// Copyright © 2002, PulseCode Interactive LLC, All Rights Reserved
// inline header for Sphere

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace stevesch
{

  SSPHEREINLINE Sphere::Sphere(float radius)
  {
    set(0.0f, 0.0f, 0.0f, radius);
  }

  SSPHEREINLINE Sphere::Sphere(float x, float y, float z, float radius)
  {
    set(x, y, z, radius);
  }

  SSPHEREINLINE Sphere::Sphere(const vector3 &v, float radius)
  {
    set(v, radius);
  }

  SSPHEREINLINE Sphere::Sphere(const Sphere &s)
  {
    m_v = s.m_v;
  }

  SSPHEREINLINE void Sphere::set(float x, float y, float z, float fRadius)
  {
    m_v.set(x, y, z, fRadius);
  }

  SSPHEREINLINE void Sphere::set(const vector3 &vCenter, float fRadius)
  {
    m_v.x = vCenter.x;
    m_v.y = vCenter.y;
    m_v.z = vCenter.z;
    m_v.w = fRadius;
  }

  SSPHEREINLINE void Sphere::getCenter(vector4 &v) const
  {
    v.set(m_v.x, m_v.y, m_v.z); // w = 1.0f;
  }

  SSPHEREINLINE void Sphere::setCenter(const vector3 &v)
  {
    m_v.x = v.x;
    m_v.y = v.y;
    m_v.z = v.z;
  }

  SSPHEREINLINE void Sphere::setCenter(const vector4 &v)
  {
    m_v.x = v.x;
    m_v.y = v.y;
    m_v.z = v.z;
  }

  SSPHEREINLINE void Sphere::setCenter(float x, float y, float z)
  {
    m_v.x = x;
    m_v.y = y;
    m_v.z = z;
  }

  SSPHEREINLINE float Sphere::getRadius() const
  {
    return m_v.w;
  }

  SSPHEREINLINE void Sphere::setRadius(float radius)
  {
    m_v.w = radius;
  }

  // relies on the fact that sub does not use w component of m_v
  // prevents this: sub(v, m_v, v)-- would result in a bad w for v
  SSPHEREINLINE void Sphere::subtractCenter(vector4 &v) const
  {
    vector4::sub3(v, v, m_v);
  }

  SSPHEREINLINE float Sphere::squareDistFromCenter(const vector4 &v)
  {
    return vector4::squareDist3(v, m_v);
  }

  // relies on the fact that add copies w component from first parameter (m_v)
  SSPHEREINLINE void Sphere::translate(const vector4 &v)
  {
    vector4::add3(m_v, m_v, v);
  }

  SSPHEREINLINE bool Sphere::intersects(const Sphere &s) const
  {
    float fDistanceSquared = vector4::squareDist3(s.m_v, m_v);
    float fRadiusSumSquared = s.getRadius() + getRadius();
    fRadiusSumSquared *= fRadiusSumSquared;

    return (fDistanceSquared <= fRadiusSumSquared);
  }

  SSPHEREINLINE bool Sphere::intersects(const vector4 &vCenter, float fRadius) const
  {
    float fDistanceSquared = vector4::squareDist3(vCenter, m_v);
    float fRadiusSumSquared = fRadius + getRadius();
    fRadiusSumSquared *= fRadiusSumSquared;

    return (fDistanceSquared <= fRadiusSumSquared);
  }

  SSPHEREINLINE bool Sphere::intersects(const vector4 &pt) const
  {
    float rr = vector4::squareDist3(pt, m_v);

    return (rr <= (getRadius() * getRadius()));
  }

  /*	
	bool Sphere::intersects(const Segment& s) const
	{
		//	if (intersects(s.pos)) return TRUE;
		return FALSE;
	}
*/

  SSPHEREINLINE bool Sphere::extendsAbovePlane(const Plane &plane) const
  {
    return (plane.dotNormal(m_v) >= (plane.getDistance() - getRadius()));
  }

  // distance of center above <plane>
  SSPHEREINLINE float Sphere::centerAbovePlane(const Plane &plane) const
  {
    return (plane.dotNormal(m_v) - plane.getDistance());
  }

  // SSPHEREINLINE bool Sphere::intersectsTriangle(const vector4 &p0, const vector4 &p1, const vector4 &p2) const
  // {
  //   SASSERT(0); // see other fn.
  //   return false;
  // }

  SSPHEREINLINE void Sphere::boundingBox(Box4 &box) const
  {
    float fRadius = getRadius();
    vector4 v(fRadius, fRadius, fRadius);

    vector4::sub3(box.m_vMin, m_v, v);
    vector4::add3(box.m_vMax, m_v, v);
  }

  SSPHEREINLINE void Sphere::transform(const matrix4 &transform)
  {
    float fRadius = getRadius();
    m_v.transformAff(transform);
    setRadius(fRadius);
  }

  SSPHEREINLINE int operator==(const Sphere &s1, const Sphere &s2)
  {
    return ((s1.m_v.x == s2.m_v.x) &&
            (s1.m_v.y == s2.m_v.y) &&
            (s1.m_v.z == s2.m_v.z) &&
            (s1.m_v.w == s2.m_v.w));
  }

  SSPHEREINLINE int operator!=(const Sphere &s1, const Sphere &s2)
  {
    return ((s1.m_v.x != s2.m_v.x) ||
            (s1.m_v.y != s2.m_v.y) ||
            (s1.m_v.z != s2.m_v.z) ||
            (s1.m_v.w != s2.m_v.w));
  }

}

#endif // _STEVESCH_RENDER_GEOM_SSPHERE_INL__
