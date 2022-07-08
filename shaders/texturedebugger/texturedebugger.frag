#version 410 core

uniform sampler2D u_texture;

in vec2 uv;

out vec4 colorOut;

void main() {
    colorOut = texture( u_texture, uv );
    colorOut.a = 1.0;
    if (colorOut.g == 0 && colorOut.b == 0) {
        colorOut.rgb = vec3(colorOut.r);
    }
    gl_FragDepth = 0.0;
}
