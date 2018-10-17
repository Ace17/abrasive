#version 300 es

precision mediump float;

// Values that stay constant for the whole mesh
uniform sampler2D DiffuseTex;
uniform sampler2D LightmapTex;
uniform sampler2D NormalmapTex;
uniform float AmbientLight;
uniform vec3 LightPos; // in world space
uniform mat4 M; // model

// Interpolated values from the vertex shader
in vec2 UV; // diffuse
in vec2 UV2; // lightmap
in vec3 worldPos;
in vec3 worldNormal;

// Ouput data
out vec4 color;

vec3 rotateToWorld(vec3 v)
{
  return (M * vec4(v, 1) - M * vec4(0, 0, 0, 1)).xyz;
}

void main()
{
  float light = 0.0;

  vec3 N2 = rotateToWorld(texture2D(NormalmapTex, UV).xyz);
  vec3 N = normalize(worldNormal + 2.0 * N2 - 1.0);

  // contribution from the moving point light
  float dist = length(LightPos - worldPos);
  float intensity = 2.0;
  vec3 toLight = normalize(LightPos - worldPos);
  float attenuation = 1.0 / (dist*dist);
  light += max(0.0, dot(toLight, N)) * intensity * attenuation;

  // contribution from the lightmap
  light += length(texture2D(LightmapTex, UV2).xyz) * AmbientLight;

  color = vec4(texture2D(DiffuseTex, UV).rgb * light, 1);
}

// vim: syntax=glsl
