#pragma once

#include "SandFoxCore.h"
#include "Bindables.h"
#include "Buffer.h"
#include "Texture.h"
#include "TexSRVArray.h"
#include "BindInfo.h"





namespace SandFox
{

	struct BindPipeline
	{
		FOX_API BindPipeline();
		FOX_API BindPipeline(const BindPipeline& copy);
		FOX_API BindID GetID() const;
								 
		// Data					 
		Bind::VertexBuffer*		vb = nullptr;		// Must be set
		Bind::IndexBuffer*		ib = nullptr;		// Must only be set when using indexed drawing
		Shader*					shader = nullptr;	// Must be set
		
	private:
		BindID m_id = 0;
	};



	class FOX_API BindHandler
	{
	public:
		BindHandler();
		~BindHandler();

		void Init();
		void DeInit();

		static BindID GetID();

		// BindHandler assumes ownership of the object.
		static void PresetSampler(Bind::SamplerState* sampler, RegSampler reg); 

		// Called at creation of a drawable that uses the preset
		static void ApplyPresetSampler(RegSampler reg, BindStage stage);

		static bool BindPipeline(BindPipeline* pipeline);
		static bool BindShader(Shader* shader);
		static bool BindVS(Bind::VertexShader* vs);			// Vertex shader
		static bool BindGS(Bind::GeometryShader* gs);		// Geometry shader
		static bool BindHS(Bind::HullShader* hs);			// Hull shader
		static bool BindDS(Bind::DomainShader* ds);			// Domain shader
		static bool BindPS(Bind::PixelShader* ps);			// Pixel shader
		static bool BindIL(Bind::InputLayout* il);			// Input layout
		static bool BindPT(Bind::PrimitiveTopology* pt);	// Primitive topology
		static bool BindVB(Bind::VertexBuffer* vb);			// Vertex buffer
		static bool BindIB(Bind::IndexBuffer* ib);			// Index buffer

		static bool BindSampler(BindStage stage, Bind::SamplerState* sampler);
		static bool BindCBV(BindStage stage, Bind::ConstBuffer* cbv);
		static bool BindUAV(BindStage stage, BindableResource* uav);
		static bool BindSRV(BindStage stage, BindableResource* srv);

		static void UnbindPipeline();
		static void UnbindShader();
		static void UnbindVS();		// Vertex shader
		static void UnbindGS();		// Geometry shader
		static void UnbindHS();		// Hull shader
		static void UnbindDS();		// Domain shader
		static void UnbindPS();		// Pixel shader
		static void UnbindIL();		// Input layout
		static void UnbindPT();		// Primitive topology
		static void UnbindVB();		// Vertex buffer
		static void UnbindIB();		// Index buffer

		static void UnbindSampler(BindStage stage, RegSampler reg);
		static void UnbindCBV(BindStage stage, RegCBV reg);
		static void UnbindUAV(BindStage stage, RegUAV reg);
		static void UnbindSRV(BindStage stage, RegSRV reg);



		// --- Privates

	private:
		struct BindResource
		{
			Bindable* resource	= nullptr;
			BindID id			= 0;
		};

		struct StageResources
		{
			BindResource m_samplers[RegSamplerCount];
			BindResource m_cbvs[RegCBVCount];
			BindResource m_uavs[RegUAVCount];
			BindResource m_srvs[RegSRVCount];
		};

	private:
		bool BindResourceView(BindResource& r, BindStage stage, BindType type, BindableResource* bindable);
		void UnbindResourceView(BindResource& r, BindStage stage, BindType type);

	private:
		static BindHandler* s_bindHandler;
		static BindID s_idCounter;

		// Counter for bind IDs, 0 reserved for N/A or none bound.

		// Pipeline
		BindID m_pipeline;
		BindID m_shader;
		BindID m_vs;
		BindID m_gs;
		BindID m_hs;
		BindID m_ds;
		BindID m_ps;
		BindID m_il;
		BindID m_vb;
		BindID m_ib;
		BindID m_pt;

		// Resources
		StageResources m_stageResources[BindStageCount - 1];

		// Samplers
		Bind::SamplerState* m_savedSamplers[RegSamplerCount - 1];
	};

}

