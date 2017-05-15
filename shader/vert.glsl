#version 300 es

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;

out vec3 color;
out vec3 eyeDirection;
out vec3 normal;
out vec3 lightDirection;
out vec3 mposition;
out vec3 lPos;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPosition;

void main(void)
{
  lPos = vec3(2,4,-3);
  gl_Position = MVP * vec4(in_position, 1);
  color = in_color;
  mposition = (M * vec4(in_position,1)).xyz;
  eyeDirection = vec3(0,0,0) - (V * M * vec4(in_position,1)).xyz;
  lightDirection = (V * vec4(lightPosition,1)).xyz + eyeDirection;

  mat4 normalMatrix = V * transpose(inverse(M));
  normal = normalize(V * M * vec4(in_normal,0)).xyz;
  //normal = normalize(V * M * vec4(0,1,0,0)).xyz;
  //normal = normalize(normalMatrix * vec4(in_normal, 0.0)).xyz;
}