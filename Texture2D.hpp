#pragma once

#include <glad/glad.h>

#include <string>
#include <utility>

struct Texture2DProperties
{
    int Width = 0;
    int Height = 0;
    GLenum Format = GL_RGB;
    std::string Path = "";
};

class Texture2D
{
public:
    Texture2D();
    Texture2D(unsigned char* data, const Texture2DProperties& props, unsigned int unit=0);

    Texture2D(const Texture2D& other);
    Texture2D(Texture2D&& other);

    Texture2D& operator=(const Texture2D& other)
    {
	if (this != &other)
	{
	    this->m_unit = other.m_unit;
	    this->m_glID = other.m_glID;
	    this->m_props = other.m_props;
	}

	return *this;
    }

    Texture2D& operator=(Texture2D&& other)
    {
	if (this != &other)
	{
	    this->m_unit = std::move(other.m_unit);
	    this->m_glID = std::move(other.m_glID);
	    this->m_props = std::move(other.m_props);
	}

	return *this;
    }

    void SetupRenderData(unsigned char* data, const Texture2DProperties& props, unsigned int unit=0);

    void Bind();

    inline unsigned int GetID() const noexcept { return m_glID; }

    inline unsigned int GetUnit() const noexcept { return m_unit; }
    inline void SetUnit(unsigned int unit) noexcept { m_unit = unit; }

    inline Texture2DProperties GetProperties() const noexcept { return m_props; }

private:
    unsigned int m_unit = 0;
    unsigned int m_glID = 0;
    Texture2DProperties m_props;
};

