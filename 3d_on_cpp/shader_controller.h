#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

class ShaderController {
private:
    static ShaderController* instance_;

    static const std::string VERT_SHAD_PATH;
    static const std::string FRAG_SHAD_PATH;

    GLuint compile(GLenum type, const std::string& source);
    ShaderController(const std::string& vertexPath, const std::string& fragmentPath);
public:
    GLuint program;

    static ShaderController* get_instance();
    bool check_correctness();
    void set_uniform(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
    void use();
};