#include "FireApplication.h"
#include <ituGL/shader/Shader.h>
#include <ituGL/geometry/VertexFormat.h>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <numbers>

FireApplication::FireApplication()
    : Application(1024, 1024, "Fire Shader"), m_timeUniform(-1), m_viewProjUniform(-1), m_worldMatrixUniform(-1)
{
}

void FireApplication::Initialize()
{
    Application::Initialize();
    InitializeParticleSystem();
    InitializeGeometry();
    InitializeShaders();
}

void FireApplication::Update()
{
    Application::Update();

    // Calculates frames per second
    m_frameCount++;
    float now = GetCurrentTime();
    if (now - m_lastFpsTime >= 1.0f)
    {
        std::cout << "FPS: " << m_frameCount << "\n";
        m_frameCount = 0;
        m_lastFpsTime = now;
    }

    int width, height;
    GetMainWindow().GetDimensions(width, height);
    float aspect = static_cast<float>(width) / height;
    m_camera.SetPerspectiveProjectionMatrix(std::numbers::pi_v<float> / 3.0f, aspect, 0.1f, 10.0f);
    m_camera.SetViewMatrix(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0));
    float currentTime = GetCurrentTime();

    // Emit spark every 0.2 seconds
    if (currentTime - m_lastSparkTime >= m_sparkInterval)
    {
        glm::vec2 pos = glm::vec2(RandomRange(-0.3f, 0.3f), RandomRange(-0.7f, 0.0f));
        float size = RandomRange(3.0f, 7.0f);
        float dur = RandomRange(0.5f, 1.0f);
        Color c(1.0f, RandomRange(0.3f, 0.6f), 0.1f);
        glm::vec2 vel(RandomRange(-0.1f, 0.1f), RandomRange(0.5f, 1.5f));
        EmitParticle(pos, size, dur, c, vel);

        m_lastSparkTime = currentTime; // Reset timer
    }

    // Emit smoke particle every frame
    glm::vec2 smokePos = glm::vec2(RandomRange(-0.3f, 0.3f), -0.7f);
    float smokeSize = RandomRange(90.0f, 100.0f);
    float smokeDuration = RandomRange(8.0f, 10.0f);
    Color smokeColor = Color(0.1f, 0.1f, 0.1f, 0.1f);
    glm::vec2 smokeVelocity = glm::vec2(RandomRange(-0.02f, 0.02f), RandomRange(0.3f, 0.5f));
    EmitParticle(smokePos, smokeSize, smokeDuration, smokeColor, smokeVelocity);
}

void FireApplication::Render()
{
    // Clear screen to black
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f), true, 1.0f);
    m_shaderProgram.Use();

    m_shaderProgram.SetUniform(m_timeUniform, GetCurrentTime());
    m_shaderProgram.SetUniform(m_viewProjUniform, m_camera.GetViewProjectionMatrix());
    m_shaderProgram.SetUniform(m_worldMatrixUniform, glm::mat4(1.0f));

    // Draw flame quad
    m_quad.DrawSubmesh(0);

    m_sparkShader.Use();
    m_sparkShader.SetUniform(m_sparkTimeUniform, GetCurrentTime());
    m_sparkShader.SetUniform(m_sparkGravityUniform, -1.0f);

    m_sparkVAO.Bind();

    // Draw particles
    glDrawArrays(GL_POINTS, 0, std::min(m_sparkCount, m_sparkCapacity));

    Application::Render();
}

void FireApplication::InitializeGeometry()
{
    struct Vertex { glm::vec3 pos; glm::vec2 uv; };
    std::vector<Vertex> vertices = {
        {{-1, -1, 0}, {0, 0}},
        {{ 1, -1, 0}, {1, 0}},
        {{ 1,  1, 0}, {1, 1}},
        {{-1,  1, 0}, {0, 1}},
    };
    std::vector<unsigned short> indices = { 0, 1, 2, 0, 2, 3 };

    VertexFormat format;
    format.AddVertexAttribute<float>(3);  // position
    format.AddVertexAttribute<float>(2);  // UV

    m_quad.AddSubmesh<Vertex, unsigned short, VertexFormat::LayoutIterator>(
        Drawcall::Primitive::Triangles, vertices, indices,
        format.LayoutBegin(vertices.size(), true), format.LayoutEnd()
    );
}

void FireApplication::InitializeParticleSystem()
{
    m_sparkVBO.Bind();
    m_sparkVBO.AllocateData(m_sparkCapacity * sizeof(SparkParticle), BufferObject::Usage::DynamicDraw);

    m_sparkVAO.Bind();

    GLsizei stride = sizeof(SparkParticle);
    GLint offset = 0;
    GLuint location = 0;

    std::array<VertexAttribute, 6> attributes = {
        VertexAttribute(Data::Type::Float, 2), // position
        VertexAttribute(Data::Type::Float, 1), // size
        VertexAttribute(Data::Type::Float, 1), // birth
        VertexAttribute(Data::Type::Float, 1), // duration
        VertexAttribute(Data::Type::Float, 4), // color
        VertexAttribute(Data::Type::Float, 2), // velocity
    };

    for (const auto& attr : attributes)
    {
        m_sparkVAO.SetAttribute(location++, attr, offset, stride);
        offset += attr.GetSize();
    }

    VertexArrayObject::Unbind();
    VertexBufferObject::Unbind();

    // Load shaders
    Shader vs(Shader::VertexShader);
    LoadAndCompileShader(vs, "shaders/particles.vert");
    Shader fs(Shader::FragmentShader);
    LoadAndCompileShader(fs, "shaders/particles.frag");

    m_sparkShader.Build(vs, fs);
    m_sparkTimeUniform = m_sparkShader.GetUniformLocation("CurrentTime");
    m_sparkGravityUniform = m_sparkShader.GetUniformLocation("Gravity");

    GetDevice().EnableFeature(GL_PROGRAM_POINT_SIZE);
    GetDevice().EnableFeature(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void FireApplication::EmitParticle(const glm::vec2& position, float size, float duration, const Color& color, const glm::vec2& velocity)
{
    SparkParticle p{ position, size, GetCurrentTime(), duration, color, velocity };

    m_sparkVBO.Bind();
    int offset = (m_sparkCount % m_sparkCapacity) * sizeof(SparkParticle);
    m_sparkVBO.UpdateData(std::span(&p, 1), offset);
    VertexBufferObject::Unbind();

    ++m_sparkCount;
}


void FireApplication::InitializeShaders()
{
    Shader vs(Shader::VertexShader);
    LoadAndCompileShader(vs, "shaders/fire.vert");

    Shader fs(Shader::FragmentShader);
    LoadAndCompileShader(fs, "shaders/fire.frag");

    if (!m_shaderProgram.Build(vs, fs)) {
        std::cerr << "Shader linking failed\n";
        return;
    }

    m_timeUniform = m_shaderProgram.GetUniformLocation("Time");
    m_viewProjUniform = m_shaderProgram.GetUniformLocation("ViewProjMatrix");
    m_worldMatrixUniform = m_shaderProgram.GetUniformLocation("WorldMatrix");
}

void FireApplication::LoadAndCompileShader(Shader& shader, const char* path)
{
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Can't open shader: " << path << "\n";
        return;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    shader.SetSource(ss.str().c_str());

    if (!shader.Compile()) {
        std::array<char, 256> errors{};
        shader.GetCompilationErrors(errors);
        std::cerr << "Error compiling " << path << ":\n" << errors.data() << "\n";
    }
}

// Helper functions for random number generation

float FireApplication::Random01()
{
    return static_cast<float>(rand()) / RAND_MAX;
}

float FireApplication::RandomRange(float from, float to)
{
    return Random01() * (to - from) + from;
}

glm::vec2 FireApplication::RandomDirection()
{
    return glm::normalize(glm::vec2(Random01() - 0.5f, Random01() - 0.5f));
}

Color FireApplication::RandomColor()
{
    return Color(Random01(), Random01(), Random01());
}
