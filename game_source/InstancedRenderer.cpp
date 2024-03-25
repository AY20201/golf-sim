#include"../game_headers/InstancedRenderer.h"

InstancedRenderer::InstancedRenderer(std::vector<Mesh>& instancedMesh, std::vector<Mesh>& billboardMesh)
{
	InstancedRenderer::instancedMesh = instancedMesh;
	InstancedRenderer::billboardMesh = billboardMesh;
}

void InstancedRenderer::InitializeBuffer(std::vector<glm::mat4> matrixData, std::vector<Mesh>& meshData, GLuint bufferID)
{
	if (matrixData.size() == 0) return;

	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, matrixData.size() * sizeof(glm::mat4), &matrixData[0], GL_STATIC_DRAW);

	for (auto& mesh : meshData)
	{
		GLuint vaoID = mesh.vao.ID;
		glBindVertexArray(vaoID);

		std::size_t vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
	}
}

void InstancedRenderer::RenderInstances(Shader& instanceShader)
{
	if (regularMatrices.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, regularBufferID);
		for (auto& mesh : instancedMesh)
		{
			mesh.vao.Bind();

			mesh.material->SetTextures();
			glUniform1i(glGetUniformLocation(mesh.material->shader.ID, "triplanar"), mesh.triplanar);

			if (instanceShader.ID != mesh.material->shader.ID) {
				instanceShader.Activate();
				glUniform1i(glGetUniformLocation(instanceShader.ID, "albedo"), mesh.material->albedo.tex->texUnit);
			}
			glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, regularMatrices.size());
		}
	}

	if (billboardMatrices.size() > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, billboardBufferID);
		for (auto& mesh : billboardMesh)
		{
			mesh.vao.Bind();

			mesh.material->SetTextures();
			glUniform1i(glGetUniformLocation(mesh.material->shader.ID, "triplanar"), mesh.triplanar);

			if (instanceShader.ID != mesh.material->shader.ID) {
				instanceShader.Activate();
				glUniform1i(glGetUniformLocation(instanceShader.ID, "albedo"), mesh.material->albedo.tex->texUnit);
			}
			glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, billboardMatrices.size());
		}
	}
}
/*
void InstancedRenderer::ScatterAcrossTerrain(int count, Mesh& terrainMesh)
{
	InstancedRenderer::count = count;
	//modelTransformMatrices = std::vector<glm::mat4>(count);
	int maxIterations = 10000;
	int iterations = 0;

	std::vector<glm::vec3> testPoints;
	float minDist = 3.0f * 3.0f;

	while (modelTransformMatrices.size() < count && iterations < maxIterations)
	{
		glm::vec3 testPoint = terrainMesh.vertices[rand() % terrainMesh.vertices.size()].position;
		float terrainHeight = testPoint.y;
		testPoint.y = 0.0f;

		bool failedTest = false;
		for (auto& point : testPoints)
		{
			float difX = testPoint.x - point.x;
			float difZ = testPoint.z - point.z;
			if (difX * difX + difZ * difZ < minDist)
			{
				failedTest = true;
				break;
			}
		}

		if (!failedTest) 
		{
			testPoints.push_back(testPoint);
			testPoint.y = terrainHeight;

			glm::mat4 newMat = glm::mat4(1.0f);

			newMat = glm::translate(newMat, testPoint);
			newMat = glm::scale(newMat, glm::vec3(float(70 + (rand() % 120)) / 100.0f));
			newMat = glm::rotate(newMat, glm::radians(float(rand() % 360)), glm::vec3(0.0f, 1.0f, 0.0f));

			modelTransformMatrices.push_back(newMat);
		}

		iterations++;
	}
}
*/

void InstancedRenderer::ScatterAcrossTerrain(int count, CollisionMesh& terrainColMesh)
{
	InstancedRenderer::count = count;
	//modelTransformMatrices = std::vector<glm::mat4>(count);
	int maxIterations = 10000;
	int iterations = 0;

	std::vector<glm::vec3> testPoints;
	float minDist = 3.0f * 3.0f;

	while (testPoints.size() < count && iterations < maxIterations)
	{
		float width = terrainColMesh.boxCollider.extentsMax.x - terrainColMesh.boxCollider.extentsMin.x;
		float length = terrainColMesh.boxCollider.extentsMax.z - terrainColMesh.boxCollider.extentsMin.z;
		glm::vec3 testPoint = glm::vec3(terrainColMesh.boxCollider.extentsMin.x + width * (float(rand() % 100) / 100.0f), 50.0f, terrainColMesh.boxCollider.extentsMin.z + length * (float(rand() % 100) / 100.0f));

		bool failedTest = false;
		for (auto& point : testPoints)
		{
			float difX = testPoint.x - point.x;
			float difZ = testPoint.z - point.z;
			if (difX * difX + difZ * difZ < minDist)
			{
				failedTest = true;
				break;
			}
		}

		if (!failedTest)
		{
			testPoints.push_back(testPoint);
		}

		iterations++;
	}

	Raycast testRay;

	//std::cout << terrainMesh.vertices.size() << std::endl;
	//std::cout << terrainColMesh.vertices.size() << std::endl;

	for (auto& point : testPoints) 
	{
		testRay.origin = point;
		testRay.direction = glm::vec3(0.0f, -1.0f, 0.0f);
		testRay.maxDist = 100.0f;
		testRay.closestTime = 10000.0f;

		terrainColMesh.CheckRaycast(testRay);

		if (testRay.hitObject != nullptr)
		{
			glm::mat4 newMat = glm::mat4(1.0f);

			newMat = glm::translate(newMat, testRay.collisionPoint);
			newMat = glm::scale(newMat, glm::vec3(float(70 + (rand() % 120)) / 100.0f));
			newMat = glm::rotate(newMat, glm::radians(float(rand() % 360)), glm::vec3(0.0f, 1.0f, 0.0f));

			modelTransformMatrices.push_back(newMat);
		}

		testRay.time = 0.0f;
		testRay.foundCollision = false;
		testRay.collisionPoint = glm::vec3(0.0f);
		testRay.hitObject = nullptr;
	}
}

void InstancedRenderer::RecalculateBillboards(glm::vec3 playerPosition, float billboardRadius)
{
	if (playerPosition != lastPlayerPosition)
	{
		billboardMatrices.clear();
		regularMatrices.clear();
		for (int i = 0; i < modelTransformMatrices.size(); i++)
		{
			glm::vec3 pos = glm::vec3(modelTransformMatrices[i] * glm::vec4(1.0f));

			if (glm::length(playerPosition - pos) > billboardRadius)
			{
				billboardMatrices.push_back(modelTransformMatrices[i]);
			}
			else
			{
				regularMatrices.push_back(modelTransformMatrices[i]);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		InitializeBuffer(regularMatrices, instancedMesh, regularBufferID);
		InitializeBuffer(billboardMatrices, billboardMesh, billboardBufferID);
		lastPlayerPosition = playerPosition;
	}
}