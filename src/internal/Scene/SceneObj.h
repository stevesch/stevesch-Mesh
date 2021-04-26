#ifndef STEVESCH_RENDER_SCENE_SCENEOBJ_H_
#define STEVESCH_RENDER_SCENE_SCENEOBJ_H_

#include <stevesch-MathVec.h>

namespace stevesch
{

  class SceneObj
  {
  public:
    stevesch::quat mqLtoW;    // LtoW rotation
    stevesch::vector3 mvLtoW; // LtoW position
    stevesch::vector3 mAngularV;
    stevesch::vector3 mLinearV;
    float mRadius;
    //FaceMesh* mMesh;

  public:
    SceneObj()
        : mqLtoW(1.0f), mvLtoW(stevesch::c_vZERO), mAngularV(stevesch::c_vZERO), mLinearV(stevesch::c_vZERO), mRadius(1.0f)
    //, mMesh(nullptr)
    {
    }

    inline void calcLtoW(stevesch::matrix4 &ltow) const
    {
      mqLtoW.toMatrix(ltow, mvLtoW);
    }

    void updateTransform(float dt);
  };

  void randomizeAngularVelocity(stevesch::vector3 &angularVelocity, float speedMin, float speedMax);

}

#endif
