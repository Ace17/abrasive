#version 300 es

precision mediump float;

// Values that stay constant for the whole mesh.

// Input vertex data
in vec2 textPos;
in vec2 textUV;

// Output data; will be interpolated for each fragment
out vec2 UV;

void main()
{
  gl_Position = vec4(textPos.x, textPos.y, 0, 1);
  UV = textUV;
}
// vim: syntax=glsl
