
#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "stdafx.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// New files
// Material
// Texture <- stb

// TEMP


class Scene
{
protected:
	ID3D11Device*			dxdevice;
	ID3D11DeviceContext*	dxdevice_context;
	int						window_width;
	int						window_height;

public:

	Scene(
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context,
		int window_width,
		int window_height);

	virtual void Init() = 0;

	virtual void Update(
		float dt,
		InputHandler* input_handler) = 0;
	
	virtual void Render() = 0;
	
	virtual void Release() = 0;

	virtual void WindowResize(
		int window_width,
		int window_height);
};

class OurTestScene : public Scene
{
	//
	// Constant buffers (CBuffers) for data that is sent to shaders
	//

	// CBuffer for transformation matrices
	ID3D11Buffer* transformation_buffer = nullptr;
	ID3D11Buffer* light_Buffer = nullptr;
	ID3D11Buffer* phong_Buffer = nullptr;
	// + other CBuffers

	ID3D11SamplerState* samplerState = nullptr;

	// 
	// CBuffer client-side definitions
	// These must match the corresponding shader definitions 
	//

	struct TransformationBuffer
	{
		mat4f ModelToWorldMatrix;
		mat4f WorldToViewMatrix;
		mat4f ProjectionMatrix;
	};

	struct LightBuffer
	{
		vec4f lightPosition;
		vec4f cameraPosition;
	};

	struct alignas(16) PhongBuffer
	{
		vec4f ambientColor;
		vec4f diffuseColor;
		vec4f specularColor;
		alignas(16) float shininess;
	};

	D3D11_SAMPLER_DESC samplerDesc;

	//
	// Scene content
	//
	Camera* camera;

	vec4f lightPosition;

	QuadModel* quad;
	std::vector<Model*> h_models;
	OBJModel* sponza;
	OBJModel* trojan;

	// Model-to-world transformation matrices
	mat4f Msponza;
	std::vector<mat4f> Mh_models;
	mat4f Mquad;
	mat4f Mtrojan;

	// World-to-view matrix
	mat4f Mview;
	// Projection matrix
	mat4f Mproj;

	// Misc
	float time = 0;
	float angle = 0;			// A per-frame updated rotation angle (radians)...
	float angle_vel = fPI / 2;	// ...and its velocity (radians/sec)
	float camera_vel = 5.0f;	// Camera movement velocity in units/s
	float fps_cooldown = 0;

	void InitTransformationBuffer();

	void UpdateTransformationBuffer(
		mat4f ModelToWorldMatrix,
		mat4f WorldToViewMatrix,
		mat4f ProjectionMatrix);

	void InitLightBuffer();

	void UpdateLightBuffer(vec4f light, vec4f camera);

	void InitPhongBuffer();

	void UpdatePhongBuffer(vec4f ambient, vec4f diffuse, vec4f specular, float shininess);

public:
	OurTestScene(
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context,
		int window_width,
		int window_height);

	void Init() override;

	void AddModel(Model* toAdd)
	{
		h_models.push_back(toAdd);
		Mh_models.push_back(mat4f_zero);
	}

	void Update(
		float dt,
		InputHandler* input_handler) override;

	void Render() override;

	void Release() override;

	void WindowResize(
		int window_width,
		int window_height) override;
};

#endif