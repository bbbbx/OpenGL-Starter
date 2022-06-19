#version 410 core

// FIXME: 1/(4pi) 是 0.7853981633974483
float oneOnFourPi = 0.0795774715459;

vec4 henyeyGreenstein(float cosAngle, vec4 eccentricity) {
  vec4 eccentricity2 = eccentricity * eccentricity;
  return oneOnFourPi * ((vec4(1.0) - eccentricity2) / pow(vec4(1.0) + eccentricity2 - 2.0*eccentricity*cosAngle, vec4(3.0/2.0)));
}

float henyeyGreenstein(float cosAngle, float occentricity) {
  return henyeyGreenstein(cosAngle, vec4(occentricity)).x;
}

float watooHenyeyGreenstein(float cosAngle) {
  return dot(
    henyeyGreenstein(cosAngle, vec4(-0.5, 0.3, 0.96, 0.0)),
    vec4(0.5, 0.5, 0.03, 0.0)
  );
}