#pragma once
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>

class Shader {
public:
    GLuint program;

    Shader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        std::string vCode = loadFile(vertexPath);
        std::string fCode = loadFile(fragmentPath);

        GLuint vertex = compile(GL_VERTEX_SHADER, vCode);
        GLuint fragment = compile(GL_FRAGMENT_SHADER, fCode);

        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);

        glBindAttribLocation(program, 0, "position");

        glLinkProgram(program);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use()
    {
        glUseProgram(program);
    }

private:
    std::string loadFile(const std::string& path)
    {
        std::ifstream file(path);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    GLuint compile(GLenum type, const std::string& source)
    {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        return shader;
    }
};