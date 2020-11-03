#include "Block.h"
#include "UI_Constraint_solver.h"


namespace UI
{
	void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda)
	{
		lambda(block.get(), parent.get());
		for (auto& child: block->m_childs)
		{
			Traverse(child, block, lambda);
		}
	}

	void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda_pre, const std::function<void(Block* block, Block* parent)>& lambda_post)
	{
		lambda_pre(block.get(), parent.get());
		for (auto& child: block->m_childs)
		{
			Traverse(child, block, lambda_pre, lambda_post);
		}
		lambda_post(block.get(), parent.get());
	}

	void Render(Render::Renderer2D* renderer, const BlockPtr& root, Render::View view, float time, int flags)
	{
    	renderer->SetUp(view);
    	Render::Encoder* encoder = renderer->GetEncoder();
		Traverse(root, nullptr, [encoder, time, flags](UI::Block* block, UI::Block* parent)
		{
			if (block->IsClipping())
				encoder->PushScissors(block->GetBox());
			block->Emit(encoder, time, flags);
		}, [encoder](UI::Block* block, UI::Block* parent)
		{
			if (block->IsClipping())
				encoder->PopScissors();
		});
		renderer->Draw(time);
	}

	void Position(Block* block, Block* parent, const Render::View& view, float time)
	{
		glm::aabb2 parent_box = parent == nullptr ? view.view_box : parent->GetBox();
		const auto& cnst = block->GetConstraints();

		uint8_t mask[2] = {0};
		float cst_values[2][5];
		ComputeValues(view.view_box, parent_box, cnst.data(), cnst.size(), view.GetPixelPerDotScalingFactor(),
		              cst_values, mask);
		glm::aabb2 current_box;

		if (block->GetTransitionMask() != 0)
		{
			auto& ctrl = block->GetControllers();
			const auto& tcnst = block->GetTransitionConstraints();
			auto& ttcnst = block->GetTransitionConstraintsTarget();

			float alt_cst_values[2][5];
			ComputeValues(view.view_box, parent_box, tcnst.data(), tcnst.size(), view.GetPixelPerDotScalingFactor(),
			              alt_cst_values, mask);

			InterpolateTransitionValues(&tcnst[0], &ctrl[0], ctrl.size(), alt_cst_values, cst_values, time);

			if (!ttcnst.empty())
			{
				for (auto& c: ttcnst)
				{
					block->UpdateProp(c.type, c.unit, c.value, time);
				}
				ttcnst.clear();
			}
		}

		current_box = SolveConstraints(view.view_box, parent_box, cnst.data(), cnst.size(), cst_values, mask);

		block->SetBox(current_box);
		block->SetRadius(ResolveRadius(current_box, block->GetRadiusVal(), block->GetRadiusUnits(),
		                               view.GetPixelPerDotScalingFactor()));

	}

	void DoLayout(const BlockPtr& block, const Render::View& view, float time, glm::vec2 mouse_pos, bool mouse_down, bool mouse_left_click)
	{
		Traverse(block, nullptr, [view, time, mouse_pos, mouse_down, mouse_left_click](Block* block, Block* parent)
		{
			auto actor = block->GetActor();
			if (actor)
			{
				actor->BeforeCST(block, parent, time, mouse_pos, mouse_down, mouse_left_click);
				Position(block, parent, view, time);
				actor->AfterCST(block, parent, time, mouse_pos, mouse_down, mouse_left_click);
			}
			else
			{
				Position(block, parent, view, time);
			}
		});
	}

//	void Action(const BlockPtr& block, const Render::View& view, float time)
//	{
//		Traverse(block, nullptr, [view, mouse_pos, mouse_down, mouse_left_click](Block* block, Block* parent)
//		{
//			glm::aabb2 box = block->GetBox();
//			if (glm::is_inside(box, mouse_pos))
//			{
//				block->SetBox(glm::aabb2(box.minp - 10.0f , box.maxp + 10.0f));
//			}
//		});
//	}
}
