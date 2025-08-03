#include "etpch.h"
#include "Citro3DMesh.h"

#include <sstream>

namespace Entry {
	
	Citro3DMesh::Citro3DMesh(const std::string& path)
	{
		// TODO: Update to full support OBJ files
		std::vector<float> vertices;
		std::vector<uint16_t> indices;

		std::ifstream file(path);
		if (!file.is_open())
			return;

		std::string line;
		while (std::getline(file, line)) {
			std::istringstream ss(line);
			std::string prefix;
			ss >> prefix;

			if (prefix == "v") {
				float x, y, z;
				ss >> x >> y >> z;
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
			}
			else if (prefix == "f") {
				std::array<std::string, 3> indices_str;
				ss >> indices_str[0] >> indices_str[1] >> indices_str[2];

				indices.push_back(std::stoi(indices_str[0]) - 1);
				indices.push_back(std::stoi(indices_str[1]) - 1);
				indices.push_back(std::stoi(indices_str[2]) - 1);
			}
			else if (prefix == "o") {
				ss >> m_Name;
			}

		}

		m_VertexArray = VertexArray::Create();

		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float)));

		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
			});
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		Ref<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices.data(), indices.size()));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		printf("Loaded \"%s\" successfully!\n", m_Name.c_str());
	}
	Citro3DMesh::~Citro3DMesh()
	{
	}

	void Citro3DMesh::Bind()
	{
		m_VertexArray->Bind();
	}
}