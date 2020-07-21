%module pl
%include "std_string.i"
%include "std_vector.i"
%{
#include <glm/glm.hpp>
#include "../Application.h"
#include "../IApplication.h"
#include "../MenuStateManager/IMenuState.h"
#include "../MenuStateManager/LuaMenuState.h"
#include "../MenuStateManager/MenuStateManager.h"
#include "../MenuStateManager/MenuFactory.h"
#include "../UI/Enums.h"
#include "../UI/color.h"
#include "../UI/View.h"
#include "../UI/Block.h"
#include "../UI/UI_Load.h"
#include "../UI/color.h"
#include "../UI/Actor.h"
#include "../UI/Emitters.h"
#pragma warning( disable : 4244 )

%}


%include "../../libs/swig-openframeworks/glm.i"

%include "IApplication.h"
%include "Application.h"

%include "MenuStateManager/IMenuState.h"
%include "MenuStateManager/LuaMenuState.h"
%include "MenuStateManager/MenuStateManager.h"
%include "MenuStateManager/MenuFactory.h"
%include "UI/Enums.h"
%include "UI/color.h"
%include "UI/View.h"
%include "UI/Block.h"
%include "UI/UI_Load.h"
%include "UI/color.h"
%include "UI/Actor.h"
%include "UI/Emitters.h"

