//Std
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstdlib>
//OpenGL
#include <glew.h>
#include <gl/GL.h>

//Engine
#include <window.h>
#include <shader.h>

//Math
#include <vec2.hpp>
#include <geometric.hpp> 


// Defines
#define PI 3.14159f
#define GRAVITY glm::vec2(0.0f, -9.8f)
#define RESTITUTION 0.8f
#define TOTAL_PARTICLES 5000

//Structs
struct Particle
{
	glm::vec2 position;
	glm::vec2 velocity;
	float radius;
	float restitution;
};

std::vector<float> createCircleVertices(float centerX, float centerY, float radius, int numSegments) 
{
	std::vector<float> vertices;

	float fullCircle = 2.0f * PI;

	for (int i = 0; i <= numSegments; i++) {
		float angle = (fullCircle / numSegments) * i;

		float x = centerX + radius * cos(angle);
		float y = centerY + radius * sin(angle);

		vertices.push_back(x);
		vertices.push_back(y);
	}
	return vertices;
}

void physicsUpdate(float dt, Particle& particle)
{
	particle.velocity += dt * GRAVITY;
	particle.position += dt * particle.velocity;
}

void particleCollision(float dt, Particle& particleA, Particle& particleB, int p1, int p2,  std::vector<glm::vec2>& displacement)
{
	glm::vec2 particleDisplacement = particleB.position - particleA.position;
	
	float distance = glm::length(particleDisplacement);

	float sumOfRadii = particleA.radius + particleB.radius;

	if (distance <= sumOfRadii)
	{
		float overlap = sumOfRadii - distance;
		glm::vec2 collisionNormal = glm::normalize(particleDisplacement);

		displacement[p1] += collisionNormal * (overlap / 2.0f);
		displacement[p2] -= collisionNormal * (overlap / 2.0f);
		
		float speedA = glm::dot(particleA.velocity, collisionNormal);
		float speedB = glm::dot(particleB.velocity, collisionNormal);

		glm::vec2 deltaVelocityA = collisionNormal * (speedB - speedA);
		glm::vec2 deltaVelocityB = collisionNormal * (speedA - speedB);

		particleA.velocity += deltaVelocityA * particleA.restitution;
		particleB.velocity += deltaVelocityB * particleB.restitution;
	}

}

void bounce(Particle& particle)
{
	// Bottom boundary
	if (particle.position.y - particle.radius <= -1.0f)
	{
		particle.position.y = -1.0f + particle.radius; // Clamp position
		particle.velocity.y = particle.restitution * -particle.velocity.y;
	}

	// Top boundary
	if (particle.position.y + particle.radius >= 1.0f)
	{
		particle.position.y = 1.0f - particle.radius; // Clamp position
		particle.velocity.y = particle.restitution * -particle.velocity.y;
	}

	// Left boundary
	if (particle.position.x - particle.radius <= -1.0f)
	{
		particle.position.x = -1.0f + particle.radius; // Clamp position
		particle.velocity.x = particle.restitution * -particle.velocity.x;
	}

	// Right boundary
	if (particle.position.x + particle.radius >= 1.0f)
	{
		particle.position.x = 1.0f - particle.radius; // Clamp position
		particle.velocity.x = particle.restitution * -particle.velocity.x;
	}
}


int main(int argc, char** argv)
{
	
	srand(time(NULL));

	Window window;

	if (!window.init("Physics Engine", 900, 700))
		return -1;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW didn't initalize!" << std::endl;
		return -1;
	}

	std::string vertex = "particle.vert";
	std::string fragment = "particle.frag";

	Shader::getShaders(vertex, fragment);

	GLuint vertexID, fragmentID;

	std::vector<float> vertices = createCircleVertices(0.0f, 0.0f, 0.01f, 36);
	
	vertexID = Shader::CompileShader(vertex, ShaderType::Vertex);
	fragmentID = Shader::CompileShader(fragment, ShaderType::Fragment);

	GLuint program = glCreateProgram();
	
	glAttachShader(program, vertexID);
	glAttachShader(program, fragmentID);

	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR, Shader program Linking FAILED\n" << infoLog << std::endl;
	}

	GLuint vao, vbo, vboPositions;
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vboPositions);
	
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * TOTAL_PARTICLES, NULL, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(1); // Enable attribute 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glVertexAttribDivisor(1, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Particle particles[TOTAL_PARTICLES];

	std::vector<glm::vec2> positions;
	positions.reserve(TOTAL_PARTICLES); // Important for performance!
	
	for (int i = 0; i < TOTAL_PARTICLES; i++)
	{
		particles[i].position = glm::vec2(((float)rand() / RAND_MAX) * 2.0 - (1.0), (float)rand() / RAND_MAX);
		particles[i].velocity = glm::vec2(0.0f, 0.0f);
		particles[i].radius = 0.01f;
		particles[i].restitution = 0.8f;
		positions.push_back(particles[i].position);
	}

	// In main.cpp, before the while loop
	float lastFrameTime = 0.0f;

	while (!window.shouldClose())
	{
	
		// Calculate delta time
		float currentFrameTime = (float)glfwGetTime();
		float dt = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		int sub_steps = 8; // Or 8. More steps = more stable, but slower
		float sub_dt = dt / sub_steps;

		for (int i = 0; i < sub_steps; i++)
		{
			std::vector<glm::vec2> displacements(TOTAL_PARTICLES, glm::vec2(0.0f, 0.0f));

			// Physics update with the smaller sub-step delta time
			for (int p = 0; p < TOTAL_PARTICLES; p++)
			{
				physicsUpdate(sub_dt, particles[p]);
				bounce(particles[p]);
			}

			// Collision detection and resolution
			for (int p1 = 0; p1 < TOTAL_PARTICLES; p1++)
			{
				for (int p2 = p1 + 1; p2 < TOTAL_PARTICLES; p2++)
				{
					particleCollision(dt, particles[p1], particles[p2], p1, p2, displacements);
				}

			}

			for (int p = 0; p < TOTAL_PARTICLES; p++)
			{
				particles[p].position += displacements[p];
			}
		}

		// After all sub-steps, update the positions for rendering
		for (int i = 0; i < TOTAL_PARTICLES; i++)
		{
			positions.at(i) = particles[i].position;
		}


		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);

		glBindBuffer(GL_ARRAY_BUFFER, vboPositions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec2), positions.data());

		glBindVertexArray(vao);
		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, vertices.size() / 2, TOTAL_PARTICLES);
		
		window.update();
	}
}