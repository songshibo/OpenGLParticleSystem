#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <custom/shader.h>
#include <custom/camera.h>

#include <Particle.hpp>

// Const variables & Global variables
const int Window_Width = 800;
const int Window_Height = 600;

Camera *camera;
GLFWwindow *window;

//Input
float lastX = Window_Width / 2.0f;
float lastY = Window_Height / 2.0f;

//FPS calculation
int FPS = 0;
float deltaSum = 0.0f;

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void SetUpGlobalStatus();

int main()
{
    // glfw Initialize
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    int const DPI = 2; // For retina screens only
#endif
    window = glfwCreateWindow(Window_Width, Window_Height, "Particle System", nullptr, nullptr);


    // Check for valid context
    if(window == nullptr)
    {
        fprintf(stderr, "Failed to create OpenGL context");
        return EXIT_FAILURE;
    }
    // Make context before load GLAD 
    glfwMakeContextCurrent(window);
    // Load GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initalize GLAD");
        return EXIT_FAILURE;
    }

    SetUpGlobalStatus();

    Shader screen("./src/ParticleSystem/shaders/screen.vert","./src/ParticleSystem/shaders/screen.frag");
    Shader generatePosShader("./src/ParticleSystem/shaders/generatePos.vert","./src/ParticleSystem/shaders/generatePos.frag");
    screen.use();
    screen.setInt("screenTexture", 0);

    GLuint FBO;
    GLuint posTex;
    GLuint quadVAO, quadVBO;

    glGenBuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &posTex);
    glBindTexture(GL_TEXTURE_2D, posTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Window_Width, Window_Height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);


    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    std::cout << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    // Main Loop
    while(glfwWindowShouldClose(window) == false)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
                glfwSetWindowShouldClose(window, true);
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glDisable(GL_DEPTH_TEST);

        generatePosShader.use();
        glBindVertexArray(quadVAO);
        generatePosShader.setVec3("_pos", glm::vec3(0.0f, 1.0f, 0.0f));
        generatePosShader.setFloat("b_radius", 2.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glDisable(GL_DEPTH_TEST);
        // glClear(GL_COLOR_BUFFER_BIT);
        // glClearColor(0.3f, 0.0f, 0.0f, 1.0f);

        // screen.use();
        // glBindVertexArray(quadVAO);
        // glBindTexture(GL_TEXTURE_2D, posTex);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

// OpenGL & Particles & other class instance setup 
void SetUpGlobalStatus()
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}