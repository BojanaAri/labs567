#include <Camera.hpp>
#include <iostream>

Camera::Camera(glm::vec3 ground, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    CameraHeight = 0.1f;
    jumpHeight = 0.25f;
    CrouchHeight = -0.15f;
    isJumping = false;
    isFalling = false;
    isCrouching = false;
    isStandingUp = false;
    crouched = false;
    Ground = ground;
    Position = ground;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}
// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    CameraHeight = 1.0f;
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;

    Front.y  = 0.0f;


    if (direction == JUMP){
        if (!isJumping && !isFalling){
            isJumping = true;
        }
    }
    if (direction == CROUCH){
        if (!isCrouching && !isStandingUp){
            isCrouching = true;
        }
    }
    if (direction == STAND_UP){
        if (crouched && !isStandingUp){
            isStandingUp = true;
        }
    }
}

void Camera:: jump(float deltaTime){
    float velocity = MovementSpeed * deltaTime;
    if (isJumping){

        if(Position.y + 0.5f * velocity <= jumpHeight){
            Position += glm::vec3(0.0f, 0.5f, 0.0f) * velocity;
        } else{
            isJumping = false;
            isFalling = true;
        }
    }
}

void Camera:: ProcessFalling(float deltaTime){
    float velocity = MovementSpeed * deltaTime;
    if(isFalling){
        if(Position.y - 0.5f * velocity >= 0.0f) {
            Position -= glm::vec3(0.0f, 0.5f, 0.0f)*velocity;
            std::cout << Position.y << std::endl;
        }
        else{
            isFalling = false;
            isJumping = false;
            Position = glm::vec3(Position.x, 0.0f, Position.z);
        }
    }
}

void Camera:: Crouching(float deltaTime){
    float velocity = MovementSpeed * deltaTime;
    if (isCrouching){
        if(Position.y - 0.5f * velocity >= CrouchHeight){
            Position -= glm::vec3(0.0f, 0.5f, 0.0f) * velocity;
        } else{
            isCrouching = false;
            crouched = true;
        }
    }
}

void Camera:: StandingUp(float deltaTime){
    float velocity = MovementSpeed * deltaTime;
    if(isStandingUp){
        if(Position.y + 0.5f * velocity <= 0.0f) {
            Position += glm::vec3(0.0f, 0.5f, 0.0f) * velocity;
        }
        else{
            isCrouching = false;
            isStandingUp = false;
            crouched = false;
            Position = glm::vec3(Position.x, 0.0f, Position.z);
        }
    }
}

// Processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
    if (Zoom >= 1.0f && Zoom <= 45.0f)
        Zoom -= yoffset;
    if (Zoom <= 1.0f)
        Zoom = 1.0f;
    if (Zoom >= 45.0f)
        Zoom = 45.0f;
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(static_cast<double>(Yaw))) *
                                 cos(glm::radians(static_cast<double>(Pitch))));
    front.y = static_cast<float>(sin(glm::radians(static_cast<double>(Pitch))));
    front.z = static_cast<float>(sin(glm::radians(static_cast<double>(Yaw))) *
                                 cos(glm::radians(static_cast<double>(Pitch))));
    Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(
            Front, WorldUp)); // Normalize the vectors, because their length gets
    // closer to 0 the more you look up or down which
    // results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
