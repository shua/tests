#version 330

in vec4 ex_Color;
in vec2 ex_UV;

out vec4 out_Color;

uniform sampler2D TextureSampler;

void main(void)
{
    out_Color = texture2D(TextureSampler, ex_UV).rgba;
}

