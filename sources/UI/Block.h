#pragma once
#include "utils/stack_vector.h"
#include "2DEngine/color.h"
#include "utils/glm_ext.h"
#include "2DEngine/View.h"
#include "UI/Enums.h"
#include "UI/Emitters.h"
#include "UI/Actors.h"
#include "Render/Texture.h"
#include "Controller/mcontroller.h"

#include <glm/matrix.hpp>

#include <memory>
#include <functional>
#include <initializer_list>
#include <inttypes.h>
#include <2DEngine/Renderer2D.h>


namespace UI
{
	class Block;
	typedef std::shared_ptr<Block> BlockPtr;
	typedef std::shared_ptr<const Block> CnstBlockPtr;

	typedef std::map<std::string, std::map<BlockPtr, std::vector<Constraint> > > Animation;

	class Block
	{
		friend void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda);
		friend void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda_pre, const std::function<void(Block* block, Block* parent)>& lambda_post);
		friend void Position(Block* block, Block* parent, const Render::View& view, float time);
	public:
		typedef stack::vector<MController<float>, 1> PropControllers;
		typedef stack::vector<Constraint, 1> TransitionConstraints;

		Block() = default;
		~Block()
		{}

		explicit Block(const std::string& name, std::initializer_list<Constraint> cnst): m_name(name), m_constraints(cnst) {}
		explicit Block(const char* name, std::initializer_list<Constraint> cnst): m_name(name), m_constraints(cnst) {}

		void Rename(const char* new_name) { m_name = new_name; }
		void Rename(const std::string& new_name) { m_name = new_name; }

		void AddChild(const BlockPtr& child) { m_childs.push_back(child); }
		void RemoveAllChildren() { m_childs.clear(); }
		BlockPtr GetBlock(const std::string& name, bool recursive=false);
		CnstBlockPtr GetBlock(const std::string& name, bool recursive=false) const;
		BlockPtr RemoveBlock(const std::string& name, bool recursive=false);
		BlockPtr RemoveBlock(BlockPtr block, bool recursive=false);
		// BlockPtr Copy() const;

		// aabb
		glm::aabb2 GetBox() const { return m_box; }
		void SetBox(const glm::aabb2& box) { m_box = box; }
		glm::vec2 GetPositionUL() const { return m_box.minp; }
		glm::vec2 GetPositionC() const { return m_box.center(); }
		glm::vec2 GetSize() const { return m_box.size(); }
		void SetDepth(int d) { m_depth = d; }
		int GetDepth() const { return m_depth; }

		// Radius
		glm::vec4 GetRadius() const { return m_radius; }
		glm::vec4 GetRadiusVal() const { return m_radius_val; }
		const Constraint::Unit* GetRadiusUnits() const { return &m_radius_unit[0]; }
		void SetRadius(const glm::vec4& r) { m_radius = r; }
		void SetRadiusVal(const glm::vec4& r) { m_radius_val = r; }
		void SetRadiusUnit(const Constraint::Unit* u) { memcpy(m_radius_unit, u, 4 * sizeof(Constraint::Unit)); }

		// Constraints and controllers
		void PushConstraint(const Constraint& cnst) { m_constraints.push_back(cnst); };
		void UpdateConstraint(Constraint c);
		const stack::vector<Constraint, 4>& GetConstraints() const { return m_constraints; };
		PropControllers& GetControllers() { return m_controllers; };
		const TransitionConstraints& GetTransitionConstraints() const { return m_transition_constraints; };
		TransitionConstraints& GetTransitionConstraintsTarget() { return m_transition_target_constraints; };
		void PushTargetTransitionConstraints(const Constraint& cnst) { m_transition_target_constraints.push_back(cnst); };
		void PushOnCompletionLambda(Constraint::Type type, std::function<void()> lambda) { m_on_complete.push_back(std::make_pair(type, lambda));}
		uint8_t GetTransitionMask() { return m_transition_mask; }
		void UpdateProp(Constraint::Type type, Constraint::Unit new_unit, float new_value, float time);
		void SetTransitionProperty(Constraint::Type type, float duration);
		void ClearTransitionProperty(Constraint::Type type);

		// Emitters
		void Emit(Render::Encoder* r, float time = 0.0f, int flags = 0, float ppd=1.0) { if (m_emitter) (*m_emitter)(r, this, time, flags, ppd); }

		template <typename R, typename... Ts>
		void EmplaceEmitter(Ts&&... args) {
		    m_emitter.reset(new R(std::forward<Ts>(args)...));
		}
		template <typename R>
		R* GetEmitter() { return static_cast<R*>(m_emitter.get()); }

		// Actors
		template <typename R, typename... Ts>
		void EmplaceActor(Ts&&... args) {
		    m_actor.reset(new R(std::forward<Ts>(args)...));
		}
		IActor* GetActor() const { return m_actor.get(); }

		// Clipping
		void EnableClipping(bool flag) { clip_overflow = flag; }
		bool IsClipping() const { return clip_overflow; }

	private:
		void UpdateTransitionConstraint(Constraint c, float duration);

		std::string m_name;
		glm::aabb2 m_box;
		glm::vec4 m_radius;
		glm::vec4 m_radius_val = glm::vec4(0);

		Constraint::Unit m_radius_unit[4] = { Constraint::Point };
		stack::vector<Constraint, 4> m_constraints;
		TransitionConstraints m_transition_constraints;
		TransitionConstraints m_transition_target_constraints;
		PropControllers m_controllers;
		uint8_t m_transition_mask;
		stack::vector<std::pair<Constraint::Type, std::function<void()> >, 1> m_on_complete;

		stack::vector<BlockPtr, 4> m_childs;

		std::unique_ptr<IEmitter> m_emitter = nullptr;
		std::unique_ptr<IActor> m_actor = nullptr;

		int m_depth = 0;
		bool clip_overflow = false;
	};

    BlockPtr make_block(const std::string& name, std::initializer_list<Constraint> constraints);

    BlockPtr make_block(const char* name, std::initializer_list<Constraint> constraints);

    BlockPtr make_block(const char* name, std::initializer_list<Constraint> constraints, Render::color c);

    BlockPtr make_block(const char* name, std::initializer_list<Constraint> constraints,
    		Render::TexturePtr tex,
    		ImSize::Enum size = ImSize::Auto,
    		ImPos::Enum pos = ImPos::LeftTop,
    		ImTransform::Enum t = ImTransform::None);
}
