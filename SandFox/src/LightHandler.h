#pragma once

#include "SandFoxCore.h"
#include "ConstantBuffer.h"
#include "TexSRVArray.h"
#include "Light.h"

namespace SandFox
{

	class FOX_API LightHandler
	{
	public:
		LightHandler();
		~LightHandler();

		void Init(GraphicsTechnique technique, cs::Color ambient, float ambientIntensity);
		void DeInit();

		void SetLights(const Light* lights, int count);
		void SetLightAmbient(const cs::Color& color, float intensity);

		void Bind(GraphicsTechnique technique);
		void BindImmediate();
		void BindDeferred();

	private:
		void UpdateBuffer();

	private:
		struct LightInfo
		{
			Vec3 ambient;
			int lightCount;

			Light lights[FOX_C_MAX_LIGHTS];
		};

		RenderTexture* m_renderTextures;
		TexSRVArray m_srvArray;

		LightInfo m_lightInfo;
		Bind::ConstBuffer m_lightInfoBuffer;
	};

}
