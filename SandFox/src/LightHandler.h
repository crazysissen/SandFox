#pragma once

#include "SandFoxCore.h"
#include "ConstantBuffer.h"
#include "TexSRVArray.h"
#include "DrawQueue.h"
#include "Light.h"

namespace SandFox
{

	typedef int LightID;

	enum LightShadowQuality
	{
		LightShadowQualityDefault,	
		LightShadowQualityLower,
		LightShadowQualityLowest,
		LightShadowQualityHigher,
		LightShadowQualityHighest,	// Danger

		LightShadowQualityCount		// <-- Keep last!
	};



	class FOX_API LightHandler
	{
	public:
		LightHandler();
		~LightHandler();

		void Init(GraphicsTechnique technique, cs::Color ambient, float ambientIntensity);
		void DeInit();



		LightID AddDirectional(
			const cs::Vec3& angles, 
			float intensity = 10.0f, 
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR,
			LightShadowQuality quality = LightShadowQualityDefault,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f)
		);

		LightID AddPoint(
			const cs::Vec3& angles,
			float intensity = 10.0f,
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR,
			LightShadowQuality quality = LightShadowQualityDefault,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f)
		);

		LightID AddSpot(
			const cs::Vec3& angles,
			const cs::Vec3& direction,
			float spread = 1.0f,
			float intensity = 10.0f,
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR,
			LightShadowQuality quality = LightShadowQualityDefault,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f)
		);

		LightID AddLight(
			const Light& light,
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR,
			LightShadowQuality quality = LightShadowQualityDefault
		);



		Light& GetLight(LightID id);
		void RemoveLight(LightID id);
		void SetAmbient(const cs::Color& color, float intensity);
		void SetSamplePoints();

		void Update(DrawQueue* drawQueue);
		void DrawMap(LightID index, DrawQueue* drawQueue);
		void DrawAllMaps(DrawQueue* drawQueue);

		void Bind(GraphicsTechnique technique);
		void BindImmediate();
		void BindDeferred();





		// Privates

	private:
		struct LightStruct
		{
			LightID id;
			Light light;

			bool redraw;
			float nearClip;
			float farClip;
			LightShadowQuality quality;
			RenderTexture texture;
			ComPtr<ID3D11ShaderResourceView> srv;
			ComPtr<ID3D11DepthStencilView> dsv;
		};

		struct LightInfo
		{
			Vec3 ambient;
			int lightCount;

			Light lights[FOX_C_MAX_LIGHTS];
		};

	private:
		void DrawMapAtIndex(int index, DrawQueue* drawQueue);
		void UpdateLightInfo();

		static LightID Predicate(const LightStruct& l);

	private:
		bool m_flush;
		LightID m_id;
		cs::Point m_mapResolutions[LightShadowQualityCount];

		cs::List<LightStruct> m_lights;
		Vec3 m_ambient;

		Texture* m_white;
		ID3D11ShaderResourceView* m_whiteSRV;
		Bind::VertexShader m_vs;

		LightInfo m_lightInfo;
		Bind::ConstBuffer m_lightInfoBuffer;
	};

}
