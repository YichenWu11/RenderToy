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
#define PCSS

#include "./LightUtil.hlsl"

cbuffer cbPerObject : register(b0) {
  float4x4 gRotation;
  float4x4 gScale;
  float4x4 gTranslate;
};

cbuffer cbMatIndex : register(b0, space1) {
  uint matIndex;
  /* for alignment */
  uint Pad0;
  uint Pad1;
  uint Pad2;
};

cbuffer cbPass : register(b1) {
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
  float3 gMainLightPosW;
  float gMainLightExtent; // 光源的宽(高)[假设光源为正方形面光源]
};

struct MaterialData {
  float4 DiffuseAlbedo;
  float3 FresnelR0;
  float Roughness;
  float4x4 MatTransform;
  uint DiffuseMapIndex;
  uint NormalMapIndex;
  uint MatPad1;
  uint MatPad2;
};

StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);

TextureCube gCubeMap : register(t0);
Texture2D gShadowMap : register(t1);
Texture2D gCgLutMap : register(t2);
Texture2D gSsaoMap : register(t3);

Texture2D gTextureMaps[50] : register(t4);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);

//---------------------------------------------------------------------------------------
// Transforms a normal map sample to world space.
//---------------------------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW,
                                float3 tangentW) {
  // Uncompress each component from [0,1] to [-1,1].
  float3 normalT = 2.0f * normalMapSample - 1.0f;

  // Build orthonormal basis.
  float3 N = unitNormalW;
  float3 T = normalize(tangentW - dot(tangentW, N) * N);
  float3 B = cross(N, T);

  float3x3 TBN = float3x3(T, B, N);

  // Transform from tangent space to world space.
  float3 bumpedNormalW = mul(normalT, TBN);

  return bumpedNormalW;
}

//---------------------------------------------------------------------------------------
// PCF for shadow mapping.
//---------------------------------------------------------------------------------------

// 计算平均遮挡物的搜索范围
int CalcBlockerSearchSize(float3 posW) {
  uint width, height, numMips;
  gShadowMap.GetDimensions(0, width, height, numMips);

  float n = 1.0f;
  float fovY = 3.1415926f / 4.0f * 3.0f;
  float h = 2.0f * n * tan(fovY / 2.0);

  float4x4 inv_light_world = {
      -gMainLightPosW.x,
      0.0,
      0.0,
      0.0,
      0.0,
      -gMainLightPosW.y,
      0.0,
      0.0,
      0.0,
      0.0,
      -gMainLightPosW.z,
      0.0,
      0.0,
      0.0,
      0.0,
      1.0,
  };

  float3 posLightSpace = mul(float4(posW, 1.0f), inv_light_world).xyz;
  float d = posLightSpace.z;

  if (d > -1.0f) {
    return 0;
  }
  d = -d;

  int maxSearchSize = 32;
  int minSearchSize = 3;
  float s = ((d - gNearZ) / (gFarZ - gNearZ));
  s = 1 - s;
  return minSearchSize + (maxSearchSize - minSearchSize) * s;
}

// 计算 kernel 尺寸
float CalcPCFKernelSize(float4 shadowPosH, float3 posW, int depth_search_size) {
  // Texel size.
  uint width, height, numMips;
  gShadowMap.GetDimensions(0, width, height, numMips);

  float n = 1.0f;
  float fovY = 3.1415926f / 4.0f * 3.0f;
  float h = 2.0f * n * tan(fovY / 2.0);

  int search_radius = depth_search_size / 2;

  shadowPosH.xyz /= shadowPosH.w;

  float4x4 light_world = {
      gMainLightPosW.x,
      0.0,
      0.0,
      0.0,
      0.0,
      gMainLightPosW.y,
      0.0,
      0.0,
      0.0,
      0.0,
      gMainLightPosW.z,
      0.0,
      0.0,
      0.0,
      0.0,
      1.0,
  };

  float3 light_pos = mul(float4(0, 0, 0, 1), light_world).xyz;
  float depth = length(posW - light_pos);

  // Depth in NDC space.
  // depth = shadowPosH.z;

  float blocker_depth = 0.0f;
  int cnt = 0;

  for (int i = -search_radius; i <= search_radius; ++i) {
    for (int j = -search_radius; j <= search_radius; ++j) {
      float2 offset = float2(i, j) / width;
      float depth_c =
          gShadowMap.SampleLevel(gsamPointClamp, shadowPosH.xy + offset, 0).r;
      if (depth_c < depth) {
        blocker_depth += depth_c;
        ++cnt;
      }
    }
  }

  if (cnt == 0) {
    return 1;
  }

  blocker_depth /= cnt;
  float w_penumbra = (depth - blocker_depth) * gMainLightExtent / blocker_depth;
  w_penumbra *= 16.0f;
  return w_penumbra;
}

float nrand(in float2 uv) {
  float2 noise =
      (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
  return abs(noise.x + noise.y) * 0.5;
}

#define N_SAMPLE 16
static float2 poissonDisk[16] = {
    float2(-0.94201624, -0.39906216),  float2(0.94558609, -0.76890725),
    float2(-0.094184101, -0.92938870), float2(0.34495938, 0.29387760),
    float2(-0.91588581, 0.45771432),   float2(-0.81544232, -0.87912464),
    float2(-0.38277543, 0.27676845),   float2(0.97484398, 0.75648379),
    float2(0.44323325, -0.97511554),   float2(0.53742981, -0.47373420),
    float2(-0.26496911, -0.41893023),  float2(0.79197514, 0.19090188),
    float2(-0.24188840, 0.99706507),   float2(-0.81409955, 0.91437590),
    float2(0.19984126, 0.78641367),    float2(0.14383161, -0.14100790)};

float CalcShadowFactorPCSS(float4 shadowPosH, float3 posW, float filter_size) {
  uint width, height, numMips;
  gShadowMap.GetDimensions(0, width, height, numMips);

  int sampleCount = N_SAMPLE;
  float search_radius = filter_size / width / 2.0f;

  shadowPosH.xyz /= shadowPosH.w;

  float4x4 light_world = {
      gMainLightPosW.x,
      0.0,
      0.0,
      0.0,
      0.0,
      gMainLightPosW.y,
      0.0,
      0.0,
      0.0,
      0.0,
      gMainLightPosW.z,
      0.0,
      0.0,
      0.0,
      0.0,
      1.0,
  };

  // Depth in world space.
  float3 light_posW = mul(float4(0, 0, 0, 1), light_world).xyz;
  float depth = length(posW - light_posW);

  depth = shadowPosH.z;

  // Texel size.
  float dx = 1.0f / (float)width;

  float percentLit = 0.0f;
  float rot_theta = nrand(shadowPosH.xy);
  float cos_theta = cos(rot_theta);
  float sin_theta = sin(rot_theta);
  float2x2 rot_mat = float2x2(cos_theta, sin_theta, -sin_theta, cos_theta);
  for (int i = 0; i < sampleCount; ++i) {
    float2 p = mul(poissonDisk[i], rot_mat);
    float2 offset = float2(p * search_radius);
    percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow,
                                                shadowPosH.xy + offset, depth);
  }

  return percentLit / sampleCount;
}

// initial pcf
float CalcShadowFactor(float4 shadowPosH) {
  // Complete projection by doing division by w.
  shadowPosH.xyz /= shadowPosH.w;

  // Depth in NDC space.
  float depth = shadowPosH.z;

  uint width, height, numMips;
  gShadowMap.GetDimensions(0, width, height, numMips);

  // Texel size.
  float dx = 1.0f / (float)width;

  float percentLit = 0.0f;

  const float2 offsets[9] = {
      float2(-dx, -dx),  float2(0.0f, -dx),  float2(dx, -dx),
      float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
      float2(-dx, +dx),  float2(0.0f, +dx),  float2(dx, +dx)};

  [unroll] for (int i = 0; i < 9; ++i) {
    percentLit +=
        gShadowMap
            .SampleCmpLevelZero(gsamShadow, shadowPosH.xy + offsets[i], depth)
            .r;
  }

  return percentLit / 9.0f;
  // return gShadowMap.SampleCmpLevelZero(gsamShadow, shadowPosH.xy, depth).r;
}

//---------------------------------------------------------------------------------------
// Color Grading
//---------------------------------------------------------------------------------------
float4 ColorGrading(float4 color) {
  uint width, height, numMips;
  gCgLutMap.GetDimensions(0, width, height, numMips);

  float2 lutSize = float2(width, height);

  float blockNum = lutSize.x / lutSize.y;
  float blockIndexL = floor(color.b * blockNum);
  float blockIndexR = ceil(color.b * blockNum);

  float lutCoordXL =
      (blockIndexL * lutSize.y + color.r * lutSize.y) / lutSize.x;
  float lutCoordXR =
      (blockIndexR * lutSize.y + color.r * lutSize.y) / lutSize.x;

  float lutCoorY = color.g;

  float2 lutCoordL = float2(lutCoordXL, lutCoorY);
  float2 lutCoordR = float2(lutCoordXR, lutCoorY);

  float4 lutcolorL = gCgLutMap.Sample(gsamAnisotropicWrap, lutCoordL);
  float4 lutcolorR = gCgLutMap.Sample(gsamAnisotropicWrap, lutCoordR);

  float weight = frac(color.b * lutSize.y);

  float4 outColor = lerp(lutcolorL, lutcolorR, weight);

  color.r = saturate(color.r * outColor.r);
  color.g = saturate(color.g * outColor.g);
  color.b = saturate(color.b * outColor.b);
  color.a = saturate(color.a * outColor.a);

  return color;
}