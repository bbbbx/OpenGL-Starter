#version 410 core

vec2 positions[3] = vec2[3](
    vec2( -1, -1 ),
    vec2(  3, -1 ),
    vec2( -1,  3 )
);

out vec2 uv;

void main() {
    gl_Position = vec4( positions[gl_VertexID], 0, 1 );

    uv = gl_Position.xy * vec2( 0.5 ) + vec2( 0.5 );
}
