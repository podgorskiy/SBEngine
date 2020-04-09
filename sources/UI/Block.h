#pragma once
#include "stack_vector.h"

#include <inttypes.h>
#include <glm/matrix.hpp>

#include <vector>
#include <functional>
#include <memory>
#include <aabb.h>


namespace UI
{
	struct View
	{
		glm::ivec2 size_in_dots = glm::ivec2(640, 480);
		uint16_t dpi = 72;

		float GetAspect() const { return size_in_dots.x / float(size_in_dots.y); }

		float GetPixelPerDotScalingFactor() const { return dpi / 72.0f; }
	};

	struct Constraint
	{
		enum Type: uint8_t
		{
			CnstL           = 1u << 0u,
			CnstR           = 1u << 1u,
			CnstS           = 1u << 2u,
			CnstC           = 1u << 3u,
			CnstVp          = 4u,
			CnstV           = 1u << CnstVp,

			Invalid         = 0x0,
			Left            = CnstL,
			Right           = CnstR,
			Width           = CnstS,
			CenterLeft      = CnstC | CnstL,
			CenterRight     = CnstC | CnstR,
			Top             = CnstV | CnstL,
			Bottom          = CnstV | CnstR,
			Height          = CnstV | CnstS,
			CenterTop       = CnstV | CnstC | CnstL,
			CenterBottom    = CnstV | CnstC | CnstR,
		};

		enum Unit: uint8_t
		{
			Percentage,
			Pixel,
			Point
		};

		Constraint(Type type, Unit unit, float value): type(type), unit(unit), value(value) {};
		Constraint(): type(Invalid), unit(Pixel), value(0.0f) {};

		Type type;
		Unit unit;
		float value;
	};

	class Block;
	typedef std::shared_ptr<Block> BlockPtr;

	class Block
	{
		friend void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda);
	public:
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
	private:
		glm::aabb2 m_box;
		stack::vector<Constraint, 4> m_constraints;
		stack::vector<BlockPtr, 4> m_childs;
		glm::vec2 m_rotation = glm::vec2(1.0f, 0.0f);
		char m_name[64] = {0};
	};

	void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda)
	{
		lambda(block.get(), parent.get());
		for (auto& child: block->m_childs)
		{
			Traverse(child, block, lambda);
		}
	}

	glm::aabb2 SolveConstraints(const glm::aabb2& parent_box, const Constraint* cnst, int count, float ppd)
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
					r -= w / 2.0f;
					l += w / 2.0f;
					break;
				case Constraint::Width | Constraint::CenterRight:
					l = r = pr - cst_values[id][id_map[Constraint::CenterRight]];
					w = cst_values[id][id_map[Constraint::Width]];
					r -= w / 2.0f;
					l += w / 2.0f;
					break;
				case Constraint::Left | Constraint::Right | Constraint::Width:
					l = pl + cst_values[id][id_map[Constraint::Left]];
					r = pr - cst_values[id][id_map[Constraint::Right]];
					w = (cst_values[id][id_map[Constraint::Width]] + (r - l)) / 2.0f;
					l = r = (l + r) / 2.0;
					r -= w / 2.0f;
					l += w / 2.0f;
					break;
				default:
					spdlog::error("Unsupported combination of constraints: {}", mask[id]);
			}
			box.minp[id] = l;
			box.maxp[id] = r;
		}
		return box;
	}

	void DoLayout(const BlockPtr& block, const View& view)
	{
		Traverse(block, nullptr, [view](Block* block, Block* parent)
		{
			glm::aabb2 parent_box = parent == nullptr ? glm::aabb2(glm::vec2(0.0), view.size_in_dots) : parent->GetBox();
			const auto& cnst = block->GetConstraints();
			glm::aabb2 current_box = SolveConstraints(parent_box, cnst.data(), cnst.size(), view.GetPixelPerDotScalingFactor());
			block->SetBox(current_box);
		});
	}
}
