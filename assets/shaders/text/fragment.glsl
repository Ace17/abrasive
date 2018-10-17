#version 300 es

precision mediump float;

// Values that stay constant for the whole mesh
uniform sampler2D DiffuseTex;
uniform float TextLight;

// Interpolated values from the vertex shader
in vec2 UV; // diffuse

// Ouput data
out vec4 color;

void main()
{
  color = texture(DiffuseTex, UV) * TextLight;
}

// vim: syntax=glsl
