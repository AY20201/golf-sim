#include"../game_headers/PlayerController.h"

PlayerController::PlayerController(float movementSpeed, float height, Camera* camera, glm::vec3 boxDimensions) 
	: CollisionObject(boxDimensions, glm::vec3(0.0f, boxDimensions.y / 2.0f, 0.0f), glm::vec3(0.0f))
{
	PlayerController::movementSpeed = movementSpeed;
	PlayerController::height = height;
	PlayerController::camera = camera;
}

void PlayerController::Awake()
{
	position = parentObj->transform.position;
	velocity = glm::vec3(0.0f, 0.0f, 0.0f);

	//camera->Position = parentObj->transform.position + glm::vec3(0.0f, height, 0.0f);
	camera->Position = position + glm::vec3(0.0f, height, 0.0f);
	camera->Orientation = glm::vec3(0.0f, 0.0f, 1.0f);
	
	ray.origin = camera->Position;
	ray.direction = camera->Orientation;
}

void PlayerController::Update(float deltaTime, GLFWwindow* window)
{
	if (!escaped)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		bool wPressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		bool sPressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
		bool aPressed = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
		bool dPressed = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

		float verticalAxis = float(wPressed) - float(sPressed);
		float horizontalAxis = float(dPressed) - float(aPressed);

		glm::vec3 flatOrientation = glm::vec3(camera->Orientation.x, 0.0f, camera->Orientation.z);
		glm::vec3 movementVec = verticalAxis * flatOrientation + horizontalAxis * glm::normalize(glm::cross(flatOrientation, camera->Up));
		if (movementVec != glm::vec3(0.0f, 0.0f, 0.0f))
		{
			movementVec = glm::normalize(movementVec);
		}

		position = parentObj->transform.position;

		verticalMovementVector = verticalMovementVector + gravity * deltaTime;
		velocity = movementVec + verticalMovementVector;

		glm::vec3 newPosition = position + velocity * movementSpeed * deltaTime;

		glm::vec3 origExMin = boxCollider.extentsMin;
		glm::vec3 origExMax = boxCollider.extentsMax;

		boxCollider.TransformExtents(glm::translate(newPosition));

		//orthogonal vectors
		glm::vec3 ux = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 uy = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 uz = glm::vec3(0.0f, 0.0f, 1.0f);

		glm::vec3 newVelocityVector = velocity;

		AABB zeroAABB = AABB(glm::vec3(0.0f), glm::vec3(0.0f), glm::mat4(0.0f));

		collisionRecursionDepth = 0;
		newVelocityVector = CollideWithWorld(newVelocityVector, movementSpeed, deltaTime, verticalMovementVector, zeroAABB);

		isGrounded = newVelocityVector.y == 0.0f;

		//"ground check"
		if (isGrounded)
		{
			verticalMovementVector = glm::vec3(0.0f);
		}

		if (verticalMovementVector.y > terminalVelocity.y)
		{
			verticalMovementVector = terminalVelocity;
		}

		velocity = newVelocityVector;

		position = position + newVelocityVector * movementSpeed * deltaTime;

		boxCollider.extentsMin = origExMin;
		boxCollider.extentsMax = origExMax;

		parentObj->transform.position = position;
		camera->Position = parentObj->transform.position + glm::vec3(0.0f, height, 0.0f);

		//AudioPlayer::Instance.SetListenerPosition(camera->Position, camera->Orientation);

		ray.origin = camera->Position;//parentObj->transform.position + glm::vec3(0.0f, height / 2.0f, 0.0f);
		ray.direction = camera->Orientation;//velocity;
		ray.maxDist = 10.0f;
		ray.closestTime = 10000.0f;

		//ray.CheckRayTriangleCollision(glm::vec3(5.0f, 0.0f, -5.0f), glm::vec3(-5.0f, 0.0f, -5.0f), glm::vec3(-5.0f, 0.0f, 5.0f));

		ObjectHandler scene = ObjectHandler::scenes[ObjectHandler::currentSceneIndex];
		for (int i = 0; i < scene.gameObjects.size(); i++)
		{
			for (int j = 0; j < scene.gameObjects[i]->collisionMeshes.size(); j++)
			{
				if (scene.gameObjects[i]->collisionMeshes[j].vertices.size() > 0)
				{
					scene.gameObjects[i]->collisionMeshes[j].CheckRaycast(ray);
				}
			}
		}
		//CollisionSolver::Instance.UpdateWorldCollisionsRay(ray);

		if (ray.hitObject != nullptr)
		{
			Interactable* interactable = dynamic_cast<Interactable*>(ray.hitObject->behavior);
			if (interactable != nullptr)
			{
				interactable->hovering = true;
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && mouseReleased)
				{
					interactable->Interact();
					mouseReleased = false;
				}
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
				{
					mouseReleased = true;
				}
			}
		}

		ray.time = 0.0f;
		ray.foundCollision = false;
		ray.collisionPoint = glm::vec3(0.0f);
		ray.hitObject = nullptr;
		
		camera->Look(window);
	}
	/*
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escaped)
	{
		escaped = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && escaped)
	{
		escaped = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	*/
	
	parentObj->transform.RecalculateMatrix();
	boxCollider.TransformExtents(parentObj->transform.matrix);
}