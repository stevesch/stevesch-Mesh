#ifndef STEVESCH_RENDER_GEOM_SFRUSTUM_H_
#define STEVESCH_RENDER_GEOM_SFRUSTUM_H_
// Copyright © 2002, PulseCode Interactive LLC, All Rights Reserved

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stevesch-MathBase.h>
#include <stevesch-MathVec.h>
#include "Geom.h"

namespace stevesch
{
	typedef enum
	{
		SINTERSECT_DONE = -1,	// node and all subnodes pass the test
		SINTERSECT_OUT = 0,	// node and all subnodes fail the test
		SINTERSECT_IN = 1		// node or some subnode passes the test
	} SINTERSECTION;

  class Frustum
  {
  public:
    enum
    {
      SFRUSTUM_LEFT = 0,
      SFRUSTUM_RIGHT,
      SFRUSTUM_BOTTOM,
      SFRUSTUM_TOP,
      SFRUSTUM_NEAR,
      SFRUSTUM_FAR, // must be last for isVisibleFar

      ///////////////////
      SFRUSTUM_PLANECOUNT
    };

    enum
    {
      SPLANEMASK_LEFT = (1 << SFRUSTUM_LEFT),
      SPLANEMASK_RIGHT = (1 << SFRUSTUM_RIGHT),
      SPLANEMASK_BOTTOM = (1 << SFRUSTUM_BOTTOM),
      SPLANEMASK_TOP = (1 << SFRUSTUM_TOP),
      SPLANEMASK_NEAR = (1 << SFRUSTUM_NEAR),
      SPLANEMASK_FAR = (1 << SFRUSTUM_FAR),

      /////////////////
      SPLANEMASK_EDGES = (SPLANEMASK_LEFT | SPLANEMASK_RIGHT | SPLANEMASK_BOTTOM | SPLANEMASK_TOP),
      SPLANEMASK_Z = (SPLANEMASK_NEAR | SPLANEMASK_FAR),
      SPLANEMASK_ALL = (SPLANEMASK_EDGES | SPLANEMASK_Z)
    };

  private:
    // the following are in the "world" frame:
    vector4 m_vViewpoint;
    Sphere m_BoundingSphere;
    Plane m_Plane[SFRUSTUM_PLANECOUNT];

  public:
    Frustum();
    Frustum(float fZNear, float fZFar, const matrix4 &crProjection, const matrix4 *pViewToWorld = NULL);
    ~Frustum(){};

    Plane &getPlane(uint32_t nIndex)
    {
      SASSERT(nIndex < SFRUSTUM_PLANECOUNT);
      return m_Plane[nIndex];
    }

    const Plane &getPlane(uint32_t nIndex) const
    {
      SASSERT(nIndex < SFRUSTUM_PLANECOUNT);
      return m_Plane[nIndex];
    }

    bool isVisible(const Sphere &sBoundingSphere) const;
    bool isVisibleFar(const Sphere &sBoundingSphere) const; // is visible, ignoring far clip

    const Sphere &getBoundingSphere() const { return m_BoundingSphere; }
    const vector4 GetViewpoint() const { return m_vViewpoint; }

    void set(float fZNear, float fZFar, const matrix4 &crProjection, const matrix4 *pViewToWorld);
    void set(float fZNear, float fZFar,
             const matrix4 &crViewToScreen, // proj
             const matrix4 &crScreenToView, // proj^-1
             const matrix4 *pViewToWorld);  // view^-1

    SINTERSECTION intersection(const Sphere &s) const;
  };

} // namespace stevesch

#endif // STEVESCH_RENDER_GEOM_SFRUSTUM_H_
