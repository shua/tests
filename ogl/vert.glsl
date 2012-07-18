#version 330

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
out vec4 ex_Color;

uniform mat4 ModelMatrix;

void main(void) 
{
    gl_Position = ModelMatrix * in_Position;
//    gl_Position = in_Position;
//    ex_Color = vec4(1, 0, 0, 1);
    ex_Color = in_Color;
//    ex_Color = in_Position;
}
