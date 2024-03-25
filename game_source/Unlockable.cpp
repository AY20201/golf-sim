#include"../game_headers/Unlockable.h"

Unlockable::Unlockable(Inventory* playerInventory, std::string nameId, GameObject* openState) : Openable(playerInventory, nameId, openState)
{
	locked = true;
	Unlockable::playerInventory = playerInventory;
}
/*
void Unlockable::Awake()
{

}
*/
void Unlockable::Update(float deltaTime, GLFWwindow* window)
{
	hovering = false;
}

void Unlockable::Interact()
{
	if (locked)
	{
		for (int i = 0; i < playerInventory->items.size(); i++)
		{
			if (playerInventory->items[i]->pickupType == Pickup::PickupType::PICKUP_KEY)
			{
				std::string keyNameId = playerInventory->items[i]->nameId;
				//gold key unlocks gold door
				if (keyNameId.substr(0, keyNameId.find_first_of(" ")) == nameId.substr(0, nameId.find_first_of(" ")))
				{
					locked = false;
					Open();
					break;
				}
			}
		}
	}
}