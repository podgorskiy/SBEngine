#include <glm/glm.hpp>
#include <inttypes.h>

struct Events
{
	enum Enum: uint64_t
	{
		None,
		KeyDown,
		KeyUp,
		MouseButtonDown,
		MouseButtonUp,
		MouseMove,
		Scroll,
		WindowResize,
		Iconofication,

		UserInput,
		UI_UserAction,
		InGameEvent,
	};

	struct UserInputEvent
	{
		enum Type
		{
			None,
			CursorMove,
			CursorDown,
			CursorRelease,
			ArrowUp,
			ArrowDown,
			ArrowRight,
			ArrowLeft,
			Select
		};

		Type type;

		glm::vec2 position;
	};
};
