#pragma once
#include "Block.h"
#include "Serialization/parser.h"
#include "Render/TextureReaders/TextureLoader.h"
#include <yaml-cpp/yaml.h>
#include <iostream>


namespace UI
{
	bool AcceptUnit(serialization::Parser& parser, Constraint::Unit& unit)
	{
		if (parser.AcceptStr("px")) unit = Constraint::Pixel;
		else if (parser.AcceptStr("%")) unit = Constraint::Percentage;
		else if (parser.AcceptStr("pt")) unit = Constraint::Point;
		else if (parser.AcceptStr("cm")) unit = Constraint::Centimeters;
		else if (parser.AcceptStr("mm")) unit = Constraint::Millimeters;
		else if (parser.AcceptStr("in")) unit = Constraint::Inches;
		else if (parser.AcceptStr("vh")) unit = Constraint::ValueHeight;
		else if (parser.AcceptStr("vw")) unit = Constraint::ValueWidth;
		else if (parser.AcceptStr("vmin")) unit = Constraint::ValueMin;
		else if (parser.AcceptStr("vmax")) unit = Constraint::ValueMax;
		else return false;
		return true;
	}

	bool ParseUnitValue(const char* x, Constraint::Unit& unit, float& value)
	{
		serialization::Parser parser(x);
		if (parser.AcceptFloat(value))
		{
			parser.AcceptWhiteSpace();
			if (AcceptUnit(parser, unit))
			{
				parser.AcceptWhiteSpace();
				return parser.EOS();
			}
		}
		return false;
	}

	void ReadConstraint(YAML::Node node, const char* name, Constraint::Type type, const BlockPtr& blk)
	{
		float value = 0;
		Constraint::Unit unit = Constraint::Pixel;
		auto cnstr = node[name];
		if (cnstr.IsDefined())
		{
			std::string x = cnstr.as<std::string>();
			spdlog::info("{}: {}", name, x);
			ParseUnitValue(x.c_str(), unit, value);
			blk->PushConstraint(Constraint(type, unit, value));
		}
	}

	BlockPtr Load(const fsal::File& f)
	{
		using namespace UI::lit;
		using UI::operator""_c;

		YAML::Node ui = YAML::Load(std::string(f));

		auto root = UI::make_block({0_l, 100_wpe, 0_t, 100_hpe});

		auto doc = ui["Doc"];

		ASSERT(doc.IsSequence(), "Doc must be sequence, in file %s", f.GetPath().string().c_str())

		for (YAML::Node node: doc)
		{
			auto block = UI::make_block({});

            spdlog::info("Node name: {}", node["name"].as<std::string>());
			ReadConstraint(node, "width", Constraint::Width, block);
			ReadConstraint(node, "height", Constraint::Height, block);

			ReadConstraint(node, "top", Constraint::Top, block);
			ReadConstraint(node, "bottom", Constraint::Bottom, block);
			ReadConstraint(node, "left", Constraint::Left, block);
			ReadConstraint(node, "right", Constraint::Right, block);

			ReadConstraint(node, "ctop", Constraint::CenterTop, block);
			ReadConstraint(node, "cbottom", Constraint::CenterBottom, block);
			ReadConstraint(node, "cleft", Constraint::CenterLeft, block);
			ReadConstraint(node, "cright", Constraint::CenterRight, block);
		}
		return root;

//		auto stage = UI::make_block({130_wvh, 130_hvh, 50_clpe, 60_ctpe});
//		root->AddChild(stage);
//		auto room = UI::make_block({100_wpe, 100_hpe, 5_tvh, 0_l}, 0xFF0000FF_c);
//		stage->AddChild(room);

//		auto tile1a = UI::make_block({20_wpe, 40_hpe, 30_tpe, 20_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile2a = UI::make_block({20_wpe, 40_hpe, 25_tpe, 30_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile3a = UI::make_block({20_wpe, 40_hpe, 20_tpe, 40_lpe}, m_texture, S::Contain, P::leftCenter);
//
//		auto tile1b = UI::make_block({20_wpe, 40_hpe, 35_tpe, 30_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile2b = UI::make_block({20_wpe, 40_hpe, 30_tpe, 40_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile3b = UI::make_block({20_wpe, 40_hpe, 25_tpe, 50_lpe}, m_texture, S::Contain, P::leftCenter);
//
//		auto tile1c = UI::make_block({20_wpe, 40_hpe, 40_tpe, 40_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile2c = UI::make_block({20_wpe, 40_hpe, 35_tpe, 50_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile3c = UI::make_block({20_wpe, 40_hpe, 30_tpe, 60_lpe}, m_texture, S::Contain, P::leftCenter);
//
//		auto wall1a = UI::make_block({20_wpe, 40_hpe, 18_tpe, 10_lpe}, m_texturew, S::Contain, P::leftCenter, T::FlipX);
//		auto wall2a = UI::make_block({20_wpe, 40_hpe, 13_tpe, 20_lpe}, m_texturew, S::Contain, P::leftCenter, T::FlipX);
//		auto wall3a = UI::make_block({20_wpe, 40_hpe,  8_tpe, 30_lpe}, m_texturew, S::Contain, P::leftCenter, T::FlipX);
//
//		auto wall1b = UI::make_block({20_wpe, 40_hpe,  8_tpe, 50_lpe}, m_texturew, S::Contain, P::leftCenter);
//		auto wall2b = UI::make_block({20_wpe, 40_hpe, 13_tpe, 60_lpe}, m_texturew, S::Contain, P::leftCenter);
//		auto wall3b = UI::make_block({20_wpe, 40_hpe, 18_tpe, 70_lpe}, m_texturew, S::Contain, P::leftCenter);

//		room->AddChild(wall3a);
//		room->AddChild(wall2a);
//		room->AddChild(wall1a);
//
//		room->AddChild(wall1b);
//		room->AddChild(wall2b);
//		room->AddChild(wall3b);
//
//		room->AddChild(tile3a);
//		room->AddChild(tile2a);
//		room->AddChild(tile1a);
//
//		room->AddChild(tile3b);
//		room->AddChild(tile2b);
//		room->AddChild(tile1b);
//
//		room->AddChild(tile3c);
//		room->AddChild(tile2c);
//		room->AddChild(tile1c);


		// m_texture = Render::LoadTexture("graphics/floor/2.png");
	}

}
