cbuffer cbPerObject : register(b0)
{
	float4x4 gRotation; 
	float4x4 gScale; 
	float4x4 gTranslate; 
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
};

// struct MaterialData
// {
// 	float4   DiffuseAlbedo;
// 	float3   FresnelR0;
// 	float    Roughness;
// 	float4x4 MatTransform;
// 	uint     DiffuseMapIndex;
// 	uint     NormalMapIndex;
// 	uint     MatPad1;
// 	uint     MatPad2;
// };

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentU : TANGENT;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gRotation*gScale*gTranslate);
    vout.PosH = mul(posW, gViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = float4(vin.NormalL, 1.0f);
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}
