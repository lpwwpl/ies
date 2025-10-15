#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

// 材质属性
struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// 光源属性
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Material material;
uniform Light light;

// IES纹理
uniform sampler2D iesTexture;

// 计算IES光照
vec3 calculateIESLighting(vec3 position, vec3 normal, vec3 viewDir)
{
    // 计算到光源的方向
    vec3 lightDir = normalize(light.position - position);
    float distance = length(light.position - position);
    
    // 计算法线点积
    float diff = max(dot(normal, lightDir), 0.0);
    
    // 计算球面坐标角度
    float theta = degrees(acos(lightDir.z));
    float phi = degrees(atan(lightDir.y, lightDir.x));
    if (phi < 0.0) phi += 360.0;
    
    // 将角度映射到纹理坐标
    vec2 texCoord = vec2(phi / 360.0, theta / 180.0);
    
    // 从IES纹理中查找光强
    float iesIntensity = texture(iesTexture, texCoord).r;
    
    // 计算衰减
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
    
    // 计算漫反射
    vec3 diffuse = light.color * diff * material.diffuse;
    
    // 计算镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.color * spec * material.specular;
    
    // 组合光照贡献
    return (diffuse + specular) * iesIntensity * light.intensity * attenuation;
}

void main()
{
    // 规范化法线
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(-FragPos); // 视图方向
    
    // 计算IES光照
    vec3 result = calculateIESLighting(FragPos, norm, viewDir);
    
    // 添加环境光
    //vec3 ambient = vec3(0.1) * material.diffuse;
    //result += ambient;
    
    // 输出最终颜色
    FragColor = vec4(result, 1.0);
}