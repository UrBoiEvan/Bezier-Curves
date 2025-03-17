#ifndef POINTSOBJECT_HPP
#define POINTSOBJECT_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class PointsObject {
public:
    // Constructor takes two vectors (positions and colors) of equal length.
    PointsObject(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& colors);
    ~PointsObject();

    // Update the position of the point at the given index.
    void updatePoint(int index, const glm::vec3& newPosition);

    // Draw the points normally.
    void draw(const glm::mat4& view, const glm::mat4& projection);

    // Draw the points for picking (using a picking shader).
    void drawPicking(const glm::mat4& view, const glm::mat4& projection);

private:
    // Storage for positions and colors.
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;

    // OpenGL objects.
    GLuint VAO;
    GLuint VBO_positions;
    GLuint VBO_colors;

    // Shader programs.
    GLuint shaderProgram;
    GLuint pickingShaderProgram;
};

#endif // POINTSOBJECT_HPP
