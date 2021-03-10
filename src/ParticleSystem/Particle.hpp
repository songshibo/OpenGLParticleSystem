#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <iomanip>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <custom/shader.h>

#include <ForceField.hpp>
#include <custom/camera.h>
#include <stb_image.h>

const glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
const float targetFPS = 1000.0f;

static const float VBO_quad_data[] = 
{
    //model space pos  //uv
    0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
};

const unsigned int indices[] = {  
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

class Particle
{
    public:
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 speed = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec4 color;
        // life default equal to -1.0 
        //make sure GetUnusedIndex() works first time after initialization
        float size, angle, life, life_max = -1.0f;
        float cameradistance = -1.0f;
        int tex_index;

    bool operator<(const Particle& that) const;
    bool operator<=(const Particle& that) const;
    bool operator>(const Particle& that) const;
    public:
        void CollisionWithPlane(glm::vec3 planeNormal, glm::vec3 planeCenter, glm::vec2 planeSize);
};

class ParticleManager
{
    public:
        Particle *list;
        int lasIndex = 0;
        int maxLength;
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 orientation = glm::vec3(0.0f, 1.0f, 0.0f);
        float bottom_r, c_angle;
        glm::mat4 particle_model;
        bool collision = false;
        bool model_space = false;
        float life_time_min;
        float life_time_max;
        float start_size_min;
        float start_size_max;
        float speed_min;
        float speed_max;
        float bottom_radius;
        float cone_angle ;
        float gravity_scale;
        glm::vec4 *instance_pos_size;
        glm::vec4 *instance_lifetime;

    public:
        ParticleManager(int _len, float _bottom_radius, float _cone_angle, glm::vec3 _pos, glm::vec3 _rot, bool _collision, bool _model_space, float _lmin, float _lmax, float _smin, float _smax, float _vmin, float _vmax, float _gs);
        // Find Particle in Particles List which has not benn used yet
        int GetUnusedIndex();
        void SortParticles();
        void UpdateMatrix();
        float Random1f(float min, float max);
        glm::vec3 GenerateRandomDir();
        glm::vec3 GenerateRandomStartPos();
        void ProcessKeyBoard(glm::vec3 direction, float deltaTime);
        void ParticleGeneration(double delta);
        void ParticleSimulation(int &particleCount, double delta, WindZone *wind, Camera *cam);
};

struct Character
{
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    long int Advance;
};

class CharacterRender
{
    public:
        std::map<GLchar, Character> Characters;
        GLuint tVAO, tVBO;
        Shader *textShader;
    
    public:
        CharacterRender(int width, int height);
        void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
};

Particle* merge(ParticleManager *m1, int len1, ParticleManager *m2, int len2, glm::vec4 *pos_size, glm::vec4 *lifetime);
Particle* merge_one(ParticleManager *m1, int len1, glm::vec4 *pos_size, glm::vec4 *lifetime);
Particle* merge_three(ParticleManager *m1, int len1, ParticleManager *m2, int len2, ParticleManager *m3, int len3, glm::vec4 *pos_size, glm::vec4 *lifetime);
void outputMat4(glm::mat4 m);
void outputVec4(glm::vec4 v);
void outputVec3(glm::vec3 v);
std::string f2s(float f);
GLuint loadTexture(char const * path);

#endif