
cbuffer VSInput  : register(b0)
{
	matrix mvp;
	matrix persp;
	matrix view;
	matrix model;
	matrix inv_model;
};

cbuffer LightingInfo : register(b1)
{
	matrix lightViewProj;
	matrix lightView;
}

struct VSOutput
{
	float3 world_position : WorldPos;
	float2 texture_coords : TexCord;
	float3 normal : Normal;
	float4 lightViewProj : LIGHT_VIEW_PROJ;
	float4 lightView : LIGHT_VIEW;
	float4 pos : SV_POSITION;
};

VSOutput main(float3 pos : Position, float3 normal : Normal, float2 txc : TexCord)
{
	VSOutput output;
	output.pos = mul(float4(pos, 1.0f), mvp);
	output.texture_coords = txc;
	output.world_position = mul(float4(pos, 1.0f), model).xyz;
	output.lightViewProj = mul(float4(output.world_position, 1.0), lightViewProj);
	output.lightView = mul(float4(output.world_position, 1.0), lightView);

	matrix nm = transpose(inv_model);
	float4 tt = mul(float4(normal, 0.0f), nm);
	output.normal = (float3)tt;

	// output.normal = normal;

	return output;
	// return float4(pos.x, pos.y, 0.0f, 1.0f);
}

