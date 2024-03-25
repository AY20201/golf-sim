#ifndef PICKUP_CLASS_H
#define PICKUP_CLASS_H

#include"Interactable.h"
#include"Inventory.h"

class Pickup : public Interactable
{
public:
	enum class PickupType
	{
		PICKUP_KEY,
		PICKUP_READABLE,
		PICKUP_TOOL
	};

	bool inPlayerInventory;
	Inventory* playerInventory;
	PickupType pickupType;

	Pickup(Inventory* playerInventory, std::string nameId, PickupType pickupType);
	void Awake() override;
	void Update(float deltaTime, GLFWwindow* window) override;
	void Interact() override;
};

#endif
