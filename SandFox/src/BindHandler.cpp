#include "pch.h"
#include "BindHandler.h"



SandFox::BindPipeline::BindPipeline()
    :
    m_id(BindHandler::GetID())
{
}

SandFox::BindPipeline::BindPipeline(const BindPipeline& copy)
    :
    m_id(copy.m_id),

    vb(copy.vb),
    ib(copy.ib),
    shader(copy.shader)
{
}

SandFox::BindID SandFox::BindPipeline::GetID() const
{
    return m_id;
}



SandFox::BindHandler* SandFox::BindHandler::s_bindHandler = nullptr;
SandFox::BindID SandFox::BindHandler::s_idCounter = 1;

SandFox::BindHandler::BindHandler()
    :
    m_pipeline(0),
    m_shader(0),
    m_vs(0),
    m_gs(0),
    m_ps(0),
    m_il(0),
    m_vb(0),
    m_ib(0),
    m_pt(0),

    m_stageResources { BindResource() },

    m_savedSamplers { nullptr }
{
}

SandFox::BindHandler::~BindHandler()
{
}

void SandFox::BindHandler::Init()
{
    s_bindHandler = this;

    m_savedSamplers[RegSamplerStandard - 1] = new Bind::SamplerState(RegSamplerStandard, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
    m_savedSamplers[RegSamplerPoint - 1] = new Bind::SamplerState(RegSamplerStandard, D3D11_FILTER_MIN_MAG_MIP_POINT);
    m_savedSamplers[RegSamplerShadow - 1] = new Bind::SamplerState(RegSamplerShadow, D3D11_FILTER_ANISOTROPIC, cs::Color(0.0f, 0.0f, 0.0f));
}

void SandFox::BindHandler::DeInit()
{
    if (s_bindHandler == this)
    {
        s_bindHandler = nullptr;
    }

    for (int i = 0; i < RegSamplerCount - 1; i++)
    {
        delete m_savedSamplers[i];
    }
}



SandFox::BindID SandFox::BindHandler::GetID()
{
    return s_idCounter++;
}

void SandFox::BindHandler::PresetSampler(Bind::SamplerState* sampler, RegSampler reg)
{
    BindID overrideID = 0;

    if (reg == 0)
    {
        FOX_WARN_F("Sampler cannot be preset to zero-register, as this is reserved for manual binding.");
        return;

    }

    if (s_bindHandler->m_savedSamplers[reg] != nullptr)
    {
        FOX_WARN_F("Sampler preset at nonzero register [%i] overriden by new sampler. New sampler will be rebound to all the same stages.", reg);
        overrideID = s_bindHandler->m_savedSamplers[reg]->GetBindInfo().id;
        delete s_bindHandler->m_savedSamplers[reg];
    }

    s_bindHandler->m_savedSamplers[reg] = sampler;

    if (overrideID > 0)
    {
        for (byte i = 0; i < BindStageCount - 1; i++)
        {
            BindResource& r = s_bindHandler->m_stageResources[i].m_samplers[reg];

            if (r.id == overrideID)
            {
                BindSampler((BindStage)(i + 1), sampler);
            }
        }
    }
}

void SandFox::BindHandler::ApplyPresetSampler(RegSampler reg, BindStage stage)
{
    Bind::SamplerState* s = s_bindHandler->m_savedSamplers[reg - 1];

    if (s == nullptr)
    {
        FOX_WARN_F("Sampler preset at register [%i] not applied to stage [%i] as it hasn't been set.", reg, stage);
    }

    if (!s->BoundToStage(stage))
    {
        s->Bind(stage);
    }
}



// Pipeline and pipeline stages

bool SandFox::BindHandler::BindPipeline(SandFox::BindPipeline* pipeline)
{
    bool different = s_bindHandler->m_pipeline != pipeline->GetID();
    s_bindHandler->m_pipeline = pipeline->GetID();
    return different;
}

bool SandFox::BindHandler::BindShader(Shader* shader)
{
    bool different = s_bindHandler->m_shader != shader->GetBindID();
    s_bindHandler->m_shader = shader->GetBindID();
    return different;
}

bool SandFox::BindHandler::BindVS(Bind::VertexShader* vs)
{
    bool different = s_bindHandler->m_vs != vs->GetBindID();
    s_bindHandler->m_vs = vs->GetBindID();
    return different;
}

bool SandFox::BindHandler::BindGS(Bind::GeometryShader* gs)
{
    bool different = s_bindHandler->m_gs != gs->GetBindID();
    s_bindHandler->m_gs = gs->GetBindID();
    return different;
}

bool SandFox::BindHandler::BindPS(Bind::PixelShader* ps)
{
    bool different = s_bindHandler->m_ps != ps->GetBindID();
    s_bindHandler->m_ps = ps->GetBindID();
    return different;
}

bool SandFox::BindHandler::BindIL(Bind::InputLayout* il)
{
    bool different = s_bindHandler->m_il != il->GetBindID();
    s_bindHandler->m_il = il->GetBindID();
    return different;
}

bool SandFox::BindHandler::BindPT(Bind::PrimitiveTopology* pt)
{
    bool different = s_bindHandler->m_pt != pt->GetBindID();
    s_bindHandler->m_pt = pt->GetBindID();
    return different;
}

bool SandFox::BindHandler::BindVB(Bind::VertexBuffer* vb)
{
    bool different = s_bindHandler->m_vb != vb->GetBindID();
    s_bindHandler->m_vb = vb->GetBindID();
    return different;
}

bool SandFox::BindHandler::BindIB(Bind::IndexBuffer* ib)
{
    bool different = s_bindHandler->m_ib != ib->GetBindID();
    s_bindHandler->m_ib = ib->GetBindID();
    return different;
}



// Resources

bool SandFox::BindHandler::BindSampler(BindStage stage, Bind::SamplerState* sampler)
{
    BindResource& r = s_bindHandler->m_stageResources[stage - 1].m_samplers[sampler->GetReg()];
    return s_bindHandler->BindResourceView(r, stage, BindTypeSampler, sampler);
}

bool SandFox::BindHandler::BindCBV(BindStage stage, Bind::ConstBuffer* cbv)
{
    BindResource& r = s_bindHandler->m_stageResources[stage - 1].m_cbvs[cbv->GetReg()];
    return s_bindHandler->BindResourceView(r, stage, BindTypeConstantBuffer, cbv);
}

bool SandFox::BindHandler::BindUAV(BindStage stage, BindableResource* uav)
{
    BindResource& r = s_bindHandler->m_stageResources[stage - 1].m_uavs[uav->GetReg()];
    return s_bindHandler->BindResourceView(r, stage, BindTypeUnorderedAccess, uav);
}

bool SandFox::BindHandler::BindSRV(BindStage stage, BindableResource* srv)
{
    BindResource& r = s_bindHandler->m_stageResources[stage - 1].m_srvs[srv->GetReg()];
    return s_bindHandler->BindResourceView(r, stage, BindTypeShaderResource, srv);
}

void SandFox::BindHandler::UnbindPipeline()
{
    s_bindHandler->m_pipeline = 0;
}

void SandFox::BindHandler::UnbindShader()
{
    s_bindHandler->m_shader = 0;
}

void SandFox::BindHandler::UnbindVS()
{
    s_bindHandler->m_vs = 0;
}

void SandFox::BindHandler::UnbindGS()
{
    s_bindHandler->m_gs = 0;
}

void SandFox::BindHandler::UnbindPS()
{
    s_bindHandler->m_ps = 0;
}

void SandFox::BindHandler::UnbindIL()
{
    s_bindHandler->m_il = 0;
}

void SandFox::BindHandler::UnbindPT()
{
    s_bindHandler->m_vb = 0;
}

void SandFox::BindHandler::UnbindVB()
{
    s_bindHandler->m_ib = 0;
}

void SandFox::BindHandler::UnbindIB()
{
    s_bindHandler->m_pt = 0;
}

void SandFox::BindHandler::UnbindSampler(BindStage stage, RegSampler reg)
{
    BindResource& r = s_bindHandler->m_stageResources[stage - 1].m_samplers[reg];
    s_bindHandler->UnbindResourceView(r, stage, BindTypeSampler);
}

void SandFox::BindHandler::UnbindCBV(BindStage stage, RegCBV reg)
{
    BindResource& r = s_bindHandler->m_stageResources[stage - 1].m_cbvs[reg];
    s_bindHandler->UnbindResourceView(r, stage, BindTypeConstantBuffer);
}

void SandFox::BindHandler::UnbindUAV(BindStage stage, RegUAV reg)
{
    BindResource& r = s_bindHandler->m_stageResources[stage - 1].m_uavs[reg];
    s_bindHandler->UnbindResourceView(r, stage, BindTypeUnorderedAccess);
}

void SandFox::BindHandler::UnbindSRV(BindStage stage, RegSRV reg)
{
    BindResource& r = s_bindHandler->m_stageResources[stage - 1].m_srvs[reg];
    s_bindHandler->UnbindResourceView(r, stage, BindTypeShaderResource);
}

bool SandFox::BindHandler::BindResourceView(SandFox::BindHandler::BindResource& r, BindStage stage, BindType type, BindableResource* bindable)
{
    if (stage == BindStageNone)
    {
        FOX_WARN_F("Resource type [%i] cannot be bound to StageNone, as it is a shader resource. Register: [%i]", type, bindable->GetReg());
        return false;
    }

    bool different = r.id != bindable->GetBindID();

    if (different)
    {
        bindable->AddBindStages(stage);

        if (r.resource)
        {
            r.resource->RemoveBindStages(stage);
        }

        r.resource = bindable;
        r.id = bindable->GetBindID();

        return true;
    }

    return false;
}

void SandFox::BindHandler::UnbindResourceView(SandFox::BindHandler::BindResource& r, BindStage stage, BindType type)
{
    if (stage == BindStageNone)
    {
        FOX_LOG_F("Cannot unbind resource type [%i] from StageNone. Register: [%i]", type);
        return;
    }

    if (r.id != 0)
    {
        r.id = 0;
        r.resource->RemoveBindStages(stage);
    }
}
