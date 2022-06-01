#pragma once

#include "SandFoxCore.h"

namespace SandFox
{

	// ------------------------------------------------------------------ REMEMBER
	// Update H_Registers.hlsli when updating registers!
	

	
	// Sampler and Sampler State registers
	enum RegSampler : char
	{
		RegSamplerDefault	= 0,

		RegSamplerUser0		= 1,
		RegSamplerUser1		= 2,
		RegSamplerUser2		= 3,
		RegSamplerUser3		= 4,
		RegSamplerUser4		= 5,

		RegSamplerSystem0	= 6,
		RegSamplerSystem1	= 7,
		RegSamplerSystem2	= 8,
		RegSamplerSystem3	= 9,

		RegSamplerStandard	= 10,
		RegSamplerPoint		= 11,
		RegSamplerShadow	= 12,

		RegSamplerCount	= 16	// <-- Keep last!
	};

	// Constant Buffer View registers
	enum RegCBV : char
	{
		RegCBVDefault			= 0,

		RegCBVUser0				= 1,
		RegCBVUser1				= 2,
		RegCBVUser2				= 3,
		RegCBVUser3				= 4,

		RegCBVSystem0			= 5,
		RegCBVSystem1			= 6,
		RegCBVSystem2			= 7,
							  
		RegCBVTesselationInfo	= 8,
		RegCBVObjectInfo		= 9,		// Transformation matrix
		RegCBVMaterialInfo		= 10,	
		RegCBVCameraInfo		= 11,		// Camera position, etc
		RegCBVClientInfo		= 12,		// Screen size, etc
		RegCBVLightInfo			= 13,

		RegCBVCount	= 14	// <-- Keep last!
	};

	// Unordered Access View registers
	enum RegUAV : char
	{
		RegUAVDefault		= 0,

		RegUAVUser0			= 1,
		RegUAVUser1			= 2,
		RegUAVUser2			= 3,

		RegUAVSystem0		= 4,
		RegUAVSystem1		= 5,

		RegUAVRenderTarget	= 6,

		RegUAVCount = 8		// <-- Keep last!
	};

	// Shader Resource View registers
	enum RegSRV : char
	{
		RegSRVDefault			= 0,

		RegSRVUser0				= 1,
		RegSRVUser1				= 2,
		RegSRVUser2				= 3,
		RegSRVUser3				= 4,
		RegSRVUser4				= 5,
		RegSRVUser5				= 6,

		RegSRVTexColor			= 7,
		RegSRVTexSpecularity	= 8,
		RegSRVTexNormal			= 9,
		RegSRVTexDisplacement	= 10,

		RegSRVDefPosition		= 12,
		RegSRVDefNormal			= 13,
		RegSRVDefAmbient		= 14,
		RegSRVDefDiffuse		= 15,
		RegSRVDefSpecular		= 16,
		RegSRVDefExponent		= 17,

		RegSRVCopySource		= 20,
		RegSRVParticleData		= 21,

		RegSRVCubemap			= 22,
		RegSRVShadowDepth		= 24, // Array of up to FOX_C_MAX_SHADOWS shadowed lights

		RegSRVCount = 32	// <-- Keep last!
	};



	// Primitive topology presets
	enum PTPreset : char
	{
		PTPresetUser0			= 0,
		PTPresetUser1			= 1,
		PTPresetUser2			= 2,
		PTPresetUser3			= 3,

		PTPresetPointList		= 4,
		PTPresetLineList		= 5,
		PTPresetLineStrip		= 6,
		PTPresetTriangleList	= 7,
		PTPresetTriangleStrip	= 8,

		PTPresetCount	// <-- Keep last!
	};



	// Types of bindable resource or stage
	enum BindType : char
	{
		BindTypePipeline,
		BindTypeSampler,
		BindTypeConstantBuffer,
		BindTypeShaderResource,
		BindTypeUnorderedAccess
	};

	// Stage where the resource is bound
	enum BindStage : char
	{
		BindStageNone = 0,
		BindStageVS = 1,
		BindStageHS = 2,	
		BindStageDS = 3,	
		BindStageGS = 4,
		BindStagePS = 5,

		BindStageCS = 6,

		BindStageCount = 7
	};



	typedef unsigned int BindID;

	struct BindInfo
	{
		BindID id;
		char stages = BindStageNone;	// Not applicable for pipeline bindables
	};

}