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

    inline int GetTexUnit() const noexcept { return m_texUnit; }
    inline void SetTexUnit(int unit) noexcept { m_texUnit = unit; }

private:
    unsigned int m_id;
    int m_texUnit;
};
