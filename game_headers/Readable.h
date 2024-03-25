#ifndef READABLE_CLASS_H
#define READABLE_CLASS_H

#include"../engine_headers/Texture.h"
#include"Interactable.h"
#include"PlayerController.h"

class Readable : public Interactable
{
public:
	Texture* texture;
	Texture* blankTexture;
	Shader overlayShader;
	bool open;

	static Texture* activeOverlay;

	PlayerController* playerController;

	Readable(const char* texturePath, Texture* blankTexture, Shader& overlayShader, PlayerController* playerController, std::string nameId);
	void Interact() override;
	void Awake() override;
	void Update(float deltaTime, GLFWwindow* window);
};

#endif READABLE_CLASS_H
