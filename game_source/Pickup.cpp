#include"../game_headers/Pickup.h"

Pickup::Pickup(Inventory* playerInventory, std::string nameId, PickupType pickupType) : Interactable(nameId)
{
	inPlayerInventory = false;
	Pickup::playerInventory = playerInventory;
	Pickup::pickupType = pickupType;
}

void Pickup::Interact()
{
	if (!inPlayerInventory)
	{
		playerInventory->Add(this);
		inPlayerInventory = true;

		parentObj->Enable(false);
	}
}

void Pickup::Awake() 
{

}

void Pickup::Update(float deltaTime, GLFWwindow* window)
{
	/*
	for (int i = 0; i < parentObj->meshes.size(); i++)
	{
		glm::vec3 originalColor = parentObj->meshes[i].material->albedo.color;
		if (!inPlayerInventory && hovering)
		{
			glUniform3f(glGetUniformLocation(parentObj->meshes[i].material->shader.ID, "albedoColor"), originalColor.x * 1.3f, originalColor.y * 1.3f, originalColor.z * 1.3f);
		}
		else
		{
			glUniform3f(glGetUniformLocation(parentObj->meshes[i].material->shader.ID, "albedoColor"), originalColor.x, originalColor.y, originalColor.z);
		}
	}
	*/

	hovering = false;
}