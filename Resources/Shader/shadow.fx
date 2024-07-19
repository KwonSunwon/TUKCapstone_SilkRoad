#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 weight : WEIGHT;
    float4 indices : INDICES;
    
    row_major matrix matWorld : W;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0.f;
        
    if(g_int_0 == 1)
    {
        if (g_int_1 == 1)
            Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);

        output.pos = mul(float4(input.pos, 1.f), input.matWorld);
    }
    else
    {
        if (g_int_1 == 1)
            Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);

        output.pos = mul(float4(input.pos, 1.f), g_matWorld);
    }
    
    output.clipPos = output.pos;

    return output;
}

[maxvertexcount(3 * SHADOWMAP_COUNT)]
void GS_Main(triangle VS_OUT input[3], inout TriangleStream<GS_OUT> triStream)
{
    matrix shadowCameraVP[4] = { g_mat_0, g_mat_1, g_mat_2, g_mat_3 };
    
    for (uint i = 0; i < SHADOWMAP_COUNT; ++i)
    {
        GS_OUT output;
        output.RTIndex = i;

        for (uint j = 0; j < 3; ++j)
        {
            output.pos = mul(input[j].pos, shadowCameraVP[i]);

            triStream.Append(output);
        }
        triStream.RestartStrip();

    }
}

float4 PS_Main(GS_OUT input) : SV_Target
{
    //float3 color[4] = { float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 0, 1) };
    //return float4(input.pos.z / input.pos.w * color[input.RTIndex], 0.f);
    return float4(input.pos.z / input.pos.w, 0.f, 0.f, 0.f);
}

#endif