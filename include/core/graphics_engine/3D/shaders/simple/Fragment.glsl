#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 color;      
uniform vec3 viewPos;    

uniform vec3 lightPos[16];  
uniform vec3 lightColor[16];     
uniform int lightCount;            

uniform vec3 dirLightDirection;    
uniform vec3 dirLightColor;       
uniform bool useDirLight;       

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    float ambientStrength = 0.1;
    float specularStrength = 0.5;
    float shininess = 32.0;
    
    vec3 result = vec3(0.0);
    
    if (useDirLight) {
        vec3 dirLightDir = normalize(-dirLightDirection);
        float diff = max(dot(norm, dirLightDir), 0.0);
        
        vec3 reflectDir = reflect(-dirLightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        
        vec3 ambient = ambientStrength * dirLightColor;
        vec3 diffuse = diff * dirLightColor;
        vec3 specular = specularStrength * spec * dirLightColor;
        
        result += (ambient + diffuse + specular);
    }
    
    for (int i = 0; i < lightCount; i++) {
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        
        float distance = length(lightPos[i] - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        vec3 ambient = ambientStrength * lightColor[i];
        vec3 diffuse = diff * lightColor[i];
        vec3 specular = specularStrength * spec * lightColor[i];
        
        result += (ambient + diffuse + specular) * attenuation;
    }
    
    result *= color;
    
    FragColor = vec4(result, 1.0);
}