// test vertex shder
#version 450

// vertex attribs input 
layout( location = 0 ) in vec2 inPosition;
layout( location = 1 ) in vec3 inColor;

// fragment shader outpur
layout( location = 0 ) out vec3 fragColor;

void main() 
{
    gl_Position = vec4( inPosition, 0.0, 1.0);
    fragColor = inColor;
}
