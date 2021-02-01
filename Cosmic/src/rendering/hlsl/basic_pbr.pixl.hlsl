#define MAX_DIRECTIONAL_LIGHT_COUNT 2
#define MAX_SPOT_LIGHT_COUNT 8
#define MAX_POINT_LIGHT_COUNT 16

cbuffer LightingInfo : register(b0)
{
	float3 direction;
	float3 viewPos;

	float3 lightPos;
	float3 lightColour;

	//float3 viewPos;
	int4 lightCounts;	// @NOTE: Directional, spot, point

	float4 dDirections[MAX_DIRECTIONAL_LIGHT_COUNT];
	float4 dColours[MAX_DIRECTIONAL_LIGHT_COUNT];

	float4 sPositions[MAX_SPOT_LIGHT_COUNT];		// @NOTE: z = innter cuttoff
	float4 sDirections[MAX_SPOT_LIGHT_COUNT];		// @NOTE: y = innter cuttoff
	float4 sColours[MAX_SPOT_LIGHT_COUNT];

	float4 pPositions[MAX_POINT_LIGHT_COUNT];
	float4 pColours[MAX_POINT_LIGHT_COUNT];
}

Texture2D tex : register(t0);
SamplerState splr : register(s0);

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
};

#define PI 3.14159265359
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

	float3 radiance = light.colour;

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
	float cutoff = cos(0.20944f);
	float outerCutOff = cos(0.261799f);
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
float4 main(float3 world_position : WorldPos, float2 texture_coords : TexCord, float3 normal : Normal) : SV_TARGET
{
#if 1
	PixelInfo pinfo;
	pinfo.normal = normalize(normal);
	pinfo.worldPos = world_position;
	pinfo.toView = viewPos - pinfo.worldPos;

	Material material;
	//material.albedo = float3(1, 0, 0);//pow(tex.Sample(splr, texture_coords).rgb, float3(2.2, 2.2, 2.2));
	material.albedo = pow(tex.Sample(splr, texture_coords).rgb, float3(2.2, 2.2, 2.2));
	material.roughness = 0.25f;
	material.metallic = 0.0f;
	material.F0 = float3(0.04, 0.04, 0.04); //(like plastic)

	PointLight plight;
	plight.colour = lightColour * 100;
	plight.position = lightPos;
	float3 Lo = CalculatePointLighting(pinfo, material, plight);

	plight.colour = lightColour * 100;
	plight.position = lightPos + float3(2, 0, 0);
	Lo += CalculatePointLighting(pinfo, material, plight);


	DirectionalLight dlight;
	dlight.colour = lightColour * 10;
	dlight.direction = direction;
	//float3 Lo = CalculateDirectionalLighting(pinfo, material, dlight);

	SpotLight slight;
	slight.position = lightPos;
	slight.colour = lightColour * 10;
	slight.direction = float3(0, -1, 0);
	//float3 Lo = CalculateSpotLighting(pinfo, material, slight);


	float3 color = Lo;

	// HDR tonemapping
	color = color / (color + float3(1.0, 1.0, 1.0));
	// gamma correct
	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

	float4 pixel_colour = float4(color, 1.0);

	return pixel_colour;

#else

	float3 light_dir = direction;

	float3 diffuse = tex.Sample(splr, texture_coords).rgb;
	float3 spec = float3(0.5f, 0.5f, 0.5f);
	float3 norm = normalize(normal);


	light_dir = normalize(light_dir);
	normal = normalize(normal);

	float d = max(dot(light_dir, normal), 0.1f) * 1.5f;

	float3 col = tex.Sample(splr, texture_coords).rgb * d;

	//return float4(tex.Sample(splr, texture_coords).a, 1.0f, 1.0f, 1.0f);
	// return tex.Sample(splr, texture_coords);
	return float4(col, 1.0f);
	// return float4(normal, 1.0f);
#endif
}