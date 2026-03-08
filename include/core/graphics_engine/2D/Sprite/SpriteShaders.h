#pragma once

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;

void main() {
    gl_Position = projection * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}
)";

float vertices[] = {
     0.5f,  0.5f,   1.0f, 1.0f,  
     0.5f, -0.5f,   1.0f, 0.0f, 
    -0.5f, -0.5f,   0.0f, 0.0f, 
    -0.5f,  0.5f,   0.0f, 1.0f  
};

unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3 
};