#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <custom/shader.h>
#include <glm/glm.hpp>

#include <iostream>

// Const variables & Global variables
const int Window_Width = 800;
const int Window_Height = 800;
float ratio = float(Window_Height)/float(Window_Width);

bool isDrawCircle = false;

bool isDrawing = false;
glm::vec2 firstPos = glm::vec2(0.0f, 0.0f);

float radius = 0.0f;

float vertices[] = {
    0.0f, 0.0f, 0.0f, // top right
    0.0f, 0.0f, 0.0f, // bottom right
    0.0f, 0.0f, 0.0f, // bottom left
    0.0f, 0.0f, 0.0f  // top left
};
unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3, // first Triangle
    1, 2, 3  // second Triangle
};

unsigned int VBO, VAO, EBO;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

int main()
{
    // glfw Initialize
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = glfwCreateWindow(Window_Width, Window_Height, "Mouse Drag", nullptr, nullptr);

    // useless on macos mojave 10.14.4, all windows' fps are limited to 60
    // glfwSwapInterval(0);

    // Check for valid context
    if (window == nullptr)
    {
        fprintf(stderr, "Failed to create OpenGL context");
        return EXIT_FAILURE;
    }
    // Make context before load GLAD
    glfwMakeContextCurrent(window);
    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initalize GLAD");
        return EXIT_FAILURE;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    Shader litShader("src/ParticleSystem/shaders/lit.vert", "src/ParticleSystem/shaders/lit.frag");
    Shader litCircleShader("src/ParticleSystem/shaders/lit_circle.vert", "src/ParticleSystem/shaders/lit_circle.frag");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (glfwWindowShouldClose(window) == false)
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        glBindVertexArray(VAO);

        if (!isDrawCircle)
        {
            litShader.use();
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            litCircleShader.use();
            litCircleShader.setVec2("center", firstPos);
            litCircleShader.setFloat("radius", radius);
            litCircleShader.setFloat("width_height_ratio",ratio);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        isDrawCircle = !isDrawCircle;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    float x = xpos / Window_Width;
    float y = ypos / Window_Height;

    x = 2 * x - 1;
    y = -2 * y + 1;

    // std::cout<< x << "," << y <<std::endl;

    if (!isDrawCircle)
    {
        if (!isDrawing)
        {
            firstPos = glm::vec2(x, y); // 不绘制的时候时刻记录鼠标位置，一旦绘制，这点就作为鼠标的开始点
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
            glBindVertexArray(0);
        }
        else
        {
            float new_vertices[] =
                {
                    x, firstPos.y, 0.0f,         // top right
                    x, y, 0.0f,                  // bottom right
                    firstPos.x, y, 0.0f,         // bottom left
                    firstPos.x, firstPos.y, 0.0f // top left
                };
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(new_vertices), new_vertices, GL_DYNAMIC_DRAW);
            glBindVertexArray(0);
        }
    }
    else
    {
        if (!isDrawing)
        {
            firstPos = glm::vec2(x, y); // 不绘制的时候时刻记录鼠标位置，一旦绘制，这点就作为鼠标的开始点
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
            glBindVertexArray(0);
        }
        else
        {
            glm::vec2 v_radius = glm::vec2(x,y) - firstPos;
            float length = glm::sqrt(v_radius.x * v_radius.x + v_radius.y * v_radius.y);
            radius = length;
            float new_vertices[] =
            {
                firstPos.x + radius, firstPos.y + radius, 0.0f,         // top right
                firstPos.x + radius, firstPos.y - radius, 0.0f,         // bottom right
                firstPos.x - radius, firstPos.y - radius, 0.0f,         // bottom left
                firstPos.x - radius, firstPos.y + radius, 0.0f          // top left
            };
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO); 
            glBufferData(GL_ARRAY_BUFFER, sizeof(new_vertices), new_vertices, GL_DYNAMIC_DRAW);
            glBindVertexArray(0);
        }
    }
    
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        isDrawing = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        isDrawing = false;
    }
}
