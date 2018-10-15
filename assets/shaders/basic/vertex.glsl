#version 300 es

precision mediump float;

// Values that stay constant for the whole mesh.
uniform mat4 M; // model
uniform mat4 MVP; // model/view/projection

// Input vertex data
in vec3 vertexPos;
in vec3 vertexNormal;
in vec2 vertexUV;
in vec2 vertexUV2;

// Output data; will be interpolated for each fragment
out vec3 worldPos;
out vec3 worldNormal;
out vec2 UV;
out vec2 UV2;

vec3 rotateToWorld(vec3 v)
{
  return (M * vec4(v, 1) - M * vec4(0, 0, 0, 1)).xyz;
}

void main()
{
  gl_Position = MVP * vec4(vertexPos, 1);
  UV = vertexUV;
  UV2 = vertexUV2;
  worldPos = (M * vec4(vertexPos, 1)).xyz;
  worldNormal = rotateToWorld(vertexNormal);
}
// vim: syntax=glsl
