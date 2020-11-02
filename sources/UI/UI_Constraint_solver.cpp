#include "Block.h"


namespace UI
{
	const static constexpr uint8_t id_map[16] = {
			0xff, 0x00, 0x01, 0xff, // 0000 .. 0011, NA,    Left,   Right,  NA
			0x02, 0xff, 0xff, 0xff, // 0100 .. 0111, Width, NA,     NA,     NA
			0xff, 0x03, 0x04, 0xff, // 1000 .. 1011, NA,    CLeft,  CRight, NA
			0xff, 0xff, 0xff, 0xff  // 1100 .. 1111, NA,    NA,     NA,     NA
	};

	void ComputeValues(const glm::aabb2& root_box, const glm::aabb2& parent_box, const Constraint* cnst, int count, float ppd, float (&cst_values)[2][5], uint8_t (&mask)[2])
	{
		auto p_size = parent_box.size();
		auto r_size = root_box.size();
		for (int i = 0; i < count; ++i)
		{
			int id = uint8_t(cnst[i].type & Constraint::CnstV) >> Constraint::CnstVp;
			float p = p_size[id];
			switch(cnst[i].unit)
			{
				case Constraint::Percentage: p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueHeight: p = r_size.y; p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueWidth: p = r_size.x; p *= cnst[i].value / 100.0f; break;
				case Constraint::RValueHeight: p = p_size.y; p *= cnst[i].value / 100.0f; break;
				case Constraint::RValueWidth: p = p_size.x; p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueMin: p = glm::min(r_size.x, r_size.y); p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueMax: p = glm::max(r_size.x, r_size.y); p *= cnst[i].value / 100.0f; break;
				case Constraint::RValueMin: p = glm::min(p_size.x, p_size.y); p *= cnst[i].value / 100.0f; break;
				case Constraint::RValueMax: p = glm::max(p_size.x, p_size.y); p *= cnst[i].value / 100.0f; break;
				case Constraint::Pixel: p = cnst[i].value / ppd; break;
				case Constraint::Point: p = cnst[i].value; break;
				case Constraint::Centimeters: p = cnst[i].value * (72.0f / 2.54f); break;
				case Constraint::Millimeters: p = cnst[i].value * (72.0f / 25.4f); break;
				case Constraint::Inches: p = cnst[i].value * 72.0f; break;
				default:
					continue;
			}
			mask[id] |= uint8_t(cnst[i].type & uint8_t(Constraint::CnstV - 1));
			int sid = id_map[cnst[i].type & uint8_t(Constraint::CnstV - 1)];
			assert(sid != 0xff);
			cst_values[id][sid] = p;
		}
	}

	float ComputeValue(const glm::aabb2& this_box, const float value, const Constraint::Unit unit, float ppd)
	{
		auto p_size = this_box.size();
		float p;
		switch(unit)
		{
			case Constraint::Percentage: p = glm::mean(this_box.size()) * value / 100.0f; break;
			case Constraint::RValueHeight: p = p_size.y; p *= value / 100.0f; break;
			case Constraint::RValueWidth: p = p_size.x; p *= value / 100.0f; break;
			case Constraint::RValueMin: p = glm::min(p_size.x, p_size.y); p *= value / 100.0f; break;
			case Constraint::RValueMax: p = glm::max(p_size.x, p_size.y); p *= value / 100.0f; break;
			case Constraint::Pixel: p = value / ppd; break;
			case Constraint::Point: p = value; break;
			case Constraint::Centimeters: p = value * (72.0f / 2.54f); break;
			case Constraint::Millimeters: p = value * (72.0f / 25.4f); break;
			case Constraint::Inches: p = value * 72.0f; break;
			default:
				assert(false);
		}
		return p;
	}

	glm::aabb2 SolveConstraints(const glm::aabb2& root_box, const glm::aabb2& parent_box, const Constraint* cnst, int count, const float (&cst_values)[2][5], const uint8_t (&mask)[2])
	{
		glm::aabb2 box = parent_box;
		for (uint8_t id = 0; id < 2; ++id)
		{
			float pl = parent_box.minp[id];
			float pr = parent_box.maxp[id];
			float l = pl;
			float r = pr;
			float w = 0.0f;
			switch (mask[id])
			{
				case Constraint::None:
					l = pl;
					r = pr;
					break;
				case Constraint::Right:
					l = r = pr - cst_values[id][id_map[Constraint::Right]];
					break;
				case Constraint::CenterRight:
					l = r = pr - cst_values[id][id_map[Constraint::CenterRight]];
					break;
				case Constraint::Left:
					l = r = pl + cst_values[id][id_map[Constraint::Left]];
					break;
				case Constraint::CenterLeft:
					l = r = pl + cst_values[id][id_map[Constraint::CenterLeft]];
					break;
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
				case Constraint::Width:
					l = r = (pr + pl) / 2.0f;
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
		auto s_size = box.size();
		for (int i = 0; i < count; ++i)
		{
			int id = uint8_t(cnst[i].type & Constraint::CnstV) >> Constraint::CnstVp;
			float p;
			switch(cnst[i].unit)
			{
				case Constraint::SValueHeight: p = s_size.y; p *= cnst[i].value / 100.0f;
				switch (mask[id])
				{
					case Constraint::Right:
						box.minp[id] = box.maxp[id] - p;
						break;
					case Constraint::Left:
						box.maxp[id] = box.minp[id] + p;
						break;
					case Constraint::CenterLeft:
					case Constraint::CenterRight:
						box.minp[id] -= p / 2;
						box.maxp[id] += p / 2;
						break;
				}
				break;
				case Constraint::SValueWidth: p = s_size.x; p *= cnst[i].value / 100.0f;
				switch (mask[id])
				{
					case Constraint::Right:
						box.minp[id] = box.maxp[id] - p;
						break;
					case Constraint::Left:
						box.maxp[id] = box.minp[id] + p;
						break;
					case Constraint::CenterLeft:
					case Constraint::CenterRight:
						box.minp[id] -= p / 2;
						box.maxp[id] += p / 2;
				}
				break;
				default:
					continue;
			}
		}
		return box;
	}

	glm::vec4 ResolveRadius(const glm::aabb2& this_box, const glm::vec4& values, const Constraint::Unit* units,  float ppd)
	{
    	auto output = glm::vec4(0);
		for (int i = 0; i < 4; ++i)
		{
			output[i] = ComputeValue(this_box, values[i], units[i], ppd);
		}
		return output;
	}

	void InterpolateTransitionValues(const Constraint* tcnst, MController<float>* ctrl, int count, float (&alt_cst_values)[2][5],  float (&dst_cst_values)[2][5], float time)
	{
		for (int i = 0; i < count; ++i)
		{
			int id = uint8_t(tcnst[i].type & Constraint::CnstV) >> Constraint::CnstVp;
			int sid = id_map[tcnst[i].type & uint8_t(Constraint::CnstV - 1)];

			auto v_end = alt_cst_values[id][sid];

			ctrl[i].SetEnd(v_end);
			auto v = ctrl[i].GetValue(time);

			dst_cst_values[id][sid] = v;
		}
	}
}
