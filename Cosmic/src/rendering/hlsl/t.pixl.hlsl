
Texture2D tex : register(t0);
SamplerState splr : register(s0);
//Texture2D tex;
//SamplerState splr;



float4 main(float2 texture_coords : TexCord, float3 normal : Normal) : SV_TARGET
{    
    float3 light_dir = float3(1, 1, 1);

    light_dir = normalize(light_dir);
    normal = normalize(normal);

    float d = max(dot(light_dir, normal), 0.1f) * 1.5f;

    float3 col = tex.Sample(splr, texture_coords).rgb * d;

    //return float4(tex.Sample(splr, texture_coords).a, 1.0f, 1.0f, 1.0f);
    // return tex.Sample(splr, texture_coords);
    return float4(col, 1.0f);
    // return float4(normal, 1.0f);
} 