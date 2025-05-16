#pragma once

#include <ituGL/application/Application.h>
#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/geometry/Mesh.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/geometry/VertexArrayObject.h>
#include <ituGL/shader/ShaderProgram.h>

struct SparkParticle
{
    glm::vec2 position;
    float size;
    float birth;
    float duration;
    Color color;
    glm::vec2 velocity;
};

class FireApplication : public Application
{
public:
    FireApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;

private:
    void InitializeGeometry();
    void InitializeShaders();
    void LoadAndCompileShader(Shader& shader, const char* path);

private:
    Mesh m_quad;
    ShaderProgram m_shaderProgram;

    ShaderProgram::Location m_timeUniform;
    ShaderProgram::Location m_viewProjUniform;
    ShaderProgram::Location m_worldMatrixUniform;

    Camera m_camera;

    void InitializeParticleSystem();
    void EmitParticle(const glm::vec2& pos, float size, float duration, const Color& color, const glm::vec2& velocity);
    // Helper methods for random values
    static float Random01();
    static float RandomRange(float from, float to);
    static glm::vec2 RandomDirection();
    static Color RandomColor();

    // Spark system
    VertexBufferObject m_sparkVBO;
    VertexArrayObject m_sparkVAO;
    ShaderProgram m_sparkShader;
    ShaderProgram::Location m_sparkTimeUniform;
    ShaderProgram::Location m_sparkGravityUniform;

    unsigned int m_sparkCount = 0;
    unsigned int m_sparkCapacity = 1024;
    float m_lastSparkTime = 0.0f;
    float m_sparkInterval = 0.2f;
    float m_lastFpsTime = 0.0f;
    int m_frameCount = 0;
};
