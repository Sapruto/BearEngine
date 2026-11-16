#version 450 core

layout(std140, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(location = 5) in vec4 aModel0;
layout(location = 6) in vec4 aModel1;
layout(location = 7) in vec4 aModel2;
layout(location = 8) in vec4 aModel3;
layout(location = 9) in vec4 aColor;

out vec3 vNormal;
out vec3 vFragPos;
out vec4 vColor;

void main() {
    mat4 model = mat4(aModel0, aModel1, aModel2, aModel3);

    vec4 worldPos = model * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(model))) * aNormal;
    vColor = aColor;

    gl_Position = projection * view * worldPos;
}