#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D color1;
uniform sampler2D depth1;

uniform sampler2D color2;
uniform sampler2D depth2;

void main()
{
    float d1 = texture(depth1, vTexCoord).r;
    float d2 = texture(depth2, vTexCoord).r;

    vec3 c1 = texture(color1, vTexCoord).rgb;
    vec3 c2 = texture(color2, vTexCoord).rgb;

    vec3 resultColor = (d1 <= d2) ? c1 : c2;

    FragColor = vec4(resultColor, 1.0);
}
