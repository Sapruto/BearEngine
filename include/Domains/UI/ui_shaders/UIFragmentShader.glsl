#version 330 core

in vec2 vTexCoord;
in vec4 vColor;

uniform sampler2D uTextureAtlas;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(uTextureAtlas, vTexCoord);
    FragColor = texColor * vColor;
}