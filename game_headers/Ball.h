#ifndef BALL_CLASS_H
#define BALL_CLASS_H

#include"../engine_headers/Transform.h"
#include"../engine_headers/Behavior.h"
#include"../engine_headers/GameObject.h"
#include"../engine_headers/CollisionObjectBall.h"

class Ball : public Behavior, public CollisionObjectBall
{
public:
	bool inMotion;
	bool stoppedFrame;
	bool putting;
	float mass;

	glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 angularVelocity = glm::vec3(0.0f);

	float ballStoppedPausePeriod = 2.0f;

	Ball(float mass, float radius);

	void Awake() override;
	void Update(float deltaTime, GLFWwindow* window) override;
	void LaunchBall(glm::vec3 launchDirection, glm::vec3 spin, float clubheadSpeed, float smashFactor);
	//https://www.golf-simulators.com/physics.htm
private:
	float ballStoppedPausedTimer = 0.0f;

};

#endif
