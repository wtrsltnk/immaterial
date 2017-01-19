
#include "geometry.h"
#include "shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Geometry::Geometry()
    : _shader(Geometry::shader()), _vao(0), _vbo(0), _mode(GL_TRIANGLES), _vertexCount(0), _color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{ }

void Geometry::init()
{
    if (this->_vao == 0)
        glGenVertexArrays(1, &this->_vao);
    glBindVertexArray(this->_vao);

    if (this->_vbo == 0)
        glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);

    auto vertexAttrib = glGetAttribLocation(this->_shader, "vertex");
    glVertexAttribPointer(GLuint(vertexAttrib), 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), 0);
    glEnableVertexAttribArray(GLuint(vertexAttrib));

    auto normalAttrib = glGetAttribLocation(this->_shader, "normal");
    glVertexAttribPointer(GLuint(normalAttrib), 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), reinterpret_cast<const GLvoid*>(sizeof(float) * 3));
    glEnableVertexAttribArray(GLuint(normalAttrib));

    auto uvAttrib = glGetAttribLocation(this->_shader, "uv");
    glVertexAttribPointer(GLuint(uvAttrib), 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), reinterpret_cast<const GLvoid*>(sizeof(float) * 6));
    glEnableVertexAttribArray(GLuint(uvAttrib));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Geometry::~Geometry()
{
    // TODO : cleanup vao end vbo
}

void Geometry::setShader(GLuint shader)
{
    this->_shader = shader;
}

void Geometry::setMode(GLenum mode)
{
    this->_mode = mode;
}

void Geometry::setVertices(const std::vector<tVertex>& vertices)
{
    this->_vertexCount = vertices.size();
    glBindVertexArray(this->_vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);

    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(tVertex)), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertices.size() * sizeof(tVertex)), reinterpret_cast<const GLvoid*>(&vertices[0]));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Geometry::setFaces(const std::vector<tFace>& faces)
{
    this->_faces = faces;
}

void Geometry::setColor(const glm::vec4& color)
{
    this->_color = color;
}

void Geometry::render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
{
    glUseProgram(this->_shader);
    glUniformMatrix4fv(Geometry::projectionUniform(this->_shader), 1, false, glm::value_ptr(projection));
    glUniformMatrix4fv(Geometry::viewUniform(this->_shader), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(Geometry::modelUniform(this->_shader), 1, false, glm::value_ptr(model));
    glUniform4fv(Geometry::colorUniform(this->_shader), 1, glm::value_ptr(this->_color));

    glBindVertexArray(this->_vao);
    if (!this->_faces.empty())
    {
        for (auto f : this->_faces)
            glDrawArrays(this->_mode, f._start, f._count);
    }
    else
        glDrawArrays(this->_mode, 0, this->_vertexCount);
    glBindVertexArray(0);
}

Geometry* Geometry::createBox(const glm::vec3& size, const glm::vec3& offset)
{
    std::vector<tVertex> vertices = {
        // Front Face
        { {-size.x, -size.y,  size.z }, {  0.0f,  0.0f,  1.0f}, { 0.0f, 0.0f } }, // Bottom Left Of The Texture and Quad
        { { size.x, -size.y,  size.z }, {  0.0f,  0.0f,  1.0f}, { -1.0f, 0.0f } }, // Bottom Right Of The Texture and Quad
        { { size.x,  size.y,  size.z }, {  0.0f,  0.0f,  1.0f}, { -1.0f, 1.0f } }, // Top Right Of The Texture and Quad
        { {-size.x,  size.y,  size.z }, {  0.0f,  0.0f,  1.0f}, { 0.0f, 1.0f } }, // Top Left Of The Texture and Quad
        // Back Face
        { {-size.x, -size.y, -size.z }, {  0.0f,  0.0f, -1.0f}, { 1.0f, 0.0f } }, // Bottom Right Of The Texture and Quad
        { {-size.x,  size.y, -size.z }, {  0.0f,  0.0f, -1.0f}, { 1.0f, 1.0f } }, // Top Right Of The Texture and Quad
        { { size.x,  size.y, -size.z }, {  0.0f,  0.0f, -1.0f}, { 0.0f, 1.0f } }, // Top Left Of The Texture and Quad
        { { size.x, -size.y, -size.z }, {  0.0f,  0.0f, -1.0f}, { 0.0f, 0.0f } }, // Bottom Left Of The Texture and Quad
        // Top Face
        { {-size.x,  size.y, -size.z }, {  0.0f,  1.0f,  0.0f}, { 0.0f, 1.0f } }, // Top Left Of The Texture and Quad
        { {-size.x,  size.y,  size.z }, {  0.0f,  1.0f,  0.0f}, { 0.0f, 0.0f } }, // Bottom Left Of The Texture and Quad
        { { size.x,  size.y,  size.z }, {  0.0f,  1.0f,  0.0f}, { 1.0f, 0.0f } }, // Bottom Right Of The Texture and Quad
        { { size.x,  size.y, -size.z }, {  0.0f,  1.0f,  0.0f}, { 1.0f, 1.0f } }, // Top Right Of The Texture and Quad
        // Bottom Face
        { {-size.x, -size.y, -size.z }, {  0.0f, -1.0f,  0.0f}, { 1.0f, 1.0f } }, // Top Right Of The Texture and Quad
        { { size.x, -size.y, -size.z }, {  0.0f, -1.0f,  0.0f}, { 0.0f, 1.0f } }, // Top Left Of The Texture and Quad
        { { size.x, -size.y,  size.z }, {  0.0f, -1.0f,  0.0f}, { 0.0f, 0.0f } }, // Bottom Left Of The Texture and Quad
        { {-size.x, -size.y,  size.z }, {  0.0f, -1.0f,  0.0f}, { 1.0f, 0.0f } }, // Bottom Right Of The Texture and Quad
        // Right face
        { { size.x, -size.y, -size.z }, {  1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f } }, // Bottom Right Of The Texture and Quad
        { { size.x,  size.y, -size.z }, {  1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f } }, // Top Right Of The Texture and Quad
        { { size.x,  size.y,  size.z }, {  1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f } }, // Top Left Of The Texture and Quad
        { { size.x, -size.y,  size.z }, {  1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f } }, // Bottom Left Of The Texture and Quad
        // Left Face
        { {-size.x, -size.y, -size.z }, { -1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f } }, // Bottom Left Of The Texture and Quad
        { {-size.x, -size.y,  size.z }, { -1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f } }, // Bottom Right Of The Texture and Quad
        { {-size.x,  size.y,  size.z }, { -1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f } }, // Top Right Of The Texture and Quad
        { {-size.x,  size.y, -size.z }, { -1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f } }  // Top Left Of The Texture and Quad
    };

    std::vector<tFace> faces = {
        {  0, 4 }, {  4, 4 }, {  8, 4 },
        { 12, 4 }, { 16, 4 }, { 20, 4 }
    };

    for (auto v : vertices) {
        v.pos[0] += offset.x;
        v.pos[1] += offset.y;
        v.pos[2] += offset.z;
    }

    auto result = new Geometry();

    result->init();
    result->setMode(GL_QUADS);
    result->setVertices(vertices);
    result->setFaces(faces);

    return result;
}

Geometry* Geometry::createPlane(const glm::vec3& size, const glm::vec3& offset)
{
    std::vector<tVertex> vertices = {
        // Front Face
        { {-size.x, -size.y,  size.z }, {  0.0f,  0.0f,  1.0f}, { 0.0f, 0.0f } }, // Bottom Left Of The Texture and Quad
        { { size.x, -size.y,  size.z }, {  0.0f,  0.0f,  1.0f}, { -1.0f, 0.0f } }, // Bottom Right Of The Texture and Quad
        { { size.x,  size.y,  size.z }, {  0.0f,  0.0f,  1.0f}, { -1.0f, 1.0f } }, // Top Right Of The Texture and Quad
        { {-size.x,  size.y,  size.z }, {  0.0f,  0.0f,  1.0f}, { 0.0f, 1.0f } }, // Top Left Of The Texture and Quad
    };

    std::vector<tFace> faces = {
        {  0, 4 }
    };

    for (auto v : vertices) {
        v.pos[0] += offset.x;
        v.pos[1] += offset.y;
        v.pos[2] += offset.z;
    }

    auto result = new Geometry();

    result->init();
    result->setMode(GL_QUADS);
    result->setVertices(vertices);
    result->setFaces(faces);

    return result;
}

const static float _pi = 3.141592653589793238462643383279502884f;

Geometry* Geometry::createSphere(float radius, const glm::vec3& offset)
{
    return Geometry::createBall(glm::vec3(radius), offset);
}

void createLatVertices(int i, int lats, int longs, const glm::vec3& size, const glm::vec3& offset, std::vector<tVertex>& vertices, std::vector<tFace>& faces)
{
    float lat0 = _pi * (-0.5f + float(i - 1) / lats);
    float z0  = float(sin(lat0));
    float zr0 =  float(cos(lat0));

    float lat1 = _pi * (-0.5f + float(i) / lats);
    float z1 = float(sin(lat1));
    float zr1 = float(cos(lat1));

    tFace f = { static_cast<GLsizei>(vertices.size()), 0};

    for(int j = 0; j <= longs; j++)
    {
       float lng = 2.0f * _pi * float(j - 1) / longs;
       float x = float(cos(lng));
       float y = float(sin(lng));

       tVertex v1 = { { offset.x + (size.x * x * zr0), offset.y + (size.y * y * zr0), offset.z + (size.z * z0) }, { x * zr0, y * zr0, z0 }, { float(i) / float(lats), float(j) / float(longs) } };
       vertices.push_back(v1);
       f._count++;

       tVertex v2 = { { offset.x + (size.x * x * zr1), offset.y + (size.y * y * zr1), offset.z + (size.z * z1) }, { x * zr1, y * zr1, z1 }, { float(i+1) / float(lats), float(j) / float(longs) } };
       vertices.push_back(v2);
       f._count++;
    }

    faces.push_back(f);
}

Geometry* Geometry::createBall(const glm::vec3& size, const glm::vec3& offset)
{
    std::vector<tVertex> vertices;
    std::vector<tFace> faces;

    int lats = 32, longs = 32;

    for(int i = 0; i <= lats; i++)
    {
        createLatVertices(i, lats, longs, size, offset, vertices, faces);
    }

    auto result = new Geometry();

    result->init();
    result->setMode(GL_QUAD_STRIP);
    result->setVertices(vertices);
    result->setFaces(faces);

    return result;
}

Geometry* Geometry::createCapsule(float radius, float height, const glm::vec3& offset)
{
    glm::vec3 size = glm::vec3(radius);
    std::vector<tVertex> vertices;
    std::vector<tFace> faces;

    int lats = 32, longs = 32;

    for(int i = 0; i <= (lats / 2); i++)
    {
        createLatVertices(i, lats, longs, size, glm::vec3(offset.x, offset.y, offset.z - (height / 2.0f)), vertices, faces);
    }

    {
        int i = (lats / 2);
        float lat0 = _pi * (-0.5f + float(i - 1) / lats);
        float z0  = float(sin(lat0));
        float zr0 =  float(cos(lat0));

        tFace f = { static_cast<GLsizei>(vertices.size()), 0};

        for(int j = 0; j <= longs; j++)
        {
           float lng = 2.0f * _pi * float(j - 1) / longs;
           float x = float(cos(lng));
           float y = float(sin(lng));

           tVertex v1 = { { offset.x + (size.x * x * zr0), offset.y + (size.y * y * zr0), offset.z - (height / 2.0f) + (size.z * z0) }, { x * zr0, y * zr0, z0 }, { float(i) / float(lats), float(j) / float(longs) } };
           vertices.push_back(v1);
           f._count++;

           tVertex v2 = { { offset.x + (size.x * x * zr0), offset.y + (size.y * y * zr0), offset.z + (height / 2.0f) + (size.z * z0) }, { x * zr0, y * zr0, z0 }, { float(i+height) / float(lats), float(j) / float(longs) } };
           vertices.push_back(v2);
           f._count++;
        }

        faces.push_back(f);
    }

    for(int i = lats / 2; i <= lats; i++)
    {
        createLatVertices(i, lats, longs, size, glm::vec3(offset.x, offset.y, offset.z + (height / 2.0f)), vertices, faces);
    }

    auto result = new Geometry();

    result->init();
    result->setMode(GL_QUAD_STRIP);
    result->setVertices(vertices);
    result->setFaces(faces);

    return result;
}

const static std::string vertexShader(
        "#version 150\n"

        "in vec3 vertex;"
        "in vec3 normal;"
        "in vec3 uv;"

        "uniform mat4 u_p;"
        "uniform mat4 u_v;"
        "uniform mat4 u_m;"

        "out vec2 f_uv;"
        "out vec4 f_color;"

        "void main()"
        "{"
        // Position
        "    gl_Position = u_p * u_v * u_m * vec4(vertex.xyz, 1.0);"

        // Texcoords
        "    f_uv = uv.xy;"

        // Light color
        "    mat4 normalMatrix = transpose(inverse(u_v * u_m));"
        "    vec4 lightNor = normalize(normalMatrix * vec4(normal.xyz, 1.0));"
        "    vec3 lightDir1 = normalize(vec3(0.0, 1.0, 1.0));"
        "    float NdotL1 = max(dot(lightNor.xyz, lightDir1), 0.0);"
        "    f_color = (NdotL1 * vec4(0.9, 0.9, 0.9, 1.0)) + vec4(0.8, 0.8, 0.8, 1.0);"
        "}"
    );

const static std::string fragmentShader(
        "#version 150\n"

        "uniform sampler2D tex;"
        "uniform vec4 u_color;"

        "in vec2 f_uv;"
        "in vec4 f_color;"
        "out vec4 color;"

        "void main()"
        "{"
        "   color = u_color * f_color * texture(tex, f_uv.st);"
        "}"
    );

static GLuint geomShader = 0;

GLuint Geometry::shader()
{
    if (geomShader == 0)
        geomShader = LoadShaderProgram(vertexShader, fragmentShader);

    return geomShader;
}

static GLint _projectionUniform = 0;

GLint Geometry::projectionUniform(GLuint shader)
{
    if (_projectionUniform == 0)
        _projectionUniform = glGetUniformLocation(shader, "u_p");

    return _projectionUniform;
}

static GLint _viewUniform = 0;

GLint Geometry::viewUniform(GLuint shader)
{
    if (_viewUniform == 0)
        _viewUniform = glGetUniformLocation(shader, "u_v");

    return _viewUniform;
}

static GLint _modeluniform = 0;

GLint Geometry::modelUniform(GLuint shader)
{
    if (_modeluniform == 0)
        _modeluniform = glGetUniformLocation(shader, "u_m");

    return _modeluniform;
}

static GLint _coloruniform = 0;

GLint Geometry::colorUniform(GLuint shader)
{
    if (_coloruniform == 0)
        _coloruniform = glGetUniformLocation(shader, "u_color");

    return _coloruniform;
}
