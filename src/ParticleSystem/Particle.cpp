#include "Particle.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

bool Particle::operator<(const Particle &that) const
{
    return this->cameradistance > that.cameradistance;
};

bool Particle::operator<=(const Particle &that) const
{
    return this->cameradistance >= that.cameradistance;
};

bool Particle::operator>(const Particle &that) const
{
    return this->cameradistance < that.cameradistance;
};

void Particle::CollisionWithPlane(glm::vec3 planeNormal, glm::vec3 planeCenter, glm::vec2 planeSize)
{
    //In Plane Range
    bool xInRange = (this->pos.x < (planeCenter.x + planeSize.x)) && (this->pos.x > (planeCenter.x - planeSize.x));
    bool zInRange = (this->pos.z < (planeCenter.z + planeSize.y)) && (this->pos.z > (planeCenter.z - planeSize.y));
    if (xInRange && zInRange)
    {
        // Detection
        if ((this->pos.y - planeCenter.y) > 0.01f && abs(this->pos.y - planeCenter.y) < this->size / 2)
        {
            float speed = glm::length(this->speed);
            glm::vec3 dir = glm::normalize(this->speed);
            glm::vec3 reflect = dir - 2 * (glm::dot(dir, planeNormal)) * planeNormal;
            this->speed = reflect * speed * 0.55f;
        }
    }
};

ParticleManager::ParticleManager(int _len, float _bottom_radius, float _cone_angle, glm::vec3 _pos, glm::vec3 _rot, bool _collision, bool _model_space, float _lmin, float _lmax, float _smin, float _smax, float _vmin, float _vmax, float _gs)
{
    list = new Particle[_len];
    maxLength = _len;
    bottom_r = _bottom_radius;
    c_angle = _cone_angle;
    pos = _pos;
    rotation = _rot;
    life_time_min = _lmin;
    life_time_max = _lmax;
    start_size_min = _smin;
    start_size_max = _smax;
    speed_min = _vmin;
    speed_max = _vmax;
    gravity_scale = _gs;
    collision = _collision;
    model_space = _model_space;
    UpdateMatrix();
};

void ParticleManager::UpdateMatrix()
{
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, this->pos);
    m = glm::rotate(m, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m = glm::rotate(m, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m = glm::rotate(m, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    this->particle_model = m;
};

int ParticleManager::GetUnusedIndex()
{
    //check lastIndex -> maxlength has any unused particles
    for (int i = this->lasIndex; i < this->maxLength; ++i)
    {
        if (this->list[i].life <= 0)
        {
            this->lasIndex = i;
            return i;
        }
    }

    // check 0 -> lastIndex has any unused particles
    for (int i = 0; i < this->lasIndex; ++i)
    {
        if (this->list[i].life <= 0)
        {
            this->lasIndex = i;
            return i;
        }
    }

    return -1; // all particles are used, do not generate;
};

void ParticleManager::SortParticles()
{
    std::sort(this->list, &(this->list[this->maxLength]));
};

float ParticleManager::Random1f(float min, float max)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = max - min;
    return min + (diff * random);
};

glm::vec3 ParticleManager::GenerateRandomDir()
{
    float r_angle = this->Random1f(glm::radians(0.0f), glm::radians(this->c_angle));
    float xz_angle = this->Random1f(glm::radians(0.0f), glm::radians(360.0f));
    glm::vec3 result = glm::vec3(
        sin(r_angle) * sin(xz_angle),
        cos(r_angle),
        sin(r_angle) * cos(xz_angle));
    return glm::normalize(result);
};

glm::vec3 ParticleManager::GenerateRandomStartPos()
{
    return this->pos + glm::vec3(
                           this->Random1f(-this->bottom_r, this->bottom_r),
                           0,
                           this->Random1f(-this->bottom_r, this->bottom_r));
};

void ParticleManager::ProcessKeyBoard(glm::vec3 direction, float deltaTime)
{
    float velocity = 2.5f * deltaTime;
    this->pos += direction * velocity;
    this->UpdateMatrix();
};

void ParticleManager::ParticleGeneration(double delta)
{
    // Limit to target fps
    int desire = (int)(ceil(delta * maxLength));
    int limit = (int)((1 / targetFPS) * maxLength);
    int newParticles = desire >= limit ? desire : limit;

    for (int i = 0; i < newParticles; i++)
    {
        int Index = GetUnusedIndex();
        if (Index < 0)
        {
            // means all particles are alive, do not generate new particle
            continue;
        }
        // generate lifetime randomly
        list[Index].life = Random1f(life_time_min, life_time_max);
        list[Index].life_max = list[Index].life;
        // generate position randomly
        glm::vec3 pos = GenerateRandomStartPos();
        glm::vec4 w_pos = particle_model * glm::vec4(pos.x, pos.y, pos.z, 1.0f);
        list[Index].pos = glm::vec3(w_pos.x, w_pos.y, w_pos.z);
        // generate speed randomly
        glm::vec3 dir = GenerateRandomDir();
        glm::vec4 w_dir = particle_model * glm::vec4(dir.x, dir.y, dir.z, 0.0f);
        list[Index].speed = glm::vec3(w_dir.x, w_dir.y, w_dir.z) * Random1f(speed_min, speed_max);
        // generate size randomly
        list[Index].size = Random1f(start_size_min, start_size_max);
    }
}

void ParticleManager::ParticleSimulation(int &particleCount, double delta, WindZone *wind, Camera *cam)
{
    glm::vec4 relative_gravity = particle_model * glm::vec4(gravity.x, gravity.y, gravity.z, 0.0f);
    for (int i = 0; i < maxLength; ++i)
    {
        // ref to simplify code
        Particle &p = list[i];
        if (p.life > 0.0f)
        {
            p.life -= delta;
            if (p.life > 0.0f)
            {
                if (model_space)
                {
                    p.speed += glm::vec3(relative_gravity.x, relative_gravity.y, relative_gravity.z) * float(delta * 0.5f) * gravity_scale;
                }
                else
                {
                    p.speed += gravity * (float)delta * 0.5f * gravity_scale;
                }
                p.speed += wind->GenerateWind(glfwGetTime()) * float(delta);

                if (collision)
                {
                    p.CollisionWithPlane(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(8.0f, 8.0f));
                }

                p.pos += p.speed * (float)delta;

                // world space distance
                p.cameradistance = glm::length(p.pos - cam->Position);
                // alive particles number
                particleCount++;
            }
            else
            {
                // Particles died
                p.cameradistance = -1.0f;
            }
        }
    }
    SortParticles();
};

CharacterRender::CharacterRender(int width, int height)
{
    textShader = new Shader("../src/ParticleSystem/shaders/text.vert", "../src/ParticleSystem/shaders/text.frag");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
    textShader->use();
    textShader->setMat4("projection", projection);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Libaray" << std::endl;

    FT_Face face;
    if (FT_New_Face(ft, "../src/ParticleSystem/fonts/OpenSans-Semibold.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x};
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    //Configure VAO/VBO
    glGenVertexArrays(1, &tVAO);
    glGenBuffers(1, &tVBO);
    glBindVertexArray(tVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void CharacterRender::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    textShader->use();
    textShader->setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(tVAO);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            {xpos, ypos + h, 0.0, 0.0},
            {xpos, ypos, 0.0, 1.0},
            {xpos + w, ypos, 1.0, 1.0},

            {xpos, ypos + h, 0.0, 0.0},
            {xpos + w, ypos, 1.0, 1.0},
            {xpos + w, ypos + h, 1.0, 0.0}};

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, tVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
};

Particle *merge_one(ParticleManager *m1, int len1, glm::vec4 *pos_size, glm::vec4 *lifetime)
{
    Particle *list = new Particle[len1];
    // int j = 0, k = 0;
    int i = 0;
    for (; i < len1; ++i)
    {
        list[i] = m1->list[i];
        list[i].tex_index = 2;
    }

    std::sort(list, &list[len1 - 1]);
    for (i = 0; i < len1; ++i)
    {
        pos_size[i] = glm::vec4(list[i].pos.x, list[i].pos.y, list[i].pos.z, list[i].size);
        lifetime[i] = glm::vec4(list[i].life / list[i].life_max, list[i].tex_index, 0.0f, 0.0f);
    }
    return list;
};

Particle *merge(ParticleManager *m1, int len1, ParticleManager *m2, int len2, glm::vec4 *pos_size, glm::vec4 *lifetime)
{
    Particle *list = new Particle[len1 + len2];
    // int j = 0, k = 0;
    int i = 0;
    for (; i < len1; ++i)
    {
        list[i] = m1->list[i];
        list[i].tex_index = 1;
    }
    for (; i < (len1 + len2); ++i)
    {
        list[i] = m2->list[i - len1];
        list[i].tex_index = 2;
    }

    std::sort(list, &list[len1 + len2 - 1]);
    for (i = 0; i < (len1 + len2); ++i)
    {
        pos_size[i] = glm::vec4(list[i].pos.x, list[i].pos.y, list[i].pos.z, list[i].size);
        lifetime[i] = glm::vec4(list[i].life / list[i].life_max, list[i].tex_index, 0.0f, 0.0f);
    }

    // for(int i = 0; i < (len1 + len2); ++i)
    // {
    //     if (j == (len1 - 1))
    //     {
    //         list[i] = m2->list[k];
    //         k++;
    //         lifetime[i] = glm::vec4(list[i].life / list[i].life_max, 2.0f, 0.0f, 0.0f);
    //     }
    //     else if(k == (len2 - 1))
    //     {
    //         list[i] = m1->list[j];
    //         j++;
    //         lifetime[i] = glm::vec4(list[i].life / list[i].life_max, 1.0f, 0.0f, 0.0f);
    //     }
    //     else
    //     {
    //         if(m1->list[j] < m2->list[k])
    //         {
    //             list[i] = m1->list[j];
    //             j++;
    //             lifetime[i] = glm::vec4(list[i].life / list[i].life_max, 1.0f, 0.0f, 0.0f);
    //         }else
    //         {
    //             list[i] = m2->list[k];
    //             k++;
    //             lifetime[i] = glm::vec4(list[i].life / list[i].life_max, 2.0f, 0.0f, 0.0f);
    //         }
    //     }
    // }
    return list;
};

Particle *merge_three(ParticleManager *m1, int len1, ParticleManager *m2, int len2, ParticleManager *m3, int len3, glm::vec4 *pos_size, glm::vec4 *lifetime)
{
    Particle *list = new Particle[len1 + len2 + len3];
    // int j = 0, k = 0;
    int i = 0;
    for (; i < len1; ++i)
    {
        list[i] = m1->list[i];
        list[i].tex_index = 1;
    }
    for (; i < (len1 + len2); ++i)
    {
        list[i] = m2->list[i - len1];
        list[i].tex_index = 2;
    }

    for (; i < (len1 + len2 + len3); ++i)
    {
        list[i] = m3->list[i - len1 - len2];
        list[i].tex_index = 3;
    }

    std::sort(list, &list[len1 + len2 + len3 - 1]);
    for (i = 0; i < (len1 + len2 + len3); ++i)
    {
        pos_size[i] = glm::vec4(list[i].pos.x, list[i].pos.y, list[i].pos.z, list[i].size);
        lifetime[i] = glm::vec4(list[i].life / list[i].life_max, list[i].tex_index, 0.0f, 0.0f);
    }

    return list;
};

void outputMat4(glm::mat4 m)
{
    std::cout << "(" << m[0].x << "," << m[0].y << "," << m[0].z << "," << m[0].w << std::endl;
    std::cout << m[1].x << "," << m[1].y << "," << m[1].z << "," << m[1].w << std::endl;
    std::cout << m[2].x << "," << m[2].y << "," << m[2].z << "," << m[2].w << std::endl;
    std::cout << m[3].x << "," << m[3].y << "," << m[3].z << "," << m[3].w << ")" << std::endl;
};

void outputVec4(glm::vec4 v)
{
    std::cout << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")" << std::endl;
};
void outputVec3(glm::vec3 v)
{
    std::cout << "(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
};

std::string f2s(float f)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(1) << f;
    return stream.str();
};

GLuint loadTexture(char const *path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    stbi_set_flip_vertically_on_load(true);
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
};