// ies_spotlight.frag
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 LightDir;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform float lightIntensity;
uniform float lightAngle;
uniform float lightPenumbra;

uniform sampler2D iesTexture;
uniform sampler2D diffuseTexture;

void main() {
    // 基础材质属性
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightDir);
    
    // IES光照计算
    vec3 toLight = normalize(lightPosition - FragPos);
    vec3 toSpot = normalize(-lightDirection);
    
    float angle = acos(dot(toLight, toSpot));
    float cutoff = radians(lightAngle);
    float penumbra = radians(lightPenumbra);
    
    // IES纹理采样 - 根据角度计算UV坐标
    float verticalAngle = angle / cutoff;
    float horizontalAngle = atan(lightDir.x, lightDir.z) / (2.0 * 3.14159265) + 0.5;
    
    vec2 iesUV = vec2(horizontalAngle, verticalAngle);
    float iesFactor = texture(iesTexture, iesUV).r;
    
    // 聚光灯衰减
    float theta = dot(lightDir, normalize(-lightDirection));
    float epsilon = cutoff - penumbra;
    float intensity = clamp((theta - cutoff) / epsilon, 0.0, 1.0);
    intensity *= iesFactor * lightIntensity;
    
    // 漫反射
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;
    
    // 组合结果
    vec3 result = (diffuse + specular) * intensity;
    FragColor = vec4(result, 1.0);
}