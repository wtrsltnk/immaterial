#ifndef UIFONT_H_
#define UIFONT_H_

#include "stb_truetype.h"
#include "font-icons.h"

#include <GL/glextl.h>
#include <glm/glm.hpp>
#include <string>

#define BMAP_SIZE 1024
class FontShader
{
public:
    static void LoadShader();

    static unsigned int _program;
    static unsigned int _u_projection;
    static unsigned int _u_view;
    static unsigned int _u_global_color;

};

class Font
{
public:
    Font();
    virtual ~Font();

    float Size() const { return this->_fontSize; }
    int IconOffset() const { return this->_iconOffset; }

    bool InitializeFont(const char* fontpath, float Size = 15.0f, int iconOffset = 0xf000);
    void DrawIcon(const glm::mat4& proj, const glm::mat4& view, float x, float y, int icon, const glm::vec4& global_color = glm::vec4(1, 1, 1, 1));
    void GetBakedQuad(int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q);

protected:
    stbtt_bakedchar _charData[1024];
    float _fontSize;
    int _iconOffset;
    GLuint _textureId;
    GLuint _vertexArrayObject;
    GLuint _vertexBufferObject;

};

#endif /* UIFONT_H_ */
