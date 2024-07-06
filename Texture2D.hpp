#pragma once

#include <glad/glad.h>

#include <string>

class Texture2D
{
public:
    Texture2D()
        : m_id(0), m_texUnit(0) {}

    Texture2D(const std::string& imgPath, bool flip, int internalFormat, GLenum format, GLenum texUnit=GL_TEXTURE0);

    void Init(const std::string& imgPath, bool flip, int internalFormat, GLenum format, GLenum texUnit=GL_TEXTURE0);

    void Activate() const noexcept;

private:
    unsigned int m_id;
    GLenum m_texUnit;
};
