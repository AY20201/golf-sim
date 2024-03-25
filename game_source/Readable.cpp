#include"../game_headers/Readable.h"

Texture* Readable::activeOverlay;

Readable::Readable(const char* texturePath, Texture* blankTexture, Shader& overlayShader, PlayerController* playerController, std::string nameId) : Interactable(nameId)
{
	texture = new Texture(texturePath, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_UNSIGNED_BYTE);
	Readable::blankTexture = blankTexture;
	Readable::overlayShader = overlayShader;
	Readable::playerController = playerController;
	open = false;
}

void Readable::Interact()
{
	open = true;
}

void Readable::Awake()
{

}

void Readable::Update(float deltaTime, GLFWwindow* window)
{
	overlayShader.Activate();

	activeOverlay = blankTexture;
	//assumes that this will be called after playerController.Update();
	if (open)
	{
		playerController->escaped = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		activeOverlay = texture;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && open)
	{
		playerController->escaped = false;
		open = false;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		glfwSetCursorPos(window, (playerController->camera->width / 2), (playerController->camera->height / 2));
	}
}