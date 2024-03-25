#ifndef OPENABLE_CLASS_H
#define OPENABLE_CLASS_H

#include"Interactable.h"
#include"Pickup.h"
#include"Inventory.h"
#include<string>

class Openable : public Interactable
{
public:
	bool open;
	Inventory* playerInventory;
	GameObject* openState;
	//Mesh originalStateMesh;
	//CollisionMesh originalStateCollisionMesh;

	Openable(Inventory* playerInventory, std::string nameId, GameObject* openState);
	void Awake() override;
	void Update(float deltaTime, GLFWwindow* window) override;
	void Interact() override;
	void Open();
};

#endif