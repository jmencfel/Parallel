#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>
#include <fstream>
#include <chrono>
#include <random>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"


// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Body.h"
#include "Particle.h"
// parallel
#include <omp.h>
#include <thread>


// main function
//const double G = 0.000000000066742;
const double G = 0.66742;
const int num_bodies = 2048;
static const auto num_threads = std::thread::hardware_concurrency();
std::vector<std::shared_ptr<Particle>> bodies;
void bruteForceSim(float deltaTime)
{
#pragma omp parallel for num_threads(num_threads) schedule(dynamic)
	for (int i = 0; i < num_bodies; i++)
	{
		glm::vec3 temp = glm::vec3(0);
#pragma omp parallel for num_threads(num_threads) schedule(dynamic)
		for (int j = 0; j < num_bodies; j++)
		{
			if (j == i) continue;
			glm::vec3 normal = glm::normalize(glm::vec3(bodies.at(j)->getPos() - bodies.at(i)->getPos()));
			float distance = glm::length(glm::vec3(bodies.at(i)->getPos() - bodies.at(j)->getPos()));
			float magnitude = (G * bodies.at(i)->getMass() * bodies.at(j)->getMass()) / pow(distance, 2);
			if (magnitude > 3.0f) magnitude = 3.0f;
			temp += normal * magnitude;
		}
		bodies.at(i)->applyForce(temp);

	}
}
void integrateAll(GLfloat deltaTime)
{
#pragma omp parallel for num_threads(num_threads) schedule(dynamic)
	for (int i = 0; i < num_bodies; i++)
	{
		bodies.at(i)->setVel(bodies.at(i)->getVel() + (bodies.at(i)->getAcc()*deltaTime));
		bodies.at(i)->setPos(bodies.at(i)->getPos() + bodies.at(i)->getVel()* deltaTime);
	}
}
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 6.0f, 20.0f));
			
	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_real_distribution<double> pos_distribution(-10.0f, 10.0f);
	std::uniform_real_distribution<double> mass_distribution(1.0f, 50.0f);

	std::ofstream data("OMP times.csv", std::ofstream::out);

	// create particles
	for (int i = 0; i < num_bodies; i++)
	{
		auto p = std::make_shared<Particle>();
		p->getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_white.frag"));
		auto x = pos_distribution(e);
		auto y = pos_distribution(e)/10.0f;
		auto z = pos_distribution(e);
		p->setPos(glm::vec3(x,y,z));
		glm::vec3 spin =  glm::cross(p->getPos(), glm::vec3(0, 1, 0));
		p->setVel(spin);
		auto s = mass_distribution(e);
		p->setMass(50.0f);
		p->getMesh().scale(glm::vec3(.02f, .02f, .02f) * s);
		bodies.push_back(std::move(p));
	}
	/*
	///////////////black hole
	auto p = std::make_shared<Particle>();
	p->getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_white.frag"));
	p->setPos(glm::vec3(0));
	p->setMass(5000.0f);
	p->getMesh().scale(glm::vec3(5));
	bodies.push_back(std::move(p));
	*/

	// time 
	GLdouble deltaTime = 0.01f;
	GLfloat currentTime = (GLfloat)glfwGetTime();
	GLdouble accumulator = 0.0;


	while (!glfwWindowShouldClose(app.getWindow()))
	{

		// Set frame time
		GLfloat newTime = (GLfloat)glfwGetTime();
		// the animation can be sped up or slowed down by multiplying currentFrame by a factor.
		GLfloat frameTime = newTime - currentTime;
		currentTime = newTime;		
		accumulator += frameTime;
		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(deltaTime);

		////////////////////////////TIMEse STEP SOLUTION loop
		while (accumulator >= deltaTime)
		{		
			/////////////////physics calculations    and time measurements
			auto start = std::chrono::system_clock::now();
			bruteForceSim(deltaTime);
			
			/////////////////integration ( changing of the positions )
			integrateAll(deltaTime);
			auto end = std::chrono::system_clock::now();
			auto total = end - start;
			data << ", " << std::chrono::duration_cast<std::chrono::nanoseconds>(total).count();
			accumulator -= deltaTime;

			
		}

		//RENDER 

		app.clear();
		for (int i = 0; i < num_bodies; i++)
		app.draw(bodies.at(i)->getMesh());
		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}





