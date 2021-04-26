#ifndef STEVESCH_RENDER_GEOM_SBOX4_INL_
#define STEVESCH_RENDER_GEOM_SBOX4_INL_
// Copyright © 2002, PulseCode Interactive LLC, All Rights Reserved
// inline header for box4

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace stevesch
{

  SBOX4INLINE Box4::Box4(const Box4 &box)
  {
    m_vMin = box.getMin();
    m_vMax = box.getMax();
  }

  SBOX4INLINE Box4::Box4(const vector4 &vMin, const vector4 &vMax)
  {
    m_vMin = vMin;
    m_vMax = vMax;
  }

  SBOX4INLINE void Box4::booleanOr (const Box4 &box)
  {
    vector4::min(m_vMin, m_vMin, box.getMin());
    vector4::max(m_vMax, m_vMax, box.getMax());
  }

  SBOX4INLINE void Box4::booleanOr (const vector4 &v)
  {
    vector4::min(m_vMin, m_vMin, v);
    vector4::max(m_vMax, m_vMax, v);
  }

  SBOX4INLINE bool Box4::in(const Box4 &box) const
  {
    do
    {
      const vector4 &vMin = box.getMin();
      if (m_vMin.x < vMin.x)
        break;
      if (m_vMin.y < vMin.y)
        break;
      if (m_vMin.z < vMin.z)
        break;

      const vector4 &vMax = box.getMax();
      if (m_vMax.x > vMax.x)
        break;
      if (m_vMax.y > vMax.y)
        break;
      if (m_vMax.z > vMax.z)
        break;

      return true;
    } while (false);

    return false;
  }

  SBOX4INLINE bool Box4::intersects(const Box4 &box) const
  {
    do
    {
      const vector4 &vMax = box.getMax();
      if (m_vMin.x > vMax.x)
        break;
      if (m_vMin.y > vMax.y)
        break;
      if (m_vMin.z > vMax.z)
        break;

      const vector4 &vMin = box.getMin();
      if (m_vMax.x < vMin.x)
        break;
      if (m_vMax.y < vMin.y)
        break;
      if (m_vMax.z < vMin.z)
        break;

      return true;

    } while (false);
    return false;
  }

  SBOX4INLINE void Box4::getCenter(vector4 &v) const
  {
    vector4::add(v, getMin(), getMax());
    v *= 0.5f;
  }

  SBOX4INLINE void Box4::offset(const vector4 &v)
  {
    m_vMin += v;
    m_vMax += v;
  }

  SBOX4INLINE void Box4::expand(const vector4 &v)
  {
    m_vMin -= v;
    m_vMax += v;
  }

  SBOX4INLINE void Box4::getDimensions(vector4 &v) const
  {
    vector4::sub(v, getMax(), getMin());
  }

  SBOX4INLINE void Box4::deflateToOctant(bool highX, bool highY, bool highZ)
  {
    vector4 vCenter;
    getCenter(vCenter);

    if (highX)
      m_vMin.x = vCenter.x;
    else
      m_vMax.x = vCenter.x;

    if (highY)
      m_vMin.y = vCenter.y;
    else
      m_vMax.y = vCenter.y;

    if (highZ)
      m_vMin.z = vCenter.z;
    else
      m_vMax.z = vCenter.z;
  }
} // namespace stevesch

#endif // STEVESCH_RENDER_GEOM_SBOX4_INL_
