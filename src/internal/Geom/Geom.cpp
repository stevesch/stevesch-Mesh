// TO-CHECK: Plane::transform
#include "Geom.h"
#include <cmath>

#ifdef SDEBUG_TRACE_LINE
#undef STHIS_FILE
static char STHIS_FILE[] = __FILE__;
#define new SDEBUG_NEW
#endif

namespace stevesch
{
  template <typename T_PTR> void SSAFE_DELETE(T_PTR& ptr) {
    if (ptr) {
      delete ptr;
      ptr = nullptr;
    }
  }

  // template <typename T_PTR> void SSAFE_DELETE_ARRAY(T_PTR& arrayPtr) {
  //   if (arrayPtr) {
  //     delete [] arrayPtr;
  //     arrayPtr = nullptr;
  //   }
  // }

  // given a unit length "normal" vector n, generate vectors p and q vectors
  // that are an orthonormal basis for the plane space perpendicular to n.
  // i.e. this makes p,q such that n,p,q are all perpendicular to each other.
  // q will equal n x p. if n is not unit length then p will be unit length but
  // q wont be.
  void SPlaneSpace(const vector4 &n, vector4 &p, vector4 &q)
  {
    if (fabsf(n.z) > stevesch::c_fRecipSqrt2) // M_SQRT1_2
    {
      // choose p in y-z plane
      float a = n.y * n.y + n.z * n.z;
      float k = stevesch::rsqrtf(a);
      p.x = 0.0f;
      p.y = -n.z * k;
      p.z = n.y * k;
      p.w = 1.0f;

      // set q = n x p
      q.x = a * k;
      q.y = -n.x * p.z;
      q.z = n.x * p.y;
      q.w = 1.0f;
    }
    else
    {
      // choose p in x-y plane
      float a = n.x * n.x + n.y * n.y;
      float k = stevesch::rsqrtf(a);
      p.x = -n.y * k;
      p.y = n.x * k;
      p.z = 0.0f;
      p.w = 1.0f;
      // set q = n x p
      q.x = -n.z * p.y;
      q.y = n.z * p.x;
      q.z = a * k;
      q.w = 1.0f;
    }
  }

  ///////////////////////////////////////////////////////////////

  void SPlaneSpace2(matrix4 &rBasis, const vector4 &a, const vector4 &b)
  {
    vector4::normalize3(rBasis.col[0], a); // = a^

    vector4::addScaled3(rBasis.col[1], b, rBasis.col[0], -rBasis.col[0].dot3(b)); // j = b - (b.a^)*a^
    rBasis.col[1].normalize3();
    vector4::cross3(rBasis.col[2], rBasis.col[0], rBasis.col[1]);

    // complete the matrix
    rBasis.col[0].w = 0.0f;
    rBasis.col[1].w = 0.0f;
    rBasis.col[2].w = 0.0f;
    rBasis.col[3].set(0.0f, 0.0f, 0.0f, 1.0f);
  }

  ///////////////////////////////////////////////////////////////

  float Plane::timeToPlaneAlongVector(const vector4 &vPoint, const vector4 &vDir) const
  {
    // plane eqn.: vOnPlane.vNormal - d = 0
    // line eqn.:  vOnLine = vPoint + vDir*t -->
    // setting vOnPlane == vOnLine yields:
    // (vPoint + vDir*t).vNormal - d = 0
    // --> vPoint.vNormal + vDir.vNormal*t - d = 0
    // --> t = (d - vPoint.vNormal) / (vDir.vNormal)
    float fNegDistPerp = getDistance() - dotNormal(vPoint);
    float fDir = dotNormal(vDir);

    return fNegDistPerp / fDir;
  }

  void Plane::transform(const matrix4 &mtx)
  {
    // transform plane by mtx:

    // p0 = n0*d0
    // p1 = p0 * A;
    // n1 = n0 * Ar;
    // d1 = p1 . n1
    /*
		// d1 = d0*[n0*A . n0*Ar], where A=Ar*At (Ar is rotational part, At is translation)
		// n1 = n0*Ar
		vector4 nA( m_v.x, m_v.y, m_v.z, 1.0f );
		float fDistance = m_v.w;
		m_v.mulSub( mtx );				// n1 = n0*Ar
		nA *= mtx;						// nA = n0*A
		float fFactor = nA.dot(m_v);	// n0*A . n0*Ar
		m_v.w = fDistance * fFactor;
*/
    vector4 p1;
    vector4::scale3(p1, m_v, m_v.w);
    p1.w = 1.0f;
    vector4::transform(p1, mtx, p1);      // old row: p1 *= mtx;
    vector4::transformSub(m_v, mtx, m_v); // old row: m_v.mulSub( mtx );
    m_v.w = p1.dot3(m_v);
  }

  ///////////////////////////////////////////////////////////////

  // virtual
  BodyBase::~BodyBase()
  {
    detachAllChildren();

    detachFromParent();

    // node must be completely detached before destruction
    SASSERT(NULL == m_pLocalToParent);
    SASSERT(NULL == m_pParent);
    SASSERT(NULL == m_pSibling);
    SASSERT(NULL == m_pFirstChild);
  }

  void BodyBase::detachAllChildren()
  {
    while (NULL != m_pFirstChild)
    {
      SASSERT(this == m_pFirstChild->m_pParent);
      m_pFirstChild->detachFromParent();
    }
  }

  void BodyBase::setDirty(uint32_t dirty)
  {
    SASSERT(0 == (dirty & ~DIRTY_MASK));

    m_DirtyFlags |= dirty;
    BodyBase *pCurrent = m_pFirstChild;
    while (pCurrent)
    {
      pCurrent->setDirty(dirty);
      pCurrent = pCurrent->m_pSibling;
    }
  }

  void BodyBase::detachFromParent()
  {
    if (m_pParent)
    {
      m_pParent->_removeChild(this); // cleanly disconnects linkage with parent and sibling
    }
  }

  void BodyBase::attachChild(BodyBase *pChild, ParentLink *pLocalToParent)
  {
    pChild->detachFromParent();
    _addChild(pChild, pLocalToParent);
  }

  void BodyBase::notifyLinkChange(ParentLink *pLink, uint32_t uFlags)
  {
    // dirties child whose link is pLink
    BodyBase *pCurrent = m_pFirstChild;
    while (pCurrent)
    {
      if (pLink == pCurrent->getLtoP())
      {
        pCurrent->setDirty(uFlags);
        break;
      }
      pCurrent = pCurrent->m_pSibling;
    }
  }

  void BodyBase::_addChild(BodyBase *pChild, ParentLink *pLocalToParent)
  {
    // adds the new child to the head of the sibling list

    BodyBase *pPrevChild = m_pFirstChild;

    m_pFirstChild = pChild;
    pChild->m_pSibling = pPrevChild;

    pChild->m_pParent = this;

    uint32_t uDirty =
        DIRTY_MATRIX |
        DIRTY_QUATERNION |
        DIRTY_EULER |
        DIRTY_BOUNDINGSPHERE;

    if (pLocalToParent != pChild->m_pLocalToParent)
    {
      SSAFE_DELETE(pChild->m_pLocalToParent);
      pChild->m_pLocalToParent = pLocalToParent;
    }

    if (pLocalToParent)
    {
      uDirty |= DIRTY_INTERNAL_DEPENDENCY;
    }

    pChild->setDirty(uDirty);
  }

  void BodyBase::_removeChild(BodyBase *pChild)
  {
    BodyBase *pPrev = NULL;
    BodyBase *pCurrent = m_pFirstChild;

    while (pCurrent)
    {
      if (pCurrent == pChild)
      {
        // child located-- unlink it
        pCurrent->m_pParent = NULL;

        if (pPrev)
        {
          pPrev->m_pSibling = pCurrent->m_pSibling;
        }
        else
        {
          m_pFirstChild = pCurrent->m_pSibling;
        }
        pCurrent->m_pSibling = NULL;

        // do this after unlinking, because it can
        // end up deleting this, which causes a loop
        // if 'pCurrent' isn't already unlinked
        SSAFE_DELETE(pCurrent->m_pLocalToParent);
        break;
      }

      pPrev = pCurrent;
      pCurrent = pCurrent->m_pSibling;
    }
    /*
		pChild->setDirty(
			DIRTY_MATRIX |
			DIRTY_QUATERNION |
			DIRTY_EULER |
			DIRTY_BOUNDINGSPHERE |
			DIRTY_INTERNAL_DEPENDENCY );
*/
  }

  ///////////////////////////////////////////////////////////////

  Body3D::Body3D() : //		m_pAttachBtoW	(NULL),	/// XXX to go away?

                     m_Quaternion(0.0f, 0.0f, 0.0f, 1.0f),
                     m_Rotation(0.0f, 0.0f, 0.0f),
                     m_RelativeSphere(1.0f), // offset = (0, 0, 0), radius = 1
                     m_BoundingSphere(1.0f), // offset = (0, 0, 0), radius = 1
                     m_LtoW(1.0f)
  {
    setUserFlags(SBODY_DERIVED_3D);
  }

  // virtual
  void Body3D::setPosition(float x, float y, float z)
  {
    m_LtoW.col[3].x = x;
    m_LtoW.col[3].y = y;
    m_LtoW.col[3].z = z;
    setDirty(
        DIRTY_BOUNDINGSPHERE);
  }

  // virtual
  void Body3D::attachChild(BodyBase *pChild, ParentLink *pLocalToParent)
  {
    BodyBase::attachChild(pChild, pLocalToParent);
  }

  // virtual
  void Body3D::detachFromParent()
  {
    if (isAttached())
    {
      // preserve our world-frame transform
      matrix4 mtxLtoW;
      getLtoW(mtxLtoW);

      BodyBase::detachFromParent();

      setLtoW(mtxLtoW);
    }
  }

  // virtual
  void Body3D::setRotation(float x, float y, float z)
  {
    m_Rotation.x = x;
    m_Rotation.y = y;
    m_Rotation.z = z;
    clrDirty(DIRTY_EULER);
    setDirty(
        DIRTY_QUATERNION |
        DIRTY_MATRIX |
        DIRTY_BOUNDINGSPHERE);
  }

  Body3D::~Body3D()
  {
    // must do this at each derived level because our children may
    // require full instantiation of Body3D, but by the time we
    // get into the BodyBase destructor, our 'getLtoW' (called
    // from within our childrens' detachFromParent) won't be legit.
    detachAllChildren();

    detachFromParent(); // must do at this level because detachFromParent is virtual
                        // (and within destructors, virtual functions are not
                        //  called above current derivation level-- i.e. calling
                        //  detachFromParent from within BodyBase would
                        //  call BodyBase::detachFromParent, NOT Body3D::detachFromParent
                        //  even if the object is an Body3D.)
  }

  // virtual
  void Body3D::setQuaternion(const quat &q)
  {
    m_Quaternion = q;
    clrDirty(DIRTY_QUATERNION);
    setDirty(
        DIRTY_EULER |
        DIRTY_MATRIX |
        DIRTY_BOUNDINGSPHERE);
  }

  // virtual
  void Body3D::setLtoW(const matrix4 &mtx)
  {
    m_LtoW = mtx;
    clrDirty(DIRTY_MATRIX);
    setDirty(
        DIRTY_QUATERNION |
        DIRTY_EULER |
        //DIRTY_SCALE |
        DIRTY_BOUNDINGSPHERE);
  }

  /*
	void Body3D::setRelativeLtoP( const matrix4& LtoP )
	{
		if (NULL == m_pLocalToParent)
		{
			m_pLocalToParent = new matrix4( LtoP );
		}
		else
		{
			*m_pLocalToParent = LtoP;
		}
	}


	void Body3D::clearRelativeLtoP()
	{
		SSAFE_DELETE( m_pLocalToParent );
	}
*/
  bool Body3D::isPositionValid()
  {
    const vector4 &rPos = getPosition();

    do
    {
      if (std::isnan(rPos.x))
        break;

      if (std::isnan(rPos.y))
        break;

      if (std::isnan(rPos.z))
        break;

      if (!(rPos.y > -50000.0f))
        break;

      return true;

    } while (false);

    return false;
  }

  void Body3D::buildLtoW()
  {
    if (isDirty(DIRTY_MATRIX | DIRTY_INTERNAL_DEPENDENCY))
    {
      //m_pLocalToParent
      if (getLtoP())
      {
        m_LtoW.identity();
        getLtoP()->toParentFrame(m_LtoW); //getLtoW( m_LtoW );
      }
      else
      {
        vector4 vPosition = m_LtoW.col[3];

        SASSERT(!isDirty(DIRTY_INTERNAL_DEPENDENCY));

        if (isDirty(DIRTY_EULER))
        {
          SASSERT(!isDirty(DIRTY_QUATERNION));
          m_Quaternion.toMatrix(m_LtoW, vPosition);
        }
        else
        {
#if SCOORDINATE_SYSTEM_IS_NASA_STANDARD_AIRPLANE
          const vector4 &rot = m_Rotation;
          m_LtoW.zMatrixRH(rot.z); // yaw (z) RH

          matrix4 m;
          if (rot.y != 0.0f)
          {
            m.yMatrixRH(rot.y); // pitch (y) RH
            m_LtoW.rMulSub(m);
          }

          if (rot.x != 0.0f)
          {
            m.xMatrixRH(rot.x); // roll (x) RH
            m_LtoW.rMulSub(m);
          }
#else
          const vector4 &rot = m_Rotation;
          m_LtoW.yMatrix(rot.y); // yaw (y)

          matrix4 m;
          if (rot.x != 0.0f)
          {
            m.xMatrix(rot.x); // pitch (x)
            m_LtoW.mulSub(m); // TO-CHECK: LMulSub
          }

          if (rot.z != 0.0f)
          {
            m.zMatrix(rot.z); // roll (z)
            m_LtoW.mulSub(m); // TO-CHECK: LMulSub
          }
#endif

          m_LtoW.col[3] = vPosition;
        }
      }

      // Clear the dirty flag
      clrDirty(DIRTY_MATRIX | DIRTY_INTERNAL_DEPENDENCY);
    }
    /*
		if (m_pParent)
		{
			matrix4::mul( m_LtoW, m_pParent->getLtoW(), m_LtoW );
		}
*/
  }

  const quat &Body3D::getQuaternion()
  {
    if (isDirty(DIRTY_QUATERNION | DIRTY_INTERNAL_DEPENDENCY)
        //|| (m_pAttachBtoW)
    )
    {
      if (isDirty(DIRTY_EULER))
      {
        //				SASSERT( m_pAttachBtoW || !isDirty(DIRTY_MATRIX) );
        SASSERT(!isDirty(DIRTY_MATRIX) || isDirty(DIRTY_INTERNAL_DEPENDENCY) || getLtoP());
        m_Quaternion.fromOrthonormalMatrix(getLtoW());
      }
      else
      {
        m_Quaternion.fromEuler(m_Rotation);
      }
      clrDirty(DIRTY_QUATERNION);
    }
    return m_Quaternion;
  }

  const vector4 &Body3D::getRotation()
  {
    if (isDirty(DIRTY_EULER | DIRTY_INTERNAL_DEPENDENCY)
        //|| (m_pAttachBtoW)
    )
    {
      if (isDirty(DIRTY_QUATERNION))
      {
        //				SASSERT( m_pAttachBtoW || !isDirty(DIRTY_MATRIX) );
        SASSERT(!isDirty(DIRTY_MATRIX) || isDirty(DIRTY_INTERNAL_DEPENDENCY) || getLtoP());
        m_Quaternion.fromOrthonormalMatrix(getLtoW());
        clrDirty(DIRTY_QUATERNION);
      }

      m_Quaternion.toEuler(m_Rotation);
      clrDirty(DIRTY_EULER);
    }
    return m_Rotation;
  }

  /*	
	void Body3D::attach(const matrix4* pMatrix)
	{
		setDirty( DIRTY_MATRIX | DIRTY_QUATERNION | DIRTY_DEFAULT );
		m_pAttachBtoW = pMatrix;
	}
	

	void Body3D::detach()
	{
		if (m_pAttachBtoW)
		{
			setLtoW( *m_pAttachBtoW );
			m_pAttachBtoW = NULL;
		}
	}
*/

  const Body3D &Body3D::lookAt(const vector4 &vPosition, const vector4 &vTarget, const vector4 &vUp)
  {
    matrix4 LtoW;
    matrix4::lookAtLHWorld(LtoW, vPosition, vTarget, vUp);
    setLtoW(LtoW);
    return *this;
  }

  // returns true if this segment intersects seg1
  bool Segment::testColinearIntersection(const Segment &seg1) const
  {
    vector4 vBasis;
    vector4 vTemp;

    const vector4 &v0 = getV0();

    vector4::sub3(vBasis, getV1(), v0);

    // all following t values are getV0() + t*vBasis

    float fRecipBasisMagSquared = vBasis.recipSquareMag3(); // 1 / (vBasis . vBasis)

    vector4::sub3(vTemp, seg1.getV0(), v0);
    float tj0 = vBasis.dot3(vTemp) * fRecipBasisMagSquared;

    vector4::sub3(vTemp, seg1.getV1(), v0);
    float tj1 = vBasis.dot3(vTemp) * fRecipBasisMagSquared;

    // sort parameterization values
    // (ti0 > ti1), ti0 == 0.0 by definition
    if (tj0 > tj1)
    {
      float t = tj0;
      tj0 = tj1;
      tj1 = t;
    }

    // these values are defined implicitly:
    float ta = 0.0f; // ti0
    float tb = 1.0f; // ti1

    // find the intersection of the line segments (assuming they're colinear)
    // also choose the normal that we want to use
    if (tj0 > ta)
    {
      ta = tj0;
    }

    if (tj1 < tb)
    {
      tb = tj1;
    }

    // final test for intersection-- if segment's length is negative,
    // then the lines don't intersect
    bool bIntersect = (ta <= tb);
    return bIntersect;
  }

  // returns true if this segment intersects seg1
  // this segment is replaced by the union of the two input segments
  // line status bits are set as follows
  //
  ////////////////////////////////////////
  // LineStatus:
  //
  // bottom 2 bits show which normal should be used:
  // 0x00: edgei1 x edgej0
  // 0x01: -nj
  // 0x02: edgej1 x edgei0
  // 0x03: ni
  //
  // high bit is set if seg1 is opposite direction of this segment
  // 0x80000000: [getV1() - getV0()] . [seg1.getV1() - seg1.getV0()] < 0.0
  ////////////////////////////////////////
  bool Segment::mergeColinearIntersection(const Segment &seg1, uint32_t &LineStatus)
  {
    vector4 vBasis;
    vector4 vTemp;

    LineStatus = 0x01;

    const vector4 &v0 = getV0();

    vector4::sub3(vBasis, getV1(), v0);

    // all following t values are getV0() + t*vBasis

    float fRecipBasisMagSquared = vBasis.recipSquareMag3(); // 1 / (vBasis . vBasis)

    vector4::sub3(vTemp, seg1.getV0(), v0);
    float tj0 = vBasis.dot3(vTemp) * fRecipBasisMagSquared;

    vector4::sub3(vTemp, seg1.getV1(), v0);
    float tj1 = vBasis.dot3(vTemp) * fRecipBasisMagSquared;

    // sort parameterization values
    // (ti0 > ti1), ti0 == 0.0 by definition

    int jIndex = 0;
    if (tj0 > tj1)
    {
      float t = tj0;
      tj0 = tj1;
      tj1 = t;
      jIndex = 1;
      LineStatus |= 0x80000000; // mark seg1 as reversed
    }

    // these values are defined implicitly:
    float ta = 0.0f; // ti0
    float tb = 1.0f; // ti1

    // find the intersection of the line segments (assuming they're colinear)
    // also choose the normal that we want to use
    if (tj0 > ta)
    {
      ta = tj0;
      setV0(seg1.get(jIndex));
      LineStatus ^= 0x01;
    }

    if (tj1 < tb)
    {
      tb = tj1;
      setV1(seg1.get(1 - jIndex));
      LineStatus ^= 0x03;
    }

    // final test for intersection-- if segment's length is negative,
    // then the lines don't intersect
    bool bIntersect = (ta <= tb);
    return bIntersect;
  }

  ParentLink::~ParentLink()
  {
  }

  bool SScreenToWorld(vector4 *pWorld, const vector4 *pScreen, int nVerts,
                      const matrix4 &crViewToScreen, // proj
                      const matrix4 &crScreenToView, // proj^-1
                      const matrix4 *pViewToWorld)   // view^-1
  {
    vector4 vScreen;
    matrix4 mtxScreenToWorld;
    const matrix4 *pScreenToWorld;

    // TO-CHECK: e22
    float Q = crViewToScreen.m22; // Q = fZFar / (fZFar - fZNear)
    // TO-CHECK: e32
    float B = crViewToScreen.m23; // B = -Q * fZNear
    float fZNear = -B / Q;
    //		float fZFar = B / (1.0f - Q);

    if (pViewToWorld)
    {
      // TO-CHECK: mul
      matrix4::mul(mtxScreenToWorld, *pViewToWorld, crScreenToView);
      pScreenToWorld = &mtxScreenToWorld;
    }
    else
    {
      // treat pViewToWorld as identity
      pScreenToWorld = &crScreenToView;
    }

    while (nVerts-- > 0)
    {
      vScreen.x = pScreen->x;
      vScreen.y = pScreen->y;

      // most expanded form, where fzView is the view-relative Z of the screen point (in view coordinates)
      //			vScreen.z = (fZView - fZNear) / (fZFar - fZNear);
      //			vScreen.z *= fZFar;
      //			vScreen.w = (vScreen.z + Q * fZNear)/Q; // == vScreen.z/Q + fZNear

      // simplifying yields:
      //			vScreen.z = (fZView - fZNear) * Q;
      //			vScreen.w = fZView;

      // finally, interpreting vScreen.z as the view-relative Z
      float fZView = pScreen->z; // use temp variable in case we're transforming in place
      vScreen.z = (fZView - fZNear) * Q;
      vScreen.w = fZView;

      vScreen.x *= vScreen.w;
      vScreen.y *= vScreen.w;

      // TO-CHECK: mul
      vector4::transform(*pWorld, *pScreenToWorld, vScreen);

      pWorld++;
      pScreen++;
    }

    return true;
  }

  bool SWorldToScreen(vector4 *pScreen, const vector4 *pWorld, int nVerts,
                      const matrix4 &crViewToScreen, // proj
                      const matrix4 *pWorldToView)   // view
  {
    matrix4 mtxWorldToScreen;

    const matrix4 *pMtxToScreen = &crViewToScreen; // initially view-to-screen transform

    if (pWorldToView)
    {
      // TO-CHECK: mul
      matrix4::mul(mtxWorldToScreen, *pMtxToScreen, *pWorldToView);
      pMtxToScreen = &mtxWorldToScreen;
    }

    //		vector4 vToViewZ;
    //		pWorldToView->GetColumn(2, vToViewZ);

    while (nVerts-- > 0)
    {
      // TO-CHECK: mul
      vector4::transform(*pScreen, *pMtxToScreen, *pWorld);

      pScreen->x /= pScreen->w;
      pScreen->y /= pScreen->w;

      pScreen->z = pScreen->w;
      pScreen->w = 1.0f;

      pWorld++;
      pScreen++;
    }
    return true;
  }

} // namespace stevesch
