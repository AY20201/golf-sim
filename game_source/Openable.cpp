#include"../game_headers/Openable.h"

Openable::Openable(Inventory* playerInventory, std::string nameId, GameObject* openState) : Interactable(nameId)
{
	Openable::playerInventory = playerInventory;
	Openable::openState = openState;
	open = false;
}

void Openable::Awake()
{
	openState->Enable(false);
}

void Openable::Update(float deltaTime, GLFWwindow* window)
{
	hovering = false;
}

void Openable::Interact()
{
	Open();
}

void Openable::Open()
{
	parentObj->Enable(false);
	openState->Enable(true);

	open = true;
}