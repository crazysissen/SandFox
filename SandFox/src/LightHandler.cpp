#include "pch.h"
#include "LightHandler.h"

#include "Graphics.h"

SandFox::LightHandler::LightHandler()
	:
	m_id(),
	m_flush(),

	m_lights(),
	m_lightInfo(),
	m_lightInfoBuffer(),

	m_shadows{ {} }
{
}

SandFox::LightHandler::~LightHandler()
{
}

void SandFox::LightHandler::Init(GraphicsTechnique technique, cs::Color ambient, float ambientIntensity)
{
	m_id = 1;
	m_flush = false;

	m_lightInfo.ambient = (Vec3)ambient * ambientIntensity;
	m_lightInfo.lightCount = 0;
	m_lightInfoBuffer.Load(RegCBVLightInfo, &m_lightInfo, sizeof(LightInfo), false);
	UpdateLightInfo();

	m_technique = technique;
	BindInfo();

	m_mapResolutions[0] = { 512, 512 };
	m_mapResolutions[1] = { 256, 256 };
	m_mapResolutions[2] = { 1024, 1024 };
	m_mapResolutions[3] = { 2048, 2048 };
	m_mapResolutions[4] = { 4096, 4096 };

	for (int i = 0; i < FOX_C_MAX_SHADOWS; i++)
	{
		m_shadows[i].initialized = false;
		m_shadows[i].redraw = false;
		m_shadows[i].quality = LightShadowQualityDefault;
	}

	m_shader = Shader::Get(ShaderTypeShadow);

	BindHandler::ApplyPresetSampler(RegSamplerShadow, (technique == GraphicsTechniqueImmediate) ? BindStagePS : BindStageCS);
}

void SandFox::LightHandler::DeInit()
{
}

SandFox::LightID SandFox::LightHandler::AddDirectional(const cs::Vec3& angles, float intensity, const cs::Color& color)
{
	return AddLight(Light::Directional(angles, intensity, color));
}

SandFox::LightID SandFox::LightHandler::AddDirectional(LightShadowQuality quality, const cs::Vec3& angles, float intensity, float nearPlane, float farPlane, const cs::Color& color)
{
	return AddLight(quality, Light::Directional(angles, intensity, color), nearPlane, farPlane);
}

SandFox::LightID SandFox::LightHandler::AddPoint(const cs::Vec3& position, float intensity, const cs::Color& color)
{
	return AddLight(Light::Point(position, intensity, color));
}

SandFox::LightID SandFox::LightHandler::AddPoint(LightShadowQuality quality, const cs::Vec3& position, float intensity, float nearPlane, float farPlane, const cs::Color& color)
{
	FOX_WARN("Shadow mapped point lights not supported, mapping disabled for this light.");

	return AddLight(Light::Point(position, intensity, color));
}

SandFox::LightID SandFox::LightHandler::AddSpot(const cs::Vec3& position, const cs::Vec3& angles, float spread, float intensity, const cs::Color& color)
{
	return AddLight(Light::Spot(position, angles, spread, intensity, color));
}

SandFox::LightID SandFox::LightHandler::AddSpot(LightShadowQuality quality, const cs::Vec3& position, const cs::Vec3& angles, float spread, float intensity, float nearPlane, float farPlane, const cs::Color& color)
{
	return AddLight(quality, Light::Spot(position, angles, spread, intensity, color), nearPlane, farPlane);
}

SandFox::LightID SandFox::LightHandler::AddLight(const Light& light)
{
	m_lightStructs.Add({ m_id++, -1 });
	m_lights.Add(light);

	Light& l = m_lights.Back();

	l.shadow = false;
	l.nearClip = 0;
	l.farClip = 0;
	l.shadowIndex = -1;
	l.projection = dx::XMMatrixIdentity();

	return  m_lightStructs.Back().id;
}

SandFox::LightID SandFox::LightHandler::AddLight(LightShadowQuality quality, const Light& light, float nearPlane, float farPlane)
{
	int index = AddShadow(quality);
	if (index == -1)
	{
		return AddLight(light);
	}

	m_lightStructs.Add({ m_id++, index });
	m_lights.Add(light);

	Light& l = m_lights.Back();
	l.shadow = true;
	l.nearClip = nearPlane;
	l.farClip = farPlane;
	l.shadowIndex = index;
	l.projection = dx::XMMatrixIdentity();

	return m_lightStructs.Back().id;
}

SandFox::Light& SandFox::LightHandler::GetLight(LightID id)
{
	for (int i = 0; i < m_lights.Size(); i++)	// TODO: add log(n) search
	{
		if (m_lightStructs[i].id == id)
		{
			return m_lights[i];
		}
	}

	FOX_CRITICAL("Cannot get light, ID doesn't exist.");
	EXC("Cannot get light, ID doesn't exist.");
}

void SandFox::LightHandler::RemoveLight(LightID id)
{
	for (int i = 0; i < m_lights.Size(); i++)	// TODO: add log(n) search
	{
		if (m_lightStructs[i].id == id)
		{
			if (m_lightStructs[i].shadowIndex != -1)
			{
				RemoveShadow(m_lightStructs[i].shadowIndex);
			}

			m_lightStructs.Remove(i);
			m_lights.Remove(i);
			return;
		}
	}

	FOX_WARN("Cannot remove light, ID doesn't exist.");
}

void SandFox::LightHandler::SetAmbient(const cs::Color& color, float intensity)
{
	m_lightInfo.ambient = (Vec3)color * intensity;
}

void SandFox::LightHandler::SetSamplePoints(const cs::Vec3* points, int count)
{
	m_samplePoints = points;
	m_samplePointCount = count;
}

void SandFox::LightHandler::UpdateMap(LightID id)
{
	int i = m_lightStructs.SearchLinear(id, Predicate);

	if (i == -1)
	{
		FOX_WARN_F("Cannot queue update of shadow map at ID [%i], as the image does not exist.", id);
		return;
	}

	if (m_lightStructs[i].shadowIndex == -1)
	{
		FOX_WARN_F("Cannot queue update of shadow map at ID [%i], as the image does not have a shadow attached.", id);
		return;
	}

	m_shadows[m_lightStructs[i].shadowIndex].redraw = true;
}



void SandFox::LightHandler::Update(DrawQueue* drawQueue)
{
	for (int i = 0; i < m_lights.Size(); i++)
	{
		if (m_lightStructs[i].shadowIndex != -1 && m_shadows[m_lightStructs[i].shadowIndex].redraw)
		{
			DrawMapAtIndex(i, drawQueue);
		}
	}
}

void SandFox::LightHandler::DrawMap(LightID id, DrawQueue* drawQueue)
{
	int i = m_lightStructs.SearchLinear(id, Predicate);

	if (i == -1)
	{
		FOX_WARN_F("Cannot draw shadow map of image at ID [%i], as the image does not exist.", id);
		return;
	}

	DrawMapAtIndex(i, drawQueue);
}

void SandFox::LightHandler::DrawAllMaps(DrawQueue* drawQueue)
{
	for (int i = 0; i < m_lights.Size(); i++)
	{
		if (m_lightStructs[i].shadowIndex != -1)
		{
			DrawMapAtIndex(i, drawQueue);
		}
	}
}

void SandFox::LightHandler::DrawMapAtIndex(int index, DrawQueue* drawQueue)
{
	LightStruct& ls = m_lightStructs[index];
	Light& l = m_lights[index];

	if (ls.shadowIndex == -1)
	{
		FOX_WARN_F("Cannot draw shadow map of image at ID [%i], as it does not have a shadow map.", index);
		return;
	}

	ShadowStruct& s = m_shadows[ls.shadowIndex];



	// Get projection

	switch (l.type)
	{
	case LightTypeDirectional:
		l.projection = l.GetViewDirectional(m_samplePoints, m_samplePointCount);
		break;

	case LightTypeSpot:
		l.projection = l.GetViewSpot();
		break;

	default:
		FOX_FTRACE_F("Unsupported light type [%i] for shadow mapping.", l.type);
		return;
	}



	// Drawing

	ID3D11ShaderResourceView* srvs[FOX_C_MAX_SHADOWS] = { nullptr };
	if (m_technique == GraphicsTechniqueDeferred)
	{
		Graphics::Get().GetContext()->CSSetShaderResources(RegSRVShadowDepth, FOX_C_MAX_SHADOWS, srvs);
	}
	else
	{
		Graphics::Get().GetContext()->PSSetShaderResources(RegSRVShadowDepth, FOX_C_MAX_SHADOWS, srvs);
	}

	Graphics::SetProjection(l.projection);
	Shader::ShaderOverride(false);
	m_shader->Bind();
	Shader::ShaderOverride(true);

	s.viewport.Apply();

	Graphics::Get().GetContext()->ClearDepthStencilView(s.dsv.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);
	Graphics::Get().GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	Graphics::Get().GetContext()->OMSetRenderTargets(0, nullptr, s.dsv.Get());

	Graphics::Get().SetDepthStencil(true, true, D3D11_COMPARISON_GREATER);

	drawQueue->DrawMain();

	Graphics::Get().GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	Shader::ShaderOverride(false);

	s.redraw = false;

	Graphics::Get().GetContext()->Flush();

	Graphics::Get().SetDepthStencil(true, true);
	BindHandler::UnbindShader();
	BindHandler::UnbindPipeline();
}



void SandFox::LightHandler::BindMaps()
{
	Graphics::Get().GetContext()->OMSetRenderTargets(0, nullptr, nullptr);

	ID3D11ShaderResourceView* srvs[FOX_C_MAX_SHADOWS] = { nullptr };

	for (int i = 0; i < FOX_C_MAX_SHADOWS; i++)
	{
		if (m_shadows[i].initialized)
		{
			srvs[i] = m_shadows[i].srv.Get();
		}
	}

	if (m_technique == GraphicsTechniqueDeferred)
	{
		Graphics::Get().GetContext()->CSSetShaderResources(RegSRVShadowDepth, FOX_C_MAX_SHADOWS, srvs);

		return;
	}

	Graphics::Get().GetContext()->PSSetShaderResources(RegSRVShadowDepth, FOX_C_MAX_SHADOWS, srvs);
}

void SandFox::LightHandler::BindInfo()
{
	if (m_technique == GraphicsTechniqueDeferred)
	{
		BindDeferred();
		return;
	}

	BindImmediate();
}

void SandFox::LightHandler::BindImmediate()
{
	m_lightInfoBuffer.Bind(BindStagePS);
}

void SandFox::LightHandler::BindDeferred()
{
	m_lightInfoBuffer.Bind(BindStageCS);
}

void SandFox::LightHandler::UpdateLightInfo()
{
	std::memcpy(m_lightInfo.lights, m_lights.Data(), sizeof(Light) * m_lights.Size());
	m_lightInfo.lightCount = m_lights.Size();

	m_lightInfoBuffer.Write(&m_lightInfo);
}

int SandFox::LightHandler::AddShadow(LightShadowQuality quality)
{
	if (m_shadowCount >= FOX_C_MAX_SHADOWS)
	{
		FOX_WARN("Cannot add shadow map to light, as program has reached the maximum number of shadowed lights.");
		return -1;
	}

	// Linear search for shadow slot. This will never be a bottleneck
	int index = 0;
	for (; index < FOX_C_MAX_SHADOWS; index++)
	{
		if (!m_shadows[index].initialized)
		{
			break;
		}
	}

	Point res = m_mapResolutions[quality];

	ShadowStruct& s = m_shadows[index];
	s.initialized = true;
	s.quality = quality;
	s.redraw = true;
	s.texture.Load(nullptr, res.x, res.y, false, 0, DXGI_FORMAT_R32_TYPELESS, D3D11_BIND_DEPTH_STENCIL);
	s.viewport.Load((float)res.x, (float)res.y);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1 };
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(s.texture.GetTexture().Get(), &srvd, &s.srv));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.Texture2D.MipSlice = 0u;
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateDepthStencilView(s.texture.GetTexture().Get(), &dsvd, &s.dsv));

	Graphics::Get().GetContext()->ClearDepthStencilView(s.dsv.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

	return index;
}

void SandFox::LightHandler::RemoveShadow(int index)
{
	ShadowStruct& s = m_shadows[index];

	s.srv.Reset();
	s.dsv.Reset();
	s.texture.Unload();
	s.initialized = false;
	s.redraw = false;
}

SandFox::LightID SandFox::LightHandler::Predicate(const LightStruct& l)
{
	return l.id;
}
