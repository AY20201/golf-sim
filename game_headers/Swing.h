#ifndef SWING_CLASS_H
#define SWING_CLASS_H

#include"../engine_headers/Transform.h"
#include"../engine_headers/GameObject.h"
#include"../engine_headers/Behavior.h"
#include"../engine_headers/Camera.h"
#include"../game_headers/Ball.h"

struct Club
{
	float launchAngle;
	float maxSwingSpeed;
	float smashFactor;
	float swingSpeedMultiplier; //changes 0.33 to something else
	float backspinRPM;
	float sidespinRPM;
	std::string clubName;
};

class Swing : public Behavior
{
public:
	float clubLength;
	float armLength;

	glm::vec3 clubheadPosition;
	glm::vec3 handPosition;

	glm::vec3 initialCameraOrientation;
	glm::vec3 targetCameraOrientation;

	glm::vec3 swingOrientation;
	glm::vec3 handOrientation;

	glm::vec3 projectedLaunchAngle;
	glm::vec3 restingSurfaceNormal;
	glm::vec3 flatClubRotationAxis;

	glm::vec3 flatPlayerOrientation;
	glm::vec3 forwardPlayerOrientation; //direction facing
	glm::vec3 leftPlayerOrientation; //direction of shot

	glm::vec3 target = glm::vec3(162.6f, 0.0f, -4.84f);

	glm::vec2 mousePosition;

	glm::vec3 clubBallOffset = glm::vec3(0.0f, 0.0f, 0.0f);

	Camera* camera;
	GameObject* club;
	Ball* ball;

	Club clubs[10]
	{
		{ 20.0f, 125.0f, 1.45f, 0.33f, 3000.0f, 5000.0f, "Driver"}, //driver
		{ 20.5f, 98.0f, 1.41f, 0.33f, 4000.0f, 5000.0f, "4 iron"}, //4 iron
		{ 21.0f, 94.0f, 1.39f, 0.33f, 4500.0f, 5000.0f, "5 iron"}, //5 iron
		{ 22.0f, 92.0f, 1.36f, 0.33f, 5000.0f, 5000.0f, "6 iron" }, //6 iron
		{ 24.0f, 88.0f, 1.34f, 0.33f, 6000.0f, 5000.0f, "7 iron" }, //7 iron
		{ 26.0f, 87.0f, 1.32f, 0.33f, 7000.0f, 5000.0f, "8 iron" }, //8 iron
		{ 28.0f, 86.0f, 1.31f, 0.33f, 8000.0f, 5000.0f, "9 iron" }, //9 iron
		{ 30.0f, 85.5f, 1.30f, 0.33f, 8200.0f, 5000.0f, "Pitching Wedge"}, //PW
		{ 0.05f, 15.0f, 1.8f, 0.25f, 0.0f, 0.0f, "Putter" },
		{ 15.0f, 110.0f, 1.65f, 0.33f, 500.0f, 5000.0f, "Driver Off Deck"}
	};
	int activeClub;

	bool inBackswing = false;
	bool inDownswing = false;

	float backswingSpeedMultiplier = 0.8f;
	float downswingSpeedMultiplier = 0.9f;
	float puttingSpeedMultiplier = 0.3f;

	float impactSpeedThreshold = 5.0f;

	float swingTilt = 0.75f;
	float handRotationOffset = 0.95f;
	float handLagMultiplier = 0.75f;

	bool putting;

	Swing(float clubLength, float armLength, Camera* camera, glm::vec3 initialCameraOrientation, GameObject* club, GameObject* ball);

	void Awake() override;
	void Update(float deltaTime, GLFWwindow* window) override;
	void GetSwingOrientation();
	void OrientCamera(float deltaTime, GLFWwindow* window);
	void SwitchClubs(GLFWwindow* window);

private:
	float prevMouseX;
	glm::vec3 prevClubheadPosition;
	glm::vec3 prevCameraOrientation;
	float currentSpeedMultiplier;
	bool firstClick;
	float UnitsToYards(float measurement);
	float UnitsToMiles(float measurement);
};

#endif