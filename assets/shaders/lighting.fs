#version 330

// Фрагментный шейдер: одно точечное освещение от Солнца + мягкий ambient.
// colDiffuse и texture0 подставляет raylib из материала модели.

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 sunPos;   // позиция Солнца в мировых координатах
uniform float ambient; // доля фонового света [0..1]

out vec4 finalColor;

void main()
{
    vec4 texel = texture(texture0, fragTexCoord);
    vec3 base = texel.rgb * colDiffuse.rgb * fragColor.rgb;

    vec3 n = normalize(fragNormal);
    vec3 l = normalize(sunPos - fragPosition);
    float diff = max(dot(n, l), 0.0);

    vec3 lit = base * (ambient + (1.0 - ambient) * diff);
    finalColor = vec4(lit, texel.a * colDiffuse.a);
}
