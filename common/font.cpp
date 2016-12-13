#include "font.h"
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "shader.h"

typedef struct sVertex
{
    float position[3];
    float texCoords[2];

} tVertex;

Font::Font()
    : _fontSize(12.0f), _iconOffset(0), _textureId(0), _vertexArrayObject(0), _vertexBufferObject(0)
{ }

Font::~Font()
{ }

void Font::GetBakedQuad(int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q)
{
    stbtt_bakedchar *b = this->_charData + char_index;
    int round_x = STBTT_ifloor(*xpos + b->xoff);
    int round_y = STBTT_ifloor(*ypos - b->yoff);

    q->x0 = (float)round_x;
    q->y0 = (float)round_y;
    q->x1 = (float)round_x + b->x1 - b->x0;
    q->y1 = (float)round_y - b->y1 + b->y0;

    q->s0 = b->x0 / (float)pw;
    q->t0 = b->y0 / (float)pw;
    q->s1 = b->x1 / (float)ph;
    q->t1 = b->y1 / (float)ph;

    *xpos += b->xadvance;
}

bool Font::InitializeFont(const char* fontpath, float fontSize, int iconOffset)
{
    FontShader::LoadShader();

    this->_fontSize = fontSize;
    this->_iconOffset = iconOffset;

    // Load font.
    FILE* fp = NULL;

    if (fopen_s(&fp, fontpath, "rb") != 0)
        return false;

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char* ttfBuffer = (unsigned char*)malloc(size);
    if (!ttfBuffer)
    {
        fclose(fp);
        return false;
    }

    fread(ttfBuffer, 1, size, fp);
    fclose(fp);
    fp = 0;

    unsigned char* bmap = (unsigned char*)malloc(BMAP_SIZE*BMAP_SIZE);
    if (!bmap)
    {
        free(ttfBuffer);
        return false;
    }

    stbtt_BakeFontBitmap(ttfBuffer,0, fontSize, bmap, BMAP_SIZE, BMAP_SIZE, _iconOffset, 1024, this->_charData);
    free(ttfBuffer);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glGenTextures(1, &this->_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, BMAP_SIZE,BMAP_SIZE, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(bmap);

    tVertex* vertices = new tVertex[1024 * 6];

    int i = 0;
    for (int c = 0; c < 1024; c++)
    {
        float x = 0, y = 0;
        stbtt_aligned_quad q;
        this->GetBakedQuad(BMAP_SIZE, BMAP_SIZE, c, &x, &y, &q);

        vertices[i].texCoords[0] = q.s0;
        vertices[i].texCoords[1] = q.t0;
        vertices[i].position[0] = q.x0;
        vertices[i].position[1] = -q.y0;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s1;
        vertices[i].texCoords[1] = q.t1;
        vertices[i].position[0] = q.x1;
        vertices[i].position[1] = -q.y1;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s1;
        vertices[i].texCoords[1] = q.t0;
        vertices[i].position[0] = q.x1;
        vertices[i].position[1] = -q.y0;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s0;
        vertices[i].texCoords[1] = q.t0;
        vertices[i].position[0] = q.x0;
        vertices[i].position[1] = -q.y0;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s0;
        vertices[i].texCoords[1] = q.t1;
        vertices[i].position[0] = q.x0;
        vertices[i].position[1] = -q.y1;
        vertices[i].position[2] = 0.0f;

        i++;

        vertices[i].texCoords[0] = q.s1;
        vertices[i].texCoords[1] = q.t1;
        vertices[i].position[0] = q.x1;
        vertices[i].position[1] = -q.y1;
        vertices[i].position[2] = 0.0f;

        i++;
    }

    if (this->_vertexArrayObject == 0)
        glGenVertexArrays(1, &this->_vertexArrayObject);
    glBindVertexArray(this->_vertexArrayObject);

    if (this->_vertexBufferObject == 0)
        glGenBuffers(1, &this->_vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, i * sizeof(tVertex), (const GLvoid *)vertices, GL_STATIC_DRAW);

    GLint vertexAttrib = glGetAttribLocation(FontShader::_program, "vertex");
    glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), 0);
    glEnableVertexAttribArray(vertexAttrib);

    GLint texcoordAttrib = glGetAttribLocation(FontShader::_program, "texcoords");
    glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(tVertex), (const GLvoid *)(sizeof(GLfloat)*3));
    glEnableVertexAttribArray(texcoordAttrib);

    glBindVertexArray(0);

    delete []vertices;

    return true;
}

void Font::DrawIcon(const glm::mat4& proj, const glm::mat4& view, float x, float y, int icon, const glm::vec4& global_color)
{
    glm::mat4 local = glm::mat4(1.0f);
    if (this->_textureId == 0) return;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindTexture(GL_TEXTURE_2D, this->_textureId);
    glUseProgram(FontShader::_program);
    glUniformMatrix4fv(FontShader::_u_projection, 1, false, glm::value_ptr(proj));
    glUniform4fv(FontShader::_u_global_color, 1, glm::value_ptr(global_color));

    glBindVertexArray(this->_vertexArrayObject);

    int c = icon - this->_iconOffset;
    local = view * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0));
    glUniformMatrix4fv(FontShader::_u_view, 1, false, glm::value_ptr(local));
    glDrawArrays(GL_TRIANGLES, c * 6, 6);

    glBindVertexArray(0);
}

const std::string vertexShader(
        "#version 150\n"

        "in vec3 vertex;\n"
        "in vec2 texcoords;\n"

        "uniform mat4 u_projection;\n"
        "uniform mat4 u_view;\n"

        "out vec2 f_uv;\n"

        "void main()\n"
        "{"
        "    mat4 m = u_projection * u_view;\n"
        "    gl_Position = m * vec4(vertex.xyz, 1.0);\n"
        "    f_uv = texcoords.st;\n"
        "}"
    );

const std::string fragmentShader(
        "#version 150\n"

        "uniform sampler2D tex;\n"
        "uniform vec4 u_global_color;\n"

        "in vec2 f_uv;\n"

        "out vec4 color;"

        "void main()\n"
        "{"
        "   vec4 t = texture(tex, f_uv.st);"
        "   color = vec4(u_global_color.xyz, t.w * u_global_color.w);\n"
        "}"
    );

void FontShader::LoadShader()
{
    if (FontShader::_program == 0)
    {
        FontShader::_program = LoadShaderProgram(vertexShader, fragmentShader);

        glUseProgram(FontShader::_program);

        FontShader::_u_projection = glGetUniformLocation(FontShader::_program, "u_projection");
        FontShader::_u_view = glGetUniformLocation(FontShader::_program, "u_view");
        FontShader::_u_global_color = glGetUniformLocation(FontShader::_program, "u_global_color");
    }
}

unsigned int FontShader::_program = 0;
unsigned int FontShader::_u_projection = 0;
unsigned int FontShader::_u_view = 0;
unsigned int FontShader::_u_global_color = 0;
