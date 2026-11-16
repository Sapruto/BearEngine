#version 450 core

layout(std140, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 projection;
    vec4 viewPos;
};

layout(std140, binding = 2) uniform LightUBO {
    vec4 dirDirection;
    vec4 dirColor;
    vec4 lightParams;
};

in vec3 vNormal;
in vec3 vFragPos;
in vec4 vColor;

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(viewPos.xyz - vFragPos);

    vec3 result = vec3(0.0);

    if (lightParams.y == 1.0) {
        vec3 L = normalize(-dirDirection.xyz);
        float ndotl = max(dot(N, L), 0.0);
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(V, R), 0.0), 32.0);

        vec3 ambient = 0.1 * dirColor.rgb;
        vec3 diffuse = ndotl * dirColor.rgb;
        vec3 specular = 0.5 * spec * dirColor.rgb;

        result += ambient + diffuse + specular;
    }

 FragColor = vec4(result * vColor.rgb, 1.0);
}