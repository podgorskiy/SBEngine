#include "Block.h"
#include <functional>
#include <initializer_list>
#include <type_traits>


namespace UI
{
	void Block::UpdateProp(Constraint::Type type, Constraint::Unit new_unit, float new_value, float time)
	{
		for (int i = 0, l = m_constraints.size(); i < l; ++i)
		{
			if (m_constraints[i].type == type)
			{
				if (Constraint::to_mask(type) & m_transition_mask)
				{
					for (int j = 0, jl = m_transition_constraints.size(); j < jl; ++j)
					{
						if (m_transition_constraints[j].type == type)
						{
							m_transition_constraints[j].unit = new_unit;
							m_transition_constraints[j].value = new_value;
							m_controllers[j].SetStartTime(time);
							return;
						}
					}
					m_constraints[i].unit = new_unit;
					m_constraints[i].value = new_value;
				}
				else
				{
					m_constraints[i].unit = new_unit;
					m_constraints[i].value = new_value;
					return;
				}
			}
		}
	}

	void Block::SetTransitionProperty(Constraint::Type type, float duration)
	{
		for (int i = 0, l = m_constraints.size(); i < l; ++i)
		{
			if (m_constraints[i].type == type)
			{
				m_transition_mask |= Constraint::to_mask(type);
				m_controllers.push_back(MController<float>(duration));
				m_transition_constraints.push_back(m_constraints[i]);
				return;
			}
		}
		spdlog::error("SetTransitionProperty {}, but it does not exist", int(type));
	}

    BlockPtr make_block(std::initializer_list<Constraint> constraints)
    {
    	return std::make_shared<Block>(std::initializer_list<Constraint>(constraints));
    }

    BlockPtr make_block(std::initializer_list<Constraint> constraints, Render::color c)
    {
    	auto block = std::make_shared<Block>(std::initializer_list<Constraint>(constraints));
    	block->EmplaceEmitter<SFillEmitter>(c);
    	return block;
    }

    BlockPtr make_block(std::initializer_list<Constraint> constraints,
    		Render::TexturePtr tex,
    		ImSize::Enum size,
    		ImPos::Enum pos,
    		ImTransform::Enum t)
    {
    	auto block = std::make_shared<Block>(std::initializer_list<Constraint>(constraints));
    	block->EmplaceEmitter<SImageEmitter>(std::move(tex), size, pos, t);
    	return block;
    }
}
