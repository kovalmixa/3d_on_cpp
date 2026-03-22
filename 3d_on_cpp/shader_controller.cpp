#include "shader_controller.h"
#include <glm/gtc/type_ptr.hpp>

#include "file_functions.h"

ShaderController* ShaderController::instance_ = nullptr;
const std::string ShaderController::VERT_SHAD_PATH = "shaders/vertex.glsl";
const std::string ShaderController::FRAG_SHAD_PATH = "shaders/fragment.glsl";

ShaderController::ShaderController(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vCode = read_file(vertexPath);
    std::string fCode = read_file(fragmentPath);
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

GLuint ShaderController::compile(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

ShaderController* ShaderController::get_instance() {
    return (instance_ == nullptr) ? instance_ = new ShaderController(VERT_SHAD_PATH, FRAG_SHAD_PATH) : instance_;
}

void ShaderController::set_uniform(glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
    GLint projLoc = glGetUniformLocation(program, "u_projection");
    GLint viewLoc = glGetUniformLocation(program, "u_view");
    GLint modelLoc = glGetUniformLocation(program, "u_model");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

bool ShaderController::try_use()
{
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("PROGRAM ERROR:\n%s", infoLog);
        return false;
    }

    glUseProgram(program);
    return true;
}