#version 330 core

layout (location = 0) in vec3 a_pos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_lightSpaceMatrix;

out vec4 v_fragPosLightSpace;

void main() {
    v_fragPosLightSpace = u_lightSpaceMatrix * u_model * vec4(a_pos, 1.0);
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0);
}