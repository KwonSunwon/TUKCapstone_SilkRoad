#include "pch.h"
#include "PlayerAnimation.h"

#include "Animator.h"
#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"


void PlayerAnimation::Awake()
{


}
void PlayerAnimation::LateUpdate()
{
	float maxSpeed = GetRigidBody()->GetMaxSpeed();
	float speed = GetRigidBody()->GetXZVelocity().LengthSquared();
	if (speed > pow(maxSpeed * 0.6, 2)) {
		if (GetAnimator()->GetCurrentClipIndex() != 2)
			GetAnimator()->Play(2);
	}
	else if (speed > pow(maxSpeed * 0.01, 2)) {
		if (GetAnimator()->GetCurrentClipIndex() != 1)
			GetAnimator()->Play(1);
	}
	else {
		if (GetAnimator()->GetCurrentClipIndex() != 0)
			GetAnimator()->Play(0);
	}
	
}
