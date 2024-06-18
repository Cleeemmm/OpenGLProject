varying vec4 v_color;
varying vec2 v_texcoords;
uniform sampler2D u_sampler;
varying vec2 v_position;
uniform vec3 u_L;
varying vec3 v_N;
uniform vec3 u_Id;

vec3 diffuse(vec3 N, vec3 L, vec3 tex){
    float NdotL = max(0.0, dot(N, L));
    vec3 diffuseIntensity = u_Id * NdotL* tex;
    return diffuseIntensity;
}

vec4 specular(vec3 L, vec3 N, vec2 pos){
    vec3 R = reflect(L, N);
    vec3 V = normalize(vec3(0.0,0.0,2.0)-vec3(pos,0.0));
    float RdotV = max(0.0, pow(dot(R, V),100.0));
    return RdotV * vec4(u_Id,1.0) * v_color;
}

vec4 specularBlinn(vec3 L, vec3 N, vec2 pos){

    vec3 V = normalize(vec3(0.0,0.0,2.0)-vec3(pos,0.0));
    vec3 H = normalize(L+V);
    float RdotV = max(0.0, pow(dot(H,N),100.0));
    return RdotV * vec4(u_Id,1.0) * v_color;
}

void main(void) {
    vec3 N = normalize(v_N);
    vec3 L = normalize(-u_L);
    vec4 tex = texture2D(u_sampler, v_texcoords);
    //gl_FragColor = vec4(diffuse(N,L,tex.xyz),1.0) + specular(-L,N,v_position);
    gl_FragColor = vec4(diffuse(N,L,tex.xyz),1.0) + specularBlinn(L,N,v_position);
}