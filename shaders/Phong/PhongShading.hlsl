#include "./Common.hlsl"

struct VertexIn {
  float3 PosL : POSITION;
  float3 NormalL : NORMAL;
  float2 TexC : TEXCOORD;
  float3 TangentU : TANGENT;
};
struct VertexOut {
  float4 PosH : SV_POSITION;
  float4 ShadowPosH : POSITION0;
  float3 PosW : POSITION1;
  float4 SsaoPosH : POSITION2;
  float3 NormalW : NORMAL;
  float3 TangentW : TANGENT;
  float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin) {
  VertexOut vout = (VertexOut)0.0f;

  // Fetch the material data.
  MaterialData matData = gMaterialData[matIndex];

  // Transform to world space.
  float4x4 gWorld = mul(gRotation, mul(gScale, gTranslate));

  float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
  vout.PosW = posW.xyz;

  // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of
  // world matrix.
  vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

  vout.TangentW = mul(vin.TangentU, (float3x3)gWorld);

  // Transform to homogeneous clip space.
  vout.PosH = mul(posW, gViewProj);

  vout.SsaoPosH = mul(posW, gViewProjTex);

  // Output vertex attributes for interpolation across triangle.
  float4 texC = float4(vin.TexC, 0.0f, 1.0f);
  vout.TexC = mul(texC, matData.MatTransform).xy;

  vout.ShadowPosH = mul(posW, gShadowTransform);

  return vout;
}

float4 PS(VertexOut pin) : SV_Target {
  // Fetch the material data.
  MaterialData matData = gMaterialData[matIndex];
  float4 diffuseAlbedo = matData.DiffuseAlbedo;
  float3 fresnelR0 = matData.FresnelR0;
  float roughness = matData.Roughness;
  uint diffuseMapIndex = matData.DiffuseMapIndex;
  uint normalMapIndex = matData.NormalMapIndex;

  // Dynamically look up the texture in the array.
  diffuseAlbedo *=
      gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

  // Interpolating normal can unnormalize it, so renormalize it.
  pin.NormalW = normalize(pin.NormalW);

  float4 normalMapSample =
      gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
  float3 bumpedNormalW =
      NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

  if (normalMapIndex == -1)
    bumpedNormalW = pin.NormalW;

  // Vector from point being lit to eye.
  float3 toEyeW = normalize(gEyePosW - pin.PosW);

  // Finish texture projection and sample SSAO map.
  pin.SsaoPosH /= pin.SsaoPosH.w;
  float ambientAccess =
      gSsaoMap.Sample(gsamLinearClamp, pin.SsaoPosH.xy, 0.0f).r;

  // Light terms.
#ifdef SSAO
  float4 ambient = ambientAccess * gAmbientLight * diffuseAlbedo;
#else
  float4 ambient = gAmbientLight * diffuseAlbedo;
#endif

  // Only the first light casts a shadow.
  float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
#ifdef PCSS
  shadowFactor[0] = CalcShadowFactorPCSS(pin.ShadowPosH, pin.PosW,
                                         CalcBlockerSearchSize(pin.PosW));
#else
  shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);
#endif

  float shininess = (1.0f - roughness);
  if (normalMapIndex != -1)
    shininess *= normalMapSample.a;

  Material mat = {diffuseAlbedo, fresnelR0, shininess};
  float4 directLight = ComputeLighting(gLights, mat, pin.PosW, bumpedNormalW,
                                       toEyeW, shadowFactor);

#ifdef COLOR_GRADING
  directLight = ColorGrading(directLight);
#endif

  float4 litColor = directLight + ambient;

  // Add in specular reflections.
  if (diffuseMapIndex == 5 || diffuseMapIndex == 6) {
    float3 r = reflect(-toEyeW, bumpedNormalW);
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
    litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;
  }

  // Common convention to take alpha from diffuse albedo.
  litColor.a = saturate(diffuseAlbedo.a + 0.4f);

  return litColor;
  // return float4(ambientAccess, ambientAccess, ambientAccess, 1.0f);
  // return float4(shadowFactor.rrr, 1.0f);
}
