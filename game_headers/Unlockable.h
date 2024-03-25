#ifndef UNLOCKABLE_CLASS_H
#define UNLOCKABLE_CLASS_H

#include"Openable.h"
#include"Pickup.h"
#include"Inventory.h"
#include<string>

class Unlockable : public Openable
{
public:
	bool locked;
	std::string keyName;
	Inventory* playerInventory;

	Unlockable(Inventory* playerInventory, std::string nameId, GameObject* openState);
	//void Awake() override;
	void Update(float deltaTime, GLFWwindow* window) override;
	void Interact() override;
};

#endif
