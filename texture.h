#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glextl.h>
#include <glm/glm.hpp>

class Texture
{
    std::string _name;
    unsigned char* _data;
    int _width;
    int _height;
    int _bpp;
    GLuint _glIndex;
    static Texture* _empty;

public:
    Texture();
    explicit Texture(const std::string &filename);
    Texture(int w, int h, int bpp, unsigned char* data = nullptr);
    virtual ~Texture();

    GLuint upload();
    void bind();

    const std::string& name() const;
    void name(const std::string& name);

    void repeat(bool repeat);

    int width() const;
    int height() const;
    int bpp() const;

    Texture* Copy() const;
    void CopyFrom(const Texture& from);
    void DefaultTexture();

    glm::vec4 PixelAt(int x, int y) const;
    void SetPixelAt(const glm::vec4& pixel, int x, int y);

    void Fill(const glm::vec4& color);
    void Fill(const Texture& from);
    // expandBorder is used to puts a 1-pixel border around the copied texture so no neightbour leaking is occuring on an atlas
    void FillAtPosition(const Texture& from, const glm::vec2& pos, bool expandBorder = false);

    void SetData(int w, int h, int bpp, unsigned char* data = nullptr);
    void SetName(const std::string& _name);
    void SetDimentions(int _width, int _height, int _bpp = 3);

    void CorrectGamma(float gamma);

public:
    static void bindEmpty();

};

#endif // TEXTURE_H
