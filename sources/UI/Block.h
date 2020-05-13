#pragma once
#include "utils/stack_vector.h"
#include "Render/color.h"

#include <inttypes.h>
#include <glm/matrix.hpp>

#include <vector>
#include <functional>
#include <memory>
#include "utils/aabb.h"
#include <initializer_list>
#include <type_traits>


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
			ValueHeight,
			ValueWidth,
			ValueMin,
			ValueMax,
			Pixel,
			Point
		};

		Constraint(Type type, Unit unit, float value): type(type), unit(unit), value(value) {};
		Constraint(): type(Invalid), unit(Pixel), value(0.0f) {};

		Type type;
		Unit unit;
		float value;
	};

	struct ImSize
	{
		enum Enum: uint8_t
		{
			Auto,
			Contain,
			Cover,
			Fill,
		};
	};

	struct ImPos
	{
		enum Enum: uint8_t
		{
			LeftTop,
			leftCenter,
			leftBottom,
			RightTop,
			RightCenter,
			RightBottom,
			CenterTop,
			CenterCenter,
			CenterBottom,
		};
	};

	struct ImTransform
	{
		enum Enum: uint8_t
		{
			None  = 0,
			FlipX = 1 << 0,
			FlipY = 1 << 1
		};
	};


	namespace lit
	{
#define _LITERAL(Type, T, Unit, U) \
    inline Constraint operator "" _##T##U(long double x) { return Constraint(Constraint::Type, Constraint::Unit, (float)x); }\
    inline Constraint operator "" _##T##U(unsigned long long int x) { return Constraint(Constraint::Type, Constraint::Unit, (float)x); }

#define LITERAL(Type, T)\
    _LITERAL(Type, T, Percentage, pe) _LITERAL(Type, T, Pixel, px) _LITERAL(Type, T, Point, pt) \
    _LITERAL(Type, T, ValueHeight, vh) _LITERAL(Type, T, ValueWidth, vw) \
    _LITERAL(Type, T, ValueMin, vmin) _LITERAL(Type, T, ValueMin, vmax) \
    _LITERAL(Type, T, Point, )

		LITERAL(Left, l)
		LITERAL(Right, r)
		LITERAL(Width, w)
		LITERAL(CenterLeft, cl)
		LITERAL(CenterRight, cr)
		LITERAL(Top, t)
		LITERAL(Bottom, b)
		LITERAL(Height, h)
		LITERAL(CenterTop, ct)
		LITERAL(CenterBottom, cb)

		typedef UI::ImSize S;
		typedef UI::ImPos P;
		typedef UI::ImTransform T;
	}

	class Block;
	typedef std::shared_ptr<Block> BlockPtr;

	class IEmitter
	{
	public:
		virtual void operator ()(UI::Renderer*, const Block*, float time, int flags) = 0;
		virtual ~IEmitter() = default;
	};

	class SFillEmitter: public IEmitter
	{
	public:
		explicit SFillEmitter(color c);
		virtual void operator ()(UI::Renderer*, const Block*, float time, int flags);
	private:
		UI::color col;
	};

	class SImageEmitter: public IEmitter
	{
	public:
		SImageEmitter(Render::TexturePtr tex, ImSize::Enum size,  ImPos::Enum pos,  ImTransform::Enum t);
		virtual void operator ()(UI::Renderer*, const Block*, float time, int flags);
	private:
		glm::ivec2 image_size;
		Render::TexturePtr tex;
		UI::ImSize::Enum size;
		UI::ImPos::Enum pos;
		ImTransform::Enum t;
	};

	struct EmitterSizeCheck
	{
		enum
		{
			DataSize = 40,
		};
		void check()
		{
			static_assert(DataSize >= sizeof(IEmitter), "");
			static_assert(DataSize >= sizeof(SImageEmitter), "");
		}
	};

	class Block
	{
		friend void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda);
	public:
		Block() = default;
		~Block()
		{
			if (has_emitter) GetEmitter()->~IEmitter();
		}

		explicit Block(std::initializer_list<Constraint> cnst): m_constraints(cnst) {}

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
		void Emit(UI::Renderer* r, float time = 0.0f, int flags = 0) {	if (has_emitter) (*GetEmitter())(r, this, time, flags); }

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
		template <typename R, typename... Ts>
		void EmplaceEmitter(Ts&&... args) {
		    new (userdata) R(std::forward<Ts>(args)...);
		    has_emitter = true;
		}
	private:
		IEmitter* GetEmitter() { return (IEmitter*)userdata; }

		glm::aabb2 m_box;
		stack::vector<Constraint, 4> m_constraints;
		stack::vector<BlockPtr, 4> m_childs;
		glm::vec2 m_rotation = glm::vec2(1.0f, 0.0f);
		uint8_t userdata[EmitterSizeCheck::DataSize] = {0};
		bool has_emitter = false;
	};

    inline BlockPtr make_block(std::initializer_list<Constraint> constraints)
    {
    	return std::make_shared<Block>(std::initializer_list<Constraint>(constraints));
    }

    inline BlockPtr make_block(std::initializer_list<Constraint> constraints, color c)
    {
    	auto block = std::make_shared<Block>(std::initializer_list<Constraint>(constraints));
    	block->EmplaceEmitter<SFillEmitter>(c);
    	return block;
    }

    inline BlockPtr make_block(std::initializer_list<Constraint> constraints,
    		Render::TexturePtr tex,
    		ImSize::Enum size = ImSize::Auto,
    		ImPos::Enum pos = ImPos::LeftTop,
    		ImTransform::Enum t = ImTransform::None)
    {
    	auto block = std::make_shared<Block>(std::initializer_list<Constraint>(constraints));
    	block->EmplaceEmitter<SImageEmitter>(std::move(tex), size, pos, t);
    	return block;
    }

	inline void Traverse(const BlockPtr& block, const BlockPtr& parent, const std::function<void(Block* block, Block* parent)>& lambda)
	{
		lambda(block.get(), parent.get());
		for (auto& child: block->m_childs)
		{
			Traverse(child, block, lambda);
		}
	}

	inline void Render(UI::Renderer* renderer, const BlockPtr& root, float time = 0.0f, int flags = 0)
	{
		Traverse(root, nullptr, [renderer, time, flags](UI::Block* block, UI::Block* parent)
		{
			block->Emit(renderer, time, flags);
		});
		renderer->Draw();
	}

	inline glm::aabb2 SolveConstraints(const glm::aabb2& parent_box, const Constraint* cnst, int count, float ppd)
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
				case Constraint::ValueHeight: p = p_size.y; p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueWidth: p = p_size.x; p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueMin: p = glm::min(p_size.x, p_size.y); p *= cnst[i].value / 100.0f; break;
				case Constraint::ValueMax: p = glm::max(p_size.x, p_size.y); p *= cnst[i].value / 100.0f; break;
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
		return box;
	}

	inline void DoLayout(const BlockPtr& block, const View& view)
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

#if 0
namespace SB
{
	class EventManager;

	namespace GUI
	{
		struct Rule
		{
			enum Anchor
			{
				None,
				Left = 1,
				Right = 2,
				Width = 3,
				Top = 4,
				Bottom = 5,
				Height = 6,
				WidthAspect = 7,
				CLeft = 8,
				CRight = 9,
				CBottom = 10,
				CTop = 11,
			};

			enum Unit
			{
				Percentage,
				Px,
				Em,
				Pt
			};

			Anchor anchor;
			AnchorType type;
			float val;
			Rule(GUI::Anchor anchor, GUI::AnchorType type, float val) :anchor(anchor), type(type), val(val) {};
			Rule(const char* name, float value);
		};


		enum
		{
			MaxInputCount = 2
		};

		class IElementActor;

		class Block
		{
		public:
			typedef std::vector<Rule> TVectorRules;
			typedef std::vector<Block*> TVectorElements;

			Block();
			virtual ~Block();

			void SetHideAllButOneChild(const std::string& childName, bool hide, const glm::vec4& parentColor);

			Block* CopyDeep() const;

			void Init(const std::string& name, IElementActor* actor = nullptr);
			void SetName(const std::string& name);
			void SetDrawable(const RenderProxy& drawable);
			RenderProxy& GetDrawable();
			const RenderProxy& GetDrawable() const;

			virtual void Draw(const glm::mat4& viewProjection, bool drawChilds = true);
			virtual void RegisterForEvents(EventManager* emng, unsigned char mask = 0x1);
			virtual void UnregisterForEvents();

			virtual void Resize(const Surface& surface, float delta, bool childs = true);

			void SetAnchor(GUI::Anchor anchor, GUI::AnchorType type, float val);
			void OverwriteAnchor(GUI::Anchor anchor, GUI::AnchorType newType, float newVal);
			void ClearAnchors();

			void AddChild(Block* element);
			Block* GetElement(const std::string& name);
			const Block* GetElement(const std::string& name) const;
			Block* RemoveElement(const std::string& name);
			Block* RemoveElement(Block* element);

			void DeleteAllChilds();

			TVectorRules& GetRules();

			void SetLimits(const glm::vec2 a, const glm::vec2 b);

			virtual void SetEnabled(bool enabled);

			void SetVisible(bool visible)
			{
				m_visible = visible;
			}

			bool GetVisible() const
			{
				return m_visible;
			}

			glm::vec2 GetPosition()
			{
				return m_position;
			};

			glm::vec2 GetSize()
			{
				return m_dimensions;
			};

			std::string GetName()
			{
				return m_name;
			}

			void ForEachChild(const std::function<void(Block* child)>& lambda);

			void ForOneChild(int childIndex, const std::function<void(Block* child)>& lambda);

		protected:
			virtual Block* CopyShell() const;

			friend class Slider;
			friend class DoubleSlider;
			friend class ScrollWindow;
			friend class RadioButton;

			void UpdateMatrices();

			RenderProxy m_drawable;

			glm::vec2 m_pixel;
			glm::vec2 m_pixelToDot;

			std::string m_name;
			bool m_inFocus[MaxInputCount];
			bool m_activated;
			float m_time;

			glm::vec2 m_p_a;
			glm::vec2 m_p_b;
			glm::vec2 m_dimensions;
			glm::vec2 m_position;
			glm::vec2 m_leftBottom;
			TVectorRules m_rules;
			TVectorElements m_childs;

			glm::mat3 m_local2dTransform;
			glm::vec2 m_rotation;

			IElementActor* m_actor;

			int m_local2dTransformID;
			int m_focusID;
			int m_textureID;
			int m_texture2ID;

			glm::vec2 m_limitA;
			glm::vec2 m_limitB;

			bool m_root;

			bool m_visible;
		};

		inline void ClearBoolArray(bool* x)
		{
			for (int i = 0; i < MaxInputCount; ++i) x[i] = false;
		}
		inline void ClearFloatArray(float* x)
		{
			for (int i = 0; i < MaxInputCount; ++i) x[i] = 0.0f;
		}
	}
}
#endif
