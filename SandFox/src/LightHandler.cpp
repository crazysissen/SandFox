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
	m_shadowCount(0),

	m_shadows{ {} }
{
}

SandFox::LightHandler::~LightHandler()
{
}

void SandFox::LightHandler::Init(TextureQuality quality, GraphicsTechnique technique, cs::Color ambient, float ambientIntensity)
{
	m_id = 1;
	m_quality = quality;
	m_flush = false;
	m_shadowCount = 0;

	m_lightInfo.ambient = (Vec3)ambient * ambientIntensity;
	m_lightInfo.totalLightCount = 0;
	m_lightInfo.shadowCount = 0;
	m_lightInfoBuffer.Load(RegCBVLightInfo, &m_lightInfo, sizeof(LightInfo), false);
	UpdateLightInfo();

	m_technique = technique;
	BindInfo();

	

	m_shader = Shader::Get(ShaderTypeShadow);

	uint res = GetTextureQuality(quality);

	// Create Texture resource
	D3D11_TEXTURE2D_DESC td;
	td.Width = res;
	td.Height = res;
	td.MipLevels = 1;
	td.ArraySize = FOX_C_MAX_SHADOWS;
	td.Format = DXGI_FORMAT_R32_TYPELESS;
	td.SampleDesc = { 1, 0 };
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateTexture2D(&td, nullptr, &m_shadowArray));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvd.Texture2DArray.MostDetailedMip = 0;
	srvd.Texture2DArray.MipLevels = 1;
	srvd.Texture2DArray.ArraySize = FOX_C_MAX_SHADOWS;
	srvd.Texture2DArray.FirstArraySlice = 0u;
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(m_shadowArray.Get(), &srvd, &m_shadowSRV));

	m_viewport.Load(res, res);

	for (int i = 0; i < FOX_C_MAX_SHADOWS; i++)
	{
		m_shadows[i].used = false;
		m_shadows[i].redraw = false;
		//m_shadows[i].quality = TextureQualityDefault;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
		dsvd.Format = DXGI_FORMAT_D32_FLOAT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
		dsvd.Texture2DMSArray.FirstArraySlice = i;
		dsvd.Texture2DMSArray.ArraySize = 1u;
		EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateDepthStencilView(m_shadowArray.Get(), &dsvd, &m_shadows[i].dsv));
	}

	BindHandler::ApplyPresetSampler(RegSamplerShadow, (technique == GraphicsTechniqueImmediate) ? BindStagePS : BindStageCS);
}

void SandFox::LightHandler::DeInit()
{
}

SandFox::LightID SandFox::LightHandler::AddDirectional(const cs::Vec3& angles, float intensity, const cs::Color& color)
{
	return AddLight(Light::Directional(angles, intensity, color));
}

SandFox::LightID SandFox::LightHandler::AddDirectionalShadowed(const cs::Vec3& angles, float shadowAreaWidth, float intensity, const cs::Color& color, float nearPlane, float farPlane)
{
	Light l = Light::Directional(angles, intensity, color);
	l.directionalWidth = shadowAreaWidth;

	return AddLightShadowed(l, nearPlane, farPlane);
}

SandFox::LightID SandFox::LightHandler::AddPoint(const cs::Vec3& position, float intensity, const cs::Color& color)
{
	return AddLight(Light::Point(position, intensity, color));
}

SandFox::LightID SandFox::LightHandler::AddPointShadowed(const cs::Vec3& position, float intensity, const cs::Color& color, float nearPlane, float farPlane)
{
	FOX_WARN("Shadow mapped point lights not supported, mapping disabled for this light.");

	return AddLightShadowed(Light::Point(position, intensity, color));
}

SandFox::LightID SandFox::LightHandler::AddSpot(const cs::Vec3& position, const cs::Vec3& angles, float spread, float intensity, const cs::Color& color)
{
	return AddLight(Light::Spot(position, angles, spread, intensity, color));
}

SandFox::LightID SandFox::LightHandler::AddSpotShadowed(const cs::Vec3& position, const cs::Vec3& angles, float spread, float intensity, const cs::Color& color, float nearPlane, float farPlane)
{
	return AddLightShadowed(Light::Spot(position, angles, spread, intensity, color), nearPlane, farPlane);
}

SandFox::LightID SandFox::LightHandler::AddLight(const Light& light)
{
	m_lightStructs.Add({ m_id++ });
	m_lights.Add(light);

	Light& l = m_lights.Back();

	//l.shadow = false;
	l.nearClip = 0;
	l.farClip = 0;
	l.shadowIndex = -1;
	l.projection = dx::XMMatrixIdentity();

	OrderLights();

	return  m_lightStructs.Back().id;
}

SandFox::LightID SandFox::LightHandler::AddLightShadowed(const Light& light, float nearPlane, float farPlane)
{
	int index = AddShadow(/*quality*/);
	if (index == -1)
	{
		FOX_ERROR("Failed to add shadow.");
		return AddLight(light);
	}

	m_lightStructs.Add({ m_id++  });
	m_lights.Add(light);

	Light& l = m_lights.Back();
	//l.shadow = true;
	l.nearClip = nearPlane;
	l.farClip = farPlane;
	l.shadowIndex = index;
	l.projection = dx::XMMatrixIdentity();

	OrderLights();

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
			if (m_lights[i].shadowIndex != -1)
			{
				RemoveShadow(m_lights[i].shadowIndex);
			}

			m_lightStructs.Remove(i);
			m_lights.Remove(i);

			OrderLights();

			return;
		}
	}

	FOX_WARN("Cannot remove light, ID doesn't exist.");
}

void SandFox::LightHandler::SetAmbient(const cs::Color& color, float intensity)
{
	m_lightInfo.ambient = (Vec3)color * intensity;
}

//void SandFox::LightHandler::SetSamplePoints(const cs::Vec3* points, int count)
//{
//	m_samplePoints = points;
//	m_samplePointCount = count;
//}

void SandFox::LightHandler::SetFocalPoint(const cs::Vec3& focalPoint, float distance)
{
	m_focalPoint = focalPoint;
	m_focalPointDistance = distance;
}

void SandFox::LightHandler::UpdateMap(LightID id)
{
	int i = m_lightStructs.SearchLinear(id, Predicate);

	if (i == -1)
	{
		FOX_FTRACE_F("Cannot queue update of shadow map at ID [%i], as the image does not exist.", id);
		return;
	}

	if (m_lights[i].shadowIndex == -1)
	{
		FOX_FTRACE_F("Cannot queue update of shadow map at ID [%i], as the image does not have a shadow attached.", id);
		return;
	}

	m_shadows[m_lights[i].shadowIndex].redraw = true;
}



void SandFox::LightHandler::Update(DrawQueue* drawQueue)
{
	for (int i = 0; i < m_lights.Size(); i++)
	{
		if (m_lights[i].shadowIndex != -1 && m_shadows[m_lights[i].shadowIndex].redraw)
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
		if (m_lights[i].shadowIndex != -1)
		{
			DrawMapAtIndex(i, drawQueue);
		}
	}
}

void SandFox::LightHandler::DrawMapAtIndex(int index, DrawQueue* drawQueue)
{
	LightStruct& ls = m_lightStructs[index];
	Light& l = m_lights[index];

	if (l.shadowIndex == -1)
	{
		FOX_WARN_F("Cannot draw shadow map of image at ID [%i], as it does not have a shadow map.", ls.id);
		return;
	}

	ShadowStruct& s = m_shadows[l.shadowIndex];



	// Get projection

	switch (l.type)
	{
	case LightTypeDirectional:
		l.projection = l.GetViewDirectional(m_focalPoint, m_focalPointDistance/*m_samplePoints, m_samplePointCount*/);
		break;

	case LightTypeSpot:
		l.projection = l.GetViewSpot();
		break;

	default:
		FOX_FTRACE_F("Unsupported light type [%i] for shadow mapping.", l.type);
		return;
	}



	// Drawing

	ID3D11ShaderResourceView* srvs[1u /*FOX_C_MAX_SHADOWS*/] = { nullptr };
	if (m_technique == GraphicsTechniqueDeferred)
	{
		Graphics::Get().GetContext()->CSSetShaderResources(RegSRVShadowDepth, 1u /*FOX_C_MAX_SHADOWS*/ , srvs);
	}
	else
	{
		Graphics::Get().GetContext()->PSSetShaderResources(RegSRVShadowDepth, 1u /*FOX_C_MAX_SHADOWS*/, srvs);
	}

	Graphics::SetProjection(l.projection);
	Shader::ShaderOverride(false);
	m_shader->Bind();
	Shader::ShaderOverride(true);

	m_viewport.Apply();

	Graphics::Get().GetContext()->ClearDepthStencilView(s.dsv.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);
	Graphics::Get().GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	Graphics::Get().GetContext()->OMSetRenderTargets(0, nullptr, s.dsv.Get());

	Graphics::Get().SetDepthStencil(true, true, D3D11_COMPARISON_GREATER);

	//drawQueue->DrawPre();
	drawQueue->DrawMain();
	//drawQueue->DrawPost();

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

	//OrderLights();
	//PushLights();

	/*ID3D11ShaderResourceView* srvs[FOX_C_MAX_SHADOWS] = { nullptr };

	for (int i = 0; i < FOX_C_MAX_SHADOWS; i++)
	{
		if (m_shadows[i].initialized)
		{
			srvs[m_shadows[i].targetIndex] = m_shadows[i].srv.Get();
		}
	}*/

	if (m_technique == GraphicsTechniqueDeferred)
	{
		Graphics::Get().GetContext()->CSSetShaderResources(RegSRVShadowDepth, 1u /*FOX_C_MAX_SHADOWS*/, m_shadowSRV.GetAddressOf());

		return;
	}

	Graphics::Get().GetContext()->PSSetShaderResources(RegSRVShadowDepth, 1u /*FOX_C_MAX_SHADOWS*/, m_shadowSRV.GetAddressOf());
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
	PushLights();

	m_lightInfo.totalLightCount = m_lights.Size();
	m_lightInfo.shadowCount = m_shadowCount;

	m_lightInfoBuffer.Write(&m_lightInfo);
}

int SandFox::LightHandler::AddShadow(/*LightShadowQuality quality*/)
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
		if (!m_shadows[index].used)
		{
			break;
		}
	}

	//Point res = m_mapResolutions[quality];

	ShadowStruct& s = m_shadows[index];
	s.used = true;
	//s.quality = quality;
	s.redraw = true;
	//s.texture.Load(nullptr, res.x, res.y, false, 0, DXGI_FORMAT_R32_TYPELESS, D3D11_BIND_DEPTH_STENCIL);
	//s.viewport.Load((float)res.x, (float)res.y);

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	//srvd.Format = DXGI_FORMAT_R32_FLOAT;
	//srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvd.Texture2D = { 0, 1 };
	//EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(s.texture.GetTexture().Get(), &srvd, &s.srv));

	//D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	//dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	//dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	//dsvd.Texture2D.MipSlice = 0u;
	//EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateDepthStencilView(s.texture.GetTexture().Get(), &dsvd, &s.dsv));

	Graphics::Get().GetContext()->ClearDepthStencilView(s.dsv.Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);

	return index;
}

void SandFox::LightHandler::RemoveShadow(int index)
{
	ShadowStruct& s = m_shadows[index];

	//s.srv.Reset();
	//s.texture.Unload();
	//s.dsv.Reset();
	s.used = false;
	s.redraw = false;
}

void SandFox::LightHandler::OrderLights()
{
	//std::memcpy(m_lightInfo.lights, m_lights.Data(), sizeof(Light) * m_lights.Size());

	//int currentShadow = 0;
	//for (int i = 0; i < m_lights.Size() && currentShadow < m_shadowCount; i++)
	//{
	//	if (m_lightInfo.lights[i].shadowIndex == -1)
	//	{
	//		int index = i;

	//		if (i > currentShadow)
	//		{
	//			Light l = m_lightInfo.lights[currentShadow];
	//			m_lightInfo.lights[currentShadow] = m_lightInfo.lights[i];
	//			m_lightInfo.lights[i] = l;

	//			index = currentShadow;
	//		}

	//		m_shadows[m_lightInfo.lights[currentShadow].shadowIndex].targetIndex = currentShadow;

	//		currentShadow++;
	//	}
	//}
}

void SandFox::LightHandler::PushLights()
{
	std::memcpy(m_lightInfo.lights, m_lights.Data(), sizeof(Light) * m_lights.Size());
}

SandFox::LightID SandFox::LightHandler::Predicate(const LightStruct& l)
{
	return l.id;
}
