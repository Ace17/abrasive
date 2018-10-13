#version 300 es

// Input vertex data
in vec4 vertexPos_model;
in vec2 vertexUV;
in vec3 a_normal;

// Output data; will be interpolated for each fragment
out vec2 UV;
out vec3 vNormal;

// Values that stay constant for the whole mesh
uniform mat4 MVP;

void main()
{
  gl_Position = MVP * vertexPos_model;
  UV = vertexUV;
  vNormal = a_normal;
}
// vim: syntax=c
