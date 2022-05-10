#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <queue>

enum Direction {
    K_FORWARD,
    K_BACKWARD,
    K_LEFT,
    K_RIGHT,
    K_UP,
    K_DOWN
};


class Submarin
{

public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    float Yaw;

    // store old yaw information to be able to drift
    std::queue<float> HistoryYaw;
    int DelayFrameNum = 20;

    // Realize the slow motion and slow stop of the sub
    std::queue<glm::vec3> HistoryPosition;

    float MovementSpeed;
    float TurningSpeed;

    Submarin(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    float getMidValYaw();
    glm::vec3 getMidValPosition();
    float getYaw();
    void ProcessKeyboard(Direction direction, float deltaTime);

private:
    void updateFront();

};

