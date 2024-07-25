#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Material.hpp"

class Shader
{
public:
    Shader()
        : ID(0), m_vertexID(0), m_fragID(0) {}

    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    Shader(const char* vertexCode, const char* fragCode);

    Shader(const Shader& other);

    unsigned int ID;

    void Init(const std::string& vertexPath, const std::string& fragmentPath);
    void Use() const noexcept;

    void SetBool(const std::string& name, bool val) const noexcept;
    void SetInt(const std::string& name, int val) const noexcept;
    void SetUInt(const std::string& name, unsigned int val) const noexcept;
    void SetFloat(const std::string& name, float val) const noexcept;
    void SetMat4(const std::string& name, const glm::mat4& m) const noexcept;
    void SetVec3(const std::string& name, const glm::vec3& v) const noexcept;

    void SetMaterial(const std::string& name, Material& mat) const noexcept;

private:
    unsigned int m_vertexID, m_fragID;
};
