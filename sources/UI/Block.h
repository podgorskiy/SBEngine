#pragma once
#include "utils/stack_vector.h"
#include "UI/color.h"
#include "utils/aabb.h"
#include "UI/View.h"
#include "UI/Enums.h"
#include "UI/Emitters.h"

#include <glm/matrix.hpp>

#include <memory>
#include <initializer_list>
#include <inttypes.h>


namespace UI
{
	class Block;
	typedef std::shared_ptr<Block> BlockPtr;

	class Block
	{
		friend void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda);
	public:
		Block() = default;
		~Block()
		{
			if (has_emitter) GetEmitter()->~IEmitter();
		}

		explicit Block(std::initializer_list<Constraint> cnst): m_constraints(cnst) {}

		void AddChild(const BlockPtr& child) { m_childs.push_back(child); }
		glm::vec2 GetPositionUL() const { return m_box.minp; }
		glm::vec2 GetPositionC() const { return m_box.center(); }
		glm::vec2 GetSize() const { return m_box.size(); }
		glm::mat3 GetTransform() const
		{
			glm::vec2 d = GetSize();
			glm::vec2 p = GetPositionC();
			return {
					glm::vec3(m_rotation * d.x, 0.0f),
					glm::vec3(glm::vec2(-m_rotation.y, m_rotation.x) * d.y, 0.0f),
					glm::vec3(p, 1.0)
			};
		}
		glm::aabb2 GetBox() const { return m_box; }
		void SetBox(const glm::aabb2& box) { m_box = box; }
		void PushConstraint(const Constraint& cnst) { m_constraints.push_back(cnst); };
		const stack::vector<Constraint, 4>& GetConstraints() const { return m_constraints; };
		void Emit(UI::Renderer* r, float time = 0.0f, int flags = 0) {	if (has_emitter) (*GetEmitter())(r, this, time, flags); }

//		virtual inline void Event(EventTouch* event)
//		{
//			for(int i=0,l=m_childs.size();i<l;i++)
//			{
//				if( m_childs[i] != NULL )
//					m_childs[i]->Event( event );
//				if(event->IsConsumed())
//					break;
//				l=m_childs.size();
//			}
//		}
		template <typename R, typename... Ts>
		void EmplaceEmitter(Ts&&... args) {
		    new (userdata) R(std::forward<Ts>(args)...);
		    has_emitter = true;
		}
	private:
		IEmitter* GetEmitter() { return (IEmitter*)userdata; }

		glm::aabb2 m_box;
		stack::vector<Constraint, 4> m_constraints;
		stack::vector<BlockPtr, 4> m_childs;
		glm::vec2 m_rotation = glm::vec2(1.0f, 0.0f);
		uint8_t userdata[EmitterSizeCheck::DataSize] = {0};
		bool has_emitter = false;
	};

    BlockPtr make_block(std::initializer_list<Constraint> constraints);

    BlockPtr make_block(std::initializer_list<Constraint> constraints, color c);

    BlockPtr make_block(std::initializer_list<Constraint> constraints,
    		Render::TexturePtr tex,
    		ImSize::Enum size = ImSize::Auto,
    		ImPos::Enum pos = ImPos::LeftTop,
    		ImTransform::Enum t = ImTransform::None);

	void Render(UI::Renderer* renderer, const BlockPtr& root, View view, float time = 0.0f, int flags = 0);

	void DoLayout(const BlockPtr& block, const View& view);
}
