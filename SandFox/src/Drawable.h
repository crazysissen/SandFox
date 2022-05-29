#pragma once

#include "SandFoxCore.h"
#include "BindHandler.h"
#include "IBindable.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Transform.h"
#include "Debug.h"

namespace SandFox
{

	typedef uint DrawableID;

	class IDrawable
	{
	public:
		virtual void Draw() = 0;
	};



	// In general, do not inherit from this. Instead use Drawable<>!
	class FOX_API DrawableBase // : public IDrawable
	{
	public:
		Transform transform;

		DrawableBase(Transform transform);
		DrawableBase(const DrawableBase&) = delete;
		DrawableBase(DrawableBase&&) = delete;
		virtual ~DrawableBase();

		static void ReleaseStatics();



	protected:
		void Execute();
		void ExecuteIndexed();

		static void AddToAllStatics(IBindable* bindable);

		struct BindConfiguration
		{
			IBindable* bindable;
			BindStage stage;
		};

		static cs::List<IBindable*> s_allStatics;
		static DrawableID s_idCounter;

		// Owned resources
		cs::List<IBindable*> m_bindables;
		BindPipeline m_pipeline;

		// Used in real time
		cs::List<BindConfiguration> m_configurations;
		BindPipeline* m_configuredPipeline;

		DrawableID m_id;
	};



	template<class T>
	class Drawable : public DrawableBase
	{
	public:
		using DrawableBase::transform;

		Drawable(Transform transform = Transform());
		virtual ~Drawable();



	protected:
		bool StaticInit();

		// For adding bindables handled by the system
		void SetPipeline(const BindPipeline& pipeline);
		void AddBind(IBindable* bindable, BindStage stage);
		void AddResource(BindableResource* resource, BindStage stage);
		void AppendStage(BindStage stage); // Call this immediately after adding a new Resource to add an additional target stage.

		// For adding bindables handled by the system
		static void StaticSetPipeline(const BindPipeline& pipeline);
		static void StaticAddBind(IBindable* bindable);
		static void StaticAddSampler(RegSampler sampler, BindStage stage);
		static void StaticAddResource(BindableResource* resource, BindStage stage);
		static void StaticAppendStage(BindStage stage); // Call this immediately after adding a new Resource to add an additional target stage.

		void Bind();



	private:
		static cs::List<BindConfiguration> s_configurations;
		static cs::List<IBindable*> s_bindables;
		static BindPipeline s_pipeline;
		static bool s_initialized;

		using DrawableBase::s_allStatics;
		using DrawableBase::m_bindables;
		using DrawableBase::m_configurations;
		using DrawableBase::m_pipeline;
	};





	// ---------------------------------------------- Implementation

	template<class T>
	inline Drawable<T>::Drawable(Transform transform)
		:
		DrawableBase(transform)
	{
	}

	template<class T>
	inline Drawable<T>::~Drawable()
	{
	}

	template<class T>
	inline bool Drawable<T>::StaticInit()
	{
		bool s = !s_initialized;
		s_initialized = false;
		return s;
	}

	template<class T>
	inline void Drawable<T>::SetPipeline(const BindPipeline& pipeline)
	{
		m_configuredPipeline = &m_pipeline;
		m_pipeline = pipeline;
		return;
	}

	template<class T>
	inline void Drawable<T>::AddBind(IBindable* bindable, BindStage stage)
	{
		m_bindables.Add(bindable);
		m_configurations.Add({ bindable, stage });
		return;
	}

	template<class T>
	inline void Drawable<T>::AddResource(BindableResource* resource, BindStage stage)
	{
		m_bindables.Add(resource);
		m_configurations.Add({ resource, stage });
		return;
	}

	template<class T>
	inline void Drawable<T>::AppendStage(BindStage stage)
	{
		m_configurations.Add({ m_bindables.Back(), stage });
		return;
	}

	template<class T>
	inline void Drawable<T>::StaticSetPipeline(const BindPipeline& pipeline)
	{
		s_pipeline = pipeline;
		AddToAllStatics(s_pipeline.vb);

		if (s_pipeline.ib)
		{
			AddToAllStatics(s_pipeline.ib);
		}
	}

	template<class T>
	inline void Drawable<T>::StaticAddBind(IBindable* bindable)
	{
		AddToAllStatics(bindable);
		s_bindables.Add(bindable);
		s_configurations.Add({ bindable, BindStageNone });
	}

	template<class T>
	inline void Drawable<T>::StaticAddSampler(RegSampler sampler, BindStage stage)
	{
		BindHandler::ApplyPresetSampler(sampler, stage);
	}

	template<class T>
	inline void Drawable<T>::StaticAddResource(BindableResource* resource, BindStage stage)
	{
		AddToAllStatics(resource);
		s_bindables.Add(resource);
		s_configurations.Add({ resource, stage });
	}

	template<class T>
	inline void Drawable<T>::StaticAppendStage(BindStage stage)
	{
		s_configurations.Add({ s_bindables.Back(), stage });
	}

	template<class T>
	inline void Drawable<T>::Bind()
	{
		if (m_configuredPipeline == nullptr)
		{
			if (s_pipeline.shader != nullptr)
			{
				m_configuredPipeline = &s_pipeline;
			}
			else
			{
				FOX_WARN("Cannot bind Drawable without configured pipeline.");
				return;
			}
		}

		if (BindHandler::BindPipeline(m_configuredPipeline))
		{
			m_configuredPipeline->shader->Bind();
			m_configuredPipeline->vb->Bind();

			if (m_configuredPipeline->ib)
			{
				m_configuredPipeline->ib->Bind();
			}
		}

		for (BindConfiguration& b : s_configurations)
		{
			b.bindable->Bind(b.stage);
		}

		for (BindConfiguration& b : m_configurations)
		{
			b.bindable->Bind(b.stage);
		}
	}



	template<class T>
	cs::List<DrawableBase::BindConfiguration> Drawable<T>::s_configurations;

	template<class T>
	cs::List<IBindable*> Drawable<T>::s_bindables;

	template<class T>
	BindPipeline Drawable<T>::s_pipeline;

	template<class T>
	bool Drawable<T>::s_initialized = false;

}