#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 mvp;
uniform float minValue;
uniform float maxValue;

out vec3 fragColor;

vec3 valueToColor(float value) {
    float normalized = (value - minValue) / (maxValue - minValue);
    normalized = clamp(normalized, 0.0, 1.0);
    
    if (normalized < 0.25) {
        return vec3(0.0, normalized * 4.0, 1.0);
    } else if (normalized < 0.5) {
        return vec3(0.0, 1.0, 1.0 - (normalized - 0.25) * 4.0);
    } else if (normalized < 0.75) {
        return vec3((normalized - 0.5) * 4.0, 1.0, 0.0);
    } else {
        return vec3(1.0, 1.0 - (normalized - 0.75) * 4.0, 0.0);
    }
}

void main() {
    gl_Position = mvp * vec4(position, 1.0);
    gl_PointSize = 3.0;
    fragColor = valueToColor(position.z);
}