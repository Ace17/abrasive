#version 300 es

precision mediump float;

// Interpolated values from the vertex shader
in vec2 UV; // diffuse
in vec2 UV2; // lightmap
in vec3 vNormal;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh
uniform sampler2D DiffuseTex;
uniform sampler2D LightmapTex;
uniform float AmbientLight;

void main()
{
  float light = length(texture2D(LightmapTex, UV2).xyz);
  color = vec4(texture2D(DiffuseTex, UV).rgb * light * AmbientLight, 1);
}

// vim: syntax=glsl
