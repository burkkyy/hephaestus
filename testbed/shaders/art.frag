#version 450

layout (location = 0) in vec3 color;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform Push {
  mat4 transform;
  vec4 color;
  vec4 data;
} push;

vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263,0.416,0.557);

    return a + b*cos( 6.28318*(c*t+d) );
}

void main() {
  float time = push.data.z;
  
  // vec2 uv = (gl_FragCoord.xy * 2.0 - push.data.xy) / push.data.y;
  // vec2 uv0 = uv;
  // vec3 finalColor = vec3(0.0);

  // for(int i = 0; i < 4; i++){
  //   uv = fract(uv * 1.2) - 0.5;
    
  //   float d = length(uv) * exp(-length(uv0));

  //   vec3 col = palette(length(uv0) + i * 0.4 + time * 0.5);

  //   d = sin(d * 8.0 + time) / 8.0;
  //   d = abs(d);

  //   d = pow(0.01 / d, 2.0);
  
  //   finalColor += col * d;
  // }

  // outColor = vec4(finalColor, 1.0);

  // Circle example
  float dist = distance(gl_FragCoord.xy, push.data.xy / 2);

  float compare = sin(20 * time)*10 + 100;

  if (dist < compare) {
    discard;
  } else {
    outColor = push.color;
  }
}
