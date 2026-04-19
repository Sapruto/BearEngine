#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D inputTexture;
uniform vec3 baseColor;
uniform float opacity;

void main() {
    vec4 color = texture(inputTexture, TexCoord);
    FragColor = vec4(color.rgb, color.a * opacity);
}