#version 300 es

precision mediump float;

// Values that stay constant for the whole mesh
uniform sampler2D DiffuseTex;
uniform sampler2D LightmapTex;
uniform float AmbientLight;
uniform vec3 LightPos; // in world space

// Interpolated values from the vertex shader
in vec2 UV; // diffuse
in vec2 UV2; // lightmap
in vec3 worldPos;
in vec3 worldNormal;

// Ouput data
out vec4 color;

void main()
{
  float light = 0.0;

  // contribution from the moving point light
  float dist = length(LightPos - worldPos);
  float intensity = 1.0;
  vec3 toLight = normalize(LightPos - worldPos);
  float attenuation = 1.0 / (dist*dist);
  light += max(0.0, dot(toLight, worldNormal)) * intensity * attenuation;

  // contribution from the ambient light
  light += AmbientLight;

  // contribution from the lightmap
  light *= length(texture2D(LightmapTex, UV2).xyz);

  color = vec4(texture2D(DiffuseTex, UV).rgb * light, 1);
}

// vim: syntax=glsl
