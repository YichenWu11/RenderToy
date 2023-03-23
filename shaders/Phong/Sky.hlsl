#include "Common.hlsl"

struct VertexIn {
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
};

struct VertexOut {
    float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};

VertexOut VS(VertexIn vin) {
    VertexOut vout;

    // Use local vertex position as cubemap lookup vector.
    vout.PosL = vin.PosL;

    // Transform to world space.
    float4x4 gWorld = mul(gRotation, mul(gScale, gTranslate));
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    // Always center sky about camera.
    posW.xyz += gEyePosW;

    // Set z = w so that z/w = 1 (i.e., skydome always on far plane).
    vout.PosH = mul(posW, gViewProj).xyww;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target {
// #ifdef COLOR_GRADING
//     return ColorGrading(gCubeMap.Sample(gsamLinearWrap, pin.PosL));
// #else
//     return gCubeMap.Sample(gsamLinearWrap, pin.PosL);
// #endif

#ifdef SKYBOX
    // return Blur(pin.PosL); 
    float4 ret_color = gCubeMap.Sample(gsamLinearWrap, pin.PosL);

    float3 rgb_color = ret_color.xyz;
    // tone mapping
    rgb_color = Uncharted2Tonemap(rgb_color * 4.5f);
    rgb_color = rgb_color * (1.0f / Uncharted2Tonemap(float3(11.2f, 11.2f, 11.2f)));
    rgb_color = float3(pow(rgb_color.x, 1.0 / 2.2), pow(rgb_color.y, 1.0 / 2.2), pow(rgb_color.z, 1.0 / 2.2));

    // return float4(rgb_color, ret_color.a);
    return float4(ACESToneMapping(ret_color.xyz, 1.1), ret_color.a);
    // return ret_color;
#else
    return float4(0.5, 0.5, 0.5, 1.0);
#endif
}
