#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "Vertex.h"
#include "tiny_obj_loader.h"

class IndexedTriangleList {
public:

	IndexedTriangleList(std::vector<Vertex> verts_in, std::vector<size_t> indices_in) : vertices(std::move(verts_in)), indices(std::move(indices_in)) {}

	IndexedTriangleList() = default;


	static IndexedTriangleList Load(const std::string& filename) {
		IndexedTriangleList tl;

		// check first line of file to see if CCW winding comment exists
		bool isCCW = false;
		{
			std::ifstream file(filename);
			std::string firstline;
			std::getline(file, firstline);
			std::transform(firstline.begin(), firstline.end(), firstline.begin(), std::tolower);
			if (firstline.find("ccw") != std::string::npos) {
				isCCW = true;
			}
		}

		// these will be filled by obj loading function
		using namespace tinyobj;
		attrib_t attrib;
		std::vector<shape_t> shapes;
		std::string err;

		// load/parse the obj file
		const bool ret = LoadObj(&attrib, &shapes, nullptr, &err, filename.c_str());

		// check for errors
		if (!err.empty() && err.substr(0, 4) != "WARN") {
			throw std::runtime_error(("LoadObj returned error:" + err + " File:" + filename).c_str());
		}
		if (!ret) {
			throw std::runtime_error(("LoadObj returned false  File:" + filename).c_str());
		}
		if (shapes.size() == 0u) {
			throw std::runtime_error(("LoadObj object file had no shapes  File:" + filename).c_str());
		}

		// extract vertex data
		// attrib.vertices is a flat std::vector of floats corresponding
		// to vertex positions, laid out as xyzxyzxyz... etc.
		// first preallocate required space in OUR std::vector
		tl.vertices.reserve(attrib.vertices.size() / 3u);
		// iterate over individual vertices, construct Vec3s in OUR vector
		for (int i = 0; i < attrib.vertices.size(); i += 3) {
			tl.vertices.emplace_back(Vertex(DirectX::XMFLOAT4(attrib.vertices[i + 0], attrib.vertices[i + 1], attrib.vertices[i + 2], 1.0f)));
		}

		// extract index data
		// obj file can contain multiple meshes, we assume just 1
		const auto& mesh = shapes[0].mesh;
		// mesh contains a std::vector of num_face_vertices (uchar)
		// and a flat std::vector of indices. If all faces are triangles
		// then for any face f, the first index of that faces is [f * 3n]
		tl.indices.reserve(mesh.indices.size());
		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
			// make sure there are no non-triangle faces
			if (mesh.num_face_vertices[f] != 3u) {
				std::stringstream ss;
				ss << "LoadObj error face #" << f << " has "
					<< mesh.num_face_vertices[f] << " vertices";
				throw std::runtime_error(ss.str().c_str());
			}

			// load set of 3 indices for each face into OUR index std::vector
			for (size_t vn = 0; vn < 3u; vn++) {
				const auto idx = mesh.indices[f * 3u + vn];
				tl.indices.push_back(size_t(idx.vertex_index));
			}

			// reverse winding if file marked as CCW
			if (isCCW) {
				// swapping any two indices reverse the winding dir of triangle
				std::swap(tl.indices.back(), *std::prev(tl.indices.end(), 2));
			}
		}

		return tl;
	}

	static IndexedTriangleList LoadNormals(const std::string& filename) {
		IndexedTriangleList tl;

		// check first line of file to see if CCW winding comment exists
		bool isCCW = false;
		{
			std::ifstream file(filename);
			std::string firstline;
			std::getline(file, firstline);
			std::transform(firstline.begin(), firstline.end(), firstline.begin(), std::tolower);
			if (firstline.find("ccw") != std::string::npos) {
				isCCW = true;
			}
		}

		// these will be filled by obj loading function
		using namespace tinyobj;
		attrib_t attrib;
		std::vector<shape_t> shapes;
		std::string err;

		// load/parse the obj file
		const bool ret = LoadObj(&attrib, &shapes, nullptr, &err, filename.c_str());

		// check for errors
		if (!err.empty() && err.substr(0, 4) != "WARN") {
			throw std::runtime_error(("LoadObj returned error:" + err + " File:" + filename).c_str());
		}
		if (!ret) {
			throw std::runtime_error(("LoadObj returned false  File:" + filename).c_str());
		}
		if (shapes.size() == 0u) {
			throw std::runtime_error(("LoadObj object file had no shapes  File:" + filename).c_str());
		}

		// extract vertex data
		// attrib.vertices is a flat std::vector of floats corresponding
		// to vertex positions, laid out as xyzxyzxyz... etc.
		// first preallocate required space in OUR std::vector
		tl.vertices.reserve(attrib.vertices.size() / 3u);
		// iterate over individual vertices, construct Vec3s in OUR vector
		for (int i = 0; i < attrib.vertices.size(); i += 3) {
			tl.vertices.emplace_back(
				Vertex(
					DirectX::XMFLOAT4(
						attrib.vertices[i + 0],
						attrib.vertices[i + 1],
						attrib.vertices[i + 2],
						1.0f
					)
				)
			);
		}

		// extract index data
		// obj file can contain multiple meshes, we assume just 1
		const auto& mesh = shapes[0].mesh;
		// mesh contains a std::vector of num_face_vertices (uchar)
		// and a flat std::vector of indices. If all faces are triangles
		// then for any face f, the first index of that faces is [f * 3n]
		tl.indices.reserve(mesh.indices.size());
		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
			// make sure there are no non-triangle faces
			if (mesh.num_face_vertices[f] != 3u) {
				std::stringstream ss;
				ss << "LoadObj error face #" << f << " has " << mesh.num_face_vertices[f] << " vertices";
				throw std::runtime_error(ss.str().c_str());
			}

			// load set of 3 indices for each face into OUR index std::vector
			for (size_t vn = 0; vn < 3u; vn++) {
				const auto idx = mesh.indices[f * 3u + vn];
				tl.indices.push_back(size_t(idx.vertex_index));
				// write normals into the vertices
				tl.vertices[(size_t)idx.vertex_index].n = DirectX::XMFLOAT3(
					attrib.normals[3 * idx.normal_index + 0],
					attrib.normals[3 * idx.normal_index + 1],
					attrib.normals[3 * idx.normal_index + 2]
				);
			}

			// reverse winding if file marked as CCW
			if (isCCW) {
				// swapping any two indices reverse the winding dir of triangle
				std::swap(tl.indices.back(), *std::prev(tl.indices.end(), 2));
			}
		}

		return tl;
	}

	std::vector<Vertex> vertices;
	std::vector<size_t> indices;
};