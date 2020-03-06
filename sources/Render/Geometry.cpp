#include "Geometry.h"
#include <GL/gl3w.h>

using namespace Render;


Geometry::Geometry() :
		m_numOfVertices(-1),
		m_numOfIndices(-1),
		m_indexType(-1),
		m_vertexBufferHandle(-1),
		m_indexBufferHandle(-1)
{
}

Geometry::~Geometry()
{
	DeleteBuffers();
}

static void DeleteBuffer(unsigned int& handle)
{
	if (handle != (uint32_t)-1)
	{
		GLuint* b = &handle;
		glDeleteBuffers(1, b);
		handle = -1;
	}
}

void Geometry::DeleteBuffers()
{
	DeleteBuffer(m_vertexBufferHandle);
	DeleteBuffer(m_indexBufferHandle);
}

void Geometry::FillBuffers(
		const void*	VertexArray,
		int			numOfVertices,
		int			verticesStride,
		const void*	indexArray,
		int			numOfIndices,
		int			indexSize,
		bool		dynamic)
{
	if (indexSize == 2)
	{
		m_indexType = GL_UNSIGNED_SHORT;
	}
	else if (indexSize == 4)
	{
		m_indexType = GL_UNSIGNED_INT;
	}
	m_stride = verticesStride;
	FillVertexBuffer(VertexArray, numOfVertices, dynamic);
	FillIndexBuffer(indexArray, numOfIndices);
}

void Geometry::FillVertexBuffer(
		const void*	VertexArray,
		int			numOfVertices,
		bool		dynamic)
{
	DeleteBuffer(m_vertexBufferHandle);
	m_numOfVertices = numOfVertices;
	glGenBuffers(1, &m_vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferHandle);
	int vertexBufferSize = GetVertexSize()*numOfVertices;
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, VertexArray, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Geometry::FillIndexBuffer(
	const void*	indexArray,
	int			numOfIndices,
	bool		dynamic)
{
	DeleteBuffer(m_indexBufferHandle);
	m_numOfIndices = numOfIndices;
	glGenBuffers(1, &m_indexBufferHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferHandle);
	int indexBufferSize = GetIndexSize()*numOfIndices;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indexArray, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Geometry::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferHandle);
}

void Geometry::UnBind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Geometry::DrawElements() const
{
	glDrawElements(GL_TRIANGLES, m_numOfIndices, m_indexType, 0);
}

int Geometry::GetVertexSize() const
{
	return m_stride;
}

int Geometry::GetIndexSize() const
{
	switch(m_indexType)
	{
		case GL_UNSIGNED_SHORT:
			return sizeof(unsigned short);
		case GL_UNSIGNED_INT:
			return sizeof(unsigned int);
		default: return 0;
	}
}
