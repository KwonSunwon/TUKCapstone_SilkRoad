#ifndef _LIGHTING_FX_
#define _LIGHTING_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

struct PS_OUT
{
    float4 diffuse : SV_Target0;
    float4 specular : SV_Target1;
};

// [Directional Light]
// g_int_0 : Light index
// g_tex_0 : Position RT
// g_tex_1 : Normal RT
// g_tex_2 : Shadow RT
// g_mat_0 : ShadowCamera VP
// Mesh : Rectangle

VS_OUT VS_DirLight(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}






PS_OUT PS_DirLight(VS_OUT input)
{
    // 안개 파라미터
    float fogStart = 5000.0f; // 안개가 시작되는 거리
    float fogEnd = 10000.0f; // 안개가 완전히 덮치는 거리
    float fogDensity = 0.0f; // 선형 안개 밀도
    float4 fogColor = float4(0.0, 0.0, 0.0, 1.0); // 안개의 색상
    
    PS_OUT output = (PS_OUT)0;

    float3 viewPos = g_tex_0.Sample(g_sam_0, input.uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);

    float3 viewNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;

    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);

    // 그림자
    if (length(color.diffuse) != 0)
    {
        float4 worldPos = mul(float4(viewPos.xyz, 1.f), g_matViewInv);
        float shadowFactor = CalcShadowFactor(worldPos);
        float shadowColor = lerp(float3(0, 0, 0), float3(1, 1, 1), shadowFactor);
        
        color.diffuse *= shadowColor;
        color.specular *= shadowColor;
    }
    if (viewPos.z > fogEnd)
        clip(-1);
   
    float fogFactor = (fogEnd - viewPos.z) / (fogEnd - fogStart);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
        
    output.diffuse = lerp(color.diffuse + color.ambient, fogColor, 1 - fogFactor);
    output.specular = lerp(color.specular, fogColor, 1 - fogFactor);

    

    return output;
}

// [Point Light]
// g_int_0 : Light index
// g_tex_0 : Position RT
// g_tex_1 : Normal RT
// g_vec2_0 : RenderTarget Resolution
// Mesh : Sphere

VS_OUT VS_PointLight(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

PS_OUT PS_PointLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;

    // input.pos = SV_Position = Screen 좌표
    float2 uv = float2(input.pos.x / g_vec2_0.x, input.pos.y / g_vec2_0.y);
    float3 viewPos = g_tex_0.Sample(g_sam_0, uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);

    int lightIndex = g_int_0;
    float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
    float distance = length(viewPos - viewLightPos);
    if (distance > g_light[lightIndex].range)
        clip(-1);

    float3 viewNormal = g_tex_1.Sample(g_sam_0, uv).xyz;

    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);

    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;

    return output;
}

// [Final]
// g_tex_0 : Diffuse Color Target
// g_tex_1 : Diffuse Light Target
// g_tex_2 : Specular Light Target
// Mesh : Rectangle

VS_OUT VS_Final(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}

float4 PS_Final(VS_OUT input) : SV_Target
{
    float4 output = (float4)0;

    float4 lightPower = g_tex_1.Sample(g_sam_0, input.uv);
    if (lightPower.x == 0.f && lightPower.y == 0.f && lightPower.z == 0.f)
        clip(-1);

    float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    float4 specular = g_tex_2.Sample(g_sam_0, input.uv);

    output = (color * lightPower) + specular;
    if (g_int_0 == 1)
    {
        float2 center = float2(0.5f, 1.0f);
        float2 diff = input.uv - center;
        float distance = sqrt(diff.x * diff.x + diff.y * diff.y);
        
        
        if (distance < g_float_0)
            output = output * float4(1.f, 0.5f, 0.5f, 1.f);

    }
    
    return output;
}

#endif