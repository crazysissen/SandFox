#include "pch.h"
#include "LightHandler.h"

SandFox::LightHandler::LightHandler()
	:
	m_lightInfo(),
	m_lightInfoBuffer()
{
}

SandFox::LightHandler::~LightHandler()
{
}

void SandFox::LightHandler::Init(GraphicsTechnique technique, cs::Color ambient, float ambientIntensity)
{
	m_lightInfo.ambient = (Vec3)ambient * ambientIntensity;
	m_lightInfo.lightCount = 0;

	m_lightInfoBuffer.Load(RegCBVLightInfo, &m_lightInfo, sizeof(LightInfo), false);

	UpdateBuffer();

	Bind(technique);
}

void SandFox::LightHandler::DeInit()
{
}

void SandFox::LightHandler::SetLights(const Light* lights, int count)
{
	if (count > FOX_C_MAX_LIGHTS)
	{
		FOX_WARN_F("Cannot set more than [%i] lights, [%i] set. Excess disregarded.");
	}

	std::memcpy(m_lightInfo.lights, lights, std::min(count, FOX_C_MAX_LIGHTS) * sizeof(Light));
	m_lightInfo.lightCount = count;

	UpdateBuffer();
}

void SandFox::LightHandler::SetLightAmbient(const cs::Color& color, float intensity)
{
	m_lightInfo.ambient = (Vec3)color * intensity;
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

void SandFox::LightHandler::UpdateBuffer()
{
	m_lightInfoBuffer.Write(&m_lightInfo);
}
