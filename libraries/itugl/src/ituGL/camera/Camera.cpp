#include <ituGL/camera/Camera.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera() : m_viewMatrix(1.0f), m_projMatrix(1.0f)
{
}

void Camera::SetViewMatrix(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& up)
{
    // (todo) 03.5: Create the view matrix
    m_viewMatrix = glm::lookAt(position, lookAt, up);
}

void Camera::SetPerspectiveProjectionMatrix(float fov, float aspect, float near, float far)
{
    // (todo) 03.5: Create the perspective projection matrix
    m_projMatrix = glm::perspective(fov, aspect, near, far);
}

void Camera::SetOrthographicProjectionMatrix(const glm::vec3& min, const glm::vec3& max)
{
    // (todo) 03.5: Create the orthographic projection matrix
    m_projMatrix = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z);
}
