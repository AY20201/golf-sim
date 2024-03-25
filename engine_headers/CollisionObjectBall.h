#ifndef COLLISIONOBJECTBALL_CLASS_H
#define COLLISIONOBJECTBALL_CLASS_H

#include"Transform.h"
#include"Raycast.h"
#include"../game_headers/Surface.h"

class CollisionObjectBall
{
public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	glm::vec3 restingSurfaceNormal;
	Surface* restingSurface;

	float radius;
	Raycast collisionRay;

	CollisionObjectBall(float radius, glm::vec3 position, glm::vec3 velocity);
	void GetBounceVelocity(float deltaTime, glm::vec3 spin, bool putting);
};

#endif
