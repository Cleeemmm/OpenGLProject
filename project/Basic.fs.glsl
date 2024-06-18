varying vec4 v_color;
varying vec2 v_texcoords;
uniform sampler2D u_sampler;
varying vec3 v_position;
uniform vec3 u_L;
varying vec3 v_N;
uniform vec3 u_Id;
uniform vec3 u_Is;
uniform vec3 u_Ks;
uniform float u_shininess;


vec3 diffuse(vec3 N, vec3 L, vec3 tex){
    float NdotL = max(0.0, dot(N, L));
    vec3 diffuseIntensity = u_Id * NdotL* tex;
    return diffuseIntensity;
}

vec4 specular(vec3 L, vec3 N, vec3 pos){
    vec3 R = reflect(L, N);
    vec3 V = normalize(vec3(0.0,0.0,2.0)-pos);
    float RdotV = max(0.0, pow(dot(R, V),50.0));
    return RdotV * vec4(u_Id,1.0) * v_color;
}

vec4 specularBlinn(vec3 L, vec3 N, vec3 pos){

    vec3 V = normalize(vec3(0.0,0.0,2.0)-pos);
    vec3 H = normalize(L+V);
    float RdotV =  pow(max(0.0,dot(H,N)),u_shininess);
    return RdotV * vec4(u_Is,1.0) * vec4(u_Ks,1.0);
}

void main(void) {
    vec3 N = normalize(v_N);
    vec3 L = normalize(-u_L);
    vec4 tex = texture2D(u_sampler, v_texcoords);
    //gl_FragColor = vec4(diffuse(N,L,tex.xyz),1.0) + specular(-L,N,v_position);
    gl_FragColor = vec4(diffuse(N,L,tex.xyz),1.0) + specularBlinn(L,N,v_position);
}