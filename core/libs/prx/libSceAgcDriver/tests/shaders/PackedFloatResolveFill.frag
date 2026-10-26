#version 450

layout(push_constant) uniform Push { uint words[8]; } params;
layout(location = 0) out uvec4 color;

void main() {
    color = uvec4(params.words[gl_SampleID], 0u, 0u, 0u);
}
