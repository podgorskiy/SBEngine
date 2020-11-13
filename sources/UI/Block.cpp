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

	void Block::UpdateConstraint(Constraint c)
	{
		for (int i = 0, l = m_constraints.size(); i < l; ++i)
		{
			if (m_constraints[i].type == c.type)
			{
				m_constraints[i].unit = c.unit;
				m_constraints[i].value = c.value;
				return;
			}
		}
		PushConstraint(c);
	}

	void Block::UpdateTransitionConstraint(Constraint c, float duration)
	{
		for (int i = 0, l = m_transition_constraints.size(); i < l; ++i)
		{
			if (m_transition_constraints[i].type == c.type)
			{
				m_transition_constraints[i].unit = c.unit;
				m_transition_constraints[i].value = c.value;
				m_controllers[i].SetDuration(duration);
				return;
			}
		}
		m_controllers.push_back(MController<float>(duration));
		m_transition_constraints.push_back(c);
	}

	void Block::SetTransitionProperty(Constraint::Type type, float duration)
	{
		for (int i = 0, l = m_constraints.size(); i < l; ++i)
		{
			if (m_constraints[i].type == type)
			{
				m_transition_mask |= Constraint::to_mask(type);
				UpdateTransitionConstraint(m_constraints[i], duration);
				return;
			}
		}
		spdlog::error("SetTransitionProperty {}, but it does not exist", int(type));
	}

	void Block::ClearTransitionProperty(Constraint::Type type)
	{
		for (int j = 0; j < m_on_complete.size(); ++j)
		{
			if (type == m_on_complete[j].first)
			{
				m_on_complete.erase(m_on_complete.begin() + j);
				--j;
			}
		}
		for (int i = 0, l = m_transition_constraints.size(); i < l; ++i)
		{
			if (m_transition_constraints[i].type == type)
			{
				m_transition_mask &= ~uint8_t(Constraint::to_mask(type));
				m_transition_constraints.erase(m_transition_constraints.begin() + i);
				m_controllers.erase(m_controllers.begin() + i);
				return;
			}
		}
		spdlog::error("ClearTransitionProperty {}, but it does not exist", int(type));
	}

	CnstBlockPtr Block::GetBlock(const std::string& name, bool recursive) const
	{
		for (int i = 0, l = m_childs.size(); i < l; i++)
		{
			if (m_childs[i]->m_name == name)
			{
				return m_childs[i];
			}
		}
		if (recursive)
			for (int i = 0, l = m_childs.size(); i < l; i++)
			{
				auto p = m_childs[i]->GetBlock(name, true);
				if (p)
					return p;
			}
		return nullptr;
	}

	BlockPtr Block::GetBlock(const std::string& name, bool recursive)
	{
		CnstBlockPtr tmp = const_cast<const Block*>(this)->GetBlock(name, recursive);
		return std::const_pointer_cast<Block>(tmp);
	}

	BlockPtr Block::RemoveBlock(const std::string& name, bool recursive)
	{
		for (auto it = m_childs.begin(); it != m_childs.end(); ++it)
		{
			if ((*it)->m_name == name)
			{
				auto p = *it;
				m_childs.erase(it);
				return p;
			}
		}
		if (recursive)
			for (int i = 0, l = m_childs.size(); i < l; i++)
			{
				auto p = m_childs[i]->RemoveBlock(name, true);
				if (p)
					return p;
			}
		return nullptr;
	}

	BlockPtr Block::RemoveBlock(BlockPtr block, bool recursive)
	{
		for (auto it = m_childs.begin(); it != m_childs.end(); ++it)
		{
			if (block == *it)
			{
				auto p = *it;
				m_childs.erase(it);
				return p;
			}
		}
		if (recursive)
			for (int i = 0, l = m_childs.size(); i < l; i++)
			{
				auto p = m_childs[i]->RemoveBlock(block, true);
				if (p)
					return p;
			}
		return nullptr;
	}

    BlockPtr make_block(const char* name, std::initializer_list<Constraint> constraints)
    {
    	return std::make_shared<Block>(name, std::initializer_list<Constraint>(constraints));
    }

    BlockPtr make_block(const std::string& name, std::initializer_list<Constraint> constraints)
    {
    	return std::make_shared<Block>(name, std::initializer_list<Constraint>(constraints));
    }

    BlockPtr make_block(const char* name, std::initializer_list<Constraint> constraints, Render::color c)
    {
    	auto block = std::make_shared<Block>(name, std::initializer_list<Constraint>(constraints));
    	block->EmplaceEmitter<SFillEmitter>(c);
    	return block;
    }

    BlockPtr make_block(const char* name, std::initializer_list<Constraint> constraints,
    		Render::TexturePtr tex,
    		ImSize::Enum size,
    		ImPos::Enum pos,
    		ImTransform::Enum t)
    {
    	auto block = std::make_shared<Block>(name, std::initializer_list<Constraint>(constraints));
    	block->EmplaceEmitter<SImageEmitter>(std::move(tex), size, pos, t);
    	return block;
    }
}
