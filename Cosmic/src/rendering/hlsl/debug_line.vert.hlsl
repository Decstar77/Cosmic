
cbuffer VSInput
{
    matrix transform;
};


struct VSOutput
{    
    float4 pos : SV_POSITION;
};

VSOutput main(float3 pos : Position) 
{
    VSOutput output;
    output.pos = mul(float4(pos, 1.0f), transform);
    return output;
}

