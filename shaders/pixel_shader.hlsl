
Texture2D texDiffuse : register(t0);
SamplerState texSampler : register(s0);

cbuffer LightPosition : register(b0)
{
	float4 lightPosition;
	float4 cameraPosition;
};

cbuffer PhongValues : register(b1)
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float shininess;
};

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 WorldPos  : TEXCOORD1;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
//	return float4(input.Normal * 0.5 + 0.5, 1);

	float3 N = input.Normal;
	float3 L = normalize(lightPosition.xyz - input.WorldPos);
	
	float diffValue = max(dot(L, N), 0);
	
	float3 R = normalize(reflect(-L, N));
	float3 V = normalize(cameraPosition.xyz - input.WorldPos);

	float specValue = pow(max(dot(R, V), 0), shininess);

	float4 texColor = texDiffuse.Sample(texSampler, input.TexCoord * 100);
	return (ambient * texColor) + (texColor * diffuse * diffValue) + (specular * specValue);

	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
//	return float4(input.TexCoord, 0, 1);
}