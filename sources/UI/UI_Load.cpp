#include "UI_Load.h"
#include "Parsers.h"
#include "utils/assertion.h"
#include "ExpressionEvaluator/ExpressionEvaluator.h"
#include "Render/TextureReaders/TextureLoader.h"
#include <yaml-cpp/yaml.h>
#include <iostream>


namespace UI
{
	bool ReadValue(std::string str, ExpessionEvaluator::INTContext& ctx, Constraint::Unit& unit, float& value)
	{
		if (ParseUnitValue(str.c_str(), unit, value))
		{
			return true;
		}
		else
		{
			auto pos = str.find(':');
			if (pos != std::string::npos)
			{
				str[pos] = '\0';
				ctx.func("anonymus", str.c_str());
				ctx.Link();
				value = ctx.get_func("anonymus").Eval();
				serialization::Parser parser(str.c_str() + pos + 1);
				parser.AcceptWhiteSpace();
				if (AcceptUnit(parser, unit))
				{
					parser.AcceptWhiteSpace();
					if (parser.EOS())
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	auto ReadCST(std::string str, Constraint::Type type, ExpessionEvaluator::INTContext& ctx)
	{
		Constraint::Unit unit;
		float value;
		ReadValue(std::move(str), ctx, unit, value);
		return Constraint(type, unit, value);
	}

	void ReadCSTSet(std::function<void(const char*, Constraint::Type, ExpessionEvaluator::INTContext&)> f,  ExpessionEvaluator::INTContext& ctx)
	{
		f("width", Constraint::Width, ctx);
		f("height", Constraint::Height, ctx);

		f("top", Constraint::Top, ctx);
		f("bottom", Constraint::Bottom, ctx);
		f("left", Constraint::Left, ctx);
		f("right", Constraint::Right, ctx);

		f("ctop", Constraint::CenterTop, ctx);
		f("cbottom", Constraint::CenterBottom, ctx);
		f("cleft", Constraint::CenterLeft, ctx);
		f("cright", Constraint::CenterRight, ctx);
	}

	void ReadConstraint(YAML::Node node, const char* name, Constraint::Type type, const BlockPtr& blk, ExpessionEvaluator::INTContext& ctx)
	{
		auto cnstr = node[name];
		if (cnstr.IsDefined())
		{
			if (cnstr.IsMap())
			{
				auto duration = cnstr["duration"];
				auto d_s = duration.as<std::string>("0");
				float duration_v = 0.0f;
				ParseTime(d_s.c_str(), duration_v);
				auto vcnstr = cnstr["value"];
				auto tcnstr = cnstr["target"];
				auto x = vcnstr.as<std::string>("0");
				auto ctsr = ReadCST(x, type, ctx);
				blk->PushConstraint(ctsr);
//				if (tcnstr.IsDefined())
//				{
//					std::string t = tcnstr.as<std::string>("0");
//					blk->PushTargetTransitionConstraints(read_cst(blk, t, type, ctx));
//				}

				blk->SetTransitionProperty(type, duration_v);
			}
			else
			{
				std::string x = cnstr.as<std::string>();
				auto ctsr = ReadCST(x, type, ctx);
				blk->PushConstraint(ctsr);
			}
		}
	}

	bool BuildList(const BlockPtr& parent, const YAML::Node& sequence, ExpessionEvaluator::INTContext& ctx, std::map<std::string, BlockPtr>& namedir, const ColorMap& color_map, const IntMap& tf_map, const IntMap& shader_map);

	void LoadActors(YAML::Node node, ExpessionEvaluator::INTContext& ctx, const BlockPtr& block)
	{
		auto actor_node = node["actor"];
		if (actor_node.IsDefined())
		{
			auto actor_cls = actor_node["class"].as<std::string>("None");
			if (actor_cls == "button")
			{
				std::vector<Constraint> c_idle;
				std::vector<Constraint> c_hower;
				std::vector<Constraint> c_down;

				auto reader = [&ctx](YAML::Node node, std::vector<Constraint>& c_list)
				{
					ReadCSTSet([&node, &c_list](const char* name, Constraint::Type type, ExpessionEvaluator::INTContext& ctx)
					           {
						           auto cnstr = node[name];
						           if (cnstr.IsDefined())
						           {
							           auto ctsr = ReadCST(cnstr.as<std::string>(), type, ctx);
							           c_list.push_back(ctsr);
						           }
					           }, ctx);
				};
				reader(actor_node["idle"], c_idle);
				reader(actor_node["hower"], c_hower);
				reader(actor_node["down"], c_down);

				block->EmplaceActor<ButtonActor>(actor_node["action"].as<std::string>("none"), c_idle, c_hower, c_down);

			}
		}
	}

	void LoadEmmitters(YAML::Node node, ExpessionEvaluator::INTContext& ctx, const BlockPtr& block, const ColorMap& color_map, const IntMap& tf_map, const IntMap& shader_map)
	{
		auto bg_color = node["bg_color"];
		auto bg_img = node["bg_img"];
		auto label_node = node["label"];
		auto shader_node = node["shader"];
		auto shadow_node = node["shadow"];
		if (shadow_node.IsDefined())
		{
			auto size = shadow_node["size"].as<std::string>();
			Constraint::Unit unit;
			float value;
			auto res = ReadValue(size, ctx, unit, value);
			ASSERT(res, "Error parsing shadow size");

			Render::color c;
			auto str = shadow_node["color"].as<std::string>("000000");
			str.erase(std::remove (str.begin(), str.end(), ' '), str.end());

			if (ParseColor(str.c_str(), c))
			{
			}
			else
			{
				auto it = color_map.find(str);
				if (it != color_map.end())
					c = it->second;
			}
    	    block->EmplaceEmitter<SShadowEmitter>(c, value, unit);
		}
		else if (shader_node.IsDefined())
		{
			auto ichannel0_node = node["ichannel0"];
			auto shader_str = shader_node.as<std::string>();

			Render::TexturePtr  ichannel0_tex;

			if (ichannel0_node.IsDefined())
			{
				auto ichannel0_str = ichannel0_node.as<std::string>();
				ichannel0_tex = Render::LoadTexture(ichannel0_str);
				spdlog::info("Texture: {}", ichannel0_str);
			}

			int shader_id = 0;
			auto it = shader_map.find(shader_str);
			if (it != shader_map.end())
			{
				spdlog::info("Shader: {}", shader_str);
				shader_id = it->second;
			}
			else{
				ASSERT(false, "Could not find shader %s", shader_str.c_str());
			}

    	    block->EmplaceEmitter<SShaderEmitter>(ichannel0_tex, shader_id);
		}
		else if (bg_color.IsDefined())
		{
			Render::color c;
			auto str = bg_color.as<std::string>();
			str.erase(std::remove (str.begin(), str.end(), ' '), str.end());

			if (ParseColor(str.c_str(), c))
			{
			}
			else
			{
				auto it = color_map.find(str);
				if (it != color_map.end())
					c = it->second;
			}
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
		else if (label_node.IsDefined())
		{
			auto text = label_node["text"].as<std::string>();
			spdlog::info("Text: {}", text);
			auto label_color = label_node["color"];
			Render::color c(255);
			if (label_color.IsDefined())
			{
				auto str = label_color.as<std::string>();
				str.erase(std::remove (str.begin(), str.end(), ' '), str.end());

				if (ParseColor(str.c_str(), c))
				{
				}
				else
				{
					auto it = color_map.find(str);
					if (it != color_map.end())
						c = it->second;
				}
				spdlog::info("Label color: {}", c);
			}

			auto height = label_node["height"].as<std::string>();
			Constraint::Unit unit;
			float value;
			auto res = ReadValue(height, ctx, unit, value);
			ASSERT(res, "Error parsing text height");

			Scriber::FontStyle::Enum style = Scriber::FontStyle::Regular;
			Scriber::Align::Enum align = Scriber::Align::None;

			if (label_node["style"].IsDefined())
			{
				auto style_str = label_node["style"].as<std::string>();
				ParseStyle(style_str.c_str(), style);
			}

			int stroke = 0;
			if (label_node["stroke"].IsDefined())
			{
				stroke = label_node["stroke"].as<int>();
			}

			if (label_node["align"].IsDefined())
			{
				auto align_str = label_node["align"].as<std::string>();
				ParseAlignment(align_str.c_str(), align);
			}
			if (align == Scriber::Align::None) align = Scriber::Align::Left;

			auto typeface_str = label_node["typeface"].as<std::string>();

			int typeface_id = 0;
			auto it = tf_map.find(typeface_str);
			if (it != tf_map.end())
			{
				spdlog::info("Typeface: {}", typeface_str);
				typeface_id = it->second;
			}
			else{
				ASSERT(false, "Could not find typeface %s", typeface_str.c_str());
			}

			uint8_t a = Scriber::Aggregate(style, align);

    	    block->EmplaceEmitter<STextEmitter>((uint8_t)typeface_id, text, value, unit, c, a, stroke);
		}
	}

	//					block->UpdateProp(c.type, c.unit, c.value, time);
	BlockPtr BuildBlock(YAML::Node node, ExpessionEvaluator::INTContext& ctx, std::map<std::string, BlockPtr>& namedir, const ColorMap& color_map, const IntMap& tf_map, const IntMap& shader_map)
	{
		auto block = UI::make_block({});
		auto name = node["name"];
		if (name.IsDefined())
		{
			spdlog::info("Node name: {}", name.as<std::string>());
			namedir[name.as<std::string>()] = block;
		}
		auto constraints = node["constraints"];
		if (constraints.IsDefined())
		{
			ReadCSTSet([&constraints, &block](const char* name, Constraint::Type type, ExpessionEvaluator::INTContext& ctx){
				ReadConstraint(constraints, name, type, block, ctx);
			}, ctx);
		}

		auto cnstr = node["border-radius"];
		if (cnstr.IsDefined())
		{
			std::string x = cnstr.as<std::string>();
			// spdlog::info("{}: {}", name, x);
			glm::vec4 value;
			Constraint::Unit unit[4];
			int components;
			if (ParseUnitValueList(x.c_str(), &unit[0], &value[0], 4, &components))
			{
				switch(components)
				{
					case 1: value[3] = value[2] = value[1] = value[0]; unit[3] = unit[2] = unit[1] = unit[0]; break;
					case 2: value[2] = value[0]; value[3] = value[1]; unit[2] = unit[0]; unit[3] = unit[1]; break;
					case 3: value[3] = value[1]; unit[3] = unit[1]; break;
					default: break;
				}

				block->SetRadiusVal(value);
				block->SetRadiusUnit(unit);
			}
			else
			{
				spdlog::error("Error parsing radius {}: {}", name.as<std::string>("noname"), x);
			}
		}

		LoadEmmitters(node, ctx, block, color_map, tf_map, shader_map);
		LoadActors(node, ctx, block);

		auto blocks = node["blocks"];
		if (blocks.IsDefined())
		{
			BuildList(block, blocks, ctx, namedir, color_map, tf_map, shader_map);
		}
		auto clip_overflow = node["clip_overflow"];
		if (clip_overflow.IsDefined())
		{
			block->EnableClipping(clip_overflow.as<bool>());
		}

		return block;
	}

	bool BuildList(const BlockPtr& parent, const YAML::Node& sequence, ExpessionEvaluator::INTContext& ctx, std::map<std::string, BlockPtr>& namedir, const ColorMap& color_map, const IntMap& tf_map, const IntMap& shader_map)
	{
		ASSERT(sequence.IsSequence(), "'blocks' must be sequence")
		for (YAML::Node node: sequence)
		{
			parent->AddChild(BuildBlock(node, ctx, namedir, color_map, tf_map, shader_map));
		}
		return true;
	}

	std::pair<BlockPtr, Animation> Load(const fsal::File& f, const IntMap& tf_map, const IntMap& shader_map)
	{
		using namespace UI::lit;
		using Render::operator""_c;
		spdlog::info("Loading: {}", f.GetPath().string().c_str());
		YAML::Node root_node = YAML::Load(std::string(f));

		ExpessionEvaluator::INTContext ctx;
		//ExpessionEvaluator::JITContext ctx;

		auto vars = root_node["vars"];
		auto colors = root_node["colors"];

		std::vector<double> data;
		if (vars.IsDefined())
		{
			data.resize(vars.size());
			int i = 0;
			for (auto var: vars)
			{
				std::string key = var.first.as<std::string>();
				auto value = var.second.as<double>();
				data[i] = value;
				ctx.var(key, &data[i]);
				++i;
			}
		}
		std::map<std::string, Render::color> color_map;
		if (colors.IsDefined())
		{
			for (auto c: colors)
			{
				std::string key = c.first.as<std::string>();
				auto value = c.second.as<std::string>();
				Render::color col;
				if (ParseColor(value.c_str(), col))
					color_map[key] = col;
			}
		}

		auto root = UI::make_block({0_l, 100_wpe, 0_t, 100_hpe});
		std::map<std::string, BlockPtr> namedir;

		LoadEmmitters(root_node, ctx, root, color_map, tf_map, shader_map);

		auto blocks = root_node["blocks"];

		if (blocks.IsDefined())
		{
			ASSERT(blocks.IsSequence(), "'blocks' must be sequence, in file %s", f.GetPath().string().c_str())

			BuildList(root, blocks, ctx, namedir, color_map, tf_map, shader_map);
		}

		auto animations = root_node["animations"];

		Animation animtaion_set;

		if (animations.IsDefined())
		{
			for (auto animation: animations)
			{
				auto animation_name = animation["name"].as<std::string>();
				auto tracks = animation["tracks"];
				for (auto track: tracks)
				{
					auto block_name = track["block"].as<std::string>();
					auto block_ptr = namedir[block_name];

					ReadCSTSet([&animtaion_set, animation_name, block_ptr, track](const char* name, Constraint::Type type, ExpessionEvaluator::INTContext& ctx){
						auto cnstr = track[name];
						if (cnstr.IsDefined())
						{
							auto ctsr = ReadCST(cnstr.as<std::string>(), type, ctx);
							animtaion_set[animation_name][block_ptr].push_back(ctsr);
						}
					}, ctx);
				}

			}
		}

		return std::make_pair(root, animtaion_set);
	}
}
