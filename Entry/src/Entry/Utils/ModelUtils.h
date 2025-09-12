#pragma once

#include "Entry/Core/Core.h"
#include "Entry/Renderer/Model.h"
#include "Entry/Renderer/Renderer3D.h"

#include <fast_obj.h>
#include <string>
#include <glm/glm.hpp>

#include "StringUtils.h"

namespace Entry
{
	static void FastOBJToBuffers(
        std::vector<float>* outVertices, 
        std::vector<uint16_t>* outIndices, 
        Ref<fastObjMesh> obj_mesh)
	{
		outVertices->clear();
		outIndices->clear();

		uint32_t indexOffset = 0;

		for (uint32_t i = 0; i < obj_mesh->face_count; ++i)
		{
			uint32_t faceVertices = obj_mesh->face_vertices[i];

			for (uint32_t j = 0; j < faceVertices; ++j)
			{
				fastObjIndex idx = obj_mesh->indices[indexOffset + j];


				float px = obj_mesh->positions[3 * idx.p + 0];
				float py = obj_mesh->positions[3 * idx.p + 1];
				float pz = obj_mesh->positions[3 * idx.p + 2];

				float u = 0.0f, v = 0.0f;
				if (idx.t != 0 && obj_mesh->texcoords)
				{
					u = obj_mesh->texcoords[2 * idx.t + 0];
					v = 1.0f - obj_mesh->texcoords[2 * idx.t + 1]; // Flip V
				}

				float nx = 0.0f, ny = 0.0f, nz = 0.0f;
				if (idx.n != 0 && obj_mesh->normals)
				{
					nx = obj_mesh->normals[3 * idx.n + 0];
					ny = obj_mesh->normals[3 * idx.n + 1];
					nz = obj_mesh->normals[3 * idx.n + 2];
				}

				// Append to vertex buffer
				outVertices->push_back(-px);
				outVertices->push_back(py);
				outVertices->push_back(-pz);
				outVertices->push_back(u);
				outVertices->push_back(v);
				outVertices->push_back(-nx);
				outVertices->push_back(ny);
				outVertices->push_back(-nz);

				// Index is just the order of the vertices
				outIndices->push_back(static_cast<uint16_t>((outVertices->size() / 8) - 1));
			}
			indexOffset += faceVertices;
		}
	}

	static void FastOBJCreateMeshes(
        std::vector<Mesh>& meshes, 
        std::vector<Ref<Material>>& materials, 
        std::vector<Ref<Texture2D>>& textures, 
        Ref<fastObjMesh> obj_mesh)
	{
		meshes.clear();
		materials.clear();
		textures.clear();

		// Get Textures
		for (uint32_t i = 0; i < obj_mesh->texture_count; ++i)
		{
			if (obj_mesh->textures[i].path == nullptr) continue;
			textures.push_back(Texture2D::Create(obj_mesh->textures[i].path));
		}

		// Get Materials
		for (uint32_t i = 0; i < obj_mesh->material_count; ++i)
		{
			//Material mat = { obj_mesh->materials[i].name, obj_mesh->materials[i].map_Kd-1};
			MaterialProps props = {
				{	// Material Values
					obj_mesh->materials[i].Ka[0], obj_mesh->materials[i].Ka[1], obj_mesh->materials[i].Ka[2],
					obj_mesh->materials[i].Kd[0], obj_mesh->materials[i].Kd[1], obj_mesh->materials[i].Kd[2],
					obj_mesh->materials[i].Ks[0], obj_mesh->materials[i].Ks[1], obj_mesh->materials[i].Ks[2],
					obj_mesh->materials[i].Ks[0], obj_mesh->materials[i].Ks[1], obj_mesh->materials[i].Ks[2], // TODO: Double check
					obj_mesh->materials[i].Ke[0], obj_mesh->materials[i].Ke[1], obj_mesh->materials[i].Ke[2]
				},
				obj_mesh->materials[i].name,
				textures[obj_mesh->materials[i].map_Kd - 1],
                Renderer3D::GetDefaultShader()
			};

			materials.push_back(Material::Create(props));
		}

		// Create meshes from objects (-o)
		for (uint32_t o = 0; o < obj_mesh->object_count; ++o)
		{
			auto currentObj = obj_mesh->objects[o];
			uint32_t nextIndexOffset = o + 1 >= obj_mesh->object_count ? obj_mesh->index_count : obj_mesh->objects[o + 1].index_offset;
			uint32_t indexCount = nextIndexOffset - currentObj.index_offset;

			Mesh mesh {
                currentObj.name,
				indexCount,
				currentObj.index_offset,
                o,
				obj_mesh->face_materials[currentObj.face_offset],
                currentObj.face_count * 3,
                currentObj.face_count,
			};
			meshes.push_back(mesh);
		}

		meshes.shrink_to_fit();
		materials.shrink_to_fit();
		textures.shrink_to_fit();
	}

    //static void tinygltfToBuffers(std::vector<float>* outVertices, std::vector<uint16_t>* outIndices, tinygltf::Model& model)
    //{
    //    outVertices->clear();
    //    outIndices->clear();

    //    // Loop through all meshes
    //    for (const auto& mesh : model.meshes)
    //    {
    //        for (const auto& prim : mesh.primitives)
    //        {
    //            // Access position accessor
    //            const auto& posAccessor = model.accessors[prim.attributes.find("POSITION")->second];
    //            const auto& posBufferView = model.bufferViews[posAccessor.bufferView];
    //            const auto& posBuffer = model.buffers[posBufferView.buffer];
    //            const float* positions = reinterpret_cast<const float*>(
    //                &posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]
    //                );

    //            // Access normal accessor if it exists
    //            const float* normals = nullptr;
    //            auto itN = prim.attributes.find("NORMAL");
    //            if (itN != prim.attributes.end())
    //            {
    //                const auto& nAccessor = model.accessors[itN->second];
    //                const auto& nBufferView = model.bufferViews[nAccessor.bufferView];
    //                const auto& nBuffer = model.buffers[nBufferView.buffer];
    //                normals = reinterpret_cast<const float*>(
    //                    &nBuffer.data[nBufferView.byteOffset + nAccessor.byteOffset]
    //                    );
    //            }

    //            // Access texcoord accessor if it exists
    //            const float* texcoords = nullptr;
    //            auto itT = prim.attributes.find("TEXCOORD_0");
    //            if (itT != prim.attributes.end())
    //            {
    //                const auto& tAccessor = model.accessors[itT->second];
    //                const auto& tBufferView = model.bufferViews[tAccessor.bufferView];
    //                const auto& tBuffer = model.buffers[tBufferView.buffer];
    //                texcoords = reinterpret_cast<const float*>(
    //                    &tBuffer.data[tBufferView.byteOffset + tAccessor.byteOffset]
    //                    );
    //            }

    //            // Access indices
    //            const auto& idxAccessor = model.accessors[prim.indices];
    //            const auto& idxBufferView = model.bufferViews[idxAccessor.bufferView];
    //            const auto& idxBuffer = model.buffers[idxBufferView.buffer];
    //            const uint16_t* indices = reinterpret_cast<const uint16_t*>(
    //                &idxBuffer.data[idxBufferView.byteOffset + idxAccessor.byteOffset]
    //                );

    //            size_t vertexStart = outVertices->size() / 8; // 8 floats per vertex

    //            // Write indices (adjusted by vertex offset)
    //            for (size_t i = 0; i < idxAccessor.count; ++i)
    //            {
    //                outIndices->push_back(static_cast<uint16_t>(vertexStart + indices[i]));
    //            }

    //            // Write vertices
    //            for (size_t i = 0; i < posAccessor.count; ++i)
    //            {
    //                float px = positions[i * 3 + 0];
    //                float py = positions[i * 3 + 1];
    //                float pz = positions[i * 3 + 2];

    //                float nx = normals ? normals[i * 3 + 0] : 0.0f;
    //                float ny = normals ? normals[i * 3 + 1] : 0.0f;
    //                float nz = normals ? normals[i * 3 + 2] : 0.0f;

    //                float u = texcoords ? texcoords[i * 2 + 0] : 0.0f;
    //                float v = texcoords ? 1.0f - texcoords[i * 2 + 1] : 0.0f; // Flip V

    //                outVertices->push_back(px);
    //                outVertices->push_back(py);
    //                outVertices->push_back(pz);
    //                outVertices->push_back(u);
    //                outVertices->push_back(v);
    //                outVertices->push_back(nx);
    //                outVertices->push_back(ny);
    //                outVertices->push_back(nz);
    //            }
    //        }
    //    }
    //}

    //static void tinygltfCreateMeshes(
    //    std::vector<Mesh>& meshes,
    //    std::vector<Ref<Material>>& materials,
    //    std::vector<Ref<Texture2D>>& textures,
    //    const tinygltf::Model& model,
    //    std::string& directory)
    //{
    //    meshes.clear();
    //    materials.clear();
    //    textures.clear();

    //    // --- Load Textures ---
    //    for (const auto& tex : model.textures)
    //    {
    //        if (tex.source < 0 || tex.source >= model.images.size()) continue;
    //        const auto& img = model.images[tex.source];
    //        // img.uri can be a file path
    //        if (!img.uri.empty())
    //            textures.push_back(Texture2D::Create(directory + "\\" + img.uri));
    //    }

    //    // --- Load Materials ---
    //    for (const auto& mat : model.materials)
    //    {
    //        MaterialProps props = {};

    //        props.Values.Ambient[0] = 1.0f;
    //        props.Values.Ambient[1] = 1.0f;
    //        props.Values.Ambient[2] = 1.0f;

    //        props.Values.Specular0[0] = 0.5f;
    //        props.Values.Specular0[1] = 0.5f;
    //        props.Values.Specular0[2] = 0.5f;

    //        if (mat.values.find("baseColorFactor") != mat.values.end())
    //        {
    //            const auto& c = mat.values.at("baseColorFactor").ColorFactor();
    //            props.Values.Diffuse[0] = static_cast<float>(c[0]);
    //            props.Values.Diffuse[1] = static_cast<float>(c[1]);
    //            props.Values.Diffuse[2] = static_cast<float>(c[2]);
    //        }

    //        if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end())
    //        {
    //            const auto& e = mat.additionalValues.at("emissiveFactor").ColorFactor();
    //            props.Values.Emission[0] = static_cast<float>(e[0]);
    //            props.Values.Emission[1] = static_cast<float>(e[1]);
    //            props.Values.Emission[2] = static_cast<float>(e[2]);
    //        }

    //        // Associate texture if it exists
    //        Ref<Texture2D> diffuseMap = nullptr;
    //        if (mat.values.find("baseColorTexture") != mat.values.end())
    //        {
    //            int texIndex = mat.values.at("baseColorTexture").TextureIndex();
    //            if (texIndex >= 0 && texIndex < textures.size())
    //                diffuseMap = textures[texIndex];
    //        }

    //        props.Name = mat.name;
    //        props.DiffuseMap = diffuseMap;

    //        materials.push_back(Material::Create(props));
    //    }

    //    // --- Create Meshes ---
    //    uint32_t currentIndexOffset = 0;
    //    for (const auto& mesh : model.meshes)
    //    {
    //        for (const auto& prim : mesh.primitives)
    //        {
    //            Mesh m;
    //            m.indexCount = static_cast<uint32_t>(prim.indices >= 0 ? model.accessors[prim.indices].count : 0);
    //            m.indexOffset = currentIndexOffset;
    //            m.MaterialID = prim.material >= 0 ? prim.material : 0;
    //            meshes.push_back(m);

    //            currentIndexOffset += m.indexCount;
    //        }
    //    }
    //}
}