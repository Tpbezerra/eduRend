
//
//  Model.h
//
//  Carl Johan Gribel 2016, cjgribel@gmail.com
//

#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "stdafx.h"
#include <vector>
#include "vec\vec.h"
#include "vec\mat.h"
#include "ShaderBuffers.h"
#include "Drawcall.h"
#include "OBJLoader.h"
#include "Texture.h"
#include <functional>

using namespace linalg;

class Model
{
protected:
	// Pointers to the current device and device context
	ID3D11Device* const			dxdevice;
	ID3D11DeviceContext* const	dxdevice_context;

	// Pointers to the class' vertex & index arrays
	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* index_buffer = nullptr;

public:
	// Transformation values
	vec3f position = vec3f_zero;
	vec3f eulerAngles = vec3f_zero;
	vec3f scale = vec3f_zero;

	Model(
		ID3D11Device* dxdevice, 
		ID3D11DeviceContext* dxdevice_context) 
		:	dxdevice(dxdevice),
			dxdevice_context(dxdevice_context)
	{ }

	//
	// Abstract render method: must be implemented by derived classes
	//
	virtual void Render(std::function<void(vec4f, vec4f, vec4f, float)> phongBufferUpdate = nullptr) const = 0;

	//
	// Destructor
	//
	virtual ~Model()
	{ 
		SAFE_RELEASE(vertex_buffer);
		SAFE_RELEASE(index_buffer);
	}
};

class QuadModel : public Model
{
	unsigned nbr_indices = 0;

	Material* material;

public:

	QuadModel(
		ID3D11Device* dx3ddevice,
		ID3D11DeviceContext* dx3ddevice_context);

	void SetMaterial(const Material& mat) { *material = mat; }

	virtual void Render(std::function<void(vec4f, vec4f, vec4f, float)> phongBufferUpdate = nullptr) const;

	~QuadModel() { }
};

class CubeModel : public Model
{
	unsigned nbr_indices = 0;

	Material* material;

public:

	CubeModel(
		ID3D11Device* dx3ddevice,
		ID3D11DeviceContext* dx3ddevice_context);

	void SetMaterial(const Material& mat) { *material = mat; }

	void Render(std::function<void(vec4f, vec4f, vec4f, float)> phongBufferUpdate = nullptr) const;

	~CubeModel() { }
};

class OBJModel : public Model
{
	// index ranges, representing drawcalls, within an index array
	struct IndexRange
	{
		unsigned int start;
		unsigned int size;
		unsigned ofs;
		int mtl_index;
	};

	std::vector<IndexRange> index_ranges;
	std::vector<Material> materials;

	void append_materials(const std::vector<Material>& mtl_vec)
	{
		materials.insert(materials.end(), mtl_vec.begin(), mtl_vec.end());
	}

public:

	OBJModel(
		const std::string& objfile,
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context);

	virtual void Render(std::function<void(vec4f, vec4f, vec4f, float)> phongBufferUpdate = nullptr) const;

	~OBJModel();
};

#endif