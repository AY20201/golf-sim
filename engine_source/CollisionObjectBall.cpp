#include"../engine_headers/CollisionObjectBall.h"
#include"../engine_headers/ObjectHandler.h"

CollisionObjectBall::CollisionObjectBall(float radius, glm::vec3 position, glm::vec3 velocity)
{
	CollisionObjectBall::position = position;
	CollisionObjectBall::velocity = velocity;

	CollisionObjectBall::radius = radius;
	restingSurfaceNormal = glm::vec3(0.0f, 1.0f, 0.0f);
	restingSurface = &Surface::surfaces[Surface::Surfaces::TeeBox];
}

void CollisionObjectBall::GetBounceVelocity(float deltaTime, glm::vec3 spin, bool putting)
{
	collisionRay.origin = position;
	collisionRay.direction = glm::normalize(velocity);
	collisionRay.maxDist = glm::length(velocity * deltaTime);
	collisionRay.closestTime = 10000.0f;
	
	std::vector<GameObject*> sceneGameObjects = ObjectHandler::scenes[ObjectHandler::currentSceneIndex].gameObjects;
	for (int i = 0; i < sceneGameObjects.size(); i++)
	{
		for (int j = 0; j < sceneGameObjects[i]->collisionMeshes.size(); j++)
		{
			if (sceneGameObjects[i]->collisionMeshes[j].vertices.size() > 0)
			{
				sceneGameObjects[i]->collisionMeshes[j].CheckRaycast(collisionRay);
			}
		}
	}
	
	if (collisionRay.hitObject != nullptr)
	{
		Surface* surface = dynamic_cast<Surface*>(collisionRay.hitObject->behavior);
		if (surface != nullptr)
		{
			if (glm::length(velocity) < surface->stoppingSpeed / (0.0015151f * 3600.0f))
			{
				velocity = glm::vec3(0.0f);
				acceleration = glm::vec3(0.0f);

				collisionRay.time = 0.0f;
				collisionRay.foundCollision = false;
				collisionRay.collisionPoint = glm::vec3(0.0f);
				collisionRay.hitObject = nullptr;

				return;
			}

			restingSurfaceNormal = collisionRay.collisionNormal; 
			restingSurface = surface;

			float percentOfTerminalVelocity = glm::clamp(glm::length(velocity) / (71.582f / (0.0015151f * 3600.0f)), 0.0f, 1.0f);
			percentOfTerminalVelocity = glm::pow(1.0f - percentOfTerminalVelocity, 2.0f);

			float bounceAngle = surface->bounceAngle - surface->bounceAngle * percentOfTerminalVelocity;
			glm::vec3 symBounce = velocity - 2.0f * glm::dot(collisionRay.collisionNormal, velocity) * collisionRay.collisionNormal;
			glm::vec3 bounce = (1.0f - bounceAngle) * symBounce + bounceAngle * collisionRay.collisionNormal * glm::length(symBounce);
			glm::vec3 parallelBounce = velocity - glm::dot(collisionRay.collisionNormal, velocity) * collisionRay.collisionNormal;

			float friction = surface->firstBounceEnergyAbsorbtion + (0.75f - surface->firstBounceEnergyAbsorbtion) * percentOfTerminalVelocity;
			
			if (putting)
			{
				velocity = parallelBounce * surface->rollFriction - (glm::vec3(0.0f, -9.8f, 0.0f) * deltaTime);
			}
			else
			{
				velocity = bounce * friction; //friction per bounce
			}			
		}
	}

	collisionRay.time = 0.0f;
	collisionRay.foundCollision = false;
	collisionRay.collisionPoint = glm::vec3(0.0f);
	collisionRay.hitObject = nullptr;
}