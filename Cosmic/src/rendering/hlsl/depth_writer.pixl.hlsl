

float LinearizeDepth(float d, float zNear, float zFar)
{
	return (2.0f * zNear) / (zFar + zNear - d * (zNear - zFar));
}

float4 main(float4 projectedDepthPos : PROJ_DEPTH, float4 viewDepthPos : VIEW_DEPTH) : SV_TARGET
{
	float zNear = 1.0f;			// @HACK
	float zFar = 50.0f;			// @HACK

	float depthValue = projectedDepthPos.z / projectedDepthPos.w;

	float linearDepthValue = LinearizeDepth(projectedDepthPos.z, zNear, zFar);

	float4 color = float4(0, 0, 0, 1);
	if (depthValue < 0.9f)
	{
		color = float4(1.0, 0.0f, 0.0f, 1.0f);
	}

	if (depthValue > 0.9f)
	{
		color = float4(0.0, 1.0f, 0.0f, 1.0f);
	}

	if (depthValue > 0.995f)
	{
		color = float4(0.0, 0.0f, 1.0f, 1.0f);
	}


	//return color;
	//return float4(depthValue, viewDepthPos.xyz);
	return float4(depthValue, depthValue, depthValue, viewDepthPos.z);
	//return float4(linearDepthValue, linearDepthValue, linearDepthValue, 1);
}