#version 150

attribute vec3 a_position;
varying vec3 v_position;
attribute vec3 a_color;
varying vec4 v_color;
attribute vec3 a_normal;
varying vec3 v_normal;
attribute vec2 a_texcoords;
varying vec2 v_texcoords;

uniform mat4 v_viewMatrix;
uniform mat4 v_projectionMatrix;
uniform mat4 v_worldMatrix;

void main(void) {

    gl_Position = v_projectionMatrix * v_viewMatrix * v_worldMatrix * vec4(a_position, 1.0);
    v_color = vec4( 1.0);
    v_normal = (transpose(inverse(v_worldMatrix)) * vec4(a_normal, 0.0)).xyz;
    v_texcoords = a_texcoords;
    v_position = a_position;
}