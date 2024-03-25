#ifndef PLAYERCONTROLLER_CLASS_H
#define PLAYERCONTROLLER_CLASS_H

#include"../engine_headers/ObjectHandler.h"
#include"../engine_headers/GameObject.h"
#include"../engine_headers/Behavior.h"
#include"../engine_headers/CollisionObject.h"
#include"../engine_headers/Raycast.h"
#include"../engine_headers/Camera.h"
#include"../game_headers/Interactable.h"

class PlayerController : public Behavior, public CollisionObject
{
public:

	float movementSpeed = 1.0f;
	float height = 0.5f;
	Camera* camera;
	bool escaped = false;

	glm::vec3 gravity = glm::vec3(0.0f, -6.0f, 0.0f);
	glm::vec3 verticalMovementVector = glm::vec3(0.0f);
	glm::vec3 terminalVelocity = glm::vec3(0.0f, 4.0f, 0.0f);

	bool isGrounded = false;

	float damping = 1.0f;

	bool mouseReleased;

	Raycast ray = Raycast();
	//glm::vec3 startingPosition;

	PlayerController(float movementSpeed, float height, Camera* camera, glm::vec3 boxDimensions);
	void Awake() override;
	void Update(float deltaTime, GLFWwindow* window) override;
};

#endif

