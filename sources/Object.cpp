#include "Object.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string.h>
#include <fsal.h>
#include <MemRefFile.h>


void Object::Load(std::string path)
{
	std::map<int, int> vertex_cache;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv_coordinates;
	std::vector<int> indices;

	std::ifstream file(path);
	std::string str;
	glm::vec3 pos;
	glm::vec3 uv;
	int current_index = 0;

	std::vector<Vertex> vertices;

	while (std::getline(file, str))
	{
		if (strncmp(str.c_str(), "v ", 2) == 0)
		{
			sscanf(str.c_str(), "v %f %f %f", &pos.x, &pos.y, &pos.z);
			positions.push_back(pos);
		}
		else if (strncmp(str.c_str(), "vt ", 3) == 0)
		{
			sscanf(str.c_str(), "vt %f %f", &uv.x, &uv.y);
			uv_coordinates.push_back(uv);
		}
		else if (strncmp(str.c_str(), "f ", 2) == 0)
		{
			int pos[4] = { 0, 0, 0 , 0 };
			int uv[4] = { 0, 0, 0, 0 };
			sscanf(str.c_str(), "f %d/%d %d/%d %d/%d %d/%d", pos, uv, pos + 1, uv + 1, pos + 2, uv + 2, pos + 3, uv + 3);
			int ind[4] = { 0, 0, 0, 0 };
			for (int i = 0; i < 4; ++i)
			{
				auto key = pos[i];
				auto it = vertex_cache.find(key);
				if (it != vertex_cache.end())
				{
					ind[i] = it->second;
				}
				else
				{
					vertex_cache[key] = current_index;
					ind[i] = current_index;
					++current_index;
					Vertex v;
					v.pos = positions[pos[i] - 1];
					v.uv = uv_coordinates[uv[i] - 1];
					v.normal = glm::vec3(0);
					v.tangent = glm::vec3(0);
					vertices.push_back(v);
				}
			}
			indices.push_back(ind[0]);
			indices.push_back(ind[1]);
			indices.push_back(ind[2]);
			indices.push_back(ind[0]);
			indices.push_back(ind[2]);
			indices.push_back(ind[3]);
		}
	}

	for (size_t f = 0; f < indices.size() / 3; ++f)
	{
		int i = indices[3 * f + 0];
		int j = indices[3 * f + 1];
		int k = indices[3 * f + 2];
		glm::vec3 p1 = vertices[i].pos;
		glm::vec3 p2 = vertices[j].pos;
		glm::vec3 p3 = vertices[k].pos;
		glm::vec3 a = p1 - p3;
		glm::vec3 b = p2 - p3;
		glm::vec3 c = glm::cross(a, b);
		vertices[i].normal += c;
		vertices[j].normal += c;
		vertices[k].normal += c;
	}


	for (size_t f = 0; f < indices.size() / 3; f++)
	{
		int i = indices[3 * f + 0];
		int j = indices[3 * f + 1];
		int k = indices[3 * f + 2];

		glm::vec3 v1 = vertices[i].pos;
		glm::vec3 v2 = vertices[j].pos;
		glm::vec3 v3 = vertices[k].pos;

		glm::vec2 w1 = vertices[i].uv;
		glm::vec2 w2 = vertices[j].uv;
		glm::vec2 w3 = vertices[k].uv;

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		vertices[i].tangent += sdir;
		vertices[j].tangent += sdir;
		vertices[k].tangent += sdir;
	}

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i].normal = glm::normalize(vertices[i].normal);
		vertices[i].tangent = glm::normalize(vertices[i].tangent - vertices[i].normal * glm::dot(vertices[i].normal, vertices[i].tangent));
	}

	bgfx::VertexLayout layout;
	layout.begin()
	      .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
	      .add(bgfx::Attrib::Normal   , 3, bgfx::AttribType::Float)
	      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		  .add(bgfx::Attrib::Color0,    3, bgfx::AttribType::Float)
		  .end();

	m_vertexBuffer = bgfx::createVertexBuffer(bgfx::copy(vertices.data(), vertices.size() * sizeof(Vertex)), layout);
	m_indexBuffer = bgfx::createIndexBuffer(bgfx::copy(indices.data(), indices.size() * sizeof(int)), BGFX_BUFFER_INDEX32);
}


// The code below creates a box mesh
void Object::MakeBox()
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < 6; ++i)
	{
		glm::vec3 dir(0.0f);
		glm::vec3 dir2(0.0f);
		glm::vec3 dir3(0.0f);
		dir[i % 3] = i > 2 ? 1.0f : -1.0f;
		dir2[(i + 1) % 3] = i > 2 ? 1.0f : -1.0f;
		dir3[(i + 2) % 3] = i > 2 ? 1.0f : -1.0f;
		vertices.push_back({ dir - dir2 - dir3, dir });
		vertices.push_back({ dir - dir2 + dir3, dir });
		vertices.push_back({ dir + dir2 - dir3, dir });
		vertices.push_back({ dir + dir2 + dir3, dir });
	}
	std::vector<int> indices;
	for (int i = 0; i < 6; ++i)
	{
		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
	}

	bgfx::VertexLayout layout;
	layout.begin()
	      .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
	      .add(bgfx::Attrib::Normal   , 3, bgfx::AttribType::Float)
	      .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		  .add(bgfx::Attrib::Color0,    3, bgfx::AttribType::Float)
		  .end();

	m_vertexBuffer = bgfx::createVertexBuffer(bgfx::copy(vertices.data(), vertices.size()), layout);
	m_indexBuffer = bgfx::createIndexBuffer(bgfx::copy(indices.data(), indices.size()), BGFX_BUFFER_INDEX32);
}

void Object::Draw(int view, Render::ProgramPtr program)
{
	bgfx::setVertexBuffer(0, m_vertexBuffer);
	bgfx::setIndexBuffer(m_indexBuffer);
	bgfx::submit(view, program->GetHandle());
}
