#include "texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
    : _data(nullptr), _width(0), _height(0), _bpp(0), _glIndex(0)
{
    this->name("");
}

Texture::Texture(const std::string &filename)
    : _data(nullptr), _width(0), _height(0), _bpp(0), _glIndex(0)
{
    int w;
    int h;
    int bpp;
    unsigned char* image = stbi_load(filename.c_str(), &w, &h, &bpp, STBI_rgb_alpha);

    if(image != 0)
    {
        this->SetData(w, h, bpp, image);
        free(image);
        this->upload();
        this->name(filename);
    }
    else
    {
        std::cout << "Error loading " << filename << std::endl;
    }
}

Texture::Texture(int w, int h, int bpp, unsigned char* data)
    : _data(nullptr), _width(0), _height(0), _bpp(0), _glIndex(0)
{
    this->SetData(w, h, bpp, data);
}

Texture::~Texture()
{
    if (this->_glIndex > 0)
        glDeleteTextures(1, &this->_glIndex);
    this->_glIndex = 0;

    if (this->_data != 0)
        delete []this->_data;
    this->_data = 0;
}

GLuint Texture::upload()
{
    // Only upload when gl index == -1 (this means it's not uploaded yet)
    if (this->_glIndex == 0)
    {
        GLenum format = GL_RGB;

        // Determine format
        switch (this->_bpp)
        {
        case 1: format = GL_ALPHA; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        }

        glGenTextures(1, &this->_glIndex);
        glBindTexture(GL_TEXTURE_2D, this->_glIndex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, format, this->_width, this->_height, 0, format, GL_UNSIGNED_BYTE, this->_data);
    }

    return this->_glIndex;
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, this->_glIndex);
}

Texture* Texture::_empty = nullptr;

void Texture::bindEmpty()
{
    if (Texture::_empty == nullptr)
    {
        Texture::_empty = new Texture(64, 64, 4);
        int value;
        for (int row = 0; row < Texture::_empty->_width; row++) {
           for (int col = 0; col < Texture::_empty->_height; col++) {
              // Each cell is 8x8, value is 0 or 255 (black or white)
              value = (((row & 0x8) == 0) ^ ((col & 0x8) == 0)) * 255;
              Texture::_empty->SetPixelAt(glm::vec4(float(value), float(value), float(value), float(value)), row, col);
           }
        }
        Texture::_empty->upload();
    }
    Texture::_empty->bind();
}

const std::string& Texture::name() const
{
    return this->_name;
}

void Texture::name(const std::string& name)
{
    this->_name = name;
}

int Texture::width() const
{
    return this->_width;
}

int Texture::height() const
{
    return this->_height;
}

int Texture::bpp() const
{
    return this->_bpp;
}

void Texture::repeat(bool repeat)
{
    this->bind();

    if (repeat)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
}

Texture* Texture::Copy() const
{
    Texture* result = new Texture();

    result->_name = std::string(this->_name);

    result->_width = this->_width;
    result->_height = this->_height;
    result->_bpp = this->_bpp;
    result->_glIndex = this->_glIndex;
    size_t dataSize = static_cast<size_t>(this->width() * this->height() * this->bpp());
    if (dataSize > 0)
    {
        result->_data = new unsigned char[dataSize];
        memcpy(result->_data, this->_data, dataSize);
    }
    else
    {
        delete result;
        result = nullptr;
    }

    return result;
}

void Texture::CopyFrom(const Texture& from)
{
    if (this->_data != nullptr)
    {
        delete []this->_data;
        this->_data = nullptr;
    }

    this->_name = from._name;

    this->_width = from._width;
    this->_height = from._height;
    this->_bpp = from._bpp;
    this->_glIndex = from._glIndex;
    size_t dataSize = static_cast<size_t>(this->width() * this->height() * this->bpp());
    if (dataSize > 0)
    {
        this->_data = new unsigned char[dataSize];
        memcpy(this->_data, from._data, dataSize);
    }
}

void Texture::DefaultTexture()
{
    int value;
    for (int row = 0; row < this->_width; row++) {
       for (int col = 0; col < this->_height; col++) {
          // Each cell is 8x8, value is 0 or 255 (black or white)
          value = (((row & 0x8) == 0) ^ ((col & 0x8) == 0)) * 255;
          SetPixelAt(glm::vec4(float(value), float(value), float(value), float(value)), row, col);
       }
    }
}

glm::vec4 Texture::PixelAt(int x, int y) const
{
    glm::vec4 r(1.0f, 1.0f, 1.0f, 1.0f);
    int p = x + (y*_width);
    for (int i = 0 ; i < this->_bpp; i++)
        r[i] = _data[(p * _bpp) + i];
    return r;
}

void Texture::SetPixelAt(const glm::vec4& pixel, int x, int y)
{
    int p = x + (y*_width);
    for (int i = 0 ; i < this->_bpp; i++)
        _data[(p * _bpp) + i] = static_cast<unsigned char>(pixel[i]);
}

void Texture::Fill(const glm::vec4& color)
{
    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            this->SetPixelAt(color, x, y);
        }
    }
}

void Texture::Fill(const Texture& from)
{
    int x = 0, y = 0;
    while (x < this->width())
    {
        while (y < this->height())
        {
            FillAtPosition(from, glm::vec2(x, y));
            y += from.height();
        }
        x += from.width();
    }
}

void Texture::FillAtPosition(const Texture& from, const glm::vec2& pos, bool expandBorder)
{
    if (pos.x > this->width() || pos.y > this->height()) return;

    int w = from.width();
    int h = from.height();

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            this->SetPixelAt(from.PixelAt(x, y), int(pos.x + x), int(pos.y + y));
            if (expandBorder)
            {
                if (y == 0)
                {
                    this->SetPixelAt(from.PixelAt(x, y), int(pos.x + x), int(pos.y + y) - 1);
                    if (x == 0) this->SetPixelAt(from.PixelAt(x, y), int(pos.x + x) - 1, int(pos.y + y) - 1);
                    if (x == w-1) this->SetPixelAt(from.PixelAt(x, y), int(pos.x + x) + 1, int(pos.y + y) - 1);
                }
                else if (y == h-1)
                {
                    this->SetPixelAt(from.PixelAt(x, y), int(pos.x + x), int(pos.y + y + 1));
                    if (x == 0) this->SetPixelAt(from.PixelAt(x, y), int(pos.x + x) - 1, int(pos.y + y) + 1);
                    if (x == w-1) this->SetPixelAt(from.PixelAt(x, y), int(pos.x + x) + 1, int(pos.y + y) + 1);
                }
                if (x == 0)
                    this->SetPixelAt(from.PixelAt(x, y), int(pos.x + x) - 1, int(pos.y + y));
                else if (x == w-1)
                    this->SetPixelAt(from.PixelAt(x, y), int(pos.x + x) + 1, int(pos.y + y));
            }
        }
    }
}

void Texture::SetData(int w, int h, int bpp, unsigned char* data)
{
    size_t dataSize = static_cast<size_t>(w * h * bpp);

    if (dataSize > 0)
    {
        this->_width = w;
        this->_height = h;
        this->_bpp = bpp;

        if (this->_data != nullptr)
            delete []this->_data;
        this->_data = 0;

        this->_data = new unsigned char[dataSize];
        if (data != 0)
            memcpy(this->_data, data, sizeof(unsigned char) * dataSize);
        else
            memset(this->_data, 0, dataSize);
    }
}

void Texture::SetName(const std::string& name)
{
    this->_name = name;
}

void Texture::SetDimentions(int width, int height, int bpp)
{
    if (this->_glIndex != 0)
        return;

    if (this->_data != nullptr)
    {
        delete []this->_data;
        this->_data = nullptr;
    }

    this->_width = width;
    this->_height = height;
    this->_bpp = bpp;

    size_t dataSize = static_cast<size_t>(width * height * bpp);

    if (dataSize > 0)
        this->_data = new unsigned char[dataSize];
}

void Texture::CorrectGamma(float gamma)
{
    // Only images with rgb colors
    if (this->_bpp < 3)
        return;

    for(int j = 0; j < (this->_width * this->_height); ++j)
    {
        float r, g, b;
        r = this->_data[j * this->_bpp + 0];
        g = this->_data[j * this->_bpp + 1];
        b = this->_data[j * this->_bpp + 2];

        r *= gamma/255.0f;
        g *= gamma/255.0f;
        b *= gamma/255.0f;

        // find the value to scale back up
        float scale = 1.0f;
        float temp;
        if(r > 1.0f && (temp = (1.0f/r)) < scale) scale = temp;
        if(g > 1.0f && (temp = (1.0f/g)) < scale) scale = temp;
        if(b > 1.0f && (temp = (1.0f/b)) < scale) scale = temp;

        // scale up color values
        scale *= 255.0f;
        r *= scale;
        g *= scale;
        b *= scale;

        //fill data back in
        this->_data[j * this->_bpp + 0] = static_cast<GLubyte>(r);
        this->_data[j * this->_bpp + 1] = static_cast<GLubyte>(g);
        this->_data[j * this->_bpp + 2] = static_cast<GLubyte>(b);
    }
}
