#ifndef STEVESCH_RENDER_SMESHTYPES_H_
#define STEVESCH_RENDER_SMESHTYPES_H_

#include <stevesch-vector3.h>
#include <vector>
#include <cstdint>
// #include <stdint.h>
//#include <c_types.h>

// use face normals rather than winding order
// (allows for rendering of non-convex faces)
#define USE_FACE_NORMALS 1

namespace stevesch
{
  typedef std::uint16_t index_t;

  typedef std::vector<stevesch::vector3> positionBuffer_t;
  typedef std::vector<stevesch::vector3> normalBuffer_t;
  typedef std::vector<index_t> indexBuffer_t;

  struct PlanarFace
  {
#if USE_FACE_NORMALS
    index_t iNormal; // single normal per face
#endif

    indexBuffer_t iPosition; // array of indices for vertex positions
  };
  //typedef std::vector<PlanarFace>		faceBuffer_t;

  struct IndexedFace
  {
#if USE_FACE_NORMALS
    index_t iNormal; // single normal per face
#endif
    index_t iFirst;       // first vertex index in index buffer
    std::uint16_t iCount; // number of indices used in index buffer
  };

  typedef std::vector<IndexedFace> faceBuffer_t;

  void computeExtents(const positionBuffer_t &positions, stevesch::vector3 &vmin, stevesch::vector3 &vmax);
  float computeExtentsFrom(const positionBuffer_t &positions, const stevesch::vector3 &vcenter);

  /*
	// Maybe compress normals, e.g.
	// Lambert Azimuthal Equal-Area projection
	// (https://en.wikipedia.org/wiki/Lambert_azimuthal_equal-area_projection)
	half2 encode(half3 n)
	{
		half f = sqrt(8 * n.z + 8);
		return n.xy / f + 0.5;
	}
	half3 decode(half4 enc)
	{
		half2 fenc = enc * 4 - 2;
		half f = dot(fenc, fenc);
		half g = sqrt(1 - f / 4);
		half3 n;
		n.xy = fenc*g;
		n.z = 1 - f / 2;
		return n;
	}
*/
}
#endif
