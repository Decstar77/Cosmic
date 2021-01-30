
cbuffer VSInput  : register(b0)
{
	matrix mvp;
	matrix inv_transform;
};

struct VSOutput
{
	float2 texture_coords : TexCord;
	float3 normal : Normal;
	float4 pos : SV_POSITION;
};

VSOutput main(float3 pos : Position, float3 normal : Normal, float2 txc : TexCord)
{
	VSOutput output;
	output.pos = mul(float4(pos, 1.0f), mvp);
	output.texture_coords = txc;

	matrix nm = transpose(inv_transform);
	float4 tt = mul(float4(normal, 0.0f), nm);
	output.normal = (float3)tt;

	// output.normal = normal;

	return output;
	// return float4(pos.x, pos.y, 0.0f, 1.0f);
}

