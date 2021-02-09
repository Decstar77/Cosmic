

float LinearizeDepth(float d, float zNear, float zFar)
{
	return (2.0f * zNear) / (zFar + zNear - d * (zNear - zFar));
}

float4 main(float4 depthPos : DEPTH) : SV_TARGET
{
	float zNear = 1.0f;			// @HACK
	float zFar = 50.0f;			// @HACK

	float depthValue = depthPos.z / depthPos.w;
	float linearDepthValue = LinearizeDepth(depthPos.z, zNear, zFar);

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
	return float4(depthValue, depthValue, depthValue, 1);
	//return float4(linearDepthValue, linearDepthValue, linearDepthValue, 1);
}