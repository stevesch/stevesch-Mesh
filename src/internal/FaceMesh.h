#ifndef STEVESCH_RENDER_SFACEMESH_H_
#define STEVESCH_RENDER_SFACEMESH_H_

#include <stevesch-vector3.h>

#include "MeshTypes.h"
#include <stdint.h>
//#include <c_types.h>

namespace stevesch
{
  class FaceMesh
  {
    positionBuffer_t mPosition;
#if USE_FACE_NORMALS
    normalBuffer_t mNormal;
#endif
    indexBuffer_t mPositionIndex; // array of indices for vertex positions
    faceBuffer_t mFace;

  public:
    FaceMesh() {}
    ~FaceMesh() {}

    FaceMesh(const FaceMesh &src);
    FaceMesh &operator=(const FaceMesh &src);

    void clear();

    void compactMemory(); // give back any unused memory

    uint positionCount() const { return mPosition.size(); }
    const positionBuffer_t &positions() const { return mPosition; }
    const stevesch::vector3 &getPosition(stevesch::index_t nIndex) const;
    stevesch::vector3 &refPosition(stevesch::index_t nIndex);
    index_t addPosition(const stevesch::vector3 &v);

    indexBuffer_t &refPositionIndices() { return mPositionIndex; }
    const indexBuffer_t &getPositionIndices() const { return mPositionIndex; }

#if USE_FACE_NORMALS
    uint normalCount() const
    {
      return mNormal.size();
    }
    const normalBuffer_t &normals() const { return mNormal; }
    const stevesch::vector3 &getNormal(stevesch::index_t nIndex) const;
    stevesch::vector3 &refNormal(stevesch::index_t nIndex);
    index_t addNormal(const stevesch::vector3 &v);

    stevesch::index_t findMatchingNormal(const stevesch::vector3 &v);
#endif

    uint faceCount() const
    {
      return mFace.size();
    }
    const faceBuffer_t &faces() const { return mFace; }
    const IndexedFace &getFace(uint nIndex) const;
    stevesch::IndexedFace &refFace(uint nIndex);

    uint addFace(const stevesch::PlanarFace &face);
    uint addFace(const stevesch::IndexedFace &face);

    void computeExtents(stevesch::vector3 &vmin, stevesch::vector3 &vmax) const { stevesch::computeExtents(mPosition, vmin, vmax); }
    float computeExtentsFrom(const stevesch::vector3 &vCenter) const { return stevesch::computeExtentsFrom(mPosition, vCenter); }
  };

  inline stevesch::index_t FaceMesh::addPosition(const stevesch::vector3 &v)
  {
    mPosition.push_back(v);
    return mPosition.size() - 1;
  }

  inline uint FaceMesh::addFace(const stevesch::IndexedFace &face)
  {
    mFace.push_back(face);
    return mFace.size() - 1;
  }

  inline const stevesch::vector3 &FaceMesh::getPosition(stevesch::index_t nIndex) const
  {
    SASSERT((nIndex >= 0) && (nIndex < positionCount()));
    return mPosition[nIndex];
  }

  inline const IndexedFace &FaceMesh::getFace(uint nIndex) const
  {
    SASSERT((nIndex >= 0) && (nIndex < faceCount()));
    return mFace[nIndex];
  }

  inline stevesch::vector3 &FaceMesh::refPosition(stevesch::index_t nIndex)
  {
    SASSERT((nIndex >= 0) && (nIndex < positionCount()));
    return mPosition[nIndex];
  }

  inline IndexedFace &FaceMesh::refFace(uint nIndex)
  {
    SASSERT((nIndex >= 0) && (nIndex < faceCount()));
    return mFace[nIndex];
  }

#if USE_FACE_NORMALS
  inline stevesch::index_t FaceMesh::addNormal(const stevesch::vector3 &v)
  {
    mNormal.push_back(v);
    return mNormal.size() - 1;
  }

  inline const stevesch::vector3 &FaceMesh::getNormal(stevesch::index_t nIndex) const
  {
    SASSERT((nIndex >= 0) && (nIndex < normalCount()));
    return mNormal[nIndex];
  }

  inline stevesch::vector3 &FaceMesh::refNormal(stevesch::index_t nIndex)
  {
    SASSERT((nIndex >= 0) && (nIndex < normalCount()));
    return mNormal[nIndex];
  }

#endif
}

#endif
