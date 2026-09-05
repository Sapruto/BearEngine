#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform vec4 spriteColor;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = texColor * spriteColor;
    if (FragColor.a < 0.01) discard;
}