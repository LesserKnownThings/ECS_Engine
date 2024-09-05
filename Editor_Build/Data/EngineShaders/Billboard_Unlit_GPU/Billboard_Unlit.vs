#version 450

layout(location = 0) in vec3 inPosition;

struct Particle {
  vec4 positionLifetime;
  // Since w is not used in velocity I can use it for alpha
  vec4 velocityAlpha;
  // xyz color and w is bool useGravity
  vec4 colorUseGravity;
  vec4 scaleSpawnTime;
  mat4 worldModel;
  bool isLocal;
  bool isActive;
};

layout(std430, binding = 1) buffer Particles { Particle particles[]; };

layout(std140) uniform Matrices {
  mat4 projection;
  mat4 view;
};

uniform mat4 model;

out vec4 color;

void main() {
  Particle p = particles[gl_InstanceID];

  if (p.isLocal) {
    p.worldModel = model;
  }

  if (!p.isActive) {
    return;
  }

  vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
  vec3 up = vec3(view[0][1], view[1][1], view[2][1]);

  vec3 modelPos = (p.worldModel * vec4(p.positionLifetime.xyz, 1.0)).xyz;

  vec3 vertPos = modelPos + right * inPosition.x + up * inPosition.y;

  gl_Position = projection * view * vec4(vertPos, 1.0);

  color = vec4(p.colorUseGravity.xyz, p.velocityAlpha.w);
}