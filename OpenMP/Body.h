#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"

class Body
{
public:
	Body();
	~Body();

	/*
	** GET METHODS
	*/
	// mesh
	Mesh& getMesh() { return m_mesh; }

	// transform matrices
	glm::mat3 getTranslate() const { return m_mesh.getTranslate(); }
	glm::mat3 getRotate() const { return m_mesh.getRotate(); }
	glm::mat3 getScale() const { return m_mesh.getScale(); }

	// dynamic variables
	glm::vec3& getAcc() { return m_acc; }
	glm::vec3& getVel() { return m_vel; }
	glm::vec3& getPos() { return m_pos; }

	// physical properties
	float getMass() const { return m_mass; }
	float getCor() { return m_cor; }

	/*
	** SET METHODS
	*/
	// mesh
	void setMesh(Mesh m) { m_mesh = m; }

	// dynamic variables
	void setAcc(const glm::vec3 &vect) { m_acc = vect; }
	void setVel(const glm::vec3 &vect) { m_vel = vect;}
	void setVel(int i, float v) { m_vel[i] = v;} //set the ith coordinate of the velocity vector
	void setPos(const glm::vec3 &vect) { m_pos = vect; m_mesh.setPos(vect); }
	void setPos(int i, float p) { m_pos[i] = p; m_mesh.setPos(i, p); } //set the ith coordinate of the position vector


	// physical properties
	void setCor(float cor) { m_cor = cor; }
	void setMass(float mass) { m_mass = mass; }

	/*
	** OTHER METHODS
	*/

	// transformation methods
	void translate(const glm::vec3 &vect);
	void rotate(float angle, const glm::vec3 &vect);
	void scale(const glm::vec3 &vect);

	//my own methods to abstract physics calculations in main
	void applyForce(const glm::vec3 &vect);
	void integrate(GLfloat deltaTime);
	void Body::bounceBetween(const glm::vec3 &bottom, const glm::vec3 &top);

private:
	Mesh m_mesh; // mesh used to represent the body

	float m_mass; // mass
	float m_cor; // coefficient of restitution

	glm::vec3 m_acc; // acceleration
	glm::vec3 m_vel; // velocity
	glm::vec3 m_pos; // position
	bool inGravity = false; //needed to determine the acceleration of a body(to add(or not) gravitational pull to acceleration coming from other forces)
};

