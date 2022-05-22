#include "pch.h"
#include "LightHandler.h"

#include "Graphics.h"

SandFox::LightHandler::LightHandler()
	:
	m_id(),
	m_flush(),

	m_ambient(),
	m_lights(),
	m_white(nullptr),
	m_whiteSRV(nullptr),

	m_lightInfo(),
	m_lightInfoBuffer()
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

	Bind(technique);

	m_mapResolutions[0] = { 1024, 1024 };
	m_mapResolutions[1] = { 512, 512 };
	m_mapResolutions[2] = { 256, 256 };
	m_mapResolutions[3] = { 2048, 2048 };
	m_mapResolutions[4] = { 4096, 4096 };
}

void SandFox::LightHandler::DeInit()
{
}

SandFox::LightID SandFox::LightHandler::AddDirectional(const cs::Vec3& angles, float intensity, float nearPlane, float farPlane, LightShadowQuality quality, const cs::Color& color)
{
	return LightID();
}

SandFox::LightID SandFox::LightHandler::AddPoint(const cs::Vec3& angles, float intensity, float nearPlane, float farPlane, LightShadowQuality quality, const cs::Color& color)
{
	return LightID();
}

SandFox::LightID SandFox::LightHandler::AddSpot(const cs::Vec3& angles, const cs::Vec3& direction, float spread, float intensity, float nearPlane, float farPlane, LightShadowQuality quality, const cs::Color& color)
{
	return LightID();
}

SandFox::LightID SandFox::LightHandler::AddLight(const Light& light, float nearPlane, float farPlane, LightShadowQuality quality)
{
	m_lights.Add(
		{
			m_id++,
			light,

			true,
			nearPlane,
			farPlane,
			quality
		}
	);

	LightStruct& l = m_lights.Back();

	if (l.light.shadow)
	{
		Point res = m_mapResolutions[quality];

		l.redraw = true;
		l.quality = quality;

		l.texture.Load(cs::Color(1.0f, 1.0f, 1.0f), res.x, res.y, DXGI_FORMAT_R32_TYPELESS, D3D11_BIND_DEPTH_STENCIL);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Format = DXGI_FORMAT_R32_FLOAT;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D = { 0, 1 };
		EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(l.texture.GetTexture().Get(), &srvd, &l.srv));

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvDesc.Texture2D.MipSlice = 0u;
		EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateDepthStencilView(l.texture.GetTexture().Get(), &dsvDesc, &l.dsv));
	}
}

SandFox::Light& SandFox::LightHandler::GetLight(LightID id)
{
	for (int i = 0; i < m_lights.Size(); i++)	// TODO: add log(n) search
	{
		if (m_lights[i].id == id)
		{
			return m_lights[i].light;
		}
	}

	FOX_CRITICAL("Cannot get light, ID doesn't exist.");
	EXC("Cannot get light, ID doesn't exist.");
}

void SandFox::LightHandler::RemoveLight(LightID id)
{
	for (int i = 0; i < m_lights.Size(); i++)	// TODO: add log(n) search
	{
		if (m_lights[i].id == id)
		{
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



void SandFox::LightHandler::Update(DrawQueue* drawQueue)
{
	for (int i = 0; i < m_lights.Size(); i++)
	{
		if (m_lights[i].redraw)
		{
			DrawMapAtIndex(i, drawQueue);
		}
	}
}

void SandFox::LightHandler::DrawMap(LightID index, DrawQueue* drawQueue)
{
	int i = m_lights.SearchLinear(index, Predicate);

	if (i == -1)
	{
		FOX_WARN_F("Cannot draw shadow map at ID [%i], as it does not exist.", index);
		return;
	}

	DrawMapAtIndex(i, drawQueue);
}

void SandFox::LightHandler::DrawAllMaps(DrawQueue* drawQueue)
{
	for (int i = 0; i < m_lights.Size(); i++)
	{
		DrawMapAtIndex(i, drawQueue);
	}
}

void SandFox::LightHandler::DrawMapAtIndex(int index, DrawQueue* drawQueue)
{
	LightStruct& l = m_lights[index];

	dx::XMMATRIX projection;

	switch (l.light.type)
	{
	case LightTypeDirectional:
		projection = l.light.GetViewDirectional()
		break;

	case LightTypeSpot:
		break;

	default:
		FOX_FTRACE_F("Unsupported light type [%i] for shadow mapping.", l.light.type);
		return;
	}
}



void SandFox::LightHandler::Bind(GraphicsTechnique technique)
{
	if (technique == GraphicsTechniqueDeferred)
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
	m_lightInfoBuffer.Write(&m_lightInfo);
}

SandFox::LightID SandFox::LightHandler::Predicate(const LightStruct& l)
{
	return l.id;
}
