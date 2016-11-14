#version 400
// ----------------------------------------- Header ------------------------------------------
#ifndef SFX_GLSL_4
	#define SFX_GLSL_4
#endif 
#ifndef _MAYA_
	#define _MAYA_
#endif 



#define float2 vec2
#define float3 vec3
#define float4 vec4
#define float3x3 mat3
#define float4x4 mat4
#ifdef COMPILING_VS
	#define OUTIN out 
#else 
	#define OUTIN in 
#endif 
#ifndef half 
	#define half float 
	#define half2 float2 
	#define half3 float3 
	#define half4 float4 
#endif 


uniform float2 ShadowFilterTaps[10] = float2[] (float2(-0.84052, -0.073954), float2(-0.326235, -0.40583), float2(-0.698464, 0.457259), float2(-0.203356, 0.620585), float2(0.96345, -0.194353), float2(0.473434, -0.480026), float2(0.519454, 0.767034), float2(0.185461, -0.894523), float2(0.507351, 0.064963), float2(-0.321932, 0.595435));



// ----------------------------------- Per Frame --------------------------------------
uniform float4x4 viewI;

uniform 	bool MayaHwFogEnabled : HardwareFogEnabled = false; 
uniform 	int MayaHwFogMode : HardwareFogMode = 0; 
uniform 	float MayaHwFogStart : HardwareFogStart = 0.0; 
uniform 	float MayaHwFogEnd : HardwareFogEnd = 100.0; 
uniform 	float MayaHwFogDensity : HardwareFogDensity = 0.1; 
uniform 	float4 MayaHwFogColor : HardwareFogColor = { 0.5, 0.5, 0.5, 1.0 }; 


uniform float4x4 viewPrj;

uniform float4x4 view;


// --------------------------------------- Per Object -----------------------------------------
uniform float4x4 world;

uniform int ClampDynamicLights = 99;

uniform float4x4 wvp;


// --------------------------------------- Attributes -----------------------------------------
uniform float3 Color = {0.5,0.5,0.5};


// ----------------------------------- Lights --------------------------------------
uniform int Light0Type = 3;

uniform int Light1Type = 3;

uniform int Light2Type = 3;

uniform float3 Light0Color = float3(1.0, 1.0, 1.0);

uniform float3 Light1Color = float3(1.0, 1.0, 1.0);

uniform float3 Light2Color = float3(1.0, 1.0, 1.0);

uniform float Light0Intensity = 1.0;

uniform float Light1Intensity = 1.0;

uniform float Light2Intensity = 1.0;

uniform float3 Light0Pos = float3(1.0, 1.0, 1.0);

uniform float3 Light1Pos = float3(1.0, 1.0, 1.0);

uniform float3 Light2Pos = float3(1.0, 1.0, 1.0);

uniform float3 Light0Dir = float3(0.0, -1.0, 0.0);

uniform float3 Light1Dir = float3(0.0, -1.0, 0.0);

uniform float3 Light2Dir = float3(0.0, -1.0, 0.0);

uniform float Light0Attenuation = 0.0;

uniform float Light1Attenuation = 0.0;

uniform float Light2Attenuation = 0.0;

uniform float Light0ConeAngle = 0.46;

uniform float Light1ConeAngle = 0.46;

uniform float Light2ConeAngle = 0.46;

uniform float Light0Falloff = 0.7;

uniform float Light1Falloff = 0.7;

uniform float Light2Falloff = 0.7;

uniform bool Light0ShadowOn = true;

uniform bool Light1ShadowOn = true;

uniform bool Light2ShadowOn = true;

uniform float4x4 Light0ViewPrj;

uniform float4x4 Light1ViewPrj;

uniform float4x4 Light2ViewPrj;

uniform float Light0ShadowBias = 0.01;

uniform float Light1ShadowBias = 0.01;

uniform float Light2ShadowBias = 0.01;

uniform float3 Light0ShadowColor = float3(0, 0, 0);

uniform float3 Light1ShadowColor = float3(0, 0, 0);

uniform float3 Light2ShadowColor = float3(0, 0, 0);


// ---------------------------------------- Textures -----------------------------------------
uniform sampler2D Light0ShadowMap;

uniform sampler2D Light1ShadowMap;

uniform sampler2D Light2ShadowMap;







uniform sampler2D TransDepthTexture;

uniform sampler2D OpaqueDepthTexture;






// ---------------------------------------- Functions -----------------------------------------
int GetLightType(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0Type; 
	else if (ActiveLightIndex == 1) 
		return Light1Type; 
	else 
		return Light2Type; 
}

float3 GetLightColor(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0Color; 
	else if (ActiveLightIndex == 1) 
		return Light1Color; 
	else 
		return Light2Color; 
}

float GetLightIntensity(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0Intensity; 
	else if (ActiveLightIndex == 1) 
		return Light1Intensity; 
	else 
		return Light2Intensity; 
}

float3 GetLightPos(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0Pos; 
	else if (ActiveLightIndex == 1) 
		return Light1Pos; 
	else 
		return Light2Pos; 
}

float3 GetLightDir(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0Dir; 
	else if (ActiveLightIndex == 1) 
		return Light1Dir; 
	else 
		return Light2Dir; 
}

float GetLightAttenuation(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0Attenuation; 
	else if (ActiveLightIndex == 1) 
		return Light1Attenuation; 
	else 
		return Light2Attenuation; 
}

float GetLightConeAngle(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0ConeAngle; 
	else if (ActiveLightIndex == 1) 
		return Light1ConeAngle; 
	else 
		return Light2ConeAngle; 
}

float GetLightFalloff(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0Falloff; 
	else if (ActiveLightIndex == 1) 
		return Light1Falloff; 
	else 
		return Light2Falloff; 
}

bool GetLightShadowOn(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0ShadowOn; 
	else if (ActiveLightIndex == 1) 
		return Light1ShadowOn; 
	else 
		return Light2ShadowOn; 
}

float4x4 GetLightViewPrj(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0ViewPrj; 
	else if (ActiveLightIndex == 1) 
		return Light1ViewPrj; 
	else 
		return Light2ViewPrj; 
}

float GetLightShadowBias(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0ShadowBias; 
	else if (ActiveLightIndex == 1) 
		return Light1ShadowBias; 
	else 
		return Light2ShadowBias; 
}

sampler2D GetLightShadowMap(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0ShadowMap; 
	else if (ActiveLightIndex == 1) 
		return Light1ShadowMap; 
	else 
		return Light2ShadowMap; 
}

float4 SampleFromShadowMap( int ActiveLightIndex, float2 UVs) 
{ 
	if (ActiveLightIndex == 0) 
		return textureLod(Light0ShadowMap, UVs, 0); 
	else if (ActiveLightIndex == 1) 
		return textureLod(Light1ShadowMap, UVs, 0); 
	else 
		return textureLod(Light2ShadowMap, UVs, 0); 
}

float3 GetLightShadowColor(int ActiveLightIndex) 
{ 
	if (ActiveLightIndex == 0) 
		return Light0ShadowColor; 
	else if (ActiveLightIndex == 1) 
		return Light1ShadowColor; 
	else 
		return Light2ShadowColor; 
}

float4 sampleTransDepthTex(float2 UV)
{
	float4 col = float4(0,0,0,0);

	#if defined(SFX_CGFX_3) || defined(SFX_HLSL_3)
		col = tex2D( TransDepthTextureSampler, UV );
	#endif
	#ifdef SFX_HLSL_5
		#if defined(SFX_SWATCH) || defined(_3DSMAX_)
			col = TransDepthTexture.Sample( TransDepthTextureSampler, UV );
		#else
			col = TransDepthTexture.Sample( TransDepthTextureSampler, UV );
		#endif
	#endif
	#ifdef SFX_GLSL_4
		col = texture( TransDepthTexture, UV );
	#endif
	#ifdef SFX_OGSFX
		col = texture( TransDepthTextureSampler, UV );
	#endif
	#ifdef SFX_GLSL_1_2
		col = texture2D( TransDepthTexture, UV );
	#endif

	return col;
}

float4 sampleOpaqueDepthTex(float2 UV)
{
	float4 col = float4(0,0,0,0);

	#if defined(SFX_CGFX_3) || defined(SFX_HLSL_3)
		col = tex2D( OpaqueDepthTextureSampler, UV );
	#endif
	#ifdef SFX_HLSL_5
		#if defined(SFX_SWATCH) || defined(_3DSMAX_)
			col = OpaqueDepthTexture.Sample( OpaqueDepthTextureSampler, UV );
		#else
			col = OpaqueDepthTexture.Sample( OpaqueDepthTextureSampler, UV );
		#endif
	#endif
	#ifdef SFX_GLSL_4
		col = texture( OpaqueDepthTexture, UV );
	#endif
	#ifdef SFX_OGSFX
		col = texture( OpaqueDepthTextureSampler, UV );
	#endif
	#ifdef SFX_GLSL_1_2
		col = texture2D( OpaqueDepthTexture, UV );
	#endif

	return col;
}

struct DepthPeelOutput 
{ 
	float4 LinearDepth; 
	float Peel;
}; 

DepthPeelOutput DepthPeelFunc( float3 worldPos, float4x4 view, float4x4 viewPrj ) 
{ 
	DepthPeelOutput OUT; 

	#ifdef SFX_CGFX_3
		float currZ = abs( mul( view, float4(worldPos, 1.0f) ).z );
		float4 Pndc  = mul( viewPrj, float4(worldPos, 1.0f) );
		float2 UV = Pndc.xy / Pndc.w * float2(0.5f, 0.5f) + 0.5f;

		float prevZ = sampleTransDepthTex(UV).r;
		float opaqZ = sampleOpaqueDepthTex(UV).r;
		float bias = 0.00002f;
		if (currZ < prevZ * (1.0f + bias) || currZ > opaqZ * (1.0f - bias))
		{
			discard;
		}

		float ld = abs( mul( view, float4(worldPos, 1.0f) ).z );
		OUT.LinearDepth = float4(ld, ld, ld, ld);
	#else
	#if defined(SFX_GLSL_1_2) || defined(SFX_GLSL_4) || defined(SFX_OGSFX)
		float currZ = abs( ( view * float4(worldPos, 1.0f) ).z );
		float4 Pndc  = viewPrj * float4(worldPos, 1.0f);
		float2 UV = Pndc.xy / Pndc.w * float2(0.5f, 0.5f) + 0.5f;

		float prevZ = sampleTransDepthTex(UV).r;
		float opaqZ = sampleOpaqueDepthTex(UV).r;
		float bias = 0.00002f;
		if (currZ < prevZ * (1.0f + bias) || currZ > opaqZ * (1.0f - bias))
		{
			discard;
		}

		float ld = abs( ( view * float4(worldPos, 1.0f) ).z );
		OUT.LinearDepth = float4(ld, ld, ld, ld);
	#else
		float currZ = abs( mul( float4(worldPos, 1.0f), view ).z );
		float4 Pndc  = mul( float4(worldPos, 1.0f), viewPrj );
		float2 UV = Pndc.xy / Pndc.w * float2(0.5f, -0.5f) + 0.5f;

		float prevZ = sampleTransDepthTex(UV).r;
		float opaqZ = sampleOpaqueDepthTex(UV).r;
		float bias = 0.00002f;
		if (currZ < prevZ * (1.0f + bias) || currZ > opaqZ * (1.0f - bias))
		{
			discard;
		}

		float ld = abs( mul( float4(worldPos, 1.0f), view ).z );
		OUT.LinearDepth = float4(ld, ld, ld, ld);
#endif
	#endif

	OUT.Peel = 1.0f;

	return OUT; 
} 



// -------------------------------------- AmbientLightFunction --------------------------------------
struct AmbientLightOutput
{
	float3 LightColor;
};

AmbientLightOutput AmbientLightFunction(int ActiveLightIndex, float3 AlbedoColor, float3 LightColor, float LightIntensity)
{
	AmbientLightOutput OUT;

	float3 MulOp = ((LightColor * AlbedoColor) * LightIntensity);
	OUT.LightColor = MulOp;

	return OUT;
}

// -------------------------------------- GetLightVectorFunction --------------------------------------
struct GetLightVectorOutput
{
	float3 Result;
};

GetLightVectorOutput GetLightVectorFunction(int ActiveLightIndex, float3 LightPosition, float3 VertexWorldPosition, int LightType, float3 LightDirection)
{
	GetLightVectorOutput OUT;

	bool IsDirectionalLight = (LightType == 4);
	float3 LerpOp = mix((LightPosition - VertexWorldPosition), -(LightDirection), float(IsDirectionalLight));
	OUT.Result = LerpOp;

	return OUT;
}

// -------------------------------------- LambertDiffuseFunction --------------------------------------
struct LambertDiffuseOutput
{
	float3 Color;
};

LambertDiffuseOutput LambertDiffuseFunction(int ActiveLightIndex, float3 AlbedoColor, float3 Normal, float3 LightVector)
{
	LambertDiffuseOutput OUT;

	float SatOp = clamp(dot(Normal, LightVector), 0.0, 1.0);
	float3 Diffuse = (SatOp * AlbedoColor);
	OUT.Color = Diffuse;

	return OUT;
}

// -------------------------------------- LightDecayFunction --------------------------------------
struct LightDecayOutput
{
	float Attenuation;
};

LightDecayOutput LightDecayFunction(int ActiveLightIndex, float3 LightVectorUN, float Attenuation)
{
	LightDecayOutput OUT;

	bool IsAttenuationUsed = (Attenuation > 0.001);
	float DecayContribution463 = 0.0;
	if (IsAttenuationUsed)
	{
		float PowOp = pow(length(LightVectorUN), Attenuation);
		float DivOp = (1.0 / PowOp);
		DecayContribution463 = DivOp;
	}
	else
	{
		DecayContribution463 = 1.0;
	}
	OUT.Attenuation = DecayContribution463;

	return OUT;
}

// -------------------------------------- LightConeAngleFunction --------------------------------------
struct LightConeAngleOutput
{
	float ConeAngle;
};

LightConeAngleOutput LightConeAngleFunction(int ActiveLightIndex, float3 LightVector, float3 LightDirection, float ConeAngle, float ConeFalloff)
{
	LightConeAngleOutput OUT;

	float CosOp = cos(max(ConeFalloff, ConeAngle));
	float DotOp = dot(LightVector, -(LightDirection));
	float SmoothStepOp = smoothstep(CosOp, cos(ConeAngle), DotOp);
	OUT.ConeAngle = SmoothStepOp;

	return OUT;
}

// -------------------------------------- ShadowMapFunction --------------------------------------
struct ShadowMapOutput
{
	float LightGain;
};

ShadowMapOutput ShadowMapFunction(int ActiveLightIndex, float4x4 LightViewPrj, float ShadowMapBias, float3 VertexWorldPosition)
{
	ShadowMapOutput OUT;

	float IfElseOp546 = 0.0;
	float4 VectorConstruct = float4(VertexWorldPosition.x, VertexWorldPosition.y, VertexWorldPosition.z, 1.0);
	float4 MulOp = (LightViewPrj * VectorConstruct);
	float3 DivOp = (MulOp.xyz / MulOp.w);
	if (DivOp.x > -1.0 && DivOp.x < 1.0 && DivOp.y > -1.0 && DivOp.y < 1.0 && DivOp.z > 0.0 && DivOp.z < 1.0)
	{
		float Val = 0.5;
		float2 AddOp = ((Val * DivOp.xy) + Val);
		float SubOp = (DivOp.z - (ShadowMapBias / MulOp.w));
		float ShadowTotal = 0.0;
		for(int i=0; i<10; i+=1)
		{
			sampler2D _LightShadowMap = GetLightShadowMap(ActiveLightIndex);
			float2 MulOp586 = (0.0009 * ShadowFilterTaps[i]);
			float2 AddOp587 = (AddOp + MulOp586);
			float4 Sampler = SampleFromShadowMap(ActiveLightIndex, AddOp587);
			float IfElseOp540 = ((SubOp - Sampler.x) >= 0.0) ? (0.0) : (0.1);
			ShadowTotal += IfElseOp540;
		}
		IfElseOp546 = ShadowTotal;
	}
	else
	{
		IfElseOp546 = 1.0;
	}
	OUT.LightGain = IfElseOp546;

	return OUT;
}

// -------------------------------------- LightContributionFunction --------------------------------------
struct LightContributionOutput
{
	float3 Light;
};

LightContributionOutput LightContributionFunction(int ActiveLightIndex, float3 VertexWorldPosition, float3 LightVectorUN)
{
	LightContributionOutput OUT;

	float _LightIntensity = GetLightIntensity(ActiveLightIndex);
	int _LightType = GetLightType(ActiveLightIndex);
	bool IsDirectionalLight = (_LightType == 4);
	float DecayMul509 = 0.0;
	if (IsDirectionalLight)
	{
		DecayMul509 = 1.0;
	}
	else
	{
		float _LightAttenuation = GetLightAttenuation(ActiveLightIndex);
		LightDecayOutput LightDecay457 = LightDecayFunction(ActiveLightIndex, LightVectorUN, _LightAttenuation);
		DecayMul509 = LightDecay457.Attenuation;
	}
	bool IsSpotLight = (_LightType == 2);
	float ConeMul511 = 1.0;
	if (IsSpotLight)
	{
		float3 NormOp = normalize(LightVectorUN);
		float3 _LightDir = GetLightDir(ActiveLightIndex);
		float _LightConeAngle = GetLightConeAngle(ActiveLightIndex);
		float _LightFalloff = GetLightFalloff(ActiveLightIndex);
		LightConeAngleOutput LightConeAngle399 = LightConeAngleFunction(ActiveLightIndex, NormOp, _LightDir, _LightConeAngle, _LightFalloff);
		ConeMul511 = LightConeAngle399.ConeAngle;
	}
	bool _LightShadowOn = GetLightShadowOn(ActiveLightIndex);
	float ShadowMul512 = 1.0;
	if (_LightShadowOn)
	{
		float4x4 _LightViewPrj = GetLightViewPrj(ActiveLightIndex);
		float _LightShadowBias = GetLightShadowBias(ActiveLightIndex);
		ShadowMapOutput ShadowMap525 = ShadowMapFunction(ActiveLightIndex, _LightViewPrj, _LightShadowBias, VertexWorldPosition);
		float3 _LightShadowColor = GetLightShadowColor(ActiveLightIndex);
		float ShadowColorMix = mix(ShadowMap525.LightGain, 1.0, float(_LightShadowColor.x));
		ShadowMul512 = ShadowColorMix;
	}
	float DecayShadowConeMul = ((ShadowMul512 * ConeMul511) * DecayMul509);
	float3 _LightColor = GetLightColor(ActiveLightIndex);
	float3 MulItensity = ((_LightColor * DecayShadowConeMul) * _LightIntensity);
	OUT.Light = MulItensity;

	return OUT;
}

// -------------------------------------- BlinnSpecularFunction --------------------------------------
struct BlinnSpecularOutput
{
	float3 SpecularColor;
};

BlinnSpecularOutput BlinnSpecularFunction(int ActiveLightIndex, float3 LightVector, float3 Normal, float3 CameraVector, float SpecularPower, float3 SpecularColor)
{
	BlinnSpecularOutput OUT;

	float3 NormOp = normalize((LightVector + CameraVector));
	float SatOp = clamp(dot(Normal, NormOp), 0.0, 1.0);
	float3 BlinnSpec = (SpecularColor * pow(SatOp, SpecularPower));
	float SatOp978 = clamp(dot(Normal, LightVector), 0.0, 1.0);
	float3 MulOp = (SatOp978 * BlinnSpec);
	OUT.SpecularColor = MulOp;

	return OUT;
}

// -------------------------------------- DesaturateColorFunction --------------------------------------
struct DesaturateColorOutput
{
	float DesaturateColor;
};

DesaturateColorOutput DesaturateColorFunction(int ActiveLightIndex, float3 Color)
{
	DesaturateColorOutput OUT;

	float3 Col = float3(0.300008,0.6,0.100008);
	float DotOp = dot(clamp(Color, 0.0, 1.0), Col.xyz);
	OUT.DesaturateColor = DotOp;

	return OUT;
}

// -------------------------------------- DesaturateColorFunction --------------------------------------
DesaturateColorOutput DesaturateColorFunction(float3 Color)
{
	DesaturateColorOutput OUT;

	float3 Col = float3(0.300008,0.6,0.100008);
	float DotOp = dot(clamp(Color, 0.0, 1.0), Col.xyz);
	OUT.DesaturateColor = DotOp;

	return OUT;
}

// -------------------------------------- ShaderVertex --------------------------------------
#if defined(COMPILING_VS)
	in float3 IN_Position;
	in float3 IN_Normal;
#endif 

	OUTIN float4 OUT_Position;
	OUTIN float4 OUT_Normal;
	OUTIN float4 OUT_WorldPosition;
	OUTIN half3 OUT_FogFactor;

#if defined(COMPILING_VS)
	void main(void)
	{
		OUT_Position = float4(IN_Position, 1);
		float3 MulOp = ((float3x3(world)) * IN_Normal);
		float3 NormalN = normalize(MulOp);
		float4 WorldNormal = float4(NormalN.x, NormalN.y, NormalN.z, 1.0);
		OUT_Normal = WorldNormal;
		float4 WorldPos = (world * OUT_Position);
		OUT_WorldPosition = WorldPos;
		OUT_WorldPosition = (world * float4(IN_Position, 1));
	float4 _HPosition = mul( viewPrj, float4(OUT_WorldPosition.xyz, 1) ); 
	float fogFactor = 0.0; 
	if (MayaHwFogMode == 0) { 
				fogFactor = saturate((MayaHwFogEnd - _HPosition.z) / (MayaHwFogEnd - MayaHwFogStart)); 
	} 
	else if (MayaHwFogMode == 1) { 
				fogFactor = 1.0 / (exp(_HPosition.z * MayaHwFogDensity)); 
	} 
	else if (MayaHwFogMode == 2) { 
				fogFactor = 1.0 / (exp(pow(_HPosition.z * MayaHwFogDensity, 2))); 
	} 
	OUT_FogFactor = float3(fogFactor, fogFactor, fogFactor); 

		float4 WVSpace = (wvp * OUT_Position);
		OUT_Position = WVSpace;
	}
#endif 

// -------------------------------------- ShaderPixel --------------------------------------
#if defined(COMPILING_PS)
	out float4 OUT_Color;
#endif 

#if defined(COMPILING_PS)
	void main(void)
	{
		float InvertSatMask = (1.0 - clamp(0.0, 0.0, 1.0));
		float3 ReplaceDiffuseWithReflection = (Color.xyz * InvertSatMask);
		float3 NormOp = normalize(OUT_Normal.xyz);
		float3 FlippedNormals = mix(-(NormOp), NormOp, float(gl_FrontFacing));
		float ClampOpacity = clamp(1.0, 0.0, 1.0);
		float3 CameraPosition = viewI[3].xyz;
		float3 CamVec = (CameraPosition - OUT_WorldPosition.xyz);
		float3 CamVecNorm = normalize(CamVec);
		float4 LightLoopTotal11 = float4(0,0,0,0);
		for (int ActiveLightIndex = 0; ActiveLightIndex < 3; ++ActiveLightIndex)
		{
			if (ActiveLightIndex >= ClampDynamicLights) {continue;}
			int _LightType = GetLightType(ActiveLightIndex);
			bool IsAmbientLight = (_LightType == 5);
			float4 IfAmbientLight384 = float4(0, 0, 0, 0);
			if (IsAmbientLight)
			{
				float3 _LightColor = GetLightColor(ActiveLightIndex);
				float _LightIntensity = GetLightIntensity(ActiveLightIndex);
				AmbientLightOutput AmbientLight389 = AmbientLightFunction(ActiveLightIndex, ReplaceDiffuseWithReflection, _LightColor, _LightIntensity);
				float3 PreMulAlphaAmbient = (ClampOpacity * (AmbientLight389.LightColor * 1.0));
				float4 VectorConstruct = float4(PreMulAlphaAmbient.x, PreMulAlphaAmbient.y, PreMulAlphaAmbient.z, 0.0);
				IfAmbientLight384 = VectorConstruct;
			}
			else
			{
				float3 NoTranslucency = float3(0.0,0.0,0.0);
				float3 _LightPos = GetLightPos(ActiveLightIndex);
				float3 _LightDir = GetLightDir(ActiveLightIndex);
				GetLightVectorOutput GetLightVector848 = GetLightVectorFunction(ActiveLightIndex, _LightPos, OUT_WorldPosition.xyz, _LightType, _LightDir);
				float3 LightVecNorm = normalize(GetLightVector848.Result);
				LambertDiffuseOutput LambertDiffuse823 = LambertDiffuseFunction(ActiveLightIndex, ReplaceDiffuseWithReflection, FlippedNormals, LightVecNorm);
				LightContributionOutput LightContribution397 = LightContributionFunction(ActiveLightIndex, OUT_WorldPosition.xyz, GetLightVector848.Result);
				float3 AddTranslucency = (NoTranslucency.xyz + (LightContribution397.Light * LambertDiffuse823.Color));
				float3 Col = float3(1.0,1.0,1.0);
				BlinnSpecularOutput BlinnSpecular966 = BlinnSpecularFunction(ActiveLightIndex, LightVecNorm, FlippedNormals, CamVecNorm, 20.0, Col.xyz);
				float3 SpecLightIntensity = (BlinnSpecular966.SpecularColor * LightContribution397.Light);
				float3 Diffuse_Spec = ((ClampOpacity * AddTranslucency) + SpecLightIntensity);
				DesaturateColorOutput DesaturateColor664 = DesaturateColorFunction(ActiveLightIndex, SpecLightIntensity);
				float4 Color_Alpha = float4(Diffuse_Spec.x, Diffuse_Spec.y, Diffuse_Spec.z, DesaturateColor664.DesaturateColor);
				IfAmbientLight384 = Color_Alpha;
			}
			float4 ApplyWeight395 = IfAmbientLight384;
			LightLoopTotal11 += ApplyWeight395;
		}
		float3 NoReflection = float3(0.0,0.0,0.0);
		float3 ReflectXmask = (NoReflection.xyz * 0.0);
		float3 DefaultEmissiveColor = float3(0.0,0.0,0.0);
		float3 DefaultIBLColor = float3(0.0,0.0,0.0);
		float3 PreMultipliedAlpha = (ClampOpacity * (DefaultEmissiveColor.xyz + DefaultIBLColor.xyz));
		float3 AddReflection = (ReflectXmask + PreMultipliedAlpha);
		DesaturateColorOutput DesaturateColor363 = DesaturateColorFunction(ReflectXmask);
		float OpacityAndReflection = (ClampOpacity + DesaturateColor363.DesaturateColor);
		float4 TotalAmbientAndOpacity = float4(AddReflection.x, AddReflection.y, AddReflection.z, OpacityAndReflection);
		float4 LightLoopAndAfterLoop = (LightLoopTotal11 + TotalAmbientAndOpacity);
		float SatOp = clamp(LightLoopAndAfterLoop.w, 0.0, 1.0);
		float4 VectorConstruct = float4(LightLoopAndAfterLoop.xyz.x, LightLoopAndAfterLoop.xyz.y, LightLoopAndAfterLoop.xyz.z, SatOp);
	if (MayaHwFogEnabled) { 
		float fogFactor = (1.0 - IN_FogFactor.x) * MayaHwFogColor.a; 
		VectorConstruct.rgb	= lerp(VectorConstruct.rgb, MayaHwFogColor.rgb, fogFactor); 
	} 

		OUT_Color = VectorConstruct;
	}
#endif 

// -------------------------------------- ShaderPixelP1 --------------------------------------
#if defined(COMPILING_PS1)
	out float4 OUT_Color0;
	out float4 OUT_Color1;
#endif 

#if defined(COMPILING_PS1)
	void main(void)
	{
		DepthPeelOutput DepthPeel = DepthPeelFunc(OUT_WorldPosition.xyz, view, viewPrj);
		OUT_Color0 = ShaderPixel(IN).Color;
		OUT_Color1 = DepthPeel.LinearDepth;
	}
#endif 

// -------------------------------------- ShaderPixelP2 --------------------------------------
#if defined(COMPILING_PS2)
	out float4 OUT_Color0;
	out float4 OUT_Color1;
#endif 

#if defined(COMPILING_PS2)
	void main(void)
	{
		DepthPeelOutput DepthPeel = DepthPeelFunc(OUT_WorldPosition.xyz, view, viewPrj);
		OUT_Color0 = ShaderPixel(IN).Color;
		OUT_Color1 = ((OUT_Color0.w > 0.001f ? float4(1.0f, 1.0f, 1.0f, 1.0f) : float4(0.0f, 0.0f, 0.0f, 0.0f)) * DepthPeel.Peel);
	}
#endif 

// -------------------------------------- ShaderPixelP3 --------------------------------------
#if defined(COMPILING_PS3)
	out float4 OUT_Color0;
	out float4 OUT_Color1;
#endif 

#if defined(COMPILING_PS3)
	void main(void)
	{
		OUT_Color0 = ShaderPixel(IN).Color;
		OUT_Color1 = (OUT_Color0.w > 0.001f ? float4(1.0f, 1.0f, 1.0f, 1.0f) : float4(0.0f, 0.0f, 0.0f, 0.0f));
	}
#endif 

