#include "SceneObj.h"
//#include <c_types.h>

#ifndef ICACHE_FLASH_ATTR
#define ICACHE_FLASH_ATTR
#endif

namespace stevesch
{

  void SceneObj::updateTransform(float dt)
  {
    vector4 v4;
    v4.set(mAngularV);
    mqLtoW.integrate(v4, dt);

    vector3::addScaled(mvLtoW, mvLtoW, mLinearV, dt); // p = p + v*dt
  }

  void ICACHE_FLASH_ATTR randomizeAngularVelocity(stevesch::vector3 &angularVelocity, float speedMin, float speedMax)
  {
    // for testing-- fixed rotation axis:
#if 0
	{
		constexpr float speed = DegToRad(90.0f);
		angularVelocity.set(0.0f, 1.0f, 0.0f, speed);
		return;
	}
#endif

    // choose random axis and randomized speed around that axis
    vector3 axis;
    axis.randSpherical();
    float speedRange = speedMax - speedMin;
    float angularSpeed = S_RandGen.getFloatAB(-speedRange, speedRange);
    if (angularSpeed < 0.0f)
    {
      angularSpeed -= speedMin;
    }
    else
    {
      angularSpeed += speedMin;
    }

    axis *= angularSpeed;
    angularVelocity = axis;
  }

}
