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

void main()
{
  float light = length(texture2D(LightmapTex, UV2).xyz);
  color = texture2D(DiffuseTex, UV) * light;
}

// vim: syntax=glsl
