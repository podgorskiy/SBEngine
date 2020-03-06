#include "Render/Geometry.h"
#include <string>
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
};

class Object
{
public:
	void Load(std::string path);

	void MakeBox();

	void Bind();
	void Draw();
	void UnBind();
private:
	Render::Geometry m_geometry;
};
