#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// #define COLOR_GRADING
// #define CARTOON
// #define SSAO

#include "./LightUtil.hlsl"

cbuffer cbPerObject : register(b0)
{
	float4x4 gRotation; 
	float4x4 gScale; 
	float4x4 gTranslate; 
};

cbuffer cbMatIndex : register(b0, space1)
{
    uint matIndex;
    /* for alignment */
    uint Pad0;
    uint Pad1;
    uint Pad2;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float4x4 gViewProjTex;
    float4x4 gShadowTransform;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;

    Light gLights[MaxLights];
};

struct MaterialData
{
	float4   DiffuseAlbedo;
	float3   FresnelR0;
	float    Roughness;
	float4x4 MatTransform;
	uint     DiffuseMapIndex;
	uint     NormalMapIndex;
	uint     MatPad1;
	uint     MatPad2;
};

StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);

TextureCube gCubeMap : register(t0);
Texture2D gShadowMap : register(t1);
Texture2D gCgLutMap  : register(t2);
Texture2D gSsaoMap   : register(t3);

Texture2D gTextureMaps[50] : register(t4);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

//---------------------------------------------------------------------------------------
// Transforms a normal map sample to world space.
//---------------------------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
	float3 normalT = 2.0f*normalMapSample - 1.0f;

	// Build orthonormal basis.
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N)*N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
	float3 bumpedNormalW = mul(normalT, TBN);

	return bumpedNormalW;
}

//---------------------------------------------------------------------------------------
// PCF for shadow mapping.
//---------------------------------------------------------------------------------------
float CalcShadowFactor(float4 shadowPosH)
{
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    gShadowMap.GetDimensions(0, width, height, numMips);

    // Texel size.
    float dx = 1.0f / (float)width;

    float percentLit = 0.0f;
     
    const float2 offsets[9] =
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    [unroll]
    for(int i = 0; i < 9; ++i)
    {
        percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }
    
    return percentLit / 9.0f;
    // return gShadowMap.SampleCmpLevelZero(gsamShadow, shadowPosH.xy, depth).r;
}

//---------------------------------------------------------------------------------------
// Color Grading
//---------------------------------------------------------------------------------------
float4 ColorGrading(float4 color)
{
    uint width, height, numMips;
    gCgLutMap.GetDimensions(0, width, height, numMips);

    float2 lutSize = float2(width, height);

    float blockNum = lutSize.x / lutSize.y;
    float blockIndexL = floor(color.b * blockNum);
    float blockIndexR = ceil(color.b * blockNum);

    float lutCoordXL = (blockIndexL * lutSize.y + color.r * lutSize.y) / lutSize.x;
    float lutCoordXR = (blockIndexR * lutSize.y + color.r * lutSize.y) / lutSize.x;

    float lutCoorY = color.g;

    float2 lutCoordL = float2(lutCoordXL, lutCoorY);
    float2 lutCoordR = float2(lutCoordXR, lutCoorY);

    float4 lutcolorL = gCgLutMap.Sample(gsamAnisotropicWrap, lutCoordL);
    float4 lutcolorR = gCgLutMap.Sample(gsamAnisotropicWrap, lutCoordR);

    float weight = frac(color.b * lutSize.y);

    float4 outColor = lerp(lutcolorL, lutcolorR, weight);

    color.r = saturate(color.r*outColor.r);
    color.g = saturate(color.g*outColor.g);
    color.b = saturate(color.b*outColor.b);
    color.a = saturate(color.a*outColor.a);

    return color;
}