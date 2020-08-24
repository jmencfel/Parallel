#pragma once
// Math constants
#define _USE_MATH_DEFINES
#define __CL_ENABLE_EXCEPTIONS
#pragma comment(lib, "OpenCL.lib")
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>
#include <fstream>
#include <chrono>
#include <random>
#include <array>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"


// Other Libs
#include "SOIL2/SOIL2.h"

#include <CL/cl.hpp>

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Body.h"
#include "Particle.h"
// parallel
#include <omp.h>
#include <thread>



using namespace cl;
using namespace std;

const float G = 6.67384e-11;
const int num_bodies = 256;

std::vector<std::shared_ptr<Particle>> bodies;

array<cl_float3, num_bodies> positions;
array<cl_float3, num_bodies> velocities;
array<cl_float3, num_bodies> accelerations;
array<cl_float, num_bodies> masses;
array<cl_float3, num_bodies> new_acc;
array<cl_float3, num_bodies> new_pos;
array<cl_float3, num_bodies> new_vel;


constexpr std::size_t DATA_SIZE = sizeof(cl_float3)*num_bodies;
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 6.0f, 20.0f));


	// time 
	GLdouble deltaTime = 0.01f;
	GLfloat currentTime = (GLfloat)glfwGetTime();
	GLdouble accumulator = 0.0;
		
		std::random_device r;
		std::default_random_engine e(r());
		std::uniform_real_distribution<float> pos_distribution(-10.0f, 10.0f);
		std::uniform_real_distribution<float> mass_distribution(1.0f, 50.0f);
		
		
		std::ofstream data("opencl times.csv", std::ofstream::out);
		for (int i = 0; i < num_bodies; i++)
		{
			auto p = std::make_shared<Particle>();
			p->getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_white.frag"));
			float x = pos_distribution(e);
			float y = pos_distribution(e) / 10.0f;
			float z = pos_distribution(e);
			p->setPos(glm::vec3(x, y, z));
			glm::vec3 spin = glm::cross(p->getPos(), glm::vec3(0, 1, 0));
			//p->setVel(spin);
			auto s = mass_distribution(e);
			p->setMass(5000.0f);
			p->getMesh().scale(glm::vec3(.02f, .02f, .02f) * s);
			bodies.push_back(std::move(p));


			positions[i] = { x, y, z };
			velocities[i] = { 0,0,0};
			accelerations[i] = { 0 ,0,0};
			masses[i] = 5000.0f;
	
		}
			// Get the platforms
			vector<Platform> platforms;
			Platform::get(&platforms);

			// Assume only one platform.  Get GPU devices.
			vector<Device> devices;
			platforms[1].getDevices(CL_DEVICE_TYPE_GPU, &devices);

			// Just to test, print out device 0 name
			cout << devices[0].getInfo<CL_DEVICE_NAME>() << endl;

			// Create a context with these devices
			Context context(devices);

			// Create a command queue for device 0
			CommandQueue queue(context, devices[0]);

			// Create the buffers
			Buffer bufA(context, CL_MEM_READ_ONLY, DATA_SIZE);
			Buffer bufB(context, CL_MEM_READ_ONLY, DATA_SIZE);
			Buffer bufC(context, CL_MEM_READ_ONLY, DATA_SIZE);
			Buffer bufD(context, CL_MEM_READ_ONLY, sizeof(cl_float)*num_bodies);
			Buffer bufE(context, CL_MEM_WRITE_ONLY, DATA_SIZE);
			Buffer bufF(context, CL_MEM_WRITE_ONLY, DATA_SIZE);
			Buffer bufG(context, CL_MEM_WRITE_ONLY, DATA_SIZE);

			// Copy data to the GPU
			queue.enqueueWriteBuffer(bufA, CL_TRUE, 0, DATA_SIZE, &positions);
			queue.enqueueWriteBuffer(bufB, CL_TRUE, 0, DATA_SIZE, &velocities);
			queue.enqueueWriteBuffer(bufC, CL_TRUE, 0, DATA_SIZE, &accelerations);
			queue.enqueueWriteBuffer(bufD, CL_TRUE, 0, sizeof(cl_float)*num_bodies, &masses);
		

			// Read in kernel source
			ifstream file("kernels.cl");
			string code(istreambuf_iterator<char>(file), (istreambuf_iterator<char>()));

			// Create program
			Program::Sources source(1, make_pair(code.c_str(), code.length() + 1));
			Program program(context, source);

			// Build program for devices
			program.build(devices);

			// Create the kernel
			Kernel vecadd_kernel(program, "calculateForces");

			// Set kernel arguments
			vecadd_kernel.setArg(0, bufA);
			vecadd_kernel.setArg(1, bufB);
			vecadd_kernel.setArg(2, bufC);
			vecadd_kernel.setArg(3, bufD);
			vecadd_kernel.setArg(4, bufE);

			


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
			

			queue.enqueueWriteBuffer(bufA, CL_TRUE, 0, DATA_SIZE, &positions);
			queue.enqueueWriteBuffer(bufB, CL_TRUE, 0, DATA_SIZE, &velocities);
			queue.enqueueWriteBuffer(bufC, CL_TRUE, 0, DATA_SIZE, &accelerations);
			queue.enqueueWriteBuffer(bufD, CL_TRUE, 0, sizeof(cl_float)*num_bodies, &masses);


			NDRange global(num_bodies);
			NDRange local(256);
			queue.enqueueNDRangeKernel(vecadd_kernel, NullRange, global, local);

			queue.enqueueReadBuffer(bufE, CL_TRUE, 0, DATA_SIZE, &new_acc);
			
			
			// Copy data to the GPU
			queue.enqueueWriteBuffer(bufA, CL_TRUE, 0, DATA_SIZE, &positions);
			queue.enqueueWriteBuffer(bufB, CL_TRUE, 0, DATA_SIZE, &velocities);
			queue.enqueueWriteBuffer(bufC, CL_TRUE, 0, DATA_SIZE, &accelerations);
			// Create the kernel
			Kernel integrate_kernel(program, "integrate");
			

			// Set kernel arguments
			integrate_kernel.setArg(0, bufA);
			integrate_kernel.setArg(1, bufB);
			integrate_kernel.setArg(2, bufC);
			integrate_kernel.setArg(3, bufF);
			integrate_kernel.setArg(4, bufG);

			queue.enqueueNDRangeKernel(integrate_kernel, NullRange, global, local);

			queue.enqueueReadBuffer(bufF, CL_TRUE, 0, DATA_SIZE, &new_pos);
			queue.enqueueReadBuffer(bufG, CL_TRUE, 0, DATA_SIZE, &new_vel);

#pragma omp parallel for num_threads(8) schedule(static)
			for (int i = 0; i < num_bodies; i++)
			{
				accelerations[i] = new_acc[i];
				positions[i] = new_pos[i];
				velocities[i] = new_vel[i];
				bodies.at(i)->setAccCl(new_acc[i]);
				bodies.at(i)->moveCl(new_pos[i]);
				bodies.at(i)->setVelCl(new_vel[i]);
			}
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





