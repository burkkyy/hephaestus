#version 450

layout (location = 0) in vec3 color;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform Push {
  mat4 transform;
  vec4 color;
  vec4 data;
} push;

void main() {
  vec2 uv = (gl_FragCoord.xy * 2.0 - push.data.xy) / push.data.y;

  vec3 col = vec3(1.0, 2.0, 3.0);

  float d = length(uv);

  d = sin(d * 5.0) / 5.0;
  d = 0.02 / abs(d);

  col *= d;

  outColor = vec4(col, 1.0);

  // Circle example
  // float dist = distance(gl_FragCoord.xy, push.data.xy / 2);

  // if (dist < 200) {
  //   discard;
  // } else {
  //   outColor = push.color;
  // }
}
