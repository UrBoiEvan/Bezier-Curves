#include "PointsObject.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "common/shader.hpp"

PointsObject::PointsObject(const std::vector<glm::vec3>& initPositions, const std::vector<glm::vec3>& initColors) {
    if (initPositions.size() != initColors.size()) {
        std::cerr << "Error: positions and colors vectors must have the same size." << std::endl;
        return;
    }
    positions = initPositions;
    colors = initColors;

    // Generate VAO and two VBOs (one for positions, one for colors)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_positions);
    glGenBuffers(1, &VBO_colors);

    // Load the shader programs.
    shaderProgram = LoadShaders("pointVertexShader.glsl", "pointFragmentShader.glsl");
    pickingShaderProgram = LoadShaders("pickingPointVertexShader.glsl", "pickingPointFragmentShader.glsl");

    glBindVertexArray(VAO); // bind VAO to set up the vertex attributes

    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_colors);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO after attributes are set
}

PointsObject::~PointsObject() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO_positions);
    glDeleteBuffers(1, &VBO_colors);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(pickingShaderProgram);
}

void PointsObject::updatePoint(int index, const glm::vec3& newPosition) {
    if (index < 0 || index >= positions.size()) {
        std::cerr << "Error: point index out of range." << std::endl;
        return;
    }
    
    positions[index] = newPosition;
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
    
    //TODO: P2aTask3 - Use glBufferSubData to updated the point location in the buffer.
    // Offset = index * sizeof(glm::vec3), size = sizeof(glm::vec3)
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


// Draw the points normally.
void PointsObject::draw(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    glm::mat4 MVP = projection * view;
    GLuint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, positions.size());
    glBindVertexArray(0);
    glUseProgram(0);
}

// Draw the points for picking.
void PointsObject::drawPicking(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(pickingShaderProgram);
    glm::mat4 MVP = projection * view;
    GLuint mvpLoc = glGetUniformLocation(pickingShaderProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

    glBindVertexArray(VAO);
    // In the picking shader, we rely on gl_VertexID to generate a unique color per point.
    glDrawArrays(GL_POINTS, 0, positions.size());
    glBindVertexArray(0);
    glUseProgram(0);
}
