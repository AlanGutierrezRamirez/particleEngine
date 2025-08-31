#version 330

layout (location = 0) in vec2 aModelPos;
layout (location = 1) in vec2 aWorldPos;

void main()
{
    gl_Position = vec4(aModelPos + aWorldPos, 0.0, 1.0);
}