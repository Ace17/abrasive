#version 300 es

// Input vertex data
in vec3 vertexPos;
in vec3 vertexNormal;
in vec2 vertexUV;
in vec2 vertexUV2;

// Output data; will be interpolated for each fragment
out vec2 UV;
out vec2 UV2;
out vec3 vNormal;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main()
{
  gl_Position = MVP * vec4(vertexPos, 1);
  UV = vertexUV;
  UV2 = vertexUV2;
  vNormal = vertexNormal;
}
// vim: syntax=glsl
