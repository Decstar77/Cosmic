
cbuffer VSInput  : register(b0)
{
	matrix mvp;
	matrix persp;
	matrix view;
	matrix model;
	matrix inv_model;
};

struct VSOutput
{
	float4 depthPos : DEPTH;
	float4 pos : SV_POSITION;
};

VSOutput main(float3 pos : Position, float3 normal : Normal, float2 txc : TexCord)
{
	VSOutput output;
	output.pos = mul(float4(pos, 1.0f), mvp);
	output.depthPos = output.pos;
	return output;
}

