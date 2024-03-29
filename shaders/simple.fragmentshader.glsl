#version 330 core

in vec2 UV;
out vec4 color;
uniform sampler2D myTextureSampler;

void main()
{
	color.rgb = texture(myTextureSampler, UV).rgb;
	color.a = texture(myTextureSampler, UV).a;
}
