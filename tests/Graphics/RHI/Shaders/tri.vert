#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

const float HUE_SHIFT = 0.0;
const vec3  PALETTE = vec3(0.0, 2.0, 4.0);

void main() {
    float t = length(aPos.xy);
    vColor  = 0.5 + 0.5 * cos(t * 6.2831 + PALETTE + HUE_SHIFT);

    gl_Position = vec4(aPos, 1.0);
}