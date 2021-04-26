#ifndef STEVESCH_RENDER_SWIREMESH_H_
#define STEVESCH_RENDER_SWIREMESH_H_
#include <stevesch-vector3.h>
#include "MeshTypes.h"
#include <stdint.h>
//#include <c_types.h>

namespace stevesch
{
  struct WireMeshRef
  {
    vector3 *mPosition;
    index_t *mIndex;
    index_t mPositionCount;
    uint mIndexCount;
  };

  extern WireMeshRef kWireMesh_UnitCube;

  class WireMesh
  {
    positionBuffer_t mPosition;
    indexBuffer_t mIndex;

  public:
    WireMesh() {}
    ~WireMesh() {}

    WireMesh(const WireMesh &src);
    WireMesh &operator=(const WireMesh &src);

    WireMesh(const WireMeshRef &src);
    WireMesh &operator=(const WireMeshRef &src);

    void set(const WireMeshRef &src);
    void clear();

    void compactMemory(); // give back any unused memory

    uint positionCount() const { return mPosition.size(); }
    uint indexCount() const { return mIndex.size(); }
    uint lineCount() const { return mIndex.size() / 2; }

    const stevesch::vector3 &getPosition(index_t nIndex) const;
    index_t getIndex(uint nIndex) const;

    stevesch::vector3 &refPosition(index_t nIndex);
    index_t &refIndex(uint nIndex);

    index_t addPosition(const stevesch::vector3 &v);
    uint addIndex(index_t nindex);
    void addLine(index_t nindex0, index_t nindex1);

    void ring(float fRadiusInner, float fRadiusOuter, float fHeight, uint segments);
    void grid(float xa, float xb, uint nx, float za, float zb, uint nz);

    void computeExtents(stevesch::vector3 &vmin, stevesch::vector3 &vmax) const { stevesch::computeExtents(mPosition, vmin, vmax); }

    WireMeshRef getRef();
    //void importObj();
  };

  inline index_t WireMesh::addPosition(const stevesch::vector3 &v)
  {
    mPosition.push_back(v);
    return mPosition.size() - 1;
  }

  inline uint WireMesh::addIndex(index_t nindex)
  {
    mIndex.push_back(nindex);
    return mIndex.size() - 1;
  }

  inline const stevesch::vector3 &WireMesh::getPosition(index_t nIndex) const
  {
    SASSERT((nIndex >= 0) && (nIndex < positionCount()));
    return mPosition[nIndex];
  }

  inline index_t WireMesh::getIndex(uint nIndex) const
  {
    SASSERT((nIndex >= 0) && (nIndex < indexCount()));
    return mIndex[nIndex];
  }

  inline stevesch::vector3 &WireMesh::refPosition(index_t nIndex)
  {
    SASSERT((nIndex >= 0) && (nIndex < positionCount()));
    return mPosition[nIndex];
  }

  inline index_t &WireMesh::refIndex(uint nIndex)
  {
    SASSERT((nIndex >= 0) && (nIndex < indexCount()));
    return mIndex[nIndex];
  }

  inline WireMeshRef WireMesh::getRef()
  {
    WireMeshRef ref;
    ref.mPosition = &refPosition(0);
    ref.mIndex = &refIndex(0);
    ref.mPositionCount = positionCount();
    ref.mIndexCount = indexCount();
    return ref;
  }
}

#endif
