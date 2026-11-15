#version 330 core

out vec4 FragColor;

const vec3 BACKGROUND = vec3(0.05, 0.05, 0.1);
const int  MAX_ITER = 128;

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - vec2(800.0, 600.0)) / 600.0;
    vec2 c = uv * 1.5 - vec2(0.5, 0.0);
    vec2 z = vec2(0.0);

    float i = float(MAX_ITER);

    for (int n = 0; n < MAX_ITER; n++) {
        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
        if (dot(z, z) > 4.0) {
            i = float(n);
            break;
        }
    }

    if (i >= float(MAX_ITER) - 0.5) {
        FragColor = vec4(BACKGROUND, 1.0);
        return;
    }

    vec3 col = 0.5 + 0.5 * cos(i * 0.15 + vec3(0.0, 2.0, 4.0));
    FragColor = vec4(col, 1.0);
}