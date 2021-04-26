#ifndef STEVESCH_RENDER_GEOM_SGEOM_H_
#define STEVESCH_RENDER_GEOM_SGEOM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stevesch-MathBase.h>
#include <stevesch-MathVec.h>

// Geometry library header
//
// Copyright © 2002, PulseCode Interactive LLC, All Rights Reserved
//
// History:
//	Created:	7/12/2002, Stephen Schlueter
//	Modified:

#define SSPHEREINLINE SVECINLINE
#define SPLANEINLINE SVECINLINE
#define SBOX4INLINE SVECINLINE
#define SBODYBASEINLINE SVECINLINE

namespace stevesch
{
  class vectorTransformer;
  class Sphere;
  class Box4;
  class Plane;
  class Body3D;
  class Segment;

  ///////////////////////////////////////////////////////////////
  // given a unit length "normal" vector n, generate vectors p and q vectors
  // that are an orthonormal basis for the plane space perpendicular to n.
  // i.e. this makes p,q such that n,p,q are all perpendicular to each other.
  // q will equal n x p. if n is not unit length then p will be unit length but
  // q wont be.
  void SPlaneSpace(const vector4 &n, vector4 &p, vector4 &q);

  // given vectors a, and b, create an orthonormal basis transform such that
  // x^ -> x^' == a^
  // y^ -> y^' == [b - (b.a^)*a^]^, (direction of b that is perpendicular to a) and
  // z^ -> z^' == (x^' x y^')
  // ( a and b must be non-zero length and not colinear )
  // | -- a^ -- |
  // | -- j^ -- |
  // | -- k^ -- |
  // i = a
  // j = b - (b.a^)*a^	(component of b that is perpendicular to a)
  // k = i^ x j^
  void SPlaneSpace2(matrix4 &rBasis, const vector4 &a, const vector4 &b);

  ///////////////////////////////////////////////////////////////

  bool SScreenToWorld(vector4 *pWorld, const vector4 *pScreen, int nVerts,
                      const matrix4 &crViewToScreen,       // proj
                      const matrix4 &crScreenToView,       // proj^-1
                      const matrix4 *pViewToWorld = NULL); // view^-1 (NULL->identity)

  bool SWorldToScreen(vector4 *pScreen, const vector4 *pWorld, int nVerts,
                      const matrix4 &crViewToScreen,       // proj
                      const matrix4 *pWorldToView = NULL); // view (NULL->identity)

  ///////////////////////////////////////////////////////////////

  class vectorTransformer
  {
  public:
    // returns true if a transform was performed (otherwise just did copy or no-op)
    virtual bool transform(vector4 *pDst, const vector4 *pSrc, int nVerts) { return false; }
    virtual bool untransform(vector4 *pDst, const vector4 *pSrc, int nVerts) { return false; }
  };

  ///////////////////////////////////////////////////////////////

  // vector4-based line segment
  class Segment
  {
  protected:
    vector4 m_v[2];

  public:
    // overload new, delete, new[] and delete[] to provide aligned allocation
    // SOVERLOAD_NEW_ALIGNED(Segment, 16)

    Segment() {}
    Segment(const vector4 &v0, const vector4 &v1)
    {
      m_v[0] = v0;
      m_v[1] = v1;
    }

    const vector4 &getV0() const { return m_v[0]; }
    const vector4 &getV1() const { return m_v[1]; }

    void setV0(const vector4 &v) { m_v[0] = v; }
    void setV1(const vector4 &v) { m_v[1] = v; }

    const vector4 &get(int nIndex) const
    {
      SASSERT((nIndex == 0) || (nIndex == 1));
      return m_v[nIndex];
    }
    void set(int nIndex, const vector4 &v)
    {
      SASSERT((nIndex == 0) || (nIndex == 1));
      m_v[nIndex] = v;
    }

    vector4 &ref(int nIndex)
    {
      SASSERT((nIndex == 0) || (nIndex == 1));
      return m_v[nIndex];
    }

    void reverse()
    {
      vector4 vTemp(m_v[0]);
      m_v[0] = m_v[1];
      m_v[1] = vTemp;
    }

    bool testColinearIntersection(const Segment &seg1) const;             // returns true if this segment intersects seg1 (assuming colinear segments)
    bool mergeColinearIntersection(const Segment &seg1, uint32_t &LineStatus); // merges this segment with the given colinear segment and fills status bits
                                                                          // (returns true if segments intersect)
                                                                          ////////////////////////////////////////
                                                                          // LineStatus:
                                                                          //
                                                                          // bottom 2 bits show which normal should be used (for collision):
                                                                          // 0x00: edgei1 x edgej0
                                                                          // 0x01: -nj
                                                                          // 0x02: edgej1 x edgei0
                                                                          // 0x03: ni
                                                                          //
                                                                          // high bit is set if seg1 is opposite direction of this segment
                                                                          // 0x80000000: [getV1() - getV0()] . [seg1.getV1() - seg1.getV0()] < 0.0
                                                                          ////////////////////////////////////////
  };

  ///////////////////////////////////////////////////////////////

  class Sphere
  {
  protected:
    vector4 m_v; // aligned 4-element vector (<center>,radius = <x,y,z>,<w>)

  public:
    // overload new, delete, new[] and delete[] to provide aligned allocation
    // SOVERLOAD_NEW_ALIGNED(Sphere, 16)

    // Constructors / Destructors
    SSPHEREINLINE Sphere() {}           // empty constructor for uninitialized Sphere
    SSPHEREINLINE Sphere(float radius); // assign center to origin, radius = _radius
    SSPHEREINLINE Sphere(float x, float y, float z, float radius);
    SSPHEREINLINE Sphere(const float v[4]); // x, y, z, radius
    SSPHEREINLINE Sphere(const vector3 &center, float radius);
    SSPHEREINLINE Sphere(const Sphere &s); // copy existing sphere

    SSPHEREINLINE ~Sphere() {}

    SSPHEREINLINE void getCenter(vector4 &v) const;

    // SSPHEREINLINE const vector3& getCenter() const	{ return *((const vector3*)&m_v); }
    // SSPHEREINLINE vector3& getCenter()		{ return *((vector3*)&m_v); }
    SSPHEREINLINE vector3 getCenter() const { return vector3(m_v.x, m_v.y, m_v.z); }

    SSPHEREINLINE void subtractCenter(vector4 &v) const; // relies on the fact that sub does not use w component of m_v
                                                         // prevents this: sub(v, m_v, v)-- would result in a bad w for v

    SSPHEREINLINE float squareDistFromCenter(const vector4 &v);
    SSPHEREINLINE void translate(const vector4 &v); // relies on the fact that add copies w component from first parameter (m_v)

    SSPHEREINLINE void set(float x, float y, float z, float fRadius);
    SSPHEREINLINE void set(const vector3 &vCenter, float fRadius);

    SSPHEREINLINE void setCenter(const vector3 &v);
    SSPHEREINLINE void setCenter(const vector4 &v);
    SSPHEREINLINE void setCenter(float x, float y, float z);

    SSPHEREINLINE float getRadius() const;
    SSPHEREINLINE void setRadius(float fRadius);

    SSPHEREINLINE bool intersects(const Sphere &s) const;
    SSPHEREINLINE bool intersects(const vector4 &vCenter, float fRadius) const;

    //SSPHEREINLINE bool intersects(const segment& s) const;
    SSPHEREINLINE bool intersects(const vector4 &pt) const;

    SSPHEREINLINE bool extendsAbovePlane(const Plane &plane) const;
    // SSPHEREINLINE bool intersectsTriangle(const vector4 &p0, const vector4 &p1, const vector4 &p2) const;
    SSPHEREINLINE float centerAbovePlane(const Plane &plane) const; // distance of center above <plane>

    SSPHEREINLINE void boundingBox(Box4 &box) const;

    float timeToPlaneAlongVector(const Plane &pPlane, const vector4 &vVelocity) const;

    friend SSPHEREINLINE int operator==(const Sphere &s1, const Sphere &s2);
    friend SSPHEREINLINE int operator!=(const Sphere &s1, const Sphere &s2);

    SSPHEREINLINE void transform(const matrix4 &rTransform); // just transforms the center of the sphere
  };

  ///////////////////////////////////////////////////////////////

  class Plane
  {
  protected:
    vector4 m_v; // aligned 4-element vector (<normal>,distance = <x,y,z>,<w>)

  public:
    // overload new, delete, new[] and delete[] to provide aligned allocation
    // SOVERLOAD_NEW_ALIGNED(Plane, 16)

    SPLANEINLINE Plane() {}
    SPLANEINLINE Plane(const vector3 &vNormal, float fDistance) { m_v.set(vNormal.x, vNormal.y, vNormal.z, fDistance); }
    SPLANEINLINE Plane(float nx, float ny, float nz, float fDistance) { m_v.set(nx, ny, nz, fDistance); }
    SPLANEINLINE Plane(const Plane &plane) { m_v = plane.m_v; }
    SPLANEINLINE const Plane &operator=(const Plane &plane)
    {
      m_v = plane.m_v;
      return *this;
    }

    // construct from normal and point
    SPLANEINLINE Plane(const vector3 &vNormal, const vector3 &vPoint);

    // construct from 3 points (clockwise-facing generates normal towards viewer)
    // fast-- assumes well-formed triangles (non-degenerate, reasonable edge ratios)
    // @rightHanded: true is coordinate system is right handed (determines sign of normal)
    SPLANEINLINE Plane(vector4 &v0, vector4 &v1, vector4 &v2, bool rightHanded = false);

    SPLANEINLINE const Plane &set(const vector3 &vNormal, float fDistance);
    SPLANEINLINE const Plane &set(float nx, float ny, float nz, float fDistance);

    SPLANEINLINE float getDistance() const { return m_v.w; }
    SPLANEINLINE void getNormal(vector4 &vNormal) const
    {
      vNormal = m_v;
      vNormal.w = 1.0f;
    }
    SPLANEINLINE const vector3 &getNormal() const { return m_v; }

    SPLANEINLINE float dotNormal(const vector4 &v) const; // return dot product with normal

    SPLANEINLINE bool isVectorAbove(const vector4 &v) const;
    SPLANEINLINE bool isTriangleAbove(const vector4 *v3) const; // excludes points on plane
    //	SPLANEINLINE bool IsTriangleSpanning(const vector4* v3);
    SPLANEINLINE bool isTriangleBelow(const vector4 *v3) const; // excludes points on plane

    void transform(const matrix4 &mtx);           // full transform, including translation
    SPLANEINLINE void rotate(const matrix4 &mtx); // ignores translation in mtx
    SPLANEINLINE void rotate(const quat &qRotation);

    // returns t for when (vPoint + vDir*t) intersects the plane.
    // (time required to reach plane from vPoint with velocity vDir, or
    //  distance from point vPoint to plane along vDir if vDir is normalized)
    float timeToPlaneAlongVector(const vector4 &vPoint, const vector4 &vDir) const;
  };

  SPLANEINLINE void Plane::rotate(const matrix4 &mtx)
  {
    // TO-CHECK: MulSub
    m_v.transformSub(mtx);
  }

  SPLANEINLINE void Plane::rotate(const quat &qRotation)
  {
    float w = m_v.w;
    qRotation.rotate(m_v, m_v);
    m_v.w = w;
  }

  ///////////////////////////////////////////////////////////////

  class Box4
  {
  public:
    vector4 m_vMin;
    vector4 m_vMax;

  public:
    // overload new, delete, new[] and delete[] to provide aligned allocation
    // SOVERLOAD_NEW_ALIGNED(Box4, 16)

    Box4(){};
    ~Box4(){};
    Box4(const Box4 &box);
    Box4(const vector4 &vMin, const vector4 &vMax);

    inline const vector4 &getMin() const { return m_vMin; }
    inline const vector4 &getMax() const { return m_vMax; }

    void booleanOr (const Box4 &box);
    void booleanOr (const vector4 &v);

    bool in(const Box4 &box) const;
    bool intersects(const Box4 &box) const;

    void getCenter(vector4 &v) const;
    void getDimensions(vector4 &v) const;
    void offset(const vector4 &v);
    void expand(const vector4 &v);

    void deflateToOctant(bool highX, bool highY, bool highZ);
  };

  ///////////////////////////////////////////////////////////////

  class Tablet
  {
  protected:
    Sphere m_Sphere;
    Plane m_Plane; // truncation plane relative to sphere center

  public:
    Tablet() {}
    Tablet(const Sphere &sphere, float fDistanceX); // initialize with x-axis plane
    Tablet(const Sphere &sphere, const Plane &plane);

    void transform(const matrix4 &mtx);
    void rotate(const matrix4 &mtx);
    void rotate(const quat &qRotation);

    const Plane &getPlane() const { return m_Plane; }
    Plane &planeRef() { return m_Plane; }

    const Sphere &getSphere() const { return m_Sphere; }
    Sphere &sphereRef() { return m_Sphere; }
  };

  inline Tablet::Tablet(const Sphere &sphere, float fDistanceX) : m_Sphere(sphere),
                                                                  m_Plane(1.0f, 0.0f, 0.0f, fDistanceX)
  {
  }

  inline Tablet::Tablet(const Sphere &sphere, const Plane &plane) : m_Sphere(sphere),
                                                                    m_Plane(plane)
  {
  }

  inline void Tablet::transform(const matrix4 &mtx)
  {
    m_Sphere.transform(mtx);
    m_Plane.rotate(mtx);
  }

  inline void Tablet::rotate(const matrix4 &mtx)
  {
    m_Plane.rotate(mtx);
  }

  inline void Tablet::rotate(const quat &qRotation)
  {
    m_Plane.rotate(qRotation);
  }

  class ParentLink
  {
  public:
    virtual ~ParentLink();
    //		virtual void getLtoW( matrix4& rMtx )	{ rMtx.identity(); /* default to identity */ }
    //		virtual void GetWtoL( matrix4& rMtx )	{ rMtx.identity(); /* default to identity */ }
    virtual void toParentFrame(matrix4 &rMtx)
    { /* default to identity */
    }
    virtual void fromParentFrame(matrix4 &rMtx)
    { /* default to identity */
    }

    // nPosition --> (-1)-->'pre' (local) transform, (0)->'set' transform, (1)-->'post' (parent space)
    virtual bool modifyRelativeTransform(const matrix4 &crMtxLtoP, int nPosition = 0) { return false; }
  };

  // SALIGN_DECL(16)
  class BodyBase
  {
  public:
    // overload new, delete, new[] and delete[] to provide aligned allocation
    // SOVERLOAD_NEW_ALIGNED(BodyBase, 16)

    enum
    {
      // dirty flags
      DIRTY_MATRIX = (1 << 0), // local to world

      DIRTY_QUATERNION = (1 << 1), // quaternion rotation
      DIRTY_EULER = (1 << 2),      // euler rotation

      DIRTY_BOUNDINGSPHERE = (1 << 3), // world bounding sphere needs to be recalculated from
      // position, rotation, and relative bounding sphere

      DIRTY_RELATIVESPHERE = (1 << 4), // geometry has changed, so relative bounding sphere
      // needs to be recalculated (as well as transformed bounding sphere)

      DIRTY_SCALE = (1 << 5), // currently unused

      DIRTY_INTERNAL_DEPENDENCY = (1 << 6), // hierarchy node, etc. needs updating
      DIRTY_USER07 = (1 << 7),
      DIRTY_USER08 = (1 << 8),
      DIRTY_USER09 = (1 << 9),
      DIRTY_USER10 = (1 << 10),
      DIRTY_USER11 = (1 << 11),
      DIRTY_USER12 = (1 << 12),
      DIRTY_USER13 = (1 << 13),
      DIRTY_USER14 = (1 << 14),
      DIRTY_USER15 = (1 << 15),

      SBODY_USER_FLAG_SHIFT = 16,
      SBODY_USER_FLAG_0 = (1 << SBODY_USER_FLAG_SHIFT),
      SBODY_USER_FLAG_COUNT = (28 - SBODY_USER_FLAG_SHIFT), // top 4 bits are reserved
                                                            // for Body3D flag and anim flags

      SBODY_NOEXTRACT_POS = (1 << 28), // animation of position should stay in hierarchy
      SBODY_NOEXTRACT_ROT = (1 << 29), // animation of rotation should stay in hierarchy
                                       //			SBODY_UNUSED_30				= (1<<30),
      SBODY_DERIVED_3D = (1 << 31),    // is derived from Body3D

      DIRTY_MASK =
          DIRTY_MATRIX |
          DIRTY_QUATERNION |
          DIRTY_EULER |
          DIRTY_BOUNDINGSPHERE |
          DIRTY_RELATIVESPHERE |
          DIRTY_SCALE |
          DIRTY_INTERNAL_DEPENDENCY |
          DIRTY_USER07 |
          DIRTY_USER08 |
          DIRTY_USER09 |
          DIRTY_USER10 |
          DIRTY_USER11 |
          DIRTY_USER12 |
          DIRTY_USER13 |
          DIRTY_USER14 |
          DIRTY_USER15,

      DIRTY_DEFAULT =
          DIRTY_BOUNDINGSPHERE,

    } DIRTYFLAGS;

  private:
    uint32_t m_DirtyFlags; // dirty flags
    BodyBase *m_pSibling;
    BodyBase *m_pFirstChild;
    BodyBase *m_pParent;
    ParentLink *m_pLocalToParent; // if non-NULL, then LtoW = LtoP * PtoW, otherwise
                                  //		matrix4*		m_pLocalToParent;	// if non-NULL, then LtoW = LtoP * PtoW, otherwise
                                  // LtoP is assumed identity, so LtoW = PtoW
                                  // (m_LtoW is usually the matrix referenced externally)

    void _addChild(BodyBase *pChild, ParentLink *pLocalToParent);
    void _removeChild(BodyBase *pChild);

  public:
    // all non-virtual set position/rotation functions are routed through these virtual functions
    BodyBase();
    virtual ~BodyBase();

    virtual void setDirty(uint32_t dirty);
    inline void clrDirty(uint32_t dirty)
    {
      SASSERT(0 == (dirty & ~DIRTY_MASK));
      m_DirtyFlags &= ~dirty;
    }

    inline bool isDirty(uint32_t dirty) const { return (0 != (m_DirtyFlags & dirty)); }

    inline void internalSetDirty(uint32_t dirty)
    {
      SASSERT(0 == (dirty & ~DIRTY_MASK));
      m_DirtyFlags |= dirty;
    }

    virtual void attachChild(BodyBase *pChild, ParentLink *pLocalToParent = 0);
    virtual void detachFromParent();
    inline BodyBase *getParent() const { return m_pParent; }
    inline ParentLink *getLtoP() const { return m_pLocalToParent; }

    void notifyLinkChange(ParentLink *pLink, uint32_t uFlags = DIRTY_INTERNAL_DEPENDENCY); // dirties child whose link is pLink

    void detachAllChildren();

    inline bool isAttached() const { return (NULL != m_pParent); }

    inline bool isBody3D() const { return getUserFlags(SBODY_DERIVED_3D); }

    inline void setExtractPos(bool bExtract)
    {
      if (bExtract)
        clrUserFlags(SBODY_NOEXTRACT_POS);
      else
        setUserFlags(SBODY_NOEXTRACT_POS);
    }
    inline bool getExtractPos() const { return !getUserFlags(SBODY_NOEXTRACT_POS); }

    inline void setExtractRot(bool bExtract)
    {
      if (bExtract)
        clrUserFlags(SBODY_NOEXTRACT_ROT);
      else
        setUserFlags(SBODY_NOEXTRACT_ROT);
    }
    inline bool getExtractRot() const { return !getUserFlags(SBODY_NOEXTRACT_ROT); }

  protected:
    inline void setUserFlags(uint32_t uFlags)
    {
      SASSERT(0 == (uFlags & DIRTY_MASK));
      m_DirtyFlags |= uFlags;
    }
    inline void clrUserFlags(uint32_t uFlags)
    {
      SASSERT(0 == (uFlags & DIRTY_MASK));
      m_DirtyFlags &= ~uFlags;
    }
    inline bool getUserFlags(uint32_t uFlags) const
    {
      SASSERT(0 == (uFlags & DIRTY_MASK));
      return (0 != (m_DirtyFlags & uFlags));
    }

  } /* SALIGN(16) */;

  SBODYBASEINLINE BodyBase::BodyBase() : m_DirtyFlags(DIRTY_DEFAULT),
                                         m_pSibling(NULL),
                                         m_pFirstChild(NULL),
                                         m_pParent(NULL),
                                         m_pLocalToParent(NULL)
  {
  }

  // SALIGN_DECL(16)
  class Body3D : public BodyBase
  {
  private:
    //		vector4	m_Position;		// Current position
    quat m_Quaternion;  // Current rotation in quaternion form
    vector4 m_Rotation; // Current rotation in Nasa Standard Airplane / euler form

    Sphere m_RelativeSphere; // bounding sphere, relative to position
    Sphere m_BoundingSphere; // bounding sphere (transformed by position & rotation)

    matrix4 m_LtoW; // local to world transfrom

    void buildLtoW();

  public:
    // overload new, delete, new[] and delete[] to provide aligned allocation
    //		SOVERLOAD_NEW_ALIGNED(Body3D, 16)

    Body3D();
    virtual ~Body3D();

    virtual void attachChild(BodyBase *pChild, ParentLink *pLocalToParent = 0);
    virtual void detachFromParent();

    // all non-virtual set position/rotation functions are routed through these virtual functions
    virtual void setPosition(float x, float y, float z);
    virtual void setRotation(float x, float y, float z);
    virtual void setQuaternion(const quat &q);
    virtual void setLtoW(const matrix4 &mtx);

    virtual void recomputeRelativeBoundingSphere() { clrDirty(DIRTY_RELATIVESPHERE); }

    //		void setRelativeLtoP( const matrix4& LtoP );
    //		void clearRelativeLtoP();

    void getBoundingSphere(Sphere &s); // non-const because this may recalculate bounding sphere
    const Sphere &getBoundingSphere(); // non-const because this may recalculate bounding sphere

    void getRelativeBoundingSphere(Sphere &s); // non-const because this may recalculate relative bounding sphere
    const Sphere &getRelativeBoundingSphere(); // non-const because this may recalculate relative bounding sphere

    void setRelativeBoundingSphere(const Sphere &s);

    inline void setPosition(const vector4 &pos) { setPosition(pos.x, pos.y, pos.z); }
    inline void setPosition(const vector3 &pos) { setPosition(pos.x, pos.y, pos.z); }

    const vector4 &getPosition();
    const quat &getQuaternion();
    const vector4 &getRotation();

    void getPosition(vector4 &dst);
    void getQuaternion(quat &dst);
    void getRotation(vector4 &dst);

    const matrix4 &getLtoW();
    void getLtoW(matrix4 &m);

    inline void identity() { setLtoW(matrix4::I); }

    void mul(const matrix4 &r);
    void mulSub(const matrix4 &r);
    void lMul(const matrix4 &left);
    void lMulSub(const matrix4 &left);

    void mul(const quat &r);
    void mulSub(const quat &r);
    void lMul(const quat &left) { lMulSub(left); }
    void lMulSub(const quat &left);

    const Body3D &lookAt(const vector4 &vPosition, const vector4 &vTarget, const vector4 &vUp);

    void getUp(vector4 &v);
    void getDown(vector4 &v);
    void getLeft(vector4 &v);
    void getRight(vector4 &v);
    void getForward(vector4 &v);
    void getBackward(vector4 &v);

    bool isPositionValid();

    /// >>> XXX to go away
    //		void attach(const matrix4* pMatrix);
    //		void detach();
    //		bool isAttached()						{ if (m_pAttachBtoW == NULL) return false; return true; }
    /// <<< XXX to go away

    const Body3D &operator*=(const matrix4 &r);
    const Body3D &operator*=(const quat &r);
  } /* SALIGN(16) */;

  SBODYBASEINLINE const matrix4 &Body3D::getLtoW()
  {
    /// >>> XXX to go away
    ///		if (isDirty(DIRTY_MATRIX) || (m_pAttachBtoW))
    /// <<< XXX to go away
    if (isDirty(DIRTY_MATRIX | DIRTY_INTERNAL_DEPENDENCY))
    {
      buildLtoW();
    }

    return m_LtoW;
  }

  SBODYBASEINLINE void Body3D::getLtoW(matrix4 &m)
  {
    /// >>> XXX to go away
    ///		if (isDirty(DIRTY_MATRIX) || (m_pAttachBtoW))
    /// <<< XXX to go away
    if (isDirty(DIRTY_MATRIX | DIRTY_INTERNAL_DEPENDENCY))
    {
      buildLtoW();
    }
    m = m_LtoW;
  }

  SBODYBASEINLINE void Body3D::setRelativeBoundingSphere(const Sphere &s)
  {
    m_RelativeSphere = s;
    clrDirty(DIRTY_RELATIVESPHERE);
    setDirty(DIRTY_BOUNDINGSPHERE);
  }

  SBODYBASEINLINE void Body3D::getBoundingSphere(Sphere &s)
  {
    s = getBoundingSphere();
  }

  SBODYBASEINLINE const Sphere &Body3D::getBoundingSphere()
  {
    if (isDirty(DIRTY_MATRIX | DIRTY_BOUNDINGSPHERE | DIRTY_RELATIVESPHERE)
        //|| isAttached()
    )
    {
      m_BoundingSphere = getRelativeBoundingSphere();
      m_BoundingSphere.transform(getLtoW());

      clrDirty(DIRTY_BOUNDINGSPHERE);
    }

    return m_BoundingSphere;
  }

  SBODYBASEINLINE void Body3D::getRelativeBoundingSphere(Sphere &s)
  {
    s = getRelativeBoundingSphere();
  }

  SBODYBASEINLINE const Sphere &Body3D::getRelativeBoundingSphere()
  {
    if (isDirty(DIRTY_RELATIVESPHERE))
    {
      recomputeRelativeBoundingSphere();
    }
    return m_RelativeSphere;
  }

  SBODYBASEINLINE const vector4 &Body3D::getPosition()
  {
    /*
		if (isDirty(DIRTY_POSITION))
		{
			SASSERT( !isDirty(DIRTY_MATRIX) );
			m_Position = m_LtoW.col[3];
			clrDirty(DIRTY_POSITION);
		}
		return m_Position;
*/
    /*
		if (m_pAttachBtoW)
		{
			return (m_pAttachBtoW->col[3]);
		}
*/
    if (isDirty(DIRTY_MATRIX | DIRTY_INTERNAL_DEPENDENCY))
    {
      buildLtoW();
    }
    return m_LtoW.col[3];
  }

  SBODYBASEINLINE void Body3D::getPosition(vector4 &dst) { dst = getPosition(); }
  SBODYBASEINLINE void Body3D::getQuaternion(quat &dst) { dst = getQuaternion(); }
  SBODYBASEINLINE void Body3D::getRotation(vector4 &dst) { dst = getRotation(); }

  SBODYBASEINLINE void Body3D::getUp(vector4 &v)
  {
    const matrix4 &m = getLtoW();
    v.set(m.m01, m.m11, m.m21); // y-axis is up
  }

  SBODYBASEINLINE void Body3D::getDown(vector4 &v)
  {
    const matrix4 &m = getLtoW();
    v.set(-m.m01, -m.m11, -m.m21); // -y-axis is down
  }

  SBODYBASEINLINE void Body3D::getLeft(vector4 &v)
  {
    const matrix4 &m = getLtoW();
#if SCOORDINATE_SYSTEM_IS_LEFT_HANDED
    v.set(-m.m00, -m.m10, -m.m20); // -x-axis is left
#else
    v.set(m.m00, m.m10, m.m20);    // x-axis is left
#endif
  }

  SBODYBASEINLINE void Body3D::getRight(vector4 &v)
  {
    const matrix4 &m = getLtoW();
#if SCOORDINATE_SYSTEM_IS_LEFT_HANDED
    v.set(m.m00, m.m10, m.m20); // x-axis is right
#else
    v.set(-m.m00, -m.m10, -m.m20); // -x-axis is right
#endif
  }

  SBODYBASEINLINE void Body3D::getForward(vector4 &v)
  {
    const matrix4 &m = getLtoW();
    v.set(m.m02, m.m12, m.m22); // z-axis is forward
  }

  SBODYBASEINLINE void Body3D::getBackward(vector4 &v)
  {
    const matrix4 &m = getLtoW();
    v.set(-m.m02, -m.m12, -m.m22); // -z-axis is backward
  }

  SBODYBASEINLINE void Body3D::mul(const matrix4 &r)
  {
    // TO-CHECK: mul
    matrix4::mul(m_LtoW, getLtoW(), r);
    setDirty(
        DIRTY_QUATERNION |
        DIRTY_EULER |
        DIRTY_BOUNDINGSPHERE);
  }

  SBODYBASEINLINE void Body3D::mulSub(const matrix4 &r)
  {
    matrix4::mulSub(m_LtoW, getLtoW(), r);
    setDirty(
        DIRTY_QUATERNION |
        DIRTY_EULER |
        DIRTY_BOUNDINGSPHERE);
  }

  SBODYBASEINLINE void Body3D::lMul(const matrix4 &left)
  {
    // TO-CHECK: mul
    matrix4::mul(m_LtoW, left, getLtoW());
    setDirty(
        DIRTY_QUATERNION |
        DIRTY_EULER |
        DIRTY_BOUNDINGSPHERE);
  }

  SBODYBASEINLINE void Body3D::lMulSub(const matrix4 &left)
  {
    matrix4::mulSub(m_LtoW, left, getLtoW());
    setDirty(
        DIRTY_QUATERNION |
        DIRTY_EULER |
        DIRTY_BOUNDINGSPHERE);
  }

  SBODYBASEINLINE const Body3D &Body3D::operator*=(const matrix4 &r)
  {
    mul(r);
    return *this;
  }

  SBODYBASEINLINE void Body3D::mul(const quat &r)
  {
    r.rotate(m_LtoW.col[3], getPosition());
    quat::mul(m_Quaternion, getQuaternion(), r);
    setDirty(
        DIRTY_MATRIX |
        DIRTY_EULER |
        DIRTY_BOUNDINGSPHERE);
  }

  SBODYBASEINLINE void Body3D::mulSub(const quat &r)
  {
    quat::mul(m_Quaternion, getQuaternion(), r);
    setDirty(
        DIRTY_MATRIX |
        DIRTY_EULER |
        DIRTY_BOUNDINGSPHERE);
  }

  SBODYBASEINLINE void Body3D::lMulSub(const quat &left)
  {
    quat::mul(m_Quaternion, left, getQuaternion());
    setDirty(
        DIRTY_MATRIX |
        DIRTY_EULER |
        DIRTY_BOUNDINGSPHERE);
  }

  SBODYBASEINLINE const Body3D &Body3D::operator*=(const quat &r)
  {
    mul(r);
    return *this;
  }

};

#include "Plane.inl"
#include "Sphere.inl"
#include "Box4.inl"

#endif // _STEVESCH_RENDER_GEOM_SGEOM_H__
