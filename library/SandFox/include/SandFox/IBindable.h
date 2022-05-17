#pragma once

#include "BindInfo.h"

namespace SandFox
{

	class IBindable
	{
	public:
		virtual void Bind(BindStage stage) = 0;
	};

	class FOX_API Bindable : public IBindable
	{
	public:
		Bindable();
		virtual ~Bindable() = default;

		virtual BindType Type() = 0;

		const BindInfo& GetBindInfo();
		BindID GetBindID();

		bool BoundToStage(BindStage stage);
		bool BoundToOtherStage(BindStage stage);
		void SetBindStages(char stages);
		void AddBindStages(char stages);
		void RemoveBindStages(char stages);

	private:
		BindInfo m_bindInfo;
	};

	class FOX_API BindableResource : public Bindable
	{
	public:
		BindableResource(char reg = 0);
		virtual ~BindableResource() = default;

		// This unbinds the resource.
		void ChangeRegister();

		char GetReg();
		RegSampler GetRegSampler();
		RegCBV GetRegCBV();
		RegUAV GetRegUAV();
		RegSRV GetRegSRV();

	protected:
		void SetReg(char reg);

		// Shorthands
		static void BindSampler(BindStage stage, RegSampler reg, const ComPtr<ID3D11SamplerState>& sampler);
		static void BindSRV(BindStage stage, RegSRV reg, const ComPtr<ID3D11ShaderResourceView>& srv);
		static void BindUAV(BindStage stage, RegUAV reg, const ComPtr<ID3D11UnorderedAccessView>& uav);
		static void BindCBV(BindStage stage, RegCBV reg, const ComPtr<ID3D11Buffer>& cbv);

		static void UnbindSampler(BindStage stage, RegSampler reg);
		static void UnbindSRV(BindStage stage, RegSRV reg);
		static void UnbindUAV(BindStage stage, RegUAV reg);
		static void UnbindCBV(BindStage stage, RegCBV reg);

	private:
		char m_reg;
	};

	// Overrides:
	// 
	// 	void Bind(BindStage stage = BindStageNone) override;
	//	BindType Type() override;
	//

}