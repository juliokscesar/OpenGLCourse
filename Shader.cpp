#include "Shader.hpp"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>


Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
{
    Init(vertexPath, fragmentPath);
}

Shader::Shader(const Shader &other)
{
    this->ID = other.ID;
    this->m_vertexID = other.m_vertexID;
    this->m_fragID = other.m_fragID;
}

void Shader::Init(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::ifstream vShaderFile, fShaderFile;
    // ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::stringstream vShaderStream{}, fShaderStream{};
    std::string vertexCode{}, fragCode{};
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragCode = fShaderStream.str();    
    }
    catch(const std::ifstream::failure& e)
    {
        std::cerr << "Error while trying to read shader files from " << vertexPath << " or " << fragmentPath << ": " << e.what() << '\n';
    }
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragCode.c_str();

    int success = 0;
    char infoLog[512];

    // vertex shader
    m_vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexID, 1, &vShaderCode, nullptr);
    glCompileShader(m_vertexID);

    // check for compiler errors
    glGetShaderiv(m_vertexID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(m_vertexID, 512, nullptr, infoLog);
        std::cout << "Error compiling vertex shader " << vertexPath << ": " << infoLog << "\n";
    }

    // fragment shader
    m_fragID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragID, 1, &fShaderCode, nullptr);
    glCompileShader(m_fragID);

    // check for compiler errors
    glGetShaderiv(m_fragID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(m_fragID, 512, nullptr, infoLog);
        std::cout << "Error compiling fragment shader " << fragmentPath << ": " << infoLog << "\n";
    }

    // shader program
    ID = glCreateProgram();
    glAttachShader(ID, m_vertexID);
    glAttachShader(ID, m_fragID);
    glLinkProgram(ID);

    // check for linking errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "Error linking shader program of " << vertexPath << ", " << fragmentPath << ": " << infoLog << "\n";
    }

    // delete shaders
    glDeleteShader(m_vertexID);
    glDeleteShader(m_fragID);

    Use();
}


void Shader::Use() const noexcept
{
    glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, bool val) const noexcept
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, static_cast<int>(val));
}

void Shader::SetInt(const std::string & name, int val) const noexcept
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1i(loc, val);
}

void Shader::SetFloat(const std::string &name, float val) const noexcept
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, val);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &m) const noexcept
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &v) const noexcept
{
    GLint loc = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(v));
}
