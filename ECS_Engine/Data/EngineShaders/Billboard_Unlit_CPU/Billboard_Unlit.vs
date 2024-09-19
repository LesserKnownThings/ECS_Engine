#version 450

layout(location = 0) in vec4 inPosition;

layout(location = 1) in vec3 offsetPos;

layout(std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};

uniform mat4 model = mat4(1);

out vec4 color;

void main() {
  vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
  vec3 up = vec3(view[0][1], view[1][1], view[2][1]);

  vec3 modelPos = (model * vec4(offsetPos, 1.0)).xyz;

  vec3 vertPos = modelPos + right * inPosition.x + up * inPosition.y;

  gl_Position = projection * view * vec4(vertPos, 1.0);

  color = vec4(1.0);
}