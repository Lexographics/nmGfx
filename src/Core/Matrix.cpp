#include "Matrix.hpp"

#include "glm/gtc/matrix_transform.hpp"

namespace nmGfx
{
    glm::mat4 CalculateModelMatrix(
        const glm::vec3& translation,
        const glm::vec3& rotation,
        const glm::vec3& scale,
        const glm::vec3& offset /*= glm::vec3(0.f, 0.f, 0.f)*/,
        const glm::mat4& baseTransform /*= glm::mat4(1.f)*/)
    {
        glm::mat4 transform = baseTransform;
        
        /*
        transform = glm::translate(transform, offset);
        transform = glm::translate(transform, translation);
        
        transform = glm::scale(transform, scale);

        transform = glm::rotate(transform, glm::radians(rotation.y), {0.f, 1.f, 0.f});
        transform = glm::rotate(transform, glm::radians(rotation.x), {1.f, 0.f, 0.f});
        transform = glm::rotate(transform, glm::radians(rotation.z), {0.f, 0.f, 1.f});
        */

        transform = glm::translate(transform, offset);
        
        transform = glm::scale(glm::mat4(1.f), scale) * transform;

        transform = glm::rotate(glm::mat4(1.f), glm::radians(rotation.z), {0.f, 0.f, 1.f}) * transform;
        transform = glm::rotate(glm::mat4(1.f), glm::radians(rotation.y), {0.f, 1.f, 0.f}) * transform;
        transform = glm::rotate(glm::mat4(1.f), glm::radians(rotation.x), {1.f, 0.f, 0.f}) * transform;
        
        transform = glm::translate(glm::mat4(1.f), translation) * transform;

        return transform;
    }
    
    glm::mat4 CalculateModelMatrix(
        const glm::vec2& position,
        float rotation,
        const glm::vec2& scale,
        const glm::vec2& offset /*= glm::vec2(0.f, 0.f)*/,
        const glm::mat4& baseTransform /*= glm::mat4(1.f)*/)
    {
        return CalculateModelMatrix({position.x, position.y, 0.f}, {0.f, 0.f, rotation}, {scale.x, scale.y, 1.f}, {offset.x, offset.y, 0.f}, baseTransform);
    }

    glm::mat4 CalculatePerspective(
        float aspect,
        float fov,
        float near,
        float far)
    {
        return glm::perspective(glm::radians(fov), aspect, near, far);
    }

    glm::mat4 CalculateOrtho(
        float left,
        float right,
        float bottom,
        float top,
        float near,
        float far)
    {
        return glm::ortho(left, right, bottom, top, near, far);
    }
} // namespace nmGfx