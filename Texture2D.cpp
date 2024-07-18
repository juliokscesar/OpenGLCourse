#include "Texture2D.hpp"
#include <iostream>

#include <stb/stb_image.h>

Texture2D::Texture2D(const std::string &imgPath, bool flip, int internalFormat, GLenum format, int texUnit)
{
    Init(imgPath, flip, internalFormat, format, texUnit);
}

void Texture2D::Init(const std::string &imgPath, bool flip, int internalFormat, GLenum format, int texUnit)
{
    stbi_set_flip_vertically_on_load(flip);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(imgPath.c_str(), &width, &height, &nrChannels, 0);
    
    m_texUnit = texUnit;
    glActiveTexture(texUnit + GL_TEXTURE0);

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    // texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture2D::Activate() const noexcept
{
    glActiveTexture(m_texUnit + GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_id);
}
