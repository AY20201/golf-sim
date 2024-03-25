#ifndef INSTANCEDRENDERER_CLASS_H
#define INSTANCEDRENDERER_CLASS_H

#include"../engine_headers/Mesh.h"
#include"../engine_headers/AABB.h"
#include"../engine_headers/Raycast.h"

class InstancedRenderer
{
public:
	int count;

	std::vector<Mesh> instancedMesh;
	std::vector<Mesh> billboardMesh;

	std::vector<glm::mat4> modelTransformMatrices;
	std::vector<glm::mat4> billboardMatrices;
	std::vector<glm::mat4> regularMatrices;

	GLuint billboardBufferID;
	GLuint regularBufferID;

	glm::vec3 lastPlayerPosition;

	InstancedRenderer(std::vector<Mesh>& instancedMesh, std::vector<Mesh>& billboardMesh);
	void ScatterAcrossTerrain(int count, CollisionMesh& terrainColMesh);
	void InitializeBuffer(std::vector<glm::mat4> matrixData, std::vector<Mesh>& meshData, GLuint bufferID);
	void RenderInstances(Shader& instanceShader);
	void RecalculateBillboards(glm::vec3 playerPosition, float billboardRadius);
};

#endif
