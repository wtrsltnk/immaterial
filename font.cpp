#include "font.h"
#include "shader.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#define imagesize 1024

Font::Font()
    : _startchar(0), _endchar(128), _cdata(nullptr), _fontsize(12.0f), _texture(nullptr)
{ }

Font::~Font()
{
    if (this->_cdata != nullptr)
    {
        delete[]this->_cdata;
        this->_cdata = nullptr;
    }
}

bool Font::init(const std::string & fontname, float size, int startchar, int endchar)
{
    bool res = false;

    this->_fontsize = size;
    this->_startchar = startchar;
    this->_endchar = endchar;

    auto io = fopen(fontname.c_str(), "rb");
    if (io != nullptr)
    {
        fseek(io, 0, SEEK_END);
        size_t size = size_t(ftell(io));
        fseek(io, 0, SEEK_SET);
        auto buffer = new unsigned char[size + 1];
        if (fread(buffer, size, 1, io) > 0)
        {
            if (this->_cdata != nullptr) delete []this->_cdata;
            this->_cdata = new stbtt_bakedchar[this->_endchar - this->_startchar];
            auto bitmap = new unsigned char[imagesize * imagesize];
            stbtt_BakeFontBitmap(buffer, 0, this->_fontsize, bitmap, imagesize, imagesize, this->_startchar, this->_endchar - this->_startchar, this->_cdata);

            this->_texture = new Texture(imagesize, imagesize, 1, bitmap);
            this->_texture->upload();

            res = true;
        }
        else
        {
            std::cout << "Failed to read from font file: " << fontname << std::endl;
        }
        delete[]buffer;
        fclose(io);
    }
    else
    {
        std::cout << "Failed to open font file: " << fontname << std::endl;
    }
    return res;
}

Font::Buffer& Font::print(const std::string& text2print, Font::Buffer& buffer) const
{
    std::vector <FontVertex> vertices;
    float x = 0, y = 0;

    auto text = text2print.c_str();
    while (*text)
    {
        if (char(*text) >= 0 && char(*text) < '~')
        {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(this->_cdata, imagesize, imagesize, *text, &x, &y, &q, 1);
            FontVertex a[] = {
                { { q.x0, q.y0, 0.0f }, { q.s0, q.t0 } },
                { { q.x1, q.y0, 0.0f }, { q.s1, q.t0 } },
                { { q.x1, q.y1, 0.0f }, { q.s1, q.t1 } },
                { { q.x0, q.y1, 0.0f }, { q.s0, q.t1 } }
            };

            vertices.push_back(a[0]);
            vertices.push_back(a[1]);
            vertices.push_back(a[2]);

            vertices.push_back(a[0]);
            vertices.push_back(a[2]);
            vertices.push_back(a[3]);
        }
        ++text;
    }
    buffer.setVertices(vertices);
    buffer.setTexture(this->_texture);
    return buffer;
}

Font::Buffer::Buffer()
    : _vao(0), _vbo(0), _texture(0), _vertexCount(0), _color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{ }

Font::Buffer::~Buffer()
{ }

void Font::Buffer::init()
{
    if (this->_vao == 0)
        glGenVertexArrays(1, &this->_vao);
    glBindVertexArray(this->_vao);

    if (this->_vbo == 0)
        glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);

    auto vertexAttrib = glGetAttribLocation(Font::shader(), "a_vertex");
    glVertexAttribPointer(GLuint(vertexAttrib), 3, GL_FLOAT, GL_FALSE, sizeof(FontVertex), 0);
    glEnableVertexAttribArray(GLuint(vertexAttrib));

    auto uvAttrib = glGetAttribLocation(Font::shader(), "a_uv");
    glVertexAttribPointer(GLuint(uvAttrib), 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), reinterpret_cast<const GLvoid*>(sizeof(float) * 3));
    glEnableVertexAttribArray(GLuint(uvAttrib));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Font::Buffer::setVertices(const std::vector<FontVertex>& vertices)
{
    this->_max = glm::vec3(-9999.0f);
    this->_min = glm::vec3(9999.0f);

    for (auto v : vertices)
    {
        for (int i = 0; i < 3; i++)
        {
            if (v.pos[i] < this->_min[i]) this->_min[i] = v.pos[i];
            if (v.pos[i] > this->_max[i]) this->_max[i] = v.pos[i];
        }
    }

    this->_vertexCount = GLsizei(vertices.size());
    glBindVertexArray(this->_vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);

    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(FontVertex)), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(vertices.size() * sizeof(FontVertex)), reinterpret_cast<const GLvoid*>(&vertices[0]));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Font::Buffer::setTexture(Texture* texture)
{
    this->_texture = texture;
}

void Font::Buffer::setColor(const glm::vec4& color)
{
    this->_color = color;
}

void Font::Buffer::render(const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model)
{
    glUseProgram(Font::shader());

    if (this->_texture != nullptr)
    {
        glEnable(GL_TEXTURE_2D);
        this->_texture->bind();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniformMatrix4fv(Font::projectionUniform(), 1, false, glm::value_ptr(proj));
    glUniformMatrix4fv(Font::viewUniform(), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(Font::modelUniform(), 1, false, glm::value_ptr(model));
    glUniform4fv(Font::colorUniform(), 1, glm::value_ptr(this->_color));
    glUniform1i(Font::textureUniform(), 0);

    glBindVertexArray(this->_vao);
    glDrawArrays(GL_TRIANGLES, 0, this->_vertexCount);
    glBindVertexArray(0);
}

int Font::Buffer::width() const
{
    return int(this->_max.x - this->_min.x);
}

int Font::Buffer::height() const
{
    return int(this->_max.y - this->_min.y);
}

const glm::vec3& Font::Buffer::min() const
{
    return this->_min;
}

const glm::vec3& Font::Buffer::max() const
{
    return this->_max;
}

const std::string fontVertexShader(
        "#version 140\n"

        "in vec3 a_vertex;"
        "in vec2 a_uv;"

        "uniform mat4 u_p;"
        "uniform mat4 u_v;"
        "uniform mat4 u_m;"
        "uniform vec4 u_global_color;"

        "out vec2 f_uv;"
        "out vec4 f_color;"

        "void main()"
        "{"
        "    gl_Position = u_p * u_v * u_m * vec4(a_vertex.xyz, 1.0);"
        "    f_uv = a_uv.st;"
        "    f_color = u_global_color;"
        "}");

const std::string fontFragmentShader(
        "#version 140\n"

        "uniform sampler2D u_texture;"

        "in vec2 f_uv;"
        "in vec4 f_color;"

        "out vec4 color;"

        "void main()"
        "{"
        "   color = vec4(f_color.xyz, texture2D(u_texture, f_uv.st).a);"
        "}");

static GLuint fontShader = 0;

GLuint Font::shader()
{
    if (fontShader == 0)
        fontShader = LoadShaderProgram(fontVertexShader, fontFragmentShader);

    return fontShader;
}

static GLint _projectionUniform = 0;

GLint Font::projectionUniform()
{
    if (_projectionUniform == 0)
        _projectionUniform = glGetUniformLocation(Font::shader(), "u_p");

    return _projectionUniform;
}

static GLint _viewUniform = 0;

GLint Font::viewUniform()
{
    if (_viewUniform == 0)
        _viewUniform = glGetUniformLocation(Font::shader(), "u_v");

    return _viewUniform;
}

static GLint _modeluniform = 0;

GLint Font::modelUniform()
{
    if (_modeluniform == 0)
        _modeluniform = glGetUniformLocation(Font::shader(), "u_m");

    return _modeluniform;
}

static GLint _coloruniform = 0;

GLint Font::colorUniform()
{
    if (_coloruniform == 0)
        _coloruniform = glGetUniformLocation(Font::shader(), "u_global_color");

    return _coloruniform;
}

static GLint _textureuniform = 0;

GLint Font::textureUniform()
{
    if (_textureuniform == 0)
        _textureuniform = glGetUniformLocation(Font::shader(), "u_texture");

    return _textureuniform;
}
