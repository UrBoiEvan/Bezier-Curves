// main.cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "PointsObject.hpp"

// Function prototypes
int initWindow(void);
static void mouseCallback(GLFWwindow*, int, int, int);
int getPickedIndex();
glm::vec3 getWorldPosition(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

const GLuint windowWidth = 1024, windowHeight = 768;
GLFWwindow* window;

// The point that is currently selected.
int currSelected =-1;
float pi = 3.14159265358979323846f; // Pi constant
std::vector<glm::vec3> points; // Vector to hold the points
std::vector<glm::vec3> colors; // Vector to hold the colors
glm::vec3 storedColor; // Used to restore a point to its original color after picking color is drawn
int storedIndex;
PointsObject* pointsObj;

int main() {
    // ATTN: REFER TO https://learnopengl.com/Getting-started/Creating-a-window
    // AND https://learnopengl.com/Getting-started/Hello-Window to familiarize yourself with the initialization of a window in OpenGL
    
    if (initWindow() != 0) return -1;


    glm::mat4 projectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

    std::vector<glm::vec3> colors;
    glm::vec3 generatedColor;
    for (int i = 0; i < 8; ++i) {
        float angle = i * (2.0f * pi / 8.0f); // Angle for each point
        float x = 2.0f * cos(angle); // X position on the circle
        float y = 2.0f * sin(angle); // Y position on the circle
        points.push_back(glm::vec3(x, y, 0.0f)); // Add point position
        // Assign a color to each point 
    }

    colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Default color (green)--
    colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f)); // Default color (red)
    colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f)); // Default color (blue)
    colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f)); // Default color (yellow)
    colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f)); // Default color (magenta)
    colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f)); // Default color (cyan)
    colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f)); // Default color (green)--
    colors.push_back(glm::vec3(1.0f, 0.5f, 0.0f)); // Default color (orange)
    
    //TODO: P2aTask1 - Display 8 points on the screen each of a different color and arranged uniformly on a circle.
    pointsObj = new PointsObject(points, colors);
    
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    do {
        
        // Timing
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
            printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
        
        
        glm::mat4 viewMatrix = glm::mat4(1.0f); // Identity matrix

        
        if(currSelected >= 0){
            // Dragging for P2aTask3
            glm::vec3 worldPos = getWorldPosition(viewMatrix, projectionMatrix);
            pointsObj->updatePoint(currSelected, worldPos);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)){
            // Draw picking for P2aTask2
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            pointsObj->drawPicking(viewMatrix, projectionMatrix); // drawn in picking mode, the user will never see these colors
            currSelected = getPickedIndex();
            
            if (currSelected >= 0) {
                storedColor = pointsObj->getPointColor(currSelected);
                pointsObj->setPointColor(currSelected, glm::vec3(1.0f, 1.0f, 1.0f));
            }
        }
        //if(!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)){
        //    currSelected = -1;
        //} ^^ Uses glfw mouse callback instead of polling ^^
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // DRAWING the SCENE

        pointsObj->draw(viewMatrix, projectionMatrix);
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);

    glfwTerminate();
    return 0;
}

// Initialize GLFW and create a window
int initWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // FOR MAC

    window = glfwCreateWindow(windowWidth, windowHeight, "Harden,Evan(27541192)", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    // Set up inputs
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
    glfwSetMouseButtonCallback(window, mouseCallback);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    // Make points big
    glPointSize(20.0f);
    
    return 0;
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && currSelected >= 0) {
        pointsObj->setPointColor(currSelected, storedColor); // restore color
        currSelected = -1;
    }
}

int getPickedIndex(){ // colors are drawn in the picking mode
    glFlush();
    // --- Wait until all the pending drawing commands are really done.
    // Ultra-mega-over slow !
    // There are usually a long time between glDrawElements() and
    // all the fragments completely rasterized.
    glFinish();
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    unsigned char data[4];

    //TODO: P2aTask2 - Use glfwGetCursorPos to get the x and y value of the cursor.
    double x_pos, y_pos;
    glfwGetCursorPos(window, &x_pos, &y_pos);
    y_pos = windowHeight - y_pos; // Flip y position as glfwGetCursorPos gives the cursor position relative to top left of the screen.
    //TODO: P2aTask2 - Use glfwGetFramebufferSize and glfwGetWindowSize to get the frame buffer size and window size. On high resolution displays, these sizes might be different.
    glReadPixels(x_pos, y_pos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //TODO: P2aTask2 - Use glReadPixels(x_read, y_read, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data) to read the pixel data.
    // Note that y position has to be flipped as glfwGetCursorPos gives the cursor position relative to top left of the screen. The read location must also be multiplied by (buffer size / windowSize) for some displays.
    
    
    int pickedId = data[0] - 1;
    return pickedId;
}
glm::vec3 getWorldPosition(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    
    //TODO: P2aTask3 - Use glfwGetFramebufferSize and glfwGetWindowSize to get the frame buffer size and window size. On high resolution displays, these sizes might be different.
    double x_pos, y_pos;
    glfwGetCursorPos(window, &x_pos, &y_pos);
    y_pos = windowHeight - y_pos; // Flip y position as glfwGetCursorPos gives the cursor position relative to top left of the screen.
    //TODO: P2aTask2 - Use glfwGetCursorPos to get the x and y value of the cursor.
    // Note that y position has to be flipped as glfwGetCursorPos gives the cursor position relative to top left of the screen. The read location must also be multiplied by (buffer size / windowSize) for some displays.
    
    //TODO: P2aTask2 - glGetIntegerv(GL_VIEWPORT, viewport) to get the viewport
    
    //TODO: P2Task3 - use glm::unProject(screenPos, viewMatrix, projectionMatrix, vp) to obtaint the world position.
    // Note that vp is viewport converted to vec4; screenPos is vec3(x_screen, y_screen, 0)
    
    glm::vec3 worldPos;
    worldPos = glm::unProject(glm::vec3(x_pos, y_pos, 0.0f), viewMatrix, projectionMatrix, glm::vec4(0, 0, windowWidth, windowHeight));
    
    return worldPos;
}
