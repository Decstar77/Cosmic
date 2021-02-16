#define MAX_DIRECTIONAL_LIGHT_COUNT 2
#define MAX_SPOT_LIGHT_COUNT 8
#define MAX_POINT_LIGHT_COUNT 16

cbuffer LightingInfo : register(b0)
{
#if 0
	float3 direction;
	float3 viewPos;

	float3 lightPos;
	float3 lightColour;
#else
	float3 viewPos;
	float4 shadowParams;
	int4 lightCounts;	// @NOTE: Directional, spot, point

	struct {
		float3 direction;
		float3 colour;
	} directionalLights[MAX_DIRECTIONAL_LIGHT_COUNT];

	struct {
		float4 position;		// @NOTE: z = inner cuttoff
		float4 direction;		// @NOTE: y = outter cuttoff
		float3 colour;
	} spotLights[MAX_SPOT_LIGHT_COUNT];

	struct {
		float3 position;
		float3 colour;
	} pointLights[MAX_POINT_LIGHT_COUNT];
#endif
}

Texture2D tex : register(t0);
Texture2D shadowMap : register(t1);

SamplerState splr : register(s0);
SamplerState shadowSplr : register(s1);

struct VSInput
{
	float3 world_position : WorldPos;
	float2 texture_coords : TexCord;
	float3 normal : Normal;
	float4 lightViewProj : LIGHT_VIEW_PROJ;
	float4 lightView : LIGHT_VIEW;
};

struct DirectionalLight
{
	float3 colour;
	float3 direction;
};

struct PointLight
{
	float3 colour;
	float3 position;
};

struct SpotLight
{
	float3 colour;
	float3 direction;
	float3 position;
	float inner;
	float outter;
};

struct Material
{
	float3 albedo;
	float3 F0;
	float roughness;
	float metallic;
};

struct PixelInfo
{
	float3 normal;
	float3 worldPos;
	float3 toView;
	float4 lightViewProj;
	float4 lightView;
};

#define PI 3.14159265359

static float2 poisson128[128] = {
	float2(-0.9406119, 0.2160107),
	float2(-0.920003, 0.03135762),
	float2(-0.917876, -0.2841548),
	float2(-0.9166079, -0.1372365),
	float2(-0.8978907, -0.4213504),
	float2(-0.8467999, 0.5201505),
	float2(-0.8261013, 0.3743192),
	float2(-0.7835162, 0.01432008),
	float2(-0.779963, 0.2161933),
	float2(-0.7719588, 0.6335353),
	float2(-0.7658782, -0.3316436),
	float2(-0.7341912, -0.5430729),
	float2(-0.6825727, -0.1883408),
	float2(-0.6777467, 0.3313724),
	float2(-0.662191, 0.5155144),
	float2(-0.6569989, -0.7000636),
	float2(-0.6021447, 0.7923283),
	float2(-0.5980815, -0.5529259),
	float2(-0.5867089, 0.09857152),
	float2(-0.5774597, -0.8154474),
	float2(-0.5767041, -0.2656419),
	float2(-0.575091, -0.4220052),
	float2(-0.5486979, -0.09635002),
	float2(-0.5235587, 0.6594529),
	float2(-0.5170338, -0.6636339),
	float2(-0.5114055, 0.4373561),
	float2(-0.4844725, 0.2985838),
	float2(-0.4803245, 0.8482798),
	float2(-0.4651957, -0.5392771),
	float2(-0.4529685, 0.09942394),
	float2(-0.4523471, -0.3125569),
	float2(-0.4268422, 0.5644538),
	float2(-0.4187512, -0.8636028),
	float2(-0.4160798, -0.0844868),
	float2(-0.3751733, 0.2196607),
	float2(-0.3656596, -0.7324334),
	float2(-0.3286595, -0.2012637),
	float2(-0.3147397, -0.0006635741),
	float2(-0.3135846, 0.3636878),
	float2(-0.3042951, -0.4983553),
	float2(-0.2974239, 0.7496996),
	float2(-0.2903037, 0.8890813),
	float2(-0.2878664, -0.8622097),
	float2(-0.2588971, -0.653879),
	float2(-0.2555692, 0.5041648),
	float2(-0.2553292, -0.3389159),
	float2(-0.2401368, 0.2306108),
	float2(-0.2124457, -0.09935001),
	float2(-0.1877905, 0.1098409),
	float2(-0.1559879, 0.3356432),
	float2(-0.1499449, 0.7487829),
	float2(-0.146661, -0.9256138),
	float2(-0.1342774, 0.6185387),
	float2(-0.1224529, -0.3887629),
	float2(-0.116467, 0.8827716),
	float2(-0.1157598, -0.539999),
	float2(-0.09983152, -0.2407187),
	float2(-0.09953719, -0.78346),
	float2(-0.08604223, 0.4591112),
	float2(-0.02128129, 0.1551989),
	float2(-0.01478849, 0.6969455),
	float2(-0.01231739, -0.6752576),
	float2(-0.005001599, -0.004027164),
	float2(0.00248426, 0.567932),
	float2(0.00335562, 0.3472346),
	float2(0.009554717, -0.4025437),
	float2(0.02231783, -0.1349781),
	float2(0.04694207, -0.8347212),
	float2(0.05412609, 0.9042216),
	float2(0.05812819, -0.9826952),
	float2(0.1131321, -0.619306),
	float2(0.1170737, 0.6799788),
	float2(0.1275105, 0.05326218),
	float2(0.1393405, -0.2149568),
	float2(0.1457873, 0.1991508),
	float2(0.1474208, 0.5443151),
	float2(0.1497117, -0.3899909),
	float2(0.1923773, 0.3683496),
	float2(0.2110928, -0.7888536),
	float2(0.2148235, 0.9586087),
	float2(0.2152219, -0.1084362),
	float2(0.2189204, -0.9644538),
	float2(0.2220028, -0.5058427),
	float2(0.2251696, 0.779461),
	float2(0.2585723, 0.01621339),
	float2(0.2612841, -0.2832426),
	float2(0.2665483, -0.6422054),
	float2(0.2939872, 0.1673226),
	float2(0.3235748, 0.5643662),
	float2(0.3269232, 0.6984669),
	float2(0.3425438, -0.1783788),
	float2(0.3672505, 0.4398117),
	float2(0.3755714, -0.8814359),
	float2(0.379463, 0.2842356),
	float2(0.3822978, -0.381217),
	float2(0.4057849, -0.5227674),
	float2(0.4168737, -0.6936938),
	float2(0.4202749, 0.8369391),
	float2(0.4252189, 0.03818182),
	float2(0.4445904, -0.09360636),
	float2(0.4684285, 0.5885228),
	float2(0.4952184, -0.2319764),
	float2(0.5072351, 0.3683765),
	float2(0.5136194, -0.3944138),
	float2(0.519893, 0.7157083),
	float2(0.5277841, 0.1486474),
	float2(0.5474944, -0.7618791),
	float2(0.5692734, 0.4852227),
	float2(0.582229, -0.5125455),
	float2(0.583022, 0.008507785),
	float2(0.6500257, 0.3473313),
	float2(0.6621304, -0.6280518),
	float2(0.6674218, -0.2260806),
	float2(0.6741871, 0.6734863),
	float2(0.6753459, 0.1119422),
	float2(0.7083091, -0.4393666),
	float2(0.7106963, -0.102099),
	float2(0.7606754, 0.5743545),
	float2(0.7846709, 0.2282225),
	float2(0.7871446, 0.3891495),
	float2(0.8071781, -0.5257092),
	float2(0.8230689, 0.002674922),
	float2(0.8531976, -0.3256475),
	float2(0.8758298, -0.1824844),
	float2(0.8797691, 0.1284946),
	float2(0.926309, 0.3576975),
	float2(0.9608918, -0.03495717),
	float2(0.972032, 0.2271516) };

#define totalBlockers 128
#define poissonPCFSamples 128
//#define lightSize 0.27f
//#define lightSize2 0.27f
#define zNear 1.0f
#define zFar 50.0f
#define bias 0.005f

// ----------------------------------------------------------------------------
void FindBlockers(out float sumBlockers, out float numBlockers, float2 uv, float currentDepth, float currentZ)
{
	sumBlockers = 0;
	numBlockers = 0;

	float lightSize = shadowParams.y / 22.0f;

	float searchRad = lightSize * (currentZ - zNear) / currentZ;
	float biasedDepth = currentDepth - bias;

	for (int i = 0; i < (int)totalBlockers; ++i)
	{
		float2 uvOffset = uv + (poisson128[i] * searchRad);
		float storedDepth = shadowMap.Sample(shadowSplr, uvOffset).r;

		if (storedDepth < biasedDepth)
		{
			sumBlockers += storedDepth;
			++numBlockers;
		}
	}
}
// ----------------------------------------------------------------------------
float PoissonPCFFilter(float2 uv, float currentDepth, float filterRadius)
{
	float result = 0.0f;
	for (int i = 0; i < (int)poissonPCFSamples; ++i)
	{
		float2 uvOffset = uv + (poisson128[i] * filterRadius);
		float storedDepth = shadowMap.Sample(shadowSplr, uvOffset).r;
		result += (currentDepth - bias) > storedDepth ? 1.0 : 0.0f;
	}

	return result / float(poissonPCFSamples);
}
// ----------------------------------------------------------------------------
float ShadowCalculation(float4 lightViewProj, float4 lightView, out float3 debugColour)
{
	debugColour = float3(0, 0, 0);
#define PCF 0
#define PCSS 1
	// Perspective divide {X}[-1, 1];{Y}[-1, 1];{Z}[0, 1]
	float3 projCoords = lightViewProj.xyz / lightViewProj.w;

	// To text coords [0, 1]
	projCoords.x = lightViewProj.x / 2.0f + 0.5f;
	projCoords.y = -lightViewProj.y / 2.0f + 0.5f;
	// We don't have to do a Z component (/2 + .5) because in d3d depth is already [0, 1], in opengl it's 
	// [-1, -1] so in gl code you see a (/2 + .5) for the Z commponent

	float shadow = 0.0f;

	if ((saturate(projCoords.x) == projCoords.x)
		&& (saturate(projCoords.y) == projCoords.y))
	{
		float2 uv = projCoords.xy;
		float currentDepth = projCoords.z;
#if PCF
		float2 texelSize;
		//texelSize = 1.0 / float2(4096, 4096);
		shadowMap.GetDimensions(texelSize.x, texelSize.y);
		texelSize = 1.0 / texelSize;

		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				float2 tc = uv + float2(x, y) * texelSize;
				float pcfDepth = shadowMap.Sample(shadowSplr, tc).r;

				shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0f;
			}
		}

		shadow /= 9;
#elif PCSS

		float currentZ = lightView.z;

		// ------------------------
		// STEP 1: blocker search
		// ------------------------

		float sumBlockers;
		float numBlockers;
		FindBlockers(sumBlockers, numBlockers, uv, currentDepth, currentZ);

		debugColour = float3(numBlockers / (float)totalBlockers,
			numBlockers / (float)totalBlockers,
			numBlockers / (float)totalBlockers);

		if (numBlockers == 0)
		{
			return 1.0f;
		}

		// ------------------------
		// STEP 2: penumbra size
		// ------------------------

		float lightSize = shadowParams.x;

		float averageBlockerDepth = sumBlockers / numBlockers;

		// Orthographic projection only
		float averageBlockerZ = zNear + (zFar - zNear) * averageBlockerDepth;

		// Penumbra radius 
		float penumbraRadius = (currentZ - averageBlockerZ) / averageBlockerZ;

		// Filter radius
		float filterRadius = penumbraRadius * lightSize * zNear / currentZ;

		// ------------------------
		// STEP 3: filtering
		// ------------------------

		shadow = PoissonPCFFilter(uv, currentDepth, filterRadius);
#else
		float closestDepth = shadowMap.Sample(shadowSplr, projCoords.xy).r;
		shadow = (currentDepth - bias) > closestDepth ? 1.0f : 0.0f;
#endif
	}

	return (1.0f - shadow);
}
// ----------------------------------------------------------------------------
float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// ----------------------------------------------------------------------------
float3 CalculateDirectionalLighting(PixelInfo info, Material material, DirectionalLight light)
{
	// calculate per-light radiance
	float3 N = info.normal;
	float3 V = info.toView;

	float3 L = normalize(-light.direction);
	float3 H = normalize(V + L);

	float3 debugColour;
	float shadow = ShadowCalculation(info.lightViewProj, info.lightView, debugColour);
	//return debugColour;

	float3 radiance = light.colour * shadow;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, material.roughness);
	float G = GeometrySmith(N, V, L, material.roughness);
	float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), material.F0);

	// Combining
	float3 nominator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	float3 specular = nominator / max(denominator, 0.00001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

	// kS is equal to Fresnel
	float3 kS = F;

	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	float3 kD = float3(1.0, 1.0, 1.0) - kS;

	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - material.metallic;

	// scale light by NdotL
	float NdotL = max(dot(N, L), 0.0);

	// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	return (kD * material.albedo / PI + specular) * radiance * NdotL;
}
// ----------------------------------------------------------------------------
float3 CalculateSpotLighting(PixelInfo info, Material material, SpotLight light)
{
	// calculate per-light radiance
	float3 N = info.normal;
	float3 V = info.toView;

	float3 L = normalize(light.position - info.worldPos);
	float3 H = normalize(V + L);

	float theta = dot(L, normalize(-light.direction));
	float cutoff = light.inner;
	float outerCutOff = light.outter;
	float epsilon = cutoff - outerCutOff;
	float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

	float distance = length(light.position - info.worldPos);
	float attenuation = 1.0 / (distance * distance); // @NOTE: Use dot for sqrd 
	float3 radiance = light.colour * attenuation;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, material.roughness);
	float G = GeometrySmith(N, V, L, material.roughness);
	float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), material.F0);

	// Combining
	float3 nominator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	float3 specular = nominator / max(denominator, 0.00001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

	// kS is equal to Fresnel
	float3 kS = F;

	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	float3 kD = float3(1.0, 1.0, 1.0) - kS;

	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - material.metallic;

	// scale light by NdotL
	float NdotL = max(dot(N, L), 0.0);

	// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	return ((kD * material.albedo / PI + specular) * radiance * NdotL) * intensity;
}
// ----------------------------------------------------------------------------
float3 CalculatePointLighting(PixelInfo info, Material material, PointLight light)
{
	// calculate per-light radiance
	float3 N = info.normal;
	float3 V = info.toView;

	float3 L = normalize(light.position - info.worldPos);
	float3 H = normalize(V + L);

	float distance = length(light.position - info.worldPos);
	float attenuation = 1.0 / (distance * distance); // @NOTE: Use dot for sqrd 
	float3 radiance = light.colour * attenuation;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, material.roughness);
	float G = GeometrySmith(N, V, L, material.roughness);
	float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), material.F0);

	// Combining
	float3 nominator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	float3 specular = nominator / max(denominator, 0.00001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

	// kS is equal to Fresnel
	float3 kS = F;

	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	float3 kD = float3(1.0, 1.0, 1.0) - kS;

	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - material.metallic;

	// scale light by NdotL
	float NdotL = max(dot(N, L), 0.0);

	// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	return (kD * material.albedo / PI + specular) * radiance * NdotL;
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
float4 main(VSInput vsInput) : SV_TARGET
{
	PixelInfo pinfo;
	pinfo.normal = normalize(vsInput.normal);
	pinfo.worldPos = vsInput.world_position;
	pinfo.toView = viewPos - pinfo.worldPos;
	pinfo.lightViewProj = vsInput.lightViewProj;
	pinfo.lightView = vsInput.lightView;

	Material material;
	//material.albedo = float3(1, 0, 0);//pow(tex.Sample(splr, texture_coords).rgb, float3(2.2, 2.2, 2.2));
	material.albedo = pow(tex.Sample(splr, vsInput.texture_coords).rgb, float3(2.2, 2.2, 2.2));
	material.roughness = 0.25f;
	material.metallic = 0.0f;
	material.F0 = float3(0.04, 0.04, 0.04); //(like plastic)

#if 0
	PointLight plight;
	plight.colour = lightColour * 100;
	plight.position = lightPos;
	float3 Lo = CalculatePointLighting(pinfo, material, plight);

	DirectionalLight dlight;
	dlight.colour = lightColour * 10;
	dlight.direction = direction;
	//float3 Lo = CalculateDirectionalLighting(pinfo, material, dlight);

	SpotLight slight;
	slight.position = lightPos;
	slight.colour = lightColour * 10;
	slight.direction = float3(0, -1, 0);
	//float3 Lo = CalculateSpotLighting(pinfo, material, slight);
#else
	int directionalLightCount = lightCounts.x;
	int spotLightCount = lightCounts.y;
	int pointLightCount = lightCounts.z;

	float3 Lo = float3(0,0,0);

	for (int dindex = 0; dindex < directionalLightCount; dindex++)
	{
		DirectionalLight dlight;
		dlight.direction = directionalLights[dindex].direction;
		dlight.colour = directionalLights[dindex].colour;

		Lo += CalculateDirectionalLighting(pinfo, material, dlight);
	}

	for (int sindex = 0; sindex < spotLightCount; sindex++)
	{
		SpotLight slight;
		slight.position = spotLights[sindex].position.xyz;
		slight.direction = spotLights[sindex].direction.xyz;

		//slight.inner = cos(0.20944f);
		//slight.outter = cos(0.261799f);

		slight.inner = spotLights[sindex].position.w;
		slight.outter = spotLights[sindex].direction.w;

		slight.colour = spotLights[sindex].colour;
		Lo += CalculateSpotLighting(pinfo, material, slight);
	}

	for (int pindex = 0; pindex < pointLightCount; pindex++)
	{
		PointLight plight;
		plight.colour = pointLights[pindex].colour * 100;
		plight.position = pointLights[pindex].position;
		Lo += CalculatePointLighting(pinfo, material, plight);
	}

#endif

	float3 color = Lo;

	// HDR tonemapping
	color = color / (color + float3(1.0, 1.0, 1.0));
	// gamma correct
	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

	float4 pixel_colour = float4(color, 1.0);

	return pixel_colour;
}