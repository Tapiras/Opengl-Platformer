#version 430 core

// input 

// output
layout (location = 0) out vec2 textureCoordsOut;


void main()
{
    //genarating Vertices on the GPU
    // mostly because we have a 2D Engine


    // OpenGL coordinates:
    // -1/ 1            1/ 1
    // -1/-1            1/-1      
    vec2 vertices[6] = 
    {
        // Top left
        vec2(-0.5,  0.5),

        // Bottom left
        vec2(-0.5,  -0.5),

        // Top right
        vec2(0.5,   0.5),

        // Top right
        vec2(0.5,   0.5),

        // Bottom left
        vec2(-0.5,  -0.5),

        // Bottom right
        vec2(0.5,   -0.5)
    };

    float left      = 0.0;
    float top     = 0.0;
    float right       = 16.0;
    float bottom    = 16.0;

    vec2 textureCoords[6] = 
    {
        vec2(left, top),
        vec2(left, bottom),
        vec2(right, top),
        vec2(right, top),
        vec2(left, bottom),
        vec2(right, bottom),
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
    textureCoordsOut = textureCoords[gl_VertexID];
}