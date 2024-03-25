#include"../game_headers/Ball.h"

Ball::Ball(float mass, float radius) : CollisionObjectBall(radius, glm::vec3(0.0f), glm::vec3(0.0f))
{
	inMotion = false;
	stoppedFrame = true;
	Ball::mass = mass;
}

void Ball::LaunchBall(glm::vec3 launchDirection, glm::vec3 spin, float clubheadSpeed, float smashFactor)
{
	velocity = glm::normalize(launchDirection) * clubheadSpeed * smashFactor;
	angularVelocity = spin;
	inMotion = true;
}

void Ball::Awake()
{
	acceleration = glm::vec3(0.0f);
}

void Ball::Update(float deltaTime, GLFWwindow* window)
{
	if (inMotion)
	{
		glm::vec3 signedVelocity = glm::sign(velocity);
		glm::vec3 dragAcceleration = -0.5f * 1.22f * (radius * radius * 3.14159f) * 3.0f * velocity * velocity * signedVelocity;
		glm::vec3 magnusAcceleration = glm::cross(angularVelocity, velocity) * 0.000025f / mass;

		acceleration = gravity + dragAcceleration + magnusAcceleration;
		velocity = velocity + acceleration * deltaTime;

		float terminalVelocity = 71.582f / (0.0015151f * 3600.0f);
		if (velocity.y < 0.0f && glm::abs(velocity.y) >= terminalVelocity)
		{
			velocity = velocity * (terminalVelocity / glm::abs(velocity.y));
		}

		GetBounceVelocity(deltaTime, angularVelocity, putting);

		if (velocity == glm::vec3(0.0f) )
		{
			if (ballStoppedPausedTimer > ballStoppedPausePeriod) //pause after ball stops
			{
				inMotion = false;
				stoppedFrame = true;
				position = position + glm::vec3(0.0f, 0.0125f, 0.0f);
				ballStoppedPausedTimer = 0.0f;
			}
			ballStoppedPausedTimer += deltaTime;
		}

		angularVelocity = angularVelocity * glm::pow(2.7182818f, -deltaTime / 30.0f);

		position = position + velocity * deltaTime;
	}
	
	parentObj->transform.position = position;
	parentObj->transform.RecalculateMatrix();
}