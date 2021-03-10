#include <Particle.hpp>
#include <custom/model.h>

// Const variables & Global variables
const int Window_Width = 1000;
const int Window_Height = 600;

const glm::vec3 font_color = glm::vec3(0.1f, 0.1f, 0.1f);

// show depth info
bool depth_debug = true;

// smoke particle properties
const int MaxParticles = 500;
const float s_life_time_min = 2.0f;
const float s_life_time_max = 3.5f;
const float s_start_size_min = 1.0f;
const float s_start_size_max = 2.2f;
const float s_speed_min = 2.0f;
const float s_speed_max = 3.0f;
const float s_bottom_radius = 0.5f;
const float s_cone_angle = 45.0f;
const float s_gravity_scale = 0.0f;
const bool s_use_model_space = false;
glm::vec3 particlePos = glm::vec3(-1.0f, 0.8f, 0.0f);
glm::vec3 particleRotation = glm::vec3(0.0f, 0.0f, 0.0f);

const int fl_maxParticles = 100;
const float fl_life_time_min = 1.2f;
const float fl_life_time_max = 2.0f;
const float fl_start_size_min = 1.5f;
const float fl_start_size_max = 2.0f;
const float fl_speed_min = 0.8f;
const float fl_speed_max = 1.0f;
const float fl_bottom_radius = 0.2f;
const float fl_cone_angle = 15.0f;
const float fl_gravity_scale = 0.0f;
const bool fl_use_model_space = false;
glm::vec3 fl_particlePos = glm::vec3(-1.0f, 0.55f, 0.1f);
glm::vec3 fl_particleRotation = glm::vec3(0.0f, 0.0f, 0.0f);

const int f_MaxParticles = 60;
const float f_life_time_min = 1.0f;
const float f_life_time_max = 2.5f;
const float f_start_size_min = 0.1f;
const float f_start_size_max = 0.15f;
const float f_speed_min = 3.0f;
const float f_speed_max = 4.0f;
const float f_bottom_radius = 0.4f;
const float f_cone_angle = 70.0f;
const float f_gravity_scale = 1.0f;
const bool f_use_model_space = false;
glm::vec3 f_particlePos = glm::vec3(-1.0f, 0.3f, 0.0f);
glm::vec3 f_particleRotation = glm::vec3(0.0f, 0.0f, 0.0f);

const char black_smoke[] = "../src/ParticleSystem/textures/Black smoke/blackSmoke00.png";
const char default_particle[] = "../src/ParticleSystem/textures/default_particle.png";
const char flame[] = "../src/ParticleSystem/textures/fire_texture.tga";

Particle *total;

//Wind Zone
//disable wind
// const glm::vec3 wind_scale = glm::vec3(0.0f, 0.0f, 0.0f);
//enable
const glm::vec3 wind_scale = glm::vec3(-2.0f, 1.0f, 1.0f); // higher value higher offsets
const glm::vec3 wind_freq = glm::vec3(1.5f, 0.2f, 0.4f);   // higher value higher frequency

ParticleManager *smoke_manager;
ParticleManager *spark_manager;
ParticleManager *flame_manager;
WindZone *wind;
Camera *camera;
GLFWwindow *window;
CharacterRender *cRender;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 4.0f, 12.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);
glm::mat4 Projection;
const float FoV = 60;
const float zNear = 0.1f;
const float zFar = 100.0f;

//Input
float lastX = Window_Width / 2.0f;
float lastY = Window_Height / 2.0f;
bool firstMouse = true;

//Buffer ID
GLuint VAO, VBO, EBO, TEX1, TEX2, TEX3, instanceVBO, instanceVBO2;
glm::vec4 *instance_pos_size;
glm::vec4 *instance_lifetime;

//FPS calculation
int FPS = 0;
int s_fps = 60;
float deltaSum = 0.0f;

const float skyboxVertices[] = {
    // positions
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, double delta);
void SetUpGlobalStatus();
void ParticleGeneration(double delta);
void ParticleSimulation(int &particleCount, double delta);
void SetUpOpenGLBuffers();
void SetUpTextureBuffers();
void GenerateTextureBuffers();
unsigned int loadCubemap(vector<std::string> faces);

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
    window = glfwCreateWindow(Window_Width, Window_Height, "Particle System", nullptr, nullptr);

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

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse & enable unlimited movement
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // set mouse to the screen center
    glfwSetCursorPos(window, Window_Width / 2, Window_Height / 2);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // NOTICE! if use depth in particle system will cause display error
    // make sure turn off depth mask before render particles
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader particle_instanced_shader("../src/ParticleSystem/shaders/particle_GPU.vert", "../src/ParticleSystem/shaders/particle_GPU.frag");
    Shader plane_shader("../src/ParticleSystem/shaders/plane.vert", "../src/ParticleSystem/shaders/plane.frag");
    Shader light_shader("../src/ParticleSystem/shaders/BlinnPhong.vert", "../src/ParticleSystem/shaders/BlinnPhong.frag");
    Shader screen_shader("../src/ParticleSystem/shaders/screen.vert", "../src/ParticleSystem/shaders/screen.frag");
    Shader skybox_shader("../src/ParticleSystem/shaders/skybox.vert", "../src/ParticleSystem/shaders/skybox.frag");
    particle_instanced_shader.use();
    particle_instanced_shader.setFloat("width", (float)Window_Width);
    particle_instanced_shader.setFloat("height", (float)Window_Height);

    // WindZone & Particle & characterRender Setup
    wind = new WindZone(wind_scale, wind_freq);
    smoke_manager = new ParticleManager(MaxParticles, s_bottom_radius, s_cone_angle, particlePos, particleRotation, false, s_use_model_space, s_life_time_min, s_life_time_max, s_start_size_min, s_start_size_max, s_speed_min, s_speed_max, s_gravity_scale);
    flame_manager = new ParticleManager(fl_maxParticles, fl_bottom_radius, fl_cone_angle, fl_particlePos, fl_particleRotation, false, fl_use_model_space, fl_life_time_min, fl_life_time_max, fl_start_size_min, fl_start_size_max, fl_speed_min, fl_speed_max, fl_gravity_scale);
    spark_manager = new ParticleManager(f_MaxParticles, f_bottom_radius, f_cone_angle, f_particlePos, f_particleRotation, true, f_use_model_space, f_life_time_min, f_life_time_max, f_start_size_min, f_start_size_max, f_speed_min, f_speed_max, f_gravity_scale);
    cRender = new CharacterRender(Window_Width, Window_Height);
    instance_lifetime = new glm::vec4[MaxParticles + f_MaxParticles + fl_maxParticles];
    instance_pos_size = new glm::vec4[MaxParticles + f_MaxParticles + fl_maxParticles];

    // Camera Setup unncomment to use look at function
    //glm::vec3 origin = glm::vec3(0.0f); // this will make sure the camera look at origin point after initialization
    //glm::vec3 towards = glm::normalize(origin - cameraPos);
    // // pitch stands for the angle we're looking up or down
    //float pitch = -glm::degrees(acos(glm::dot(towards, glm::vec3(0.0f, 0.0f, -1.0f))));
    // // yaw stands for the magnitude we're looking to the left or to the right
    //glm::vec3 proj = glm::normalize(towards - glm::dot(glm::vec3(0.0f, -1.0f, 0.0f),towards) * glm::vec3(0.0f, -1.0f, 0.0f));
    //float yaw = -glm::degrees(acos(glm::dot(proj, glm::vec3(1.0f, 0.0f, 0.0f))));
    //camera = new Camera(cameraPos, cameraUp, yaw, pitch);
    camera = new Camera(cameraPos);
    Projection = glm::perspective(glm::radians(FoV), float(Window_Width) / float(Window_Height), zNear, zFar);

    // callback function setup
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VBO_quad_data), VBO_quad_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    // GPU instance, particle position
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    // first generation, no data assigned
    glBufferData(GL_ARRAY_BUFFER, (MaxParticles + f_MaxParticles + fl_maxParticles) * sizeof(glm::vec4), NULL, GL_STREAM_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)0);

    // other data
    glGenBuffers(1, &instanceVBO2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO2);
    glBufferData(GL_ARRAY_BUFFER, (MaxParticles + f_MaxParticles + fl_maxParticles) * sizeof(glm::vec4), NULL, GL_STREAM_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)0);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    // unbind VAO
    glBindVertexArray(0);

    TEX1 = loadTexture(black_smoke);
    TEX2 = loadTexture(default_particle);
    TEX3 = loadTexture(flame);

    Model campfire("../src/ParticleSystem/model/campfire/campfire.obj");
    Model plane("../src/ParticleSystem/model/plane/plane.obj");
    Model barrel("../src/ParticleSystem/model/prop/barrel.obj");
    Model shed("../src/ParticleSystem/model/prop/shed.obj");
    GLuint campfire_diffuse = loadTexture("../src/ParticleSystem/model/campfire/campfire_base.png");
    GLuint campfire_specular = loadTexture("../src/ParticleSystem/model/campfire/campfire_metal.png");
    GLuint plane_diffuse = loadTexture("../src/ParticleSystem/model/plane/Wall01_Diffuse.tga");
    GLuint barrel_diffuse = loadTexture("../src/ParticleSystem/model/prop/prop_barrel_01_a.png");
    GLuint shed_diffuse = loadTexture("../src/ParticleSystem/model/prop/prop_fence_02_a.png");

    // skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    vector<std::string> faces{
        "../src/ParticleSystem/textures/envmap_miramar/miramar_rt.jpg",
        "../src/ParticleSystem/textures/envmap_miramar/miramar_lf.jpg",
        "../src/ParticleSystem/textures/envmap_miramar/miramar_up.jpg",
        "../src/ParticleSystem/textures/envmap_miramar/miramar_dn.jpg",
        "../src/ParticleSystem/textures/envmap_miramar/miramar_ft.jpg",
        "../src/ParticleSystem/textures/envmap_miramar/miramar_bk.jpg"};

    unsigned int cubemapTexture = loadCubemap(faces);

    // frame buffer render depth
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint textureDepthbuffer;
    glGenTextures(1, &textureDepthbuffer);
    glBindTexture(GL_TEXTURE_2D, textureDepthbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthbuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::mat4 campfire_model = glm::mat4(1.0f);
    campfire_model = glm::translate(campfire_model, glm::vec3(-2.0f, 0.0f, 0.0f));
    campfire_model = glm::rotate(campfire_model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    campfire_model = glm::scale(campfire_model, glm::vec3(3.0f, 3.0f, 3.0f));
    glm::vec3 campfire_color = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 plane_model = glm::mat4(1.0f);
    plane_model = glm::translate(plane_model, glm::vec3(0.0f, 0.0f, 0.0f));
    plane_model = glm::scale(plane_model, glm::vec3(2.0f, 2.0f, 2.0f));
    glm::vec3 plane_color = glm::vec3(0.3f, 0.3f, 0.3f);

    glm::mat4 barrel_model = glm::mat4(1.0f);
    barrel_model = glm::translate(barrel_model, glm::vec3(5.0f, 0.0f, 2.5f));
    glm::mat4 barrel_model2 = glm::mat4(1.0f);
    barrel_model2 = glm::translate(barrel_model2, glm::vec3(2.5f, 1.3f, 2.0f));
    barrel_model2 = glm::rotate(barrel_model2, glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    barrel_model2 = glm::rotate(barrel_model2, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 barrel_color = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 shed_model = glm::mat4(1.0f);
    shed_model = glm::translate(shed_model, glm::vec3(2.5f, 0.45f, -2.0f));
    shed_model = glm::rotate(shed_model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 shed_color = glm::vec3(1.0f, 1.0f, 1.0f);

    //get time before loop
    double lastTime = glfwGetTime();
    // Main Loop
    while (glfwWindowShouldClose(window) == false)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // calculate delta time
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        processInput(window, deltaTime);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        glEnable(GL_DEPTH_TEST);

        //Draw static object
        glViewport(0, 0, 1024, 1024);

        light_shader.use();
        light_shader.setMat4("view", camera->GetViewMatrix());
        light_shader.setMat4("projection", Projection);
        light_shader.setVec3("viewPos", camera->Position);
        light_shader.setVec3("light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        light_shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        light_shader.setVec3("light.diffuse", glm::vec3(0.52f, 0.5f, 0.5f));
        light_shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        light_shader.setVec3("pointLights[0].position", fl_particlePos + glm::vec3(0.0f, 1.0f * (sin(currentTime * 4.0f) * 0.5f + 0.5f), 0.0f));
        light_shader.setVec3("pointLights[0].ambient", 0.2f, 0.2f, 0.2f);
        light_shader.setVec3("pointLights[0].diffuse", 1.0f, 0.83f, 0.38f);
        light_shader.setVec3("pointLights[0].specular", 1.0f, 0.83f, 0.38f);
        light_shader.setFloat("pointLights[0].constant", 1.0f);
        light_shader.setFloat("pointLights[0].linear", 0.09f);
        light_shader.setFloat("pointLights[0].quadratic", 0.032);
        light_shader.setFloat("material.shininess", 255.0f);
        light_shader.setInt("material.diffuse", 0);
        light_shader.setInt("material.specular", 1);

        light_shader.setMat4("model", campfire_model);
        light_shader.setVec3("base_color", campfire_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, campfire_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, campfire_specular);
        campfire.Draw(light_shader);

        light_shader.setMat4("model", plane_model);
        light_shader.setVec3("base_color", plane_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, plane_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        plane.Draw(light_shader);

        light_shader.setMat4("model", barrel_model);
        light_shader.setVec3("base_color", barrel_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, barrel_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        barrel.Draw(light_shader);
        light_shader.setMat4("model", barrel_model2);
        barrel.Draw(light_shader);

        light_shader.setMat4("model", shed_model);
        light_shader.setVec3("base_color", shed_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shed_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        shed.Draw(light_shader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // reset the view port to desired size
        // get actual size in case of Retina diplay
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        light_shader.setMat4("model", campfire_model);
        light_shader.setVec3("base_color", campfire_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, campfire_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, campfire_specular);
        campfire.Draw(light_shader);

        light_shader.setMat4("model", plane_model);
        light_shader.setVec3("base_color", plane_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, plane_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        plane.Draw(light_shader);

        light_shader.setMat4("model", barrel_model);
        light_shader.setVec3("base_color", barrel_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, barrel_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        barrel.Draw(light_shader);
        light_shader.setMat4("model", barrel_model2);
        barrel.Draw(light_shader);

        light_shader.setMat4("model", shed_model);
        light_shader.setVec3("base_color", shed_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shed_diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        shed.Draw(light_shader);

        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        skybox_shader.use();
        glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
        skybox_shader.setMat4("view", view);
        skybox_shader.setMat4("projection", Projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        //Generation and Simulation
        int particleCount = 0;
        smoke_manager->ParticleGeneration(deltaTime);
        smoke_manager->ParticleSimulation(particleCount, deltaTime, wind, camera);

        int sparkCount = 0;
        spark_manager->ParticleGeneration(deltaTime);
        spark_manager->ParticleSimulation(sparkCount, deltaTime, wind, camera);
        // total = merge(smoke_manager, particleCount, spark_manager, sparkCount, instance_pos_size, instance_lifetime);

        int flameCount = 0;
        flame_manager->ParticleGeneration(deltaTime);
        flame_manager->ParticleSimulation(flameCount, deltaTime, wind, camera);
        // this will merge all particle together and orderd by camera distance
        total = merge_three(smoke_manager, particleCount, spark_manager, sparkCount, flame_manager, flameCount, instance_pos_size, instance_lifetime);

        particle_instanced_shader.use();
        particle_instanced_shader.setMat4("view", camera->GetViewMatrix());
        particle_instanced_shader.setMat4("projection", Projection);
        particle_instanced_shader.setVec3("camera_pos", camera->Position);
        particle_instanced_shader.setFloat("power", 1.5f); // power factor
        particle_instanced_shader.setInt("MainTex", 0);
        particle_instanced_shader.setInt("MainTex2", 1);
        particle_instanced_shader.setInt("MainTex3", 2);
        particle_instanced_shader.setInt("depth", 3);
        // smoke color gradian
        particle_instanced_shader.setVec4("particleColor_s", glm::vec4(0.7f, 0.7f, 0.7f, 0.05f));
        particle_instanced_shader.setVec4("particleColor_e", glm::vec4(1.3f, 1.3f, 1.3f, 0.3f));
        particle_instanced_shader.setVec4("particleColor2_s", glm::vec4(1.1f, 0.83f, 0.48f, 1.0f));
        particle_instanced_shader.setVec4("particleColor2_e", glm::vec4(1.0f, 0.83f, 0.38f, 1.0f));
        particle_instanced_shader.setVec4("particleColor3_s", glm::vec4(1.0f, 0.83f, 0.38f, 0.5f));
        particle_instanced_shader.setVec4("particleColor3_e", glm::vec4(1.0f, 0.1f, 0.0f, 0.1f));
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TEX1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, TEX2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, TEX3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, textureDepthbuffer);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, (MaxParticles + f_MaxParticles + fl_maxParticles) * sizeof(glm::vec4), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf.
        glBufferSubData(GL_ARRAY_BUFFER, 0, (particleCount + sparkCount + flameCount) * sizeof(glm::vec4), instance_pos_size);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO2);
        glBufferData(GL_ARRAY_BUFFER, (MaxParticles + f_MaxParticles + fl_maxParticles) * sizeof(glm::vec4), NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, (particleCount + sparkCount + flameCount) * sizeof(glm::vec4), instance_lifetime);

        // do not write transparent object into depth texture
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, particleCount + sparkCount + flameCount);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // display a small rectangular on right top to show the depth buffer
        if (depth_debug)
        {
            screen_shader.use();
            screen_shader.setInt("depth", 0);
            glBindVertexArray(VAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureDepthbuffer);
            glDisable(GL_DEPTH_TEST);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glEnable(GL_DEPTH_TEST);
        }

        //FPS
        deltaSum += deltaTime;
        FPS++;
        cRender->RenderText("Current FPS:" + f2s(s_fps), 10.0f, 580.0f, 0.4f, font_color);
        cRender->RenderText("Smoke System Parameters :", 10.0f, 540.0f, 0.35f, font_color);
        cRender->RenderText("Max Number:" + f2s(MaxParticles) + "  Current Alive:" + f2s(particleCount), 10.0f, 520.0f, 0.25f, font_color);
        cRender->RenderText("Gravity Scale:" + f2s(s_gravity_scale), 10.0f, 500.0f, 0.25f, font_color);
        cRender->RenderText("Cone Angle:" + f2s(s_cone_angle), 10.0f, 480.0f, 0.25f, font_color);
        cRender->RenderText("Size Range:" + f2s(s_start_size_min) + "~" + f2s(s_start_size_max), 10.0f, 460.0f, 0.25f, font_color);
        cRender->RenderText("Lifecycle Range:" + f2s(s_life_time_min) + "~" + f2s(s_life_time_max), 10.0f, 440.0f, 0.25f, font_color);

        cRender->RenderText("Spark System Parameters :", 10.0f, 400.0f, 0.35f, font_color);
        cRender->RenderText("Max Number:" + f2s(f_MaxParticles) + "  Current Alive:" + f2s(sparkCount), 10.0f, 380.0f, 0.25f, font_color);
        cRender->RenderText("Gravity Scale:" + f2s(f_gravity_scale), 10.0f, 360.0f, 0.25f, font_color);
        cRender->RenderText("Cone Angle:" + f2s(f_cone_angle), 10.0f, 340.0f, 0.25f, font_color);
        cRender->RenderText("Size Range:" + f2s(f_start_size_min) + "~" + f2s(f_start_size_max), 10.0f, 320.0f, 0.25f, font_color);
        cRender->RenderText("Lifecycle Range:" + f2s(f_life_time_min) + "~" + f2s(f_life_time_max), 10.0f, 300.0f, 0.25f, font_color);

        cRender->RenderText("Flame System Parameters :", 10.0f, 260.0f, 0.35f, font_color);
        cRender->RenderText("Max Number:" + f2s(fl_maxParticles) + "  Current Alive:" + f2s(flameCount), 10.0f, 240.0f, 0.25f, font_color);
        cRender->RenderText("Gravity Scale:" + f2s(fl_gravity_scale), 10.0f, 220.0f, 0.25f, font_color);
        cRender->RenderText("Cone Angle:" + f2s(fl_cone_angle), 10.0f, 200.0f, 0.25f, font_color);
        cRender->RenderText("Size Range:" + f2s(fl_start_size_min) + "~" + f2s(fl_start_size_max), 10.0f, 180.0f, 0.25f, font_color);
        cRender->RenderText("Lifecycle Range:" + f2s(fl_life_time_min) + "~" + f2s(fl_life_time_max), 10.0f, 160.0f, 0.25f, font_color);

        if (deltaSum > 0.999f)
        {
            s_fps = FPS;
            FPS = 0;
            deltaSum = 0.0f;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete smoke_manager;
    delete spark_manager;
    delete wind;
    delete camera;
    glDeleteProgram(particle_instanced_shader.ID);
    glDeleteProgram(plane_shader.ID);
    glDeleteProgram(light_shader.ID);
    glDeleteProgram(skybox_shader.ID);
    glDeleteProgram(screen_shader.ID);
    glfwTerminate();
    return 0;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(1);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
};

void processInput(GLFWwindow *window, double delta)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, delta);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, delta);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, delta);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, delta);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        smoke_manager->ProcessKeyBoard(glm::vec3(-1.0f, 0.0f, 0.0f), delta);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        smoke_manager->ProcessKeyBoard(glm::vec3(1.0f, 0.0f, 0.0f), delta);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        smoke_manager->ProcessKeyBoard(glm::vec3(0.0f, 0.0f, -1.0f), delta);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        smoke_manager->ProcessKeyBoard(glm::vec3(0.0f, 0.0f, 1.0f), delta);
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