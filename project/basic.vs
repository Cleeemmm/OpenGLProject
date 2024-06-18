attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texture;
varying vec4 v_color;
varying mat4 v_viewMatrix;
varying mat4 v_projectionMatrix;
varying mat4 v_worldMatrix;

void main(void) {
    gl_Position = v_projectionMatrix * v_viewMatrix * (v_worldMatrix) * vec4(a_position, 1.0);
    v_color = vec4(1.0);
}