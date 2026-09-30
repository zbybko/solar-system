#version 100
precision mediump float;
varying vec3 fragPosition;
varying vec3 fragNormal;
varying vec2 fragTexCoord;
varying vec4 fragColor;
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 sunPos;
uniform float ambient;
void main()
{
    vec4 texel = texture2D(texture0, fragTexCoord);
    vec3 base = texel.rgb * colDiffuse.rgb * fragColor.rgb;
    vec3 n = normalize(fragNormal);
    vec3 l = normalize(sunPos - fragPosition);
    float diff = max(dot(n, l), 0.0);
    vec3 lit = base * (ambient + (1.0 - ambient) * diff);
    gl_FragColor = vec4(lit, texel.a * colDiffuse.a);
}
