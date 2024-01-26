#ifndef CAMERA_H
#define CAMERA_H

// Defines several possible options for camera movement
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = 90.0f;
const float PITCH = -45.0f;
const float SPEED = 20.0f;
const float SENSITIVITY = 0.01f;
const float ZOOM = 45.0f;

glm::vec3 ExtractCameraPos(const glm::mat4& a_modelView)
{
    // Get the 3 basis vector planes at the camera origin and transform them into model space
    glm::mat4 modelViewT = transpose(a_modelView);

    // Get plane normals 
    glm::vec3 n1(modelViewT[0]);
    glm::vec3 n2(modelViewT[1]);
    glm::vec3 n3(modelViewT[2]);

    // Get plane distances
    float d1(modelViewT[0].w);
    float d2(modelViewT[1].w);
    float d3(modelViewT[2].w);

    // Get the intersection of these 3 planes
    glm::vec3 n2n3 = cross(n2, n3);
    glm::vec3 n3n1 = cross(n3, n1);
    glm::vec3 n1n2 = cross(n1, n2);

    glm::vec3 top = (n2n3 * d1) + (n3n1 * d2) + (n1n2 * d3);
    float denom = dot(n1, n2n3);

    return top / -denom;
}

class Camera
{
public:

    // Camera attributes
    glm::vec3 Position;
    glm::quat Orientation;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler angles
    float Yaw;
    float Pitch;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Attributes to use when at the 3rd person
    float Distance = 75.0f;
    glm::vec3 Center;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix()
    {
        return glm::translate(glm::mat4_cast(glm::conjugate(Orientation)), -Position);
    }
    
    // Processes input received from any keyboard-like input system
    void processKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // Processes input received from a mouse input system
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event
    void processMouseScroll(float yoffset)
    {
        Distance -= (float)yoffset;
        if (Distance < 1.0f)
            Distance = 1.0f;
        if (Distance > 150.0f)
            Distance = 150.0f;
    }

    // Returns the view matrix at the 3rd person
    glm::mat4 get3rdPersonMatrix()
    {
        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, glm::vec3(0.0f, 0.0f, -Distance));
        m = glm::rotate(m, -Pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        m = glm::rotate(m, -Yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        m = glm::translate(m, glm::vec3(-Center));

        return m;
    }

private:

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the orientation of the camera
        Orientation = glm::angleAxis(glm::radians(-Yaw), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(glm::radians(Pitch), glm::vec3(1.0f, 0.0f, 0.0f));

        // Calculate the front and right vectors
        glm::quat qF = Orientation * glm::quat(0.0f, 0.0f, 0.0f, -1.0f) * glm::conjugate(Orientation);
        Front = glm::vec3(qF.x, qF.y, qF.z);
        Right = glm::normalize(glm::cross(Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
};

#endif