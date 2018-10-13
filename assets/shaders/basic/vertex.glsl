#version 300 es

// Input vertex data
in vec3 vertexPos;
in vec3 vertexNormal;
in vec2 vertexUV;

// Output data; will be interpolated for each fragment
out vec2 UV;
out vec3 vNormal;

void main()
{
  gl_Position = vec4(vertexPos, 1);
  UV = vertexUV;
  vNormal = vertexNormal;
}
// vim: syntax=glsl
