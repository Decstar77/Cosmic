
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
	float4 projectedDepthPos : PROJ_DEPTH;
	float4 viewDepthPos : VIEW_DEPTH;
	float4 pos : SV_POSITION;
};

VSOutput main(float3 pos : Position, float3 normal : Normal, float2 txc : TexCord)
{
	VSOutput output;

	output.pos = mul(float4(pos, 1.0f), mvp);
	output.projectedDepthPos = output.pos;

	output.viewDepthPos = mul(float4(pos, 1.0f), model);
	output.viewDepthPos = mul(output.viewDepthPos, view);

	return output;
}

