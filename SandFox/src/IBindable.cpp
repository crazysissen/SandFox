#include "pch.h"

#include "IBindable.h"
#include "BindHandler.h"
#include "Graphics.h"

SandFox::Bindable::Bindable()
	:
	m_bindInfo
	{
		BindHandler::GetID(),
		BindStageNone
	}
{

}

const SandFox::BindInfo& SandFox::Bindable::GetBindInfo()
{
	return m_bindInfo;
}

SandFox::BindID SandFox::Bindable::GetBindID()
{
	return m_bindInfo.id;
}

inline bool SandFox::Bindable::BoundToStage(BindStage stage)
{
	return (1 << (stage - 1)) & m_bindInfo.stages;
}

inline bool SandFox::Bindable::BoundToOtherStage(BindStage stage)
{
	return (~(1 << (stage - 1))) & m_bindInfo.stages;
}

inline void SandFox::Bindable::SetBindStages(char stages)
{
	m_bindInfo.stages = stages;
}

inline void SandFox::Bindable::AddBindStages(char stages)
{
	m_bindInfo.stages |= stages;
}

inline void SandFox::Bindable::RemoveBindStages(char stages)
{
	m_bindInfo.stages &= ~stages;
}





SandFox::BindableResource::BindableResource(char reg)
	:
	m_reg(reg)
{
}

void SandFox::BindableResource::ChangeRegister()
{
	char stages = GetBindInfo().stages;

	switch (Type())
	{
	case BindTypeSampler:
		for (int i = 0; i < BindStageCount - 1; i++)
		{
			if (0x1 & (stages >> i))
			{
				BindHandler::UnbindSampler((BindStage)(i + 1), GetRegSampler());
			}
		}
		break;

	case BindTypeConstantBuffer:
		for (int i = 0; i < BindStageCount - 1; i++)
		{
			if (0x1 & (stages >> i))
			{
				BindHandler::UnbindCBV((BindStage)(i + 1), GetRegCBV());
			}
		}

		break;

	case BindTypeUnorderedAccess:
		for (int i = 0; i < BindStageCount - 1; i++)
		{
			if (0x1 & (stages >> i))
			{
				BindHandler::UnbindUAV((BindStage)(i + 1), GetRegUAV());
			}
		}

		break;

	case BindTypeShaderResource:
		for (int i = 0; i < BindStageCount - 1; i++)
		{
			if (0x1 & (stages >> i))
			{
				BindHandler::UnbindSRV((BindStage)(i + 1), GetRegSRV());
			}
		}
		break;

	default:
		FOX_WARN("Cannot change register of non-resource type.");
		break;
	}
}

char SandFox::BindableResource::GetReg()
{
	return m_reg;
}

SandFox::RegSampler SandFox::BindableResource::GetRegSampler()
{
	return (RegSampler)m_reg;
}

SandFox::RegCBV SandFox::BindableResource::GetRegCBV()
{
	return (RegCBV)m_reg;
}

SandFox::RegUAV SandFox::BindableResource::GetRegUAV()
{
	return (RegUAV)m_reg;
}

SandFox::RegSRV SandFox::BindableResource::GetRegSRV()
{
	return (RegSRV)m_reg;
}

void SandFox::BindableResource::SetReg(char reg)
{
	m_reg = reg;
}

void SandFox::BindableResource::BindSampler(BindStage stage, RegSampler reg, const ComPtr<ID3D11SamplerState>& sampler)
{
	switch (stage)
	{
	case SandFox::BindStageVS:
		Graphics::Get().GetContext()->VSSetSamplers((uint)reg, 1, sampler.GetAddressOf());
		break;

	case SandFox::BindStageHS:
		Graphics::Get().GetContext()->HSSetSamplers((uint)reg, 1, sampler.GetAddressOf());
		break;

	case SandFox::BindStageDS:
		Graphics::Get().GetContext()->DSSetSamplers((uint)reg, 1, sampler.GetAddressOf());
		break;

	case SandFox::BindStageGS:
		Graphics::Get().GetContext()->GSSetSamplers((uint)reg, 1, sampler.GetAddressOf());
		break;

	case SandFox::BindStagePS:
		Graphics::Get().GetContext()->PSSetSamplers((uint)reg, 1, sampler.GetAddressOf());
		break;

	case SandFox::BindStageCS:
		Graphics::Get().GetContext()->CSSetSamplers((uint)reg, 1, sampler.GetAddressOf());
		break;

	default:
		FOX_ERROR_F("Sampler binding to stage [%i] not supported.", stage);
		break;
	}
}

void SandFox::BindableResource::BindSRV(BindStage stage, RegSRV reg, const ComPtr<ID3D11ShaderResourceView>& srv)
{
	switch (stage)
	{
	case SandFox::BindStageVS:
		Graphics::Get().GetContext()->VSSetShaderResources((uint)reg, 1, srv.GetAddressOf());
		break;

	case SandFox::BindStageHS:
		Graphics::Get().GetContext()->HSSetShaderResources((uint)reg, 1, srv.GetAddressOf());
		break;

	case SandFox::BindStageDS:
		Graphics::Get().GetContext()->DSSetShaderResources((uint)reg, 1, srv.GetAddressOf());
		break;

	case SandFox::BindStageGS:
		Graphics::Get().GetContext()->GSSetShaderResources((uint)reg, 1, srv.GetAddressOf());
		break;

	case SandFox::BindStagePS:
		Graphics::Get().GetContext()->PSSetShaderResources((uint)reg, 1, srv.GetAddressOf());
		break;

	case SandFox::BindStageCS:
		Graphics::Get().GetContext()->CSSetShaderResources((uint)reg, 1, srv.GetAddressOf());
		break;

	default:
		FOX_ERROR_F("Shader Resource binding to stage [%i] not supported.", stage);
		break;
	}
}

void SandFox::BindableResource::BindUAV(BindStage stage, RegUAV reg, const ComPtr<ID3D11UnorderedAccessView>& uav)
{
	switch (stage)
	{
		//case SandFox::BindStageNone:
		//	break;
		//case SandFox::BindStageVS:
		//	break;
		//case SandFox::BindStageHS:
		//	break;
		//case SandFox::BindStageDS:
		//	break;
		//case SandFox::BindStageGS:
		//	break;
		//case SandFox::BindStagePS:
		//	break;

	case SandFox::BindStageCS:
		Graphics::Get().GetContext()->CSSetUnorderedAccessViews((uint)reg, 1, uav.GetAddressOf(), nullptr);
		break;

	default:
		FOX_ERROR_F("Unordered Access binding to stage [%i] not supported.", stage);
		break;
	}
}

void SandFox::BindableResource::BindCBV(BindStage stage, RegCBV reg, const ComPtr<ID3D11Buffer>& cbv)
{
	switch (stage)
	{
	case SandFox::BindStageVS:
		Graphics::Get().GetContext()->VSSetConstantBuffers((uint)reg, 1, cbv.GetAddressOf());
		break;

	case SandFox::BindStageHS:
		Graphics::Get().GetContext()->HSSetConstantBuffers((uint)reg, 1, cbv.GetAddressOf());
		break;

	case SandFox::BindStageDS:
		Graphics::Get().GetContext()->DSSetConstantBuffers((uint)reg, 1, cbv.GetAddressOf());
		break;

	case SandFox::BindStageGS:
		Graphics::Get().GetContext()->GSSetConstantBuffers((uint)reg, 1, cbv.GetAddressOf());
		break;

	case SandFox::BindStagePS:
		Graphics::Get().GetContext()->PSSetConstantBuffers((uint)reg, 1, cbv.GetAddressOf());
		break;

	case SandFox::BindStageCS:
		Graphics::Get().GetContext()->CSSetConstantBuffers((uint)reg, 1, cbv.GetAddressOf());
		break;

	default:
		FOX_ERROR_F("Constant buffer binding to stage [%i] not supported.", stage);
		break;
	}
}

void SandFox::BindableResource::UnbindSampler(BindStage stage, RegSampler reg)
{
	ID3D11SamplerState* empty = nullptr;

	switch (stage)
	{
	case SandFox::BindStageVS:
		Graphics::Get().GetContext()->VSSetSamplers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageHS:
		Graphics::Get().GetContext()->HSSetSamplers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageDS:
		Graphics::Get().GetContext()->DSSetSamplers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageGS:
		Graphics::Get().GetContext()->GSSetSamplers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStagePS:
		Graphics::Get().GetContext()->PSSetSamplers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageCS:
		Graphics::Get().GetContext()->CSSetSamplers((uint)reg, 1, &empty);
		break;

	default:
		FOX_ERROR_F("Sampler unbinding from stage [%i] not supported.", stage);
		break;
	}
}

void SandFox::BindableResource::UnbindSRV(BindStage stage, RegSRV reg)
{
	ID3D11ShaderResourceView* empty = nullptr;

	switch (stage)
	{
	case SandFox::BindStageVS:
		Graphics::Get().GetContext()->VSSetShaderResources((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageHS:
		Graphics::Get().GetContext()->HSSetShaderResources((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageDS:
		Graphics::Get().GetContext()->DSSetShaderResources((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageGS:
		Graphics::Get().GetContext()->GSSetShaderResources((uint)reg, 1, &empty);
		break;

	case SandFox::BindStagePS:
		Graphics::Get().GetContext()->PSSetShaderResources((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageCS:
		Graphics::Get().GetContext()->CSSetShaderResources((uint)reg, 1, &empty);
		break;

	default:
		FOX_ERROR_F("Shader Resource unbinding from stage [%i] not supported.", stage);
		break;
	}
}

void SandFox::BindableResource::UnbindUAV(BindStage stage, RegUAV reg)
{
	ID3D11UnorderedAccessView* empty = nullptr;

	switch (stage)
	{
		//case SandFox::BindStageNone:
		//	break;
		//case SandFox::BindStageVS:
		//	break;
		//case SandFox::BindStageHS:
		//	break;
		//case SandFox::BindStageDS:
		//	break;
		//case SandFox::BindStageGS:
		//	break;
		//case SandFox::BindStagePS:
		//	break;

	case SandFox::BindStageCS:
		Graphics::Get().GetContext()->CSSetUnorderedAccessViews((uint)reg, 1, &empty, nullptr);
		break;

	default:
		FOX_ERROR_F("Unordered Access unbinding from stage [%i] not supported.", stage);
		break;
	}
}

void SandFox::BindableResource::UnbindCBV(BindStage stage, RegCBV reg)
{
	ID3D11Buffer* empty = nullptr;

	switch (stage)
	{
	case SandFox::BindStageVS:
		Graphics::Get().GetContext()->VSSetConstantBuffers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageHS:
		Graphics::Get().GetContext()->HSSetConstantBuffers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageDS:
		Graphics::Get().GetContext()->DSSetConstantBuffers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageGS:
		Graphics::Get().GetContext()->GSSetConstantBuffers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStagePS:
		Graphics::Get().GetContext()->PSSetConstantBuffers((uint)reg, 1, &empty);
		break;

	case SandFox::BindStageCS:
		Graphics::Get().GetContext()->CSSetConstantBuffers((uint)reg, 1, &empty);
		break;

	default:
		FOX_ERROR_F("Constant buffer unbinding from stage [%i] not supported.", stage);
		break;
	}
}
