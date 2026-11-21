#version 450 core

layout(std140, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

in vec3 vNormal;
in vec3 vFragPos;
in vec4 vColor;

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(viewPos.xyz - vFragPos);

    FragColor = vec4(vColor.rgb, 1.0);
}