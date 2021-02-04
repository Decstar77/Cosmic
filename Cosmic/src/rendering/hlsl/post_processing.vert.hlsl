struct VSOutput
{
	float2 texture_coords : TexCord;
	float4 pos : SV_POSITION;
};

VSOutput main(float3 pos : Position, float3 normal : Normal, float2 txc : TexCord)
{
	VSOutput output;
	output.pos = float4(pos, 1);
	output.texture_coords = txc;
	return output;
}

