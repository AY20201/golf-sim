#include"../game_headers/Swing.h"

Swing::Swing(float clubLength, float armLength, Camera* camera, glm::vec3 initialCameraOrientation, GameObject* club, GameObject* ball)
{
	Swing::clubLength = clubLength;
	Swing::armLength = armLength;
	Swing::initialCameraOrientation = initialCameraOrientation;
	Swing::camera = camera;
	Swing::club = club;
	Swing::ball = dynamic_cast<Ball*>(ball->behavior);

	clubheadPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	handPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	swingOrientation = glm::vec3(0.0f, 0.0f, 0.0f);
	handOrientation = glm::vec3(0.0f, 0.0f, 0.0f);
	mousePosition = glm::vec2(0.5f, 0.5f);

	firstClick = true;

	prevMouseX = 0.5f;
	currentSpeedMultiplier = 1.0f;
}

void Swing::Awake()
{
	camera->Orientation = initialCameraOrientation;
	camera->Position = parentObj->transform.position + glm::vec3(0.0f, 0.25f, 0.0f);
	restingSurfaceNormal = glm::vec3(0.0f, 1.0f, 0.0f);
	targetCameraOrientation = initialCameraOrientation;

	flatPlayerOrientation = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
	forwardPlayerOrientation = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
	leftPlayerOrientation = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));

	GetSwingOrientation();
	ball->position = parentObj->transform.position + swingOrientation * (armLength + clubLength);
	ball->velocity = projectedLaunchAngle;
	
	activeClub = 0;
}

void Swing::Update(float deltaTime, GLFWwindow* window)
{
	OrientCamera(deltaTime, window);
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		if (firstClick)
		{
			glfwSetCursorPos(window, double(mousePosition.x) * camera->width, double(mousePosition.y) * camera->height);
			firstClick = false;
		}
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		mousePosition = glm::vec2(mouseX / camera->width, mouseY / camera->height);
	}
	else
	{
		mousePosition = glm::lerp(mousePosition, glm::vec2(0.5f, 0.5f), 4.0f * deltaTime);
		firstClick = true;

		SwitchClubs(window);

		if (activeClub == 0 && ball->restingSurface->type > Surface::Surfaces::TeeBox)
		{
			activeClub = 9; //switch to driver off deck
		}
		ball->putting = putting;
	}
	
	//double mouseX, mouseY;
	//glfwGetCursorPos(window, &mouseX, &mouseY);
	//mousePosition = glm::vec2(mouseX / camera->width, mouseY / camera->height);
	
	GetSwingOrientation();

	handPosition = parentObj->transform.position + handOrientation * armLength;
	clubheadPosition = parentObj->transform.position + swingOrientation * (armLength + clubLength);

	float clubheadSpeed = glm::length(clubheadPosition - prevClubheadPosition) / deltaTime;
	if (clubheadSpeed > impactSpeedThreshold && prevMouseX > 0.5f && mousePosition.x < 0.5f && !ball->inMotion)
	{
		Club club = clubs[activeClub];

		float spinRPM = club.backspinRPM;
		glm::vec3 baseBackspin = forwardPlayerOrientation;
		glm::vec3 sidespinAxis = leftPlayerOrientation;

		float clubfaceAngle = -(mousePosition.y - 0.5f) * 90.0f;
		baseBackspin = baseBackspin * club.backspinRPM * ball->restingSurface->swingSpeedFactor + sidespinAxis * glm::sin(clubfaceAngle * 3.1415926f / 180.0f) * club.sidespinRPM;

		baseBackspin = baseBackspin * 2.0f * 3.1415926f / 60.0f;

		float swingSpeedVariance = 2.5f;

		srand(time(NULL));
		float maxSpeed = club.maxSwingSpeed + (float(rand() % 100) / 100.0f * swingSpeedVariance * 2.0f - swingSpeedVariance);
		ball->LaunchBall(glm::normalize(projectedLaunchAngle), baseBackspin, glm::clamp(clubheadSpeed * club.swingSpeedMultiplier, 0.0f, maxSpeed / (0.0015151f * 3600.0f)), club.smashFactor * ball->restingSurface->swingSpeedFactor);
	}
	
	if (!ball->inMotion)
	{
		if (ball->stoppedFrame)
		{
			restingSurfaceNormal = ball->restingSurfaceNormal;
			glm::vec3 targetDirection = glm::normalize(target - glm::vec3(ball->parentObj->transform.position.x, 0.0f, ball->parentObj->transform.position.z));
			//playerOrientation = glm::cross(ball->restingSurfaceNormal, glm::vec3(-1.0f, 0.0f, 0.0f) /*direction to green*/);
			forwardPlayerOrientation = glm::cross(restingSurfaceNormal, -targetDirection/*glm::vec3(-1.0f, 0.0f, 0.0f)*/ /*direction to green*/);
			flatPlayerOrientation = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), -targetDirection/*direction to green*/);
			leftPlayerOrientation = glm::cross(restingSurfaceNormal, flatPlayerOrientation/*glm::vec3(0.0f, 0.0f, 1.0f)*//*direction to green*/);
			targetCameraOrientation = glm::normalize(glm::vec3(flatPlayerOrientation.x, 1.0f, flatPlayerOrientation.z) * initialCameraOrientation);
			//std::cout << flatPlayerOrientation.x << std::endl;
			camera->Orientation = targetCameraOrientation;
			
			//GetSwingOrientation();

			mousePosition = glm::vec2(0.5f, 0.5f);

			ball->stoppedFrame = false;
		}

		bool rightPressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
		bool leftPressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;

		float rotationSpeed = 0.5f; //degrees
		float rotation = float(leftPressed) * rotationSpeed - float(rightPressed) * rotationSpeed;

		forwardPlayerOrientation = glm::normalize(glm::rotate(forwardPlayerOrientation, glm::radians(rotation), restingSurfaceNormal));
		leftPlayerOrientation = glm::normalize(glm::rotate(leftPlayerOrientation, glm::radians(rotation), restingSurfaceNormal));
		flatPlayerOrientation = glm::normalize(glm::rotate(flatPlayerOrientation, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)));

		camera->Orientation = glm::normalize(glm::rotate(camera->Orientation, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)));
		targetCameraOrientation = glm::normalize(glm::rotate(targetCameraOrientation, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)));
		prevCameraOrientation = glm::normalize(glm::rotate(prevCameraOrientation, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f)));

		float clubHeight = 0.702f;
		float clubBallOffsetLength = 0.264f;
		parentObj->transform.position = ball->position + clubBallOffset + glm::vec3(0.0f, 1.0f, 0.0f) * clubHeight - flatPlayerOrientation * clubBallOffsetLength;
		camera->Position = parentObj->transform.position + glm::vec3(0.0f, 0.25f, 0.0f);
	}
	
	club->transform.matrix = glm::inverse(glm::lookAt((clubheadPosition + handPosition) / 2.0f, clubheadPosition, glm::vec3(0.0f, 1.0f, 0.0f)));

	prevClubheadPosition = clubheadPosition;
	prevMouseX = mousePosition.x;
}

void Swing::GetSwingOrientation()
{
	//glm::vec2 normMouse = glm::vec2(mouseX / camera->width, mouseY / camera->height);

	if (mousePosition.x - prevMouseX >= 0.001f && mousePosition.x - 0.5f > 0.0f)
	{
		inBackswing = true;
		currentSpeedMultiplier = putting ? puttingSpeedMultiplier : backswingSpeedMultiplier;
	}
	else if (mousePosition.x - prevMouseX <= -0.01f && mousePosition.x - 0.5f < 0.0f)
	{
		inDownswing = true;
		currentSpeedMultiplier = putting ? puttingSpeedMultiplier : downswingSpeedMultiplier;
	}

	float swingRotation = (mousePosition.x - 0.5f) * currentSpeedMultiplier + 0.5f;
	float handRotation = (mousePosition.x - 0.5f) * currentSpeedMultiplier * handLagMultiplier + 0.5f;

	glm::vec3 clubRotationAxis = flatPlayerOrientation + glm::vec3(0.0f, swingTilt, 0.0f);
	glm::vec3 handRotationAxis = flatPlayerOrientation + glm::vec3(0.0f, swingTilt * handRotationOffset, 0.0f);
	glm::vec3 clubheadStartingPos = -flatPlayerOrientation + glm::vec3(0.0f, swingTilt, 0.0f);
	glm::vec3 handStartingPos = -flatPlayerOrientation + glm::vec3(0.0f, swingTilt * handRotationOffset, 0.0f);

	glm::vec3 perpendicularVector = glm::normalize(glm::vec3(flatPlayerOrientation.z, flatPlayerOrientation.y, -flatPlayerOrientation.x));
	clubRotationAxis += ((mousePosition.y - 0.5f) * 0.5f) * perpendicularVector;
	handRotationAxis += ((mousePosition.y - 0.5f) * 0.5f) * perpendicularVector;

	flatClubRotationAxis = glm::vec3(clubRotationAxis.x, forwardPlayerOrientation.y, clubRotationAxis.z);
	projectedLaunchAngle = -glm::normalize(glm::cross(flatClubRotationAxis, restingSurfaceNormal));
	float loft = clubs[activeClub].launchAngle / 90.0f;
	projectedLaunchAngle = (1.0f - loft) * projectedLaunchAngle + loft * restingSurfaceNormal;

	glm::mat4 clubR = glm::rotate(glm::mat4(1.0f), -glm::radians(swingRotation * 360.0f), glm::normalize(clubRotationAxis));
	glm::mat4 handR = glm::rotate(glm::mat4(1.0f), -glm::radians(handRotation * 360.0f), glm::normalize(handRotationAxis));

	swingOrientation = glm::normalize(glm::mat3(clubR) * glm::normalize(clubheadStartingPos));
	handOrientation = glm::normalize(glm::mat3(handR) * glm::normalize(handStartingPos));
	
}

void Swing::OrientCamera(float deltaTime, GLFWwindow* window)
{
	glm::vec3 flatCameraOrientation = glm::normalize(glm::vec3(camera->Orientation.x, 0.0f, camera->Orientation.z));

	if (abs(glm::angle(flatCameraOrientation, -flatPlayerOrientation)) < glm::radians(65.0f))
	{
		glm::vec3 orientationChange = camera->Orientation - prevCameraOrientation;
		camera->Orientation = prevCameraOrientation + glm::vec3(0.0f, orientationChange.y, 0.0f);
	}

	prevCameraOrientation = camera->Orientation;
	camera->Look(window);

	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		float lerpSpeed = 0.0f;
		if (ball->inMotion && !putting)
		{
			lerpSpeed = 8.5f * deltaTime;
			camera->Orientation = (1.0f - lerpSpeed) * camera->Orientation + lerpSpeed * leftPlayerOrientation;
		}
		else
		{
			lerpSpeed = 2.0f * deltaTime;
			camera->Orientation = (1.0f - lerpSpeed) * camera->Orientation + lerpSpeed * targetCameraOrientation;
		}
	}
}

void Swing::SwitchClubs(GLFWwindow* window)
{
	int firstKey = GLFW_KEY_1;
	int lastKey = GLFW_KEY_9;
	for (int i = firstKey; i <= lastKey; i++)
	{
		if (glfwGetKey(window, i) == GLFW_PRESS)
		{
			if (i - firstKey != activeClub)
			{
				activeClub = i - firstKey;
				std::cout << clubs[activeClub].clubName << std::endl;
				putting = activeClub == lastKey - firstKey;
				return;
			}
		}
	}
}

float Swing::UnitsToYards(float measurement)
{
	//0.5 units = 1.26388 yards
	//0.75 units = 2 yards
	return measurement * 2.6667f;
}

float Swing::UnitsToMiles(float measurement)
{
	//1 unit = 2.5277 = 0.0011363 miles
	return measurement * 0.0015151f;
}