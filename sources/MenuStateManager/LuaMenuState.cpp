#include "LuaMenuState.h"
#include "MenuFactory.h"
#include "utils/assertion.h"
#include <spdlog/spdlog.h>
#include <dear-imgui/imgui.h>
#include <yaml-cpp/yaml.h>
#include "EventIds.h"
#include "EventDispatcher/EventDispatcher.h"
#define lua_c
extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}
#include "LuaBridge/LuaBridge.h"
#include "luabind/lua_loader.h"


void print_error(lua_State* state)
{
	// The error message is on top of the stack.
	// Fetch it, print it and then pop it off the stack.
	const char* message = lua_tostring(state, -1);
	if (message)
		spdlog::error(message);
	ASSERT(false, "Lua failed: %s", message);
	lua_pop(state, 1);
}

static char* str_copy(const char* str)
{
	size_t len = strlen(str) + 1;
	void* buf = malloc(len);
	IM_ASSERT(buf);
	return (char*) memcpy(buf, (const void*) str, len);
}

int print(lua_State* L)
{
	lua_getglobal(L, "print_ptr");
	auto* p = (std::function<void(const char*)>*)lua_touserdata(L, -1);
	lua_remove(L, -1);

	int n = lua_gettop(L);
	for (int i = 1; i <= n; i++)
	{
		auto r = lua_tostring(L, i);
		//(*p)(r);
		spdlog::info("Lua {}", r);
	}
	return LUA_OK;
}


LuaMenuState::LuaMenuState(const fsal::File& yaml, MenuStateManager* msm): BaseMenuState(yaml, msm)
{
	m_lua = luaL_newstate();
	luaL_openlibs(m_lua);
	LoadUI_API(m_lua);

	m_printer = [this](const char* s) {
		m_history.push_back(str_copy(s));
	};

	luabridge::getGlobalNamespace (m_lua)
	  .addCFunction ("print", print);

	lua_pushlightuserdata(m_lua, (void*)&m_printer);
	lua_setglobal(m_lua, "print_ptr");


	YAML::Node root_node = YAML::Load(std::string(yaml));
	auto lua_code = root_node["lua"];
	ASSERT(lua_code.IsDefined(), "No lua code in LuaMenuState yaml")

	lua_getglobal(m_lua, "debug");
	lua_getfield(m_lua, -1, "traceback");

	luabridge::push(m_lua, msm);
	lua_setglobal(m_lua, "msm");

	luabridge::push(m_lua, (IMenuState*)this);
	lua_setglobal(m_lua, "this");

	auto result = luaL_dostring(m_lua, lua_code.as<std::string>().c_str());

	if (result != LUA_OK)
	{
		print_error(m_lua);
		lua_close(m_lua);
		m_lua = nullptr;
	}

	auto event_node = root_node["events"];
	if (event_node.IsDefined())
	{
		for (auto event: event_node)
		{
			std::string key = event["name"].as<std::string>();
			std::string func = event["call"].as<std::string>();

			if (key == "KeyDown")
			{
				const auto* attachment = m_eventDispatcher->AttachEventHandler(Events::KeyDown, [this, func](int64_t event, core::EventDispatcher::Box box) -> bool
				{
					int key;
					char asci;
					std::tie(key, asci) = box.Unbox<std::pair<int, char> >();
					lua_getglobal(m_lua, func.c_str());
					lua_pushnumber(m_lua, key);
					lua_pushnumber(m_lua, asci);
					lua_call(m_lua, 2, 0);
				});
				m_attachments.push_back(std::make_pair((const void*)attachment, Events::KeyDown));
			}
			else if (key == "MouseButtonDown")
			{
				const auto* attachment = m_eventDispatcher->AttachEventHandler(Events::MouseButtonDown, [this, func](int64_t event, core::EventDispatcher::Box box) -> bool
				{
					int key = box.Unbox<int>();
					lua_getglobal(m_lua, func.c_str());
					lua_pushnumber(m_lua, key);
					lua_call(m_lua, 1, 0);
				});
				m_attachments.push_back(std::make_pair((const void*)attachment, Events::MouseButtonDown));
			}

		}
	}
}

LuaMenuState::~LuaMenuState()
{
	for (auto a: m_attachments)
	{
		m_eventDispatcher->DetachEventHandler((const core::EventDispatcher::IEventAttachment*)a.first, a.second);
	}

	lua_close(m_lua);
}

void LuaMenuState::Update(Render::View viewbox, float time, float deltaTime)
{
	if (m_lua)
	{
		lua_getglobal(m_lua, "update");
		luabridge::push(m_lua, viewbox);
		lua_pushnumber(m_lua, time);
		lua_pushnumber(m_lua, deltaTime);
		lua_call(m_lua, 3, 0);
	}

//    ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiCond_FirstUseEver);
//	if (ImGui::Begin("Lua interactive"))
//	{
//		static char InputBuf[256];
//	    static int                   HistoryPos = -1;   // -1: new line, 0..History.Size-1 browsing history.
//	    static bool                  AutoScroll = true;
//	    static bool                  ScrollToBottom = false;
//
//        if (ImGui::SmallButton("Clear")) {
//	        for (int i = 0; i < m_history.size(); i++)
//	        {
//		        free(m_history[i]);
//	        }
//	        m_history.clear();
//        } ImGui::SameLine();
//
//        ImGui::Separator();
//
//        // Options menu
//        if (ImGui::BeginPopup("Options"))
//        {
//            ImGui::Checkbox("Auto-scroll", &AutoScroll);
//            ImGui::EndPopup();
//        }
//        ImGui::Separator();
//
//        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
//        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
//
//        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
//
//        for (int i = 0; i < m_history.size(); i++)
//        {
//            const char* item = m_history[i];
//            // Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
//            bool pop_color = false;
//            if (strstr(item, "[error]"))            { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }
//            else if (strncmp(item, "# ", 2) == 0)   { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); pop_color = true; }
//            ImGui::TextUnformatted(item);
//            if (pop_color)
//                ImGui::PopStyleColor();
//        }
//        if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
//            ImGui::SetScrollHereY(1.0f);
//        ScrollToBottom = false;
//
//        ImGui::PopStyleVar();
//        ImGui::EndChild();
//        ImGui::Separator();
//
//        // Command-line
//        bool reclaim_focus = false;
//        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue))
//        {
//			luaL_dostring(m_lua, InputBuf);
//			InputBuf[0] = '\0';
//            reclaim_focus = true;
//        }
//
//        // Auto-focus on window apparition
//        ImGui::SetItemDefaultFocus();
//        if (reclaim_focus)
//            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
//
//        ImGui::End();
//	}

	BaseMenuState::Update(viewbox, time, deltaTime);
}

void LuaMenuState::Draw(Render::View viewbox, Render::Renderer2D* rd)
{
	if (m_lua)
	{
		lua_getglobal(m_lua, "draw");
		lua_call(m_lua, 0, 0);
	}

	BaseMenuState::Draw(viewbox, rd);
}

RegisterMenu(LuaMenuState);
