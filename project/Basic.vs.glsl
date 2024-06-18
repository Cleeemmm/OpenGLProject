attribute vec3 a_position;
varying vec3 v_position;
attribute vec3 a_color;
varying vec4 v_color;
attribute vec3 a_N;
varying vec3 v_N;
attribute vec2 a_texcoords;
varying vec2 v_texcoords;

void main(void) {

    gl_Position = vec4(a_position, 1.0);
    v_color = vec4( 1.0);
    v_N = a_N;
    v_texcoords = a_texcoords;
    v_position = a_position;
}