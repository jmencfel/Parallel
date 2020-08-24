#include "Body.h"


Body::Body()
{
}


Body::~Body()
{
}

/* TRANSFORMATION METHODS*/
void Body::translate(const glm::vec3 &vect) {
	m_pos = m_pos + vect;
	m_mesh.translate(vect);
}

void Body::rotate(float angle, const glm::vec3 &vect) {
	m_mesh.rotate(angle, vect);
}

void Body::scale(const glm::vec3 &vect) {
	m_mesh.scale(vect);
}
void Body::addGravity(){
	inGravity = true;
	m_acc = glm::vec3(0.0f, -9.8f, 0.0f);
}
void Body::applyForce(const glm::vec3 &vect)
{
	if (inGravity)
	{
		m_acc = (vect / m_mass) + glm::vec3(0.0f, -9.8f, 0.0f);
	}
	else
	{
		m_acc = vect / m_mass;
	}
}
void Body::applyDrag(GLfloat dragForce)
{
	this->applyForce(-1.0f * dragForce*(glm::vec3(m_vel.x/m_vel.length(), m_vel.y/ m_vel.length(), m_vel.z/ m_vel.length())));
}
void Body::moveSemiImplicitEuler(GLfloat deltaTime)
{
		m_vel += (m_acc*deltaTime);
		this->setPos(m_pos + m_vel*deltaTime);
}
void Body::moveForwardEuler(GLfloat deltaTime)
{
		this->setPos(m_pos + m_vel*deltaTime);
		m_vel += (m_acc*deltaTime);
}
void Body::bounceBetween(const glm::vec3 &bottom, const glm::vec3 &top)
{
	if (m_pos.y < bottom.y)
	{
		m_pos.y = bottom.y;
		m_vel.y *= -0.8f;
		this->applyDrag(0.8f);
	}
	else if (m_pos.y > top.y)
	{
		m_pos.y = top.y;
		m_vel.y *= -1.0f;
	}
	if (m_pos.x < bottom.x)
	{
		m_pos.x = bottom.x;
		m_vel.x *= -1.0f;
	}
	else if (m_pos.x > top.x)
	{
		m_pos.x = top.x;
		m_vel.x *= -1.0f;
	}
	if (m_pos.z < bottom.z)
	{
		m_pos.z = bottom.z;
		m_vel.z *= -1.0f;
	}
	else if (m_pos.z > top.z)
	{
		m_pos.z = top.z;
		m_vel.z *= -1.0f;
	}

}

