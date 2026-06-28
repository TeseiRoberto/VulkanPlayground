#version 450

layout (location = 0) out vec3 outColor;

// Raw vertices hard coded in the shader for now...
vec2 positions[3] = {
        vec2(-0.5, 0.5),        // Bottom left vertex
        vec2(0.0, -0.5),        // Top vertex
        vec2(0.5, 0.5)          // Bottom right vertex
};


vec3 colors[3] = {
        vec3(1.0, 0.0, 0.0),    // Bottom left vertex
        vec3(0.0, 1.0, 0.0),    // Top vertex
        vec3(0.0, 0.0, 1.0)     // Bottom right vertex
};


void main()
{
        gl_Position = vec4( positions[gl_VertexIndex], 0.0, 1.0f );
        outColor = colors[gl_VertexIndex];
}
