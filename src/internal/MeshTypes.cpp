#include "MeshTypes.h"

#ifndef ICACHE_FLASH_ATTR
#define ICACHE_FLASH_ATTR
#endif

namespace stevesch
{
  void ICACHE_FLASH_ATTR computeExtents(const positionBuffer_t &positions, stevesch::vector3 &vmin, stevesch::vector3 &vmax)
  {
    uint vc = positions.size();
    if (vc > 0)
    {
      vmin = positions[0];
      vmax = vmin;
      for (uint i = 1; i < vc; ++i)
      {
        const vector3 &v = positions[i];
        vector3::min(vmin, vmin, v);
        vector3::max(vmax, vmax, v);
      }
    }
    else
    {
      vmin.set(0.0f, 0.0f, 0.0f);
      vmax.set(0.0f, 0.0f, 0.0f);
    }
  }

  float ICACHE_FLASH_ATTR computeExtentsFrom(const positionBuffer_t &positions, const stevesch::vector3 &vcenter)
  {
    float dd = 0.0f;

    uint vc = positions.size();
    for (uint i = 1; i < vc; ++i)
    {
      const vector3 &v = positions[i];
      float d2 = vector3::squareDist(vcenter, v);
      if (d2 > dd)
      {
        dd = d2;
      }
    }
    return sqrtf(dd);
  }
}
