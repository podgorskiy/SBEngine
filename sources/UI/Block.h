#pragma once
#include "utils/stack_vector.h"
#include "Render/color.h"
#include "utils/aabb.h"
#include "UI/View.h"
#include "UI/Enums.h"
#include "UI/Emitters.h"

#include <glm/matrix.hpp>

#include <vector>
#include <functional>
#include <memory>
#include <initializer_list>
#include <type_traits>
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

    inline BlockPtr make_block(std::initializer_list<Constraint> constraints)
    {
    	return std::make_shared<Block>(std::initializer_list<Constraint>(constraints));
    }

    inline BlockPtr make_block(std::initializer_list<Constraint> constraints, color c)
    {
    	auto block = std::make_shared<Block>(std::initializer_list<Constraint>(constraints));
    	block->EmplaceEmitter<SFillEmitter>(c);
    	return block;
    }

    inline BlockPtr make_block(std::initializer_list<Constraint> constraints,
    		Render::TexturePtr tex,
    		ImSize::Enum size = ImSize::Auto,
    		ImPos::Enum pos = ImPos::LeftTop,
    		ImTransform::Enum t = ImTransform::None)
    {
    	auto block = std::make_shared<Block>(std::initializer_list<Constraint>(constraints));
    	block->EmplaceEmitter<SImageEmitter>(std::move(tex), size, pos, t);
    	return block;
    }

	inline void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda)
	{
		lambda(block.get(), parent.get());
		for (auto& child: block->m_childs)
		{
			Traverse(child, block, lambda);
		}
	}

	inline void Render(UI::Renderer* renderer, const BlockPtr& root, View view, float time = 0.0f, int flags = 0)
	{
    	renderer->SetUp(view);
		Traverse(root, nullptr, [renderer, time, flags](UI::Block* block, UI::Block* parent)
		{
			block->Emit(renderer, time, flags);
		});
		renderer->Draw();
	}

	inline glm::aabb2 SolveConstraints(const glm::aabb2& parent_box, const Constraint* cnst, int count, float ppd)
	{
		uint8_t mask[2] = {0};
		float cst_values[2][5];
		uint8_t id_map[16] = {0xff, 0x00, 0x01, 0xff, 0x02, 0xff, 0xff, 0xff, 0xff, 0x03, 0x04, 0xff, 0xff, 0xff, 0xff, 0xff};
		auto p_size = parent_box.size();
		for (int i = 0; i < count; ++i)
		{
			int id = uint8_t(cnst[i].type & Constraint::CnstV) >> Constraint::CnstVp;
			mask[id] |= uint8_t(cnst[i].type & uint8_t(Constraint::CnstV - 1));
			int sid = id_map[cnst[i].type & uint8_t(Constraint::CnstV - 1)];
			assert(sid != 0xff);
			float p = p_size[id];
			switch(cnst[i].unit)
			{
				case Constraint::Percentage: p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueHeight: p = p_size.y; p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueWidth: p = p_size.x; p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueMin: p = glm::min(p_size.x, p_size.y); p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueMax: p = glm::max(p_size.x, p_size.y); p *= cnst[i].value / 100.0f; break;
				case Constraint::Pixel: p /= cnst[i].value / ppd; break;
				case Constraint::Point: p = cnst[i].value; break;
			}
			cst_values[id][sid] = p;
		}

		glm::aabb2 box = parent_box;
		for (int id = 0; id < 2; ++id)
		{
			float pl = parent_box.minp[id];
			float pr = parent_box.maxp[id];
			float l = pl;
			float r = pr;
			float w = 0.0f;
			switch (mask[id])
			{
				case Constraint::Left | Constraint::Right:
					l = pl + cst_values[id][id_map[Constraint::Left]];
					r = pr - cst_values[id][id_map[Constraint::Right]];
					break;
				case Constraint::Left | Constraint::Width:
					l = pl + cst_values[id][id_map[Constraint::Left]];
					r = l + cst_values[id][id_map[Constraint::Width]];
					break;
				case Constraint::Width | Constraint::Right:
					r = pr - cst_values[id][id_map[Constraint::Right]];
					l = r - cst_values[id][id_map[Constraint::Width]];
					break;
				case Constraint::CenterLeft | Constraint::Width:
					l = r = pl + cst_values[id][id_map[Constraint::CenterLeft]];
					w = cst_values[id][id_map[Constraint::Width]];
					r += w / 2.0f;
					l -= w / 2.0f;
					break;
				case Constraint::Width | Constraint::CenterRight:
					l = r = pr - cst_values[id][id_map[Constraint::CenterRight]];
					w = cst_values[id][id_map[Constraint::Width]];
					r += w / 2.0f;
					l -= w / 2.0f;
					break;
				case Constraint::Left | Constraint::Right | Constraint::Width:
					l = pl + cst_values[id][id_map[Constraint::Left]];
					r = pr - cst_values[id][id_map[Constraint::Right]];
					w = (cst_values[id][id_map[Constraint::Width]] + (r - l)) / 2.0f;
					l = r = (l + r) / 2.0;
					r += w / 2.0f;
					l -= w / 2.0f;
					break;
				default:
					spdlog::error("Unsupported combination of constraints: {}", mask[id]);
			}
			box.minp[id] = l;
			box.maxp[id] = r;
		}
		return box;
	}

	inline void DoLayout(const BlockPtr& block, const View& view)
	{
		Traverse(block, nullptr, [view](Block* block, Block* parent)
		{
			glm::aabb2 parent_box = parent == nullptr ? view.view_box : parent->GetBox();
			const auto& cnst = block->GetConstraints();
			glm::aabb2 current_box = SolveConstraints(parent_box, cnst.data(), cnst.size(), view.GetPixelPerDotScalingFactor());
			block->SetBox(current_box);
		});
	}
}
