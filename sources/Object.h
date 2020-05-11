#include "Render/VertexSpec.h"
#include "Render/Shader.h"
#include <string>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>


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

	void Draw(int view, Render::ProgramPtr program);
private:
	bgfx::VertexBufferHandle m_vertexBuffer;
	bgfx::IndexBufferHandle m_indexBuffer;
	// Render::VertexBuffer m_geometry;
	// Render::VertexSpec m_vertexSpec;
};
