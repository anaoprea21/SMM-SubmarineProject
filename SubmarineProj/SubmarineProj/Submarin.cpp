#include "Submarin.h"

Submarin::Submarin(glm::vec3 position) : MovementSpeed(10.0f)
, TurningSpeed(10.0f)
, Yaw(0.0f)
{
	Position = position;
	WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	updateFront();
}

float Submarin::getMidValYaw()
{
	return Yaw / 2.0f;
}

glm::vec3 Submarin::getMidValPosition()
{
	return Position / 2.0f;
}

float Submarin::getYaw()
{
	return Yaw;
}

void Submarin::updateFront()
{
	glm::vec3 front;
	front.x = -sin(glm::radians(getMidValYaw()));
	front.y = 0.0f;
	front.z = -cos(glm::radians(getMidValYaw()));
	Front = glm::normalize(front);

	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}

void Submarin::ProcessKeyboard(Direction direction, float deltaTime)
{
	if (direction == K_FORWARD && Position.x > -591)
	{
		Position -= Right * MovementSpeed * deltaTime;
	}
	if (direction == K_BACKWARD && Position.x < 565)
	{
		Position += Right * MovementSpeed * deltaTime;
	}
	if (direction == K_RIGHT && Position.z > -590)
	{
		Position += Front * MovementSpeed * deltaTime;
	}
	if (direction == K_LEFT && Position.z < 588)
	{
		Position -= Front * MovementSpeed * deltaTime;
	}
	if (direction == K_UP && Position.y < -3.0f)
		Position += WorldUp * MovementSpeed * deltaTime;
	if (direction == K_DOWN)
		Position -= WorldUp * MovementSpeed * deltaTime;


	updateFront();
}
