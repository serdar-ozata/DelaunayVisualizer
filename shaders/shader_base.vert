#version 450

layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec3 fragColor;

// Transformation matrix (sent via uniform buffer)
layout(set = 0, binding = 0) uniform UBO {
    mat4 transform;
    vec4 color;
} ubo;

void main() {
    vec4 pos = vec4(inPosition, 0.0, 1.0);
    gl_Position = ubo.transform * pos; // Apply transformation
    fragColor = ubo.color.rgb; // Pass color to fragment shader
}
