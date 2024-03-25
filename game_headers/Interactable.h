#ifndef INTERACTABLE_CLASS_H
#define INTERACTABLE_CLASS_H

#include"../engine_headers/Behavior.h"
#include"../engine_headers/GameObject.h"
#include"../engine_headers/CollisionSolver.h"

class Interactable : public Behavior
{
public:

	bool hovering;
	std::string nameId;

	Interactable(std::string nameId);
	virtual void Interact();
	//void Awake() override;
	//void Update(float deltaTime, GLFWwindow* window) override;
};

#endif
