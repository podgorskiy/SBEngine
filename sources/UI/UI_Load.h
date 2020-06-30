#pragma once
#include "Block.h"
#include "Parsers.h"
#include "Render/TextureReaders/TextureLoader.h"
#include <yaml-cpp/yaml.h>
#include <iostream>


namespace UI
{
	inline void ReadConstraint(YAML::Node node, const char* name, Constraint::Type type, const BlockPtr& blk)
	{
		float value = 0;
		Constraint::Unit unit = Constraint::Pixel;
		auto cnstr = node[name];
		if (cnstr.IsDefined())
		{
			std::string x = cnstr.as<std::string>();
			// spdlog::info("{}: {}", name, x);
			ParseUnitValue(x.c_str(), unit, value);
			blk->PushConstraint(Constraint(type, unit, value));
		}
	}

	bool BuildList(const BlockPtr& parent, const YAML::Node& sequence);

	inline void LoadEmmitters(YAML::Node node, const BlockPtr& block)
	{
		auto bg_color = node["bg_color"];
		if (bg_color.IsDefined())
		{
			color c;
			ParseColor(bg_color.as<std::string>().c_str(), c);
			spdlog::info("Color: {}", c);
    	    block->EmplaceEmitter<SFillEmitter>(c);
		}
	}

	inline BlockPtr BuildBlock(YAML::Node node)
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

		LoadEmmitters(node, block);

		auto blocks = node["blocks"];
		if (blocks.IsDefined())
		{
			BuildList(block, blocks);
		}

		return block;
	}

	inline bool BuildList(const BlockPtr& parent, const YAML::Node& sequence)
	{
		ASSERT(sequence.IsSequence(), "'blocks' must be sequence")
		for (YAML::Node node: sequence)
		{
			parent->AddChild(BuildBlock(node));
		}
		return true;
	}

	inline BlockPtr Load(const fsal::File& f)
	{
		using namespace UI::lit;
		using UI::operator""_c;
		spdlog::info("Loading: {}", f.GetPath().string().c_str());

		YAML::Node root_node = YAML::Load(std::string(f));

		auto root = UI::make_block({0_l, 100_wpe, 0_t, 100_hpe});

		LoadEmmitters(root_node, root);

		auto blocks = root_node["blocks"];

		ASSERT(blocks.IsSequence(), "'blocks' must be sequence, in file %s", f.GetPath().string().c_str())

		BuildList(root, blocks);

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
