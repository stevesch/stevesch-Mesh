#include "FaceMesh.h"

#ifndef ICACHE_FLASH_ATTR
#define ICACHE_FLASH_ATTR
#endif

namespace stevesch
{
  ICACHE_FLASH_ATTR FaceMesh::FaceMesh(const FaceMesh &src)
      : mPosition(src.mPosition)
#if USE_FACE_NORMALS
        ,
        mNormal(src.mNormal)
#endif
        ,
        mPositionIndex(src.mPositionIndex), mFace(src.mFace)
  {
  }

  FaceMesh &ICACHE_FLASH_ATTR FaceMesh::operator=(const FaceMesh &src)
  {
    if (this != &src)
    {
      mPosition = src.mPosition;
#if USE_FACE_NORMALS
      mNormal = src.mNormal;
#endif
      mPositionIndex = src.mPositionIndex;
      mFace = src.mFace;
    }
    return *this;
  }

  void ICACHE_FLASH_ATTR FaceMesh::clear()
  {
    mPosition.clear();
#if USE_FACE_NORMALS
    mNormal.clear();
#endif
    mFace.clear();
    mPositionIndex.clear();
  }

  void ICACHE_FLASH_ATTR FaceMesh::compactMemory()
  {
    mPosition.shrink_to_fit();
#if USE_FACE_NORMALS
    mNormal.shrink_to_fit();
#endif
    mPositionIndex.shrink_to_fit();
    mFace.shrink_to_fit();
    //for(auto& face : mFace)
    //{
    //	face.iPosition.shrink_to_fit();
    //}
  }

  uint FaceMesh::addFace(const PlanarFace &face)
  {
    indexBuffer_t &indices = refPositionIndices();
    ;
    index_t i0 = indices.size();
    for (auto idx : face.iPosition)
    {
      indices.push_back(idx);
    }
    index_t i1 = indices.size();

    IndexedFace f;
    f.iNormal = face.iNormal;
    f.iFirst = i0;
    f.iCount = i1 - i0;
    return addFace(f);
  }

#if USE_FACE_NORMALS
  index_t FaceMesh::findMatchingNormal(const stevesch::vector3 &v)
  {
    const normalBuffer_t &normals = mNormal;
    index_t nc = normals.size();
    for (index_t i = 0; i < nc; ++i)
    {
      const vector3 &n = normals[i];
      if (v.dot(n) > 0.9998f)
      {
        return i;
      }
    }
    return -1;
  }
#endif
}
