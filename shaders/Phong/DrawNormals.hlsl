#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 0
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

// Include common HLSL code.
#include "./Common.hlsl"

struct VertexIn {
  float3 PosL : POSITION;
  float3 NormalL : NORMAL;
  float2 TexC : TEXCOORD;
  float3 TangentU : TANGENT;
};

struct VertexOut {
  float4 PosH : SV_POSITION;
  float3 NormalW : NORMAL;
  float3 TangentW : TANGENT;
  float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin) {
  VertexOut vout = (VertexOut)0.0f;

  // Fetch the material data.
  MaterialData matData = gMaterialData[matIndex];

  // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of
  // world matrix.
  float4x4 gWorld = mul(gRotation, mul(gScale, gTranslate));
  vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
  vout.TangentW = mul(vin.TangentU, (float3x3)gWorld);

  // Transform to homogeneous clip space.
  float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
  vout.PosH = mul(posW, gViewProj);

  // Output vertex attributes for interpolation across triangle.
  float4 texC = float4(vin.TexC, 0.0f, 1.0f);
  vout.TexC = mul(texC, matData.MatTransform).xy;

  return vout;
}

float4 PS(VertexOut pin) : SV_Target {
  // Fetch the material data.
  MaterialData matData = gMaterialData[matIndex];
  float4 diffuseAlbedo = matData.DiffuseAlbedo;
  uint diffuseMapIndex = matData.DiffuseMapIndex;
  uint normalMapIndex = matData.NormalMapIndex;

  // Dynamically look up the texture in the array.
  diffuseAlbedo *=
      gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

#ifdef ALPHA_TEST
  // Discard pixel if texture alpha < 0.1.  We do this test as soon
  // as possible in the shader so that we can potentially exit the
  // shader early, thereby skipping the rest of the shader code.
  clip(diffuseAlbedo.a - 0.1f);
#endif

  // Interpolating normal can unnormalize it, so renormalize it.
  pin.NormalW = normalize(pin.NormalW);

  // 为了SSAO而使用插值顶点法线

  // Write normal in view space coordinates
  float3 normalV = mul(pin.NormalW, (float3x3)gView);
  return float4(normalV, 0.0f);
}
