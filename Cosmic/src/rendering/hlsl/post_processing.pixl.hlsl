
Texture2D tex : register(t0);
SamplerState splr : register(s0);

float4 main(float2 texture_coords : TexCord) : SV_TARGET
{
	return float4 (tex.Sample(splr, texture_coords).rgb, 1);
//return float4(texture_coords.xy, 0.0f, 1.0f);
}