#ifndef SHOTTRACER_CLASS_H
#define SHOTTRACER_CLASS_H

#include"../engine_headers/Behavior.h"
#include"../engine_headers/GameObject.h"

#include"Ball.h"

class ShotTracer : public Behavior
{
public:
	Ball* ball;
	float timestep;
	float duration;

	Mesh mesh;
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;

	float width;

	ShotTracer(Ball* ball, float timestep, float width);
	void Awake() override;
	void Update(float deltaTime, GLFWwindow* window) override;
	void AppendToMesh(glm::vec3 direction);

private:
	float currentTimestep;
};

#endif
