/* Copyright 2012-2020 Matthew Reid
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef HENYEY_GREENSTEIN_H
#define HENYEY_GREENSTEIN_H

float oneOnFourPi = 0.07957747154594767;

vec4 henyeyGreenstein(float cosAngle, vec4 eccentricity) {
  vec4 eccentricity2 = eccentricity * eccentricity;
  return oneOnFourPi * ((vec4(1.0) - eccentricity2) / pow(vec4(1.0) + eccentricity2 - 2.0*eccentricity*cosAngle, vec4(3.0/2.0)));
}

float henyeyGreenstein(float cosAngle, float occentricity) {
  return henyeyGreenstein(cosAngle, vec4(occentricity)).x;
}

float watooHenyeyGreenstein(float cosAngle) {
  // Tuned to match The "Watoo" phase function for clouds, from Bouthors et al.
	// See http://wiki.nuaj.net/index.php?title=Clouds
  return dot(
    // TODO: 相位函数决定太阳的 size？
    henyeyGreenstein(cosAngle, vec4(-0.5, 0.3, 0.99, 0.0)),
    vec4(0.5, 0.5, 0.03, 0.0)
  );
}

#endif // HENYEY_GREENSTEIN_H
