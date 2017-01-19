#ifndef FONT_H
#define FONT_H

#include <stdio.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#ifdef _WIN32
#include <GL/glextl.h>
#else
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#endif

#include "stb_truetype.h"
#include "geometry.h"
#include "texture.h"

typedef struct
{
    float pos[3];
    float uv[2];

} FontVertex;

class Font
{
    static GLuint shader();
    static GLint projectionUniform();
    static GLint viewUniform();
    static GLint modelUniform();
    static GLint colorUniform();
    static GLint textureUniform();

    int _startchar;
    int _endchar;
    stbtt_bakedchar* _cdata;

public:
    class Buffer
    {
        GLuint _vao;
        GLuint _vbo;
        Texture* _texture;
        GLsizei _vertexCount;
        glm::vec4 _color;
        glm::vec3 _min, _max;
    public:
        Buffer();
        virtual ~Buffer();

        void init();

        void setVertices(const std::vector<FontVertex>& vertices);
        void setTexture(Texture* _texture);
        void setColor(const glm::vec4& color);

        virtual void render(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model);

        int width() const;
        int height() const;
        const glm::vec3& min() const;
        const glm::vec3& max() const;
    };

public:
    Font();
    virtual ~Font();

    bool init(const std::string & fontname, float size, int _startchar = 0, int _endchar = 128);

    Buffer& print(const std::string& text2print, Buffer& buffer) const;

    float _fontsize;
    Texture* _texture;

};

#endif				// FONT_H
