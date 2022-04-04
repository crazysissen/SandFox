#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"
#include "IndexBuffer.h"
#include "Transform.h"
#include "Shader.h"

#include <typeinfo>
#include <vector>

namespace SandFox
{

	class FOX_API _Drawable
	{
	public:
		Transform m_transform;

	public:
		_Drawable(Transform transform);
		_Drawable(const _Drawable&) = delete;
		virtual ~_Drawable();

		// Main draw function
		virtual void Draw();

		// Modify bindables
		void AddBind(IBindable* bindable);
		void AddIndexBuffer(Bind::IndexBuffer* indexBuffer);
		void SetShader(Shader* shader = nullptr);

		// Virtual functions for Drawable<> implementation statics
		virtual unsigned int BindStatic() const = 0;
		virtual SandFox::Shader* GetShader() const = 0;

		dx::XMMATRIX GetTransformationMatrix();

		bool operator>(const _Drawable& ref);

		static void ReleaseStatics();

	protected:
		static int s_typeIndexCounter;
		static std::vector<std::vector<IBindable*>*>* s_staticBindableVectors;

		std::vector<IBindable*>* m_bindables;
		SandFox::Bind::IndexBuffer* m_indexBuffer;
		SandFox::Shader* m_shader;

		int m_index;
	};

	template<class T>
	class Drawable : public _Drawable
	{
	public:
		using _Drawable::m_transform;

		Drawable(Transform transform = {});
		virtual ~Drawable();

		virtual unsigned int BindStatic() const override;
		virtual SandFox::Shader* GetShader() const override;

		void AddStaticBind(IBindable* bindable);
		void AddStaticIndexBuffer(Bind::IndexBuffer* indexBuffer);
		void SetStaticShader(Shader* shader);

		bool StaticInitialization();

	protected:
		static int s_typeIndex;
		static int s_currentTypeIndex;
		static std::vector<IBindable*>* s_staticBindables;
		static SandFox::Bind::IndexBuffer* s_staticIndexBuffer;
		static SandFox::Shader* s_staticShader;

		using _Drawable::m_index;
		using _Drawable::m_bindables;
		using _Drawable::m_indexBuffer;

	private:
		bool m_staticInit;
	};



	// --------------- Implementation

	template<class T>
	Drawable<T>::Drawable(Transform transform)
		:
		_Drawable(transform),
		m_staticInit(false)
	{
		if (s_staticBindables == nullptr)
		{
			s_staticBindables = new std::vector<IBindable*>();
			m_staticInit = true;
			s_typeIndex = s_typeIndexCounter++;

			if (s_staticBindableVectors == nullptr)
			{
				s_staticBindableVectors = new std::vector<std::vector<IBindable*>*>;
			}

			s_staticBindableVectors->push_back(s_staticBindables);
		}

		m_index = s_typeIndex;
	}

	template<class T>
	inline Drawable<T>::~Drawable()
	{
	}

	template<class T>
	unsigned int Drawable<T>::BindStatic() const
	{
		if (s_currentTypeIndex != s_typeIndex)
		{
			for (IBindable* b : *s_staticBindables)
			{
				b->Bind();
			}

			if (s_staticShader != nullptr)
			{
				s_staticShader->Bind();
			}

			if (s_staticIndexBuffer != nullptr)
			{
				s_staticIndexBuffer->Bind();
				return s_staticIndexBuffer->GetCount();
			}
		}

		if (s_staticIndexBuffer != nullptr)
		{
			return s_staticIndexBuffer->GetCount();
		}

		return -1;
	}

	template<class T>
	inline SandFox::Shader* Drawable<T>::GetShader() const
	{
		if (s_staticShader)
		{
			return s_staticShader;
		}

		return m_shader;
	}

	template<class T>
	void Drawable<T>::AddStaticBind(IBindable* bindable)
	{
		if (m_staticInit)
		{
			s_staticBindables->push_back(bindable);
		}
	}

	template<class T>
	void Drawable<T>::AddStaticIndexBuffer(Bind::IndexBuffer* indexBuffer)
	{
		if (m_staticInit)
		{
			s_staticIndexBuffer = indexBuffer;

			AddStaticBind(indexBuffer);
		}
	}

	template<class T>
	inline void Drawable<T>::SetStaticShader(Shader* shader)
	{
		s_staticShader = shader;
	}

	template<class T>
	inline bool Drawable<T>::StaticInitialization()
	{
		return m_staticInit;
	}



	// Initializations of static members in template classes will be run only once by the compiler.

	template<class T>
	int Drawable<T>::s_typeIndex = 0;

	template<class T>
	int Drawable<T>::s_currentTypeIndex = -1;

	template<class T>
	std::vector<IBindable*>* Drawable<T>::s_staticBindables = nullptr;

	template<class T>
	Bind::IndexBuffer* Drawable<T>::s_staticIndexBuffer = nullptr;

	template<class T>
	SandFox::Shader* Drawable<T>::s_staticShader = nullptr;

}