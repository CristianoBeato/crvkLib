// Test fragment shaders 
#version 450

// from vertex shader input 
layout( location = 0 ) in vec3 fragColor;

// output to frame buffer 
layout( location = 0 ) out vec4 outColor;

void main( void )
{
    outColor = vec4( fragColor, 1.0 );
}
