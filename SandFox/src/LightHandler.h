#pragma once

#include "SandFoxCore.h"
#include "ConstantBuffer.h"
#include "TexSRVArray.h"
#include "DrawQueue.h"
#include "Light.h"
#include "Viewport.h"
#include "TextureQuality.h"

namespace SandFox
{

	typedef int LightID;



	class FOX_API LightHandler
	{
	public:
		LightHandler();
		~LightHandler();

		void Init(TextureQuality quality, GraphicsTechnique technique, cs::Color ambient, float ambientIntensity);
		void DeInit();



		// Non-shadowed
		LightID AddDirectional(
			const cs::Vec3& angles,
			float intensity = 10.0f,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));

		// Shadowed
		LightID AddDirectionalShadowed(
			//LightShadowQuality quality,
			const cs::Vec3& angles,
			float shadowAreaWidth = 100.0f,
			float intensity = 10.0f, 
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f),
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR);



		// Non-shadowed
		LightID AddPoint(
			const cs::Vec3& position,
			float intensity = 10.0f,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));

		// Shadowed
		LightID AddPointShadowed(
			//LightShadowQuality quality,
			const cs::Vec3& position,
			float intensity = 10.0f,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f),
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR);



		// Non-shadowed
		LightID AddSpot(
			const cs::Vec3& position,
			const cs::Vec3& angles,
			float spread = 1.0f,
			float intensity = 10.0f,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f));

		// Shadowed
		LightID AddSpotShadowed(
			//LightShadowQuality quality,
			const cs::Vec3& position,
			const cs::Vec3& angles,
			float spread = 1.0f,
			float intensity = 10.0f,
			const cs::Color& color = cs::Color(1.0f, 1.0f, 1.0f),
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR);



		// Non-shadowed
		LightID AddLight(const Light& light);

		// Shadowed
		LightID AddLightShadowed(
			//LightShadowQuality quality,
			const Light& light,
			float nearPlane = FOX_C_SHADOW_DEFAULT_NEAR,
			float farPlane = FOX_C_SHADOW_DEFAULT_FAR);



		Light& GetLight(LightID id);
		void RemoveLight(LightID id);
		void SetAmbient(const cs::Color& color, float intensity);
		// Does not assume ownership of the points.
		//void SetSamplePoints(const cs::Vec3* points, int count);
		void SetFocalPoint(const cs::Vec3& focalPoint, float distance = 50.0f);

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
			//int shadowIndex;	// -1: no light
		};

		struct ShadowStruct
		{
			bool used;
			bool redraw;
			//LightShadowQuality quality;
			//Viewport viewport;

			//int targetIndex;
			//Texture texture;
			//ComPtr<ID3D11ShaderResourceView> srv;
			ComPtr<ID3D11DepthStencilView> dsv;
		};

		struct LightInfo
		{
			Vec3 ambient;
			int totalLightCount;

			int shadowCount;
			Vec3 nul;

			Light lights[FOX_C_MAX_LIGHTS];
		};

	private:
		void DrawMapAtIndex(int index, DrawQueue* drawQueue);
		int AddShadow(/*LightShadowQuality quality*/);
		void RemoveShadow(int index);
		void OrderLights();
		void PushLights();

		static LightID Predicate(const LightStruct& l);

	private:
		bool m_flush;
		LightID m_id;
		GraphicsTechnique m_technique;
		TextureQuality m_quality;

		cs::List<Light> m_lights;
		cs::List<LightStruct> m_lightStructs;
		LightInfo m_lightInfo;
		Bind::ConstBuffer m_lightInfoBuffer;

		Shader* m_shader;

		float m_focalPointDistance;
		cs::Vec3 m_focalPoint;
		//const cs::Vec3* m_samplePoints;
		//int m_samplePointCount;
		
		int m_shadowCount;
		ShadowStruct m_shadows[FOX_C_MAX_SHADOWS];
		ComPtr<ID3D11Texture2D> m_shadowArray;
		ComPtr<ID3D11ShaderResourceView> m_shadowSRV;
		Viewport m_viewport;

		//Texture* m_white;
		//ID3D11ShaderResourceView* m_whiteSRV;
	};

}
