#version 300 es

precision mediump float;
in vec3 mposition;
in vec3 color;
in vec3 normal;
in vec3 eyeDirection;
in vec3 lightDirection;
in vec3 lPos;
out vec3 fragmentColor;

//uniform vec3 lightPosition;

void main(void)
{
  vec3 lightColor = vec3(1,1,1);
  float lightPower = 10.0f;

  float distance = length(lPos - mposition);
  
  vec3 n = normalize(normal);
  vec3 l = normalize(lightDirection);

  float cosTheta = clamp(dot(n,l),0.f,1.f);

  vec3 E = normalize(eyeDirection);
  vec3 R = reflect(-l,n);

  float cosAlpha = clamp(dot(E,R),0.f,1.f);
  fragmentColor = color * lightColor * lightPower * cosTheta / (distance * distance) + vec3(0.3,0.3,0.3) * lightColor * lightPower * pow(cosAlpha,5.f) / (distance * distance);
  //fragmentColor = vec3(1,0,1);
}
