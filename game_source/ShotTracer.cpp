#include"../game_headers/ShotTracer.h"

ShotTracer::ShotTracer(Ball* ball, float timestep, float width)
{
	ShotTracer::ball = ball;
	ShotTracer::timestep = timestep;
	ShotTracer::width = width;

	currentTimestep = 0.0f;
	duration = 0.0f;
}

void ShotTracer::Awake()
{
	AppendToMesh(ball->velocity);
}

void ShotTracer::Update(float deltaTime, GLFWwindow* window)
{
	currentTimestep += deltaTime;
	if (currentTimestep > timestep)
	{
		currentTimestep = 0.0f;
		if (ball->inMotion && !ball->putting)
		{
			AppendToMesh(ball->velocity);
			if (vertices.size() > 2)
			{
				mesh = Mesh(vertices, indices, nullptr, true, false, false);
			}
		}
		else
		{
			vertices = std::vector<Vertex>{};
			indices = std::vector<GLuint>{};
			mesh = Mesh();
		}
	}
}

void ShotTracer::AppendToMesh(glm::vec3 direction)
{
	glm::vec3 flatVelocity = glm::vec3(direction.x, 0.0f, direction.z);
	glm::vec3 leftVector = -glm::normalize(glm::cross(flatVelocity, glm::vec3(0.0f, 1.0f, 0.0f)));

	glm::vec3 leftVert = ball->position + leftVector * width / 2.0f;
	glm::vec3 rightVert = ball->position - leftVector * width / 2.0f;

	vertices.push_back(Vertex{ leftVert, glm::vec2(0.0f), glm::vec3(1.0f), glm::vec3(1.0f) });
	vertices.push_back(Vertex{ rightVert, glm::vec2(0.0f), glm::vec3(1.0f), glm::vec3(1.0f) });

	if (vertices.size() > 2)
	{
		int vertCount = int(vertices.size());
		indices.push_back(vertCount - 4);
		indices.push_back(vertCount - 3);
		indices.push_back(vertCount - 2);

		indices.push_back(vertCount - 2);
		indices.push_back(vertCount - 3);
		indices.push_back(vertCount - 1);
	}
}