#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <custom/shader.h> //着色器类
#include <custom/camera.h> //相机类
#include <custom/model.h>  //模型类

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height); //每帧的窗口回调函数，确保视口为设置大小
void mouse_callback(GLFWwindow *window, double xpos, double ypos);         //鼠标回调函数，用于处理鼠标输入控制相机旋转
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);  //滚轮回调函数，处理相机的拉近拉远
void processInput(GLFWwindow *window);                                     //处理输入，包括esc键退出，WASD控制相机移动，EQ控制地形高度
unsigned int loadTexture(const char *path);                                //通过路径读取贴图，返回纹理的索引

// settings
const unsigned int SCR_WIDTH = 1280; //窗口宽
const unsigned int SCR_HEIGHT = 720; //窗口高

// camera
Camera *camera; //相机的实例
// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 8.0f, 13.0f); //相机初始位置
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   //指向相机上方的向量
const float FoV = 60;                               //相机的视野 field of view
const float zNear = 0.1f;                           //相机的近裁剪面距离
const float zFar = 100;                             //相机的远裁剪面距离

//Input
float lastX = SCR_WIDTH / 2.0f; //处理鼠标的初始位置，默认在窗口中间
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true; //确保每一帧鼠标输入仅一次

// timing
float deltaTime = 0.0f; //用于记录每帧的长度
float lastFrame = 0.0f; //用于计算每帧的长度

// parameters
float height_sacle = 3.0f; //初始高度缩放大小

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();                                                    //初始化glfw
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 //设置窗口opengl主版本为3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 //设置窗口opengl次版本为3 opengl版本为3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //设置opengl为核心模式

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL); //初始化窗口，参数为宽、高、标题、显示器（无）与共享窗口（无）
    if (window == NULL)                                                                      //窗口创建失败直接报错
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);                                    //设置当前内容到窗口
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //设置窗口帧缓冲大小回调函数
    glfwSetCursorPosCallback(window, mouse_callback);                  //设置鼠标回调函数
    glfwSetScrollCallback(window, scroll_callback);                    //设置滚轮回调函数

    // tell GLFW to capture our mouse, 用于禁止鼠标在屏幕上的位移，仍可输入
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers 加载opengl函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Camera Setup
    glm::vec3 origin = glm::vec3(0.0f);                     // this will make sure the camera look at origin point after initialization 相机看向原点(0,0,0)
    glm::vec3 towards = glm::normalize(origin - cameraPos); //相机的前向量，即从相机位置指向原点的单位向量
    // pitch stands for the angle we're looking up or down
    float pitch = -glm::degrees(acos(glm::dot(towards, glm::vec3(0.0f, 0.0f, -1.0f)))); //相机的俯仰角，即前向量和z轴负方向（从屏幕外指向屏幕内的方向）的点成通过acos函数求得弧度后转换为角度
    // yaw stands for the magnitude we're looking to the left or to the right //yaw 即是偏航角，下为计算偏航角的过程
    glm::vec3 proj = glm::normalize(towards - glm::dot(glm::vec3(0.0f, -1.0f, 0.0f), towards) * glm::vec3(0.0f, -1.0f, 0.0f));
    float yaw = -glm::degrees(acos(glm::dot(proj, glm::vec3(1.0f, 0.0f, 0.0f))));
    camera = new Camera(cameraPos, cameraUp, yaw, pitch);                                                          //初始化相机
    glm::mat4 projection = glm::perspective(glm::radians(FoV), float(SCR_WIDTH) / float(SCR_HEIGHT), zNear, zFar); //计算相机的裁剪矩阵，一般在初始化后固定

    // configure global opengl state
    glEnable(GL_DEPTH_TEST); //允许深度测试以确保正确的遮挡效果

    // build and compile shaders 创建shader实例并编译，内部函数在shader.h中
    Shader shader("../src/ParticleSystem/shaders/geometry.vert", "../src/ParticleSystem/shaders/geometry.frag", "../src/ParticleSystem/shaders/geometry.gs");

    //load model
    // 模型实例，用模型路径初始化
    Model plane("../src/ParticleSystem/model/plane/subdiv_plane.obj");
    // 构造用于模型变换的矩阵，第一个是位置。第二个是缩放
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // plane position
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));     // plane scale size

    // 读取2张贴图
    // diffuse即是漫反射颜色贴图
    // hight即是高度图
    GLuint diffuse = loadTexture("../src/ParticleSystem/model/plane/DustySoil_1024_albedo.png");
    GLuint height = loadTexture("../src/ParticleSystem/textures/height.bmp");

    // render loop
    while (!glfwWindowShouldClose(window)) // 当检测到esc或者窗口应该关闭则退出渲染循环
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();   //获取当前时间
        deltaTime = currentFrame - lastFrame; //计算帧长
        lastFrame = currentFrame;             //更新lastFrame以便下个循环计算帧长

        // input
        //初始输入
        processInput(window);

        // render
        // 背景颜色
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // 清除颜色缓冲和深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 渲染模式。这里是线模式
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // 指定当前渲染使用着色器，并传递数据到shader
        shader.use();
        shader.setMat4("projection", projection);                         // 设置裁剪矩阵
        shader.setMat4("view", camera->GetViewMatrix());                  // 设置观察矩阵
        shader.setMat4("model", model);                                   // 设置模型矩阵
        shader.setFloat("scale", height_sacle);                           // 设置高度大小
        shader.setVec3("viewPos", camera->Position);                      // 设置世间空间相机位置
        shader.setVec3("light.direction", glm::vec3(1.0f, -1.0f, -0.3f)); //平行光方向
        shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));     // 环境光颜色
        shader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));     // 平行光颜色
        shader.setInt("material.diffuse", 0);                             // 着色器中漫反射贴图的索引
        shader.setInt("height", 1);                                       // 高度贴图的索引

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse); // 绑定贴图到索引0
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, height); // 绑定贴图到索引1

        // draw model
        plane.Draw(shader); // 用shader绘制plane模型

        glfwSwapBuffers(window); // 交换缓冲
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        height_sacle += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        height_sacle -= deltaTime;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}