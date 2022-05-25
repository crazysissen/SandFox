#pragma once

#include "SandFoxCore.h"
#include "ConstantBuffer.h"
#include "TexSRVArray.h"
#include "DrawQueue.h"
#include "Light.h"
#include "Viewport.h"

namespace SandFox
{

	typedef int LightID;

	enum LightShadowQuality
	{
		LightShadowQualityLowest,
		LightShadowQualityLower,
		LightShadowQualityDefault,	
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



		// Non-shadowed
		LightID AddDirectional(
			const cs::Vec3& angles,
			float intensity = 10.0f,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));

		// Shadowed
		LightID AddDirectional(
			LightShadowQuality quality,
			const cs::Vec3& angles, 
			float intensity = 10.0f, 
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));



		// Non-shadowed
		LightID AddPoint(
			const cs::Vec3& position,
			float intensity = 10.0f,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));

		// Shadowed
		LightID AddPoint(
			LightShadowQuality quality,
			const cs::Vec3& position,
			float intensity = 10.0f,
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));



		// Non-shadowed
		LightID AddSpot(
			const cs::Vec3& position,
			const cs::Vec3& angles,
			float spread = 1.0f,
			float intensity = 10.0f,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));

		// Shadowed
		LightID AddSpot(
			LightShadowQuality quality,
			const cs::Vec3& position,
			const cs::Vec3& angles,
			float spread = 1.0f,
			float intensity = 10.0f,
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));



		// Non-shadowed
		LightID AddLight(const Light& light);

		// Shadowed
		LightID AddLight(
			LightShadowQuality quality,
			const Light& light,
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR);



		Light& GetLight(LightID id);
		void RemoveLight(LightID id);
		void SetAmbient(const cs::Color& color, float intensity);
		// Does not assume ownership of the points.
		void SetSamplePoints(const cs::Vec3* points, int count);

		void UpdateMap(LightID id);

		void UpdateLightInfo();
		void Update(DrawQueue* drawQueue);
		void DrawMap(LightID index, DrawQueue* drawQueue);
		void DrawAllMaps(DrawQueue* drawQueue);

		void BindMaps();
		void BindInfo();
		void BindImmediate();
		void BindDeferred();





		// Privates

	private:
		struct LightStruct
		{
			LightID id;
			int shadowIndex;	// -1: no light
		};

		struct ShadowStruct
		{
			bool initialized;
			bool redraw;
			LightShadowQuality quality;
			Viewport viewport;

			Texture texture;
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
		int AddShadow(LightShadowQuality quality);
		void RemoveShadow(int index);

		static LightID Predicate(const LightStruct& l);

	private:
		bool m_flush;
		LightID m_id;
		cs::Point m_mapResolutions[LightShadowQualityCount];
		GraphicsTechnique m_technique;

		cs::List<Light> m_lights;
		cs::List<LightStruct> m_lightStructs;
		LightInfo m_lightInfo;
		Bind::ConstBuffer m_lightInfoBuffer;

		Shader* m_shader;

		const cs::Vec3* m_samplePoints;
		int m_samplePointCount;
		
		int m_shadowCount;
		ShadowStruct m_shadows[FOX_C_MAX_SHADOWS];

		//Texture* m_white;
		//ID3D11ShaderResourceView* m_whiteSRV;
	};

}
