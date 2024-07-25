#include "Texture2D.hpp"

Texture2D::Texture2D()
    : m_unit(0), m_glID(0), m_props()
{
}

Texture2D::Texture2D(unsigned char* data, const Texture2DProperties& props, unsigned int unit)
    : m_unit(unit), m_glID(0), m_props(props)
{
    SetupRenderData(data, props, unit);
}

Texture2D::Texture2D(const Texture2D& other)
    :	m_unit(other.m_unit),
	m_glID(other.m_glID),
	m_props(other.m_props)
{
}

Texture2D::Texture2D(Texture2D&& other)
    :	m_unit(std::move(other.m_unit)),
	m_glID(std::move(other.m_glID)),
	m_props(std::move(other.m_props))
{
}

void Texture2D::SetupRenderData(unsigned char* data, const Texture2DProperties& props, unsigned int unit)
{
    m_unit = unit;
    m_props = props;

    glGenTextures(1, &m_glID);

    glActiveTexture(GL_TEXTURE0 + unit);

    glBindTexture(GL_TEXTURE_2D, m_glID);

    glTexImage2D(
	GL_TEXTURE_2D,
	0,
	m_props.Format,
	m_props.Width,
	m_props.Height,
	0,
	m_props.Format,
	GL_UNSIGNED_BYTE,
	data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    // wrap settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // filter settings (with mipmap for min)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture2D::Bind()
{
    glActiveTexture(GL_TEXTURE0 + m_unit);
    glBindTexture(GL_TEXTURE_2D, m_glID);
}


