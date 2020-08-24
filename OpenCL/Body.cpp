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
void Body::moveCl(cl_float3 pos)
{
	m_pos.x = pos.x;
	m_pos.y = pos.y;
	m_pos.z = pos.z;
	m_mesh.setPos(m_pos);
}
void Body::setAccCl(cl_float3 acc)
{
	m_acc.x = acc.x;
	m_acc.y = acc.y;
	m_acc.z = acc.z;
}
void Body::setVelCl(cl_float3 vel)
{
	m_vel.x = vel.x;
	m_vel.y = vel.y;
	m_vel.z = vel.z;
}
void Body::rotate(float angle, const glm::vec3 &vect) {
	m_mesh.rotate(angle, vect);
}

void Body::scale(const glm::vec3 &vect) {
	m_mesh.scale(vect);
}

void Body::applyForce(const glm::vec3 &vect)
{
		m_acc = vect / m_mass;
}

void Body::integrate(GLfloat deltaTime)
{
		m_vel += (m_acc*deltaTime);
		this->setPos(m_pos + m_vel*deltaTime);
}



