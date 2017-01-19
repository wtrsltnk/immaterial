#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <GL/glextl.h>
#include <glm/glm.hpp>
#include <vector>

typedef struct sVertex
{
    float pos[3];
    float nor[3];
    float uv[3];

    sVertex& operator = (const sVertex& orig)
    {
        for (int i = 0; i < 3; i++)
        {
            this->uv[i] = orig.uv[i];
            this->pos[i] = orig.pos[i];
            this->nor[i] = orig.nor[i];
        }

        return (*this);
    }

} tVertex;

typedef struct sFace
{
    GLint _start;
    GLsizei _count;

} tFace;

class Geometry
{
    static GLuint shader();
    static GLint projectionUniform(GLuint shader);
    static GLint viewUniform(GLuint shader);
    static GLint modelUniform(GLuint shader);
    static GLint colorUniform(GLuint shader);

    GLuint _shader;
    GLuint _vao;
    GLuint _vbo;
    GLenum _mode;
    int _vertexCount;
    std::vector<tFace> _faces;
    glm::vec4 _color;

public:
    Geometry();
    virtual ~Geometry();

    void init();
    virtual void render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);

    void setShader(GLuint shader);
    void setMode(GLenum mode);
    void setVertices(const std::vector<tVertex>& vertices);
    void setFaces(const std::vector<tFace>& faces);
    void setColor(const glm::vec4& color);

public:
    static Geometry* createSphere(float radius, const glm::vec3& offset = glm::vec3());
    static Geometry* createBall(const glm::vec3& size, const glm::vec3& offset = glm::vec3());
    static Geometry* createCapsule(float radius, float height, const glm::vec3& offset = glm::vec3());
    static Geometry* createBox(const glm::vec3& size, const glm::vec3& offset = glm::vec3());
    static Geometry* createPlane(const glm::vec3& size, const glm::vec3& offset);
};

#endif // _GEOMETRY_H_
