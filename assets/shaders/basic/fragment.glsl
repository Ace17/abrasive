#version 300 es

precision mediump float;

// Interpolated values from the vertex shader
in vec2 UV;
in vec3 vNormal;

// Ouput data
out vec4 color;

void main()
{
  color = vec4(1.0, 0.5, 0.3, 1.0);
}

// vim: syntax=glsl
