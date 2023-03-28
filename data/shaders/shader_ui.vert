#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
    mat4 proj_ui;
    vec2 texturesize;
} ubo;

layout(location = 0) in ivec2 inPosition;
layout(location = 1) in uvec3 inColor;
layout(location = 2) in uvec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.proj_ui * vec4(inPosition.xy, 0.0, 1.0);
    // gl_Position = vec4(vec2(inPosition.xy) / 600.0f, 0.0, 1.0);
    // gl_Position = vec4(float(inPosition.x) / 600.0, float(inPosition.y) / 900.0, 0.4, 1.0);
    fragColor = vec3(inColor)/255.0;
    fragTexCoord.x = float(inTexCoord.x) / ubo.texturesize.x;
    fragTexCoord.y = float(inTexCoord.y) / ubo.texturesize.y;
}
