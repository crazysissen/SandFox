#include "pch.h"
#include "MirrorDrawable.h"

#include "BindHandler.h"

SandFox::MirrorDrawable::MirrorDrawable()
{
}

SandFox::MirrorDrawable::~MirrorDrawable()
{
}

void SandFox::MirrorDrawable::Load(IDrawable* target, Cubemap* cubemap, bool tesselation)
{
	m_target = target;
	m_cubemap = cubemap;
	m_tesselation = tesselation;

	m_shader = Shader::Get(ShaderTypeMirror);
}

void SandFox::MirrorDrawable::Draw()
{
	m_cubemap->Bind(BindStagePS);

	Shader::ShaderTesselationCurrent(m_tesselation);
	m_shader->Bind();
	Shader::ShaderOverride(true);
	m_target->Draw();
	Shader::ShaderOverride(false);

	BindHandler::UnbindShader();
}
