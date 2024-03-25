#include <iostream>

#include<Windows.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"engine_headers/Shader.h"
#include"engine_headers/ComputeShader.h"
#include"engine_headers/FrameBufferObject.h"
#include"engine_headers/Mesh.h"
#include"engine_headers/CollisionMesh.h"
#include"engine_headers/Texture.h"
#include"engine_headers/Material.h"
#include"engine_headers/Camera.h"
#include"engine_headers/GameObject.h"
#include"engine_headers/ObjectHandler.h"
#include"engine_headers/LightHandler.h"
#include"engine_source/PointLight.cpp"
#include"engine_source/DirectionalLight.cpp"
#include"engine_headers/Skybox.h"
#include"engine_headers/MeshScene.h"
#include"engine_headers/BloomRenderer.h"

#include"game_headers/PlayerController.h"
#include"game_headers/Swing.h"
#include"game_headers/Ball.h"
#include"game_headers/ShotTracer.h"
#include"game_headers/Surface.h"
#include"game_headers/InstancedRenderer.h"
#include"game_headers/Test.h"


extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

float nearClipPlane = 0.1f;
float farClipPlane = 500.0f;

int main()
{
	glfwInit();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	//specify opengl version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//specify opengl profile, core has most up to date functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	const int width = mode->width;
	const int height = mode->height;

	//create window
	GLFWwindow* window = glfwCreateWindow(width, height, "Project1", monitor, NULL);
	if (window == NULL)
	{
		std::cout << "Window failed to create" << std::endl;
		glfwTerminate();
		return -1;
	}

	//tell glfw to use window in current context
	glfwMakeContextCurrent(window);

	//load opengl through glad
	gladLoadGL();

	glViewport(0, 0, width, height);

	ComputeShader jitterComputeShader("../../engine_resource/Shaders/jitter.comp");
	jitterComputeShader.AttachTexture(2048, 2048);

	Texture* defaultAlbedo = new Texture("../../engine_resource/Textures/default_albedo.png", GL_TEXTURE_2D, GL_LINEAR, /*1,*/ GL_RGB, GL_UNSIGNED_BYTE);
	Texture* defaultNormalMap = new Texture("../../engine_resource/Textures/default_normal.png", GL_TEXTURE_2D, GL_LINEAR, /*1,*/ GL_RGB, GL_UNSIGNED_BYTE);
	Texture* defaultTransparent = new Texture("../../engine_resource/Textures/default_transparent.png", GL_TEXTURE_2D, GL_LINEAR, /*1,*/ GL_RGBA, GL_UNSIGNED_BYTE);

	Texture::defaultAlbedo = defaultAlbedo;
	Texture::defaultNormalMap = defaultNormalMap;

	FrameBufferObject lightingFrameBuffer(width, height, 1, 1);
	FrameBufferObject fogFrameBuffer(width, height, 1, 1);
	FrameBufferObject basePostFrameBuffer(width, height);
	FrameBufferObject finalPassFrameBuffer(width, height, 1, 1);
	FrameBufferObject shadowMapFrameBuffer(2048, 2048, 1, 1);
	basePostFrameBuffer.SetUpGBuffer();
	basePostFrameBuffer.InitializeRenderQuad();

	Shader shaderProgram("../../engine_resource/Shaders/default.vert", "../../engine_resource/Shaders/defaultdeferred.frag");
	Shader treeShaderProgram("../../engine_resource/Shaders/instanced.vert", "../../engine_resource/Shaders/defaultforward.frag");
	Shader waterShaderProgram("../../engine_resource/Shaders/water.vert", "../../engine_resource/Shaders/water.frag");
	Shader shadowShaderProgram("../../engine_resource/Shaders/depth.vert", "../../engine_resource/Shaders/depth.frag");
	Shader instancedShadowShaderProgram("../../engine_resource/Shaders/depthinstanced.vert", "../../engine_resource/Shaders/depthtree.frag");
	Shader skyBoxShaderProgram("../../engine_resource/Shaders/skybox.vert", "../../engine_resource/Shaders/skybox.frag");
	Shader basePostShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/basepostprocesser.frag");
	Shader lightingShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/lighting.frag");
	Shader tonemapperShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/tonemapper.frag");
	Shader bloomDownsampleShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/downsampler.frag");
	Shader bloomUpsampleShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/upsampler.frag");
	Shader overlayShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/overlay.frag");
	Shader shotTracerShaderProgram("../../engine_resource/Shaders/default.vert", "../../engine_resource/Shaders/shottracer.frag");
	Shader fogShaderProgram("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/fog.frag");

	BloomRenderer bloomRenderer(width, height, 7);
	
	Skybox sceneSkyBox(std::vector<const char*>{
		"../../engine_resource/Textures/cloud_skybox/clouds1_east.bmp", //right
		"../../engine_resource/Textures/cloud_skybox/clouds1_west.bmp", //left
		"../../engine_resource/Textures/cloud_skybox/clouds1_up.bmp", //up
		"../../engine_resource/Textures/cloud_skybox/clouds1_down.bmp", //down
		"../../engine_resource/Textures/cloud_skybox/clouds1_north.bmp", //front
		"../../engine_resource/Textures/cloud_skybox/clouds1_south.bmp"}, //back
		skyBoxShaderProgram
	);
	
	DirectionalLight globalDirectionalLight(glm::vec3(0.6f, -0.5f, -0.5f), glm::vec3(1.0f, 0.965f, 0.89f), 2.5f);

	Camera camera(width, height, glm::vec3(0.0f, 1.0f, 3.0f));

	ObjectHandler::currentSceneIndex = 0;
	ObjectHandler::scenes.push_back(ObjectHandler());

	//MeshScene fairway(Transform::Zero, std::vector<Behavior*>{ &Surface::surfaces[Surface::Surfaces::Fairway], &Surface::surfaces[Surface::Surfaces::Rough], &Surface::surfaces[Surface::Surfaces::Green], &Surface::surfaces[Surface::Surfaces::Rough], &Surface::surfaces[Surface::Surfaces::Green] }, std::vector<const char*>{ "../../engine_resource/3D Objects/holes/fairway.obj" }, shaderProgram, nullptr, true, true, true, true);
	MeshScene fairway(Transform::Zero, std::vector<Behavior*>{ &Surface::surfaces[Surface::Surfaces::Rough], &Surface::surfaces[Surface::Surfaces::Fairway], &Surface::surfaces[Surface::Surfaces::Rough], &Surface::surfaces[Surface::Surfaces::Green] }, std::vector<const char*>{ "../../engine_resource/3D Objects/holes/hole1.obj" }, shaderProgram, nullptr, true, true, true, true);
	MeshScene club(Transform::Zero, std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/club/club.obj" }, shaderProgram, nullptr, false, false, false);
	
	//MeshScene ocean(Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f)), std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/ocean/ocean.obj" }, waterShaderProgram, nullptr, false, false, false, false, false);

	MeshScene tallTree(Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f)), std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/trees/tall_tree.obj" }, treeShaderProgram, nullptr, false, false, false, false, false);
	MeshScene billboardTree(Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f)), std::vector<Behavior*>{ nullptr }, std::vector<const char*>{ "../../engine_resource/3D Objects/trees/billboard_tree.obj" }, treeShaderProgram, nullptr, false, false, false, false, false);
	
	for (int i = 0; i < tallTree.sceneGameObjects[0]->meshes.size(); i++)
	{
		tallTree.sceneGameObjects[0]->meshes[i].visible = false;
	}
	billboardTree.sceneGameObjects[0]->meshes[0].visible = false;
	//ocean.sceneGameObjects[0]->meshes[0].visible = false;
	/*
	for (int i = 0; i < fairway.sceneGameObjects.size(); i++)
	{
		for (int j = 0; j < fairway.sceneGameObjects[i]->meshes.size(); j++)
		{
			fairway.sceneGameObjects[i]->meshes[j].visible = false;
		}
	}
	*/
	InstancedRenderer treeInstances(tallTree.sceneGameObjects[0]->meshes, billboardTree.sceneGameObjects[0]->meshes);
	treeInstances.ScatterAcrossTerrain(400, fairway.sceneGameObjects[2]->collisionMeshes[0]);

	//PlayerController* playerController = new PlayerController(2.0f, 0.6f, &camera, glm::vec3(0.35f, 0.7f, 0.35f));
	Ball* ball = new Ball(0.04267f /*mass kg*/, 0.025f /*approx radius*/);
	MeshScene ballMesh(Transform::Zero, std::vector<Behavior*>{ ball }, std::vector<const char*>{ "../../engine_resource/3D Objects/club/ball.obj" }, shaderProgram, nullptr, false, false, false);
	
	Swing* swingController = new Swing(0.5f, 0.25f, &camera, glm::normalize(glm::vec3(0.5f, -0.9f, 0.5f)), club.sceneGameObjects[0], ballMesh.sceneGameObjects[0]);

	GameObject player(glm::vec3(0.0f, 0.75f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), std::vector<Mesh>{ Mesh() }, std::vector<CollisionMesh>{ CollisionMesh() }, swingController);

	ShotTracer* shotTracer = new ShotTracer(ball, 0.01f, 0.075f);
	GameObject shotTracerObj(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), std::vector<Mesh>{ Mesh() }, std::vector<CollisionMesh>{ CollisionMesh() }, shotTracer);

	treeInstances.RecalculateBillboards(player.transform.position, 70.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	double currentTime = glfwGetTime();
	double previousTime = glfwGetTime();
	float deltaTime = 0.0f;

	LightHandler::Instance.SetLightUniforms(lightingShaderProgram);
	LightHandler::Instance.SetLightUniforms(treeShaderProgram);
	LightHandler::Instance.SetLightUniforms(waterShaderProgram);

	ObjectHandler::scenes[ObjectHandler::currentSceneIndex].Awake();

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		currentTime = glfwGetTime();
		deltaTime = (float)currentTime - (float)previousTime;
		previousTime = currentTime;

		//camera.FlyController(window);
		camera.UpdateMatrix(55.0f, nearClipPlane, farClipPlane);
		camera.SetMatrices(shaderProgram, "view", "projection");
		camera.SetMatrices(shotTracerShaderProgram, "view", "projection");

		camera.SetMatrices(treeShaderProgram, "view", "projection");
		camera.SetMatrices(waterShaderProgram, "view", "projection");
		//camera.Look(window);
		//camera.FlyController(window);

		glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f); //hard coded change later
		glm::mat4 lightView = glm::lookAt(-globalDirectionalLight.direction * glm::vec3(30.0) + swingController->parentObj->transform.position, swingController->parentObj->transform.position, glm::vec3(0.0, 1.0, 0.0));
		shadowShaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram.ID, "lightMatrix"), 1, GL_FALSE, glm::value_ptr(lightProj * lightView));
		instancedShadowShaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(instancedShadowShaderProgram.ID, "lightMatrix"), 1, GL_FALSE, glm::value_ptr(lightProj * lightView));
		glUniform1f(glGetUniformLocation(instancedShadowShaderProgram.ID, "elapsedTime"), float(currentTime));
		shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "lightMatrix"), 1, GL_FALSE, glm::value_ptr(lightProj * lightView));
		treeShaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(treeShaderProgram.ID, "lightMatrix"), 1, GL_FALSE, glm::value_ptr(lightProj * lightView));
		glUniform1f(glGetUniformLocation(treeShaderProgram.ID, "elapsedTime"), float(currentTime));

		lightingShaderProgram.Activate();
		glUniform3f(glGetUniformLocation(lightingShaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform1i(glGetUniformLocation(lightingShaderProgram.ID, "skybox"), sceneSkyBox.texUnit);
		treeShaderProgram.Activate();
		glUniform3f(glGetUniformLocation(treeShaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform1i(glGetUniformLocation(treeShaderProgram.ID, "skybox"), sceneSkyBox.texUnit);
		waterShaderProgram.Activate();
		glUniform3f(glGetUniformLocation(waterShaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform1i(glGetUniformLocation(waterShaderProgram.ID, "skybox"), sceneSkyBox.texUnit);
		glUniform1f(glGetUniformLocation(waterShaderProgram.ID, "time"), currentTime);

		sceneSkyBox.Bind();

		jitterComputeShader.Dispatch();

		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].Update(deltaTime, window);

		shadowMapFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].DrawMeshes(shadowShaderProgram);

		glDisable(GL_CULL_FACE);
		//glDepthFunc(GL_ALWAYS);
		//treeInstances.RenderInstances(instancedShadowShaderProgram);
		//glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);

		shadowMapFrameBuffer.UnbindFrameBuffer();
		//glCullFace(GL_BACK);
		//
		glViewport(0, 0, width, height);
		//renders gBuffer
		basePostFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		shadowMapFrameBuffer.SetTexture(shadowMapFrameBuffer.depthTextures[0], lightingShaderProgram, "shadowMap");
		jitterComputeShader.SetTexture(lightingShaderProgram, "jitterMap");

		shadowMapFrameBuffer.SetTexture(shadowMapFrameBuffer.depthTextures[0], treeShaderProgram, "shadowMap");
		jitterComputeShader.SetTexture(treeShaderProgram, "jitterMap");

		glDisable(GL_CULL_FACE);
		ObjectHandler::scenes[ObjectHandler::currentSceneIndex].DrawMeshes();
		glEnable(GL_CULL_FACE);
		//sceneSkyBox.Draw();
		basePostFrameBuffer.UnbindFrameBuffer();

		skyBoxShaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(skyBoxShaderProgram.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera.projection * glm::mat4(glm::mat3(camera.view))));

		lightingFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[0], lightingShaderProgram, "gPosition");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[1], lightingShaderProgram, "gLightPosition");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[2], lightingShaderProgram, "gNormal");
		basePostFrameBuffer.SetTexture(basePostFrameBuffer.colorTextures[3], lightingShaderProgram, "gAlbedo");
		//basePostFrameBuffer.SetTexture(blurFrameBuffer.colorTextures[0], lightingShaderProgram, "ssao");
		
		basePostFrameBuffer.RenderQuad(lightingShaderProgram);

		sceneSkyBox.Draw();
		//ocean.sceneGameObjects[0]->meshes[0].visible = true;
		//ocean.sceneGameObjects[0]->meshes[0].Draw(ocean.sceneGameObjects[0]->transform.matrix);
		//ocean.sceneGameObjects[0]->meshes[0].visible = false;
		//should only do this when the player actually moves
		//treeInstances.RecalculateBillboards(player.transform.position, 70.0f);
		glDisable(GL_CULL_FACE);
		treeInstances.RenderInstances(treeShaderProgram);
		glEnable(GL_CULL_FACE);

		lightingFrameBuffer.UnbindFrameBuffer();

		bloomRenderer.RenderBloomTexture(bloomUpsampleShaderProgram, bloomDownsampleShaderProgram, lightingFrameBuffer.colorTextures[0].textureID, lightingFrameBuffer.colorTextures[0].textureUnit, 0.005f);

		fogFrameBuffer.BindFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		fogShaderProgram.Activate();
		basePostFrameBuffer.SetTexture(lightingFrameBuffer.colorTextures[0], fogShaderProgram, "renderedScene");
		basePostFrameBuffer.SetTexture(lightingFrameBuffer.depthTextures[0], fogShaderProgram, "renderedSceneDepth");

		basePostFrameBuffer.RenderQuad(fogShaderProgram);
		
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shotTracerShaderProgram.Activate();
		shotTracer->mesh.Draw(Transform::Zero.matrix, shotTracerShaderProgram);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		fogFrameBuffer.UnbindFrameBuffer();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		tonemapperShaderProgram.Activate();

		glUniform1i(glGetUniformLocation(tonemapperShaderProgram.ID, "renderedScene"), fogFrameBuffer.colorTextures[0].textureUnit);
		glActiveTexture(GL_TEXTURE0 + fogFrameBuffer.colorTextures[0].textureUnit);
		glBindTexture(GL_TEXTURE_2D, fogFrameBuffer.colorTextures[0].textureID);

		glUniform1i(glGetUniformLocation(tonemapperShaderProgram.ID, "bloomBlur"), bloomRenderer.mipTexUnit);
		glActiveTexture(GL_TEXTURE0 + bloomRenderer.mipTexUnit);
		glBindTexture(GL_TEXTURE_2D, bloomRenderer.BloomTexture());

		basePostFrameBuffer.RenderQuad(tonemapperShaderProgram);

		treeInstances.RecalculateBillboards(player.transform.position, 70.0f);
		//basePostFrameBuffer.SetTexture(shadowMapFrameBuffer.depthTextures[0], overlayShaderProgram, "overlay");
		//basePostFrameBuffer.RenderQuad(overlayShaderProgram);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//monkey.Clear();
	fairway.Clear();
	tallTree.Clear();
	billboardTree.Clear();

	delete Texture::defaultAlbedo;
	delete Texture::defaultNormalMap;
	delete defaultTransparent;

	glfwTerminate();
	return 0;
}