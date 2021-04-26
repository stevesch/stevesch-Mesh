#include "WireMesh.h"

#ifndef ICACHE_FLASH_ATTR
#define ICACHE_FLASH_ATTR
#endif

namespace stevesch
{
  vector3 kUnitCubeVerts[] = {
      vector3(-0.5f, -0.5f, -0.5f),
      vector3(0.5f, -0.5f, -0.5f),
      vector3(-0.5f, 0.5f, -0.5f),
      vector3(0.5f, 0.5f, -0.5f),
      vector3(-0.5f, -0.5f, 0.5f),
      vector3(0.5f, -0.5f, 0.5f),
      vector3(-0.5f, 0.5f, 0.5f),
      vector3(0.5f, 0.5f, 0.5f)};
  index_t kUnitCubeIndices[] = {
      0, 2, 2, 3, 3, 1, 1, 0,
      4, 6, 6, 7, 7, 5, 5, 4,
      0, 4, 1, 5, 2, 6, 3, 7};

  WireMeshRef kWireMesh_UnitCube = {
    kUnitCubeVerts,
    kUnitCubeIndices,
    (index_t)stevesch::sizeOfArray(kUnitCubeVerts),
    stevesch::sizeOfArray(kUnitCubeIndices)
  };

  WireMesh::WireMesh(const WireMesh &src) : mPosition(src.mPosition), mIndex(src.mIndex)
  {
  }

  WireMesh &WireMesh::operator=(const WireMesh &src)
  {
    if (this != &src)
    {
      mPosition = src.mPosition;
      mIndex = src.mIndex;
    }
    return *this;
  }

  WireMesh::WireMesh(const WireMeshRef &src)
  {
    set(src);
  }

  WireMesh &WireMesh::operator=(const WireMeshRef &src)
  {
    set(src);
    return *this;
  }

  void ICACHE_FLASH_ATTR WireMesh::set(const WireMeshRef &src)
  {
    std::copy(src.mPosition, src.mPosition + src.mPositionCount, std::back_inserter(mPosition));
    std::copy(src.mIndex, src.mIndex + src.mIndexCount, std::back_inserter(mIndex));
  }

  void ICACHE_FLASH_ATTR WireMesh::clear()
  {
    mPosition.clear();
    mIndex.clear();
  }

  void WireMesh::addLine(index_t nindex0, index_t nindex1)
  {
    mIndex.push_back(nindex0);
    mIndex.push_back(nindex1);
  }

  void ICACHE_FLASH_ATTR WireMesh::compactMemory()
  {
    mPosition.shrink_to_fit();
  }

  void ICACHE_FLASH_ATTR WireMesh::ring(float fRadiusInner, float fRadiusOuter, float fHeight, uint segments)
  {
    const float fRadius1 = fRadiusInner;
    const float fRadius2 = fRadiusOuter;
    fHeight *= 0.5f; // half-height is used internally

    const uint ni = segments;
    uint i;
    vector4 v;
    v.w = 1.0f;

    float theta = 0.0f;
    float fThetaStep = c_f2pi / ni;

    for (i = 0; i < ni; i++)
    {
      float ctheta = cosf(theta);
      float stheta = sinf(theta);

      v.x = fRadius2 * ctheta;
      v.y = fRadius2 * stheta;
      v.z = -fHeight;
      addPosition(v);
      v.z = +fHeight;
      addPosition(v);

      v.x = fRadius1 * ctheta;
      v.y = fRadius1 * stheta;
      v.z = -fHeight;
      addPosition(v);
      v.z = +fHeight;
      addPosition(v);

      theta += fThetaStep;
    }

    uint modnum = 4 * ni;
    uint m0;
    m0 = 0;
    for (i = 0; i < ni; i++)
    {
      uint m1 = (m0 + 4) % modnum;
      addLine(m0, m0 + 1);
      addLine(m0 + 1, m0 + 3);
      addLine(m0 + 3, m0 + 2);
      addLine(m0 + 2, m0);
      addLine(m0, m1);
      addLine(m0 + 1, m1 + 1);
      addLine(m0 + 2, m1 + 2);
      addLine(m0 + 3, m1 + 3);
      m0 += 4;
    }
  }

  void ICACHE_FLASH_ATTR WireMesh::grid(float xa, float xb, uint nx, float za, float zb, uint nz)
  {
    vector4 v(0.0f, 0.0f, 0.0f, 1.0f);
    index_t index = 0;
    {
      float dx = (xb - xa) / nx;
      float x = xa;
      for (uint ix = 0; ix <= nx; ++ix)
      {
        v.x = x;
        v.z = za;
        addPosition(v);
        v.z = zb;
        addPosition(v);
        x += dx;

        addIndex(index++);
        addIndex(index++);
      }
    }

    {
      float dz = (zb - za) / nz;
      float z = za;
      for (uint iz = 0; iz <= nz; ++iz)
      {
        v.z = z;
        v.x = xa;
        addPosition(v);
        v.x = xb;
        addPosition(v);
        z += dz;

        addIndex(index++);
        addIndex(index++);
      }
    }
  }
}
