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
		auto bg_img = node["bg_img"];
		auto text_node = node["text"];
		if (bg_color.IsDefined())
		{
			color c;
			ParseColor(bg_color.as<std::string>().c_str(), c);
			spdlog::info("Color: {}", c);
    	    block->EmplaceEmitter<SFillEmitter>(c);
		}
		else if (bg_img.IsDefined())
		{
			auto path = bg_img.as<std::string>();
			auto texture = Render::LoadTexture(path);
			spdlog::info("Texture: {}", path);

			ImSize::Enum size = ImSize::Auto;
			auto bg_size = node["bg_size"];
			if (bg_size.IsDefined())
			{
				auto s = bg_size.as<std::string>();
				if (s == "auto")
				{
					size = ImSize::Auto;
				}
				else if (s == "contain")
				{
					size = ImSize::Contain;
				}
				else if (s == "cover")
				{
					size = ImSize::Cover;
				}
				else if (s == "fill")
				{
					size = ImSize::Fill;
				}
				else
				{
					ASSERT(false, "Wrong bg_size %s", s.c_str());
				}
			}
			ImPos::Enum pos = ImPos::LeftTop;
			auto bg_pos = node["bg_pos"];
			if (bg_pos.IsDefined())
			{
				auto s = bg_pos.as<std::string>();
				if (s == "left_top")
				{
					pos = ImPos::LeftTop;
				}
				else if (s == "left_center")
				{
					pos = ImPos::leftCenter;
				}
				else if (s == "left_bottom")
				{
					pos = ImPos::leftBottom;
				}
				else if (s == "right_top")
				{
					pos = ImPos::RightTop;
				}
				else if (s == "right_center")
				{
					pos = ImPos::RightCenter;
				}
				else if (s == "right_bottom")
				{
					pos = ImPos::RightBottom;
				}
				else if (s == "center_top")
				{
					pos = ImPos::CenterTop;
				}
				else if (s == "center_center")
				{
					pos = ImPos::CenterCenter;
				}
				else if (s == "center_bottom")
				{
					pos = ImPos::CenterBottom;
				}
				else
				{
					ASSERT(false, "Wrong bg_pos %s", s.c_str());
				}
			}
			ImTransform::Enum t = ImTransform::None;
			auto bg_transform = node["bg_transform"];
			if (bg_transform.IsDefined())
			{
				auto s = bg_transform.as<std::string>();
				if (s == "none")
				{
					t = ImTransform::None;
				}
				else if (s == "flip_x")
				{
					t = ImTransform::FlipX;
				}
				else if (s == "flip_y")
				{
					t = ImTransform::FlipY;
				}
				else
				{
					ASSERT(false, "Wrong bg_transform %s", s.c_str());
				}
			}
    	    block->EmplaceEmitter<SImageEmitter>(std::move(texture), size, pos, t);
		}
		else if (text_node.IsDefined())
		{
			auto text = text_node.as<std::string>();
			spdlog::info("Text: {}", text);
    	    block->EmplaceEmitter<STextEmitter>(text);
		}
	}

	inline BlockPtr BuildBlock(YAML::Node node)
	{
		auto block = UI::make_block({});
		auto name = node["name"];
		if (name.IsDefined())
		{
			spdlog::info("Node name: {}", name.as<std::string>());
		}
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
	}
}
