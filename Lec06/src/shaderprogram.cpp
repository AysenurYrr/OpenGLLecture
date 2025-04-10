#include"shaderprogram.hpp"
#include<glad/glad.h>
#include<fstream>
#include<iostream>

ShaderProgram::ShaderProgram() {
    m_ProgramId = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(m_ProgramId);
}

void ShaderProgram::link() {
    glLinkProgram(m_ProgramId);
    GLint isLinked = 0;
    glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &isLinked);
    if (!isLinked) {
        GLchar infoLog[1024];
        glGetProgramInfoLog(m_ProgramId, sizeof(infoLog), NULL, infoLog);
        std::cerr << "[Program Linking Error]\n" << infoLog << std::endl;
    }
}

void ShaderProgram::use() {
    glUseProgram(m_ProgramId);
}

void ShaderProgram::attachShader(const char* fileName, unsigned int shaderType) {
    GLuint shaderId = glCreateShader(shaderType);
    std::string sourceCode = getShaderFromFile(fileName);
    const char* chSourceCode = sourceCode.c_str();

    glShaderSource(shaderId, 1, &chSourceCode, NULL);
    glCompileShader(shaderId);

    GLint isCompiled = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shaderId, sizeof(infoLog), NULL, infoLog);
        std::cerr << "[Shader Compilation Error] (" << fileName << ")\n" << infoLog << std::endl;
        return;
    }

    glAttachShader(m_ProgramId, shaderId);
    glDeleteShader(shaderId); // Shader program'a eklendikten sonra silinebilir
}

std::string ShaderProgram::getShaderFromFile(const char* fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "[Shader Load Error] Dosya acilamadi: " << fileName << std::endl;
        return "";
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

unsigned int ShaderProgram::getId() const {
    return m_ProgramId;
}