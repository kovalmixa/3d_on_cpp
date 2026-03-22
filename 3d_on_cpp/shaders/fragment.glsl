#version 330 core

in vec4 v_fragPosLightSpace;

uniform sampler2D u_shadowMap;
uniform vec4 u_color;

out vec4 FragColor;

float calculate_shadow() {
    vec3 projCoords = v_fragPosLightSpace.xyz / v_fragPosLightSpace.w;
    
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0) return 1.0;

    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    
    float bias = 0.005;
    return (currentDepth - bias > closestDepth) ? 0.5 : 1.0; 
}

void main() {
    float shadow = calculate_shadow();
    FragColor = u_color * shadow;
}