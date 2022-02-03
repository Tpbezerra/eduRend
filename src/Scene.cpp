#include "Scene.h"

//#define Trojan
//#define Cubes
//#define Sponza
//#define Sphere;

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	dxdevice(dxdevice),
	dxdevice_context(dxdevice_context),
	window_width(window_width),
	window_height(window_height)
{ }

void Scene::WindowResize(
	int window_width,
	int window_height)
{
	this->window_width = window_width;
	this->window_height = window_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{ 
	InitTransformationBuffer();
	InitLightBuffer();
	InitPhongBuffer();
	// + init other CBuffers

	HRESULT hr;
	samplerDesc =
	{
		D3D11_FILTER_ANISOTROPIC,		// Filter.
		D3D11_TEXTURE_ADDRESS_WRAP,		// AddressU.
		D3D11_TEXTURE_ADDRESS_WRAP,		// AddressV.
		D3D11_TEXTURE_ADDRESS_WRAP,		// AddressW.
		0.0f,							// MipLODBias
		16,								// Max Anisotropy
		D3D11_COMPARISON_NEVER,			// Comparison Function
		{ 1.0f, 1.0f, 1.0f, 1.0f },		// Border Colour
		-FLT_MAX,						// Min LOD
		FLT_MAX							// Max LOD
	};
	ASSERT(hr = dxdevice->CreateSamplerState(&samplerDesc, &samplerState));
	dxdevice_context->PSSetSamplers(0, 1, &samplerState);
}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)window_width / window_height,	// aspect ratio
		0.1f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	camera->moveTo({ 0, 0, 5 });

	// Create objects
	Material mat;
	mat.Ka = vec3f(0.1f, 0.1f, 0.1f);
	mat.Kd = vec3f(1.0f, 1.0f, 1.0f);
	mat.Ks = vec3f(0.3f, 0.3f, 0.3f);
	mat.Kd_texture_filename = "textures/yroadcrossing.png";
	LoadTextureFromFile(dxdevice, mat.Kd_texture_filename.c_str(), &mat.diffuse_texture);

	quad = new QuadModel(dxdevice, dxdevice_context);
	quad->SetMaterial(mat);

#ifdef Trojan
	trojan = new OBJModel("Trojan/Trojan.obj", dxdevice, dxdevice_context);
#endif // Trojan

#ifdef Cubes
	mat.Ka = vec3f(0.1f, 0.1f, 0.1f);
	mat.Kd = vec3f(1.0f, 1.0f, 1.0f);
	mat.Ks = vec3f(1.0f, 1.0f, 1.0f);
	mat.Kd_texture_filename = "textures/crate.png";
	LoadTextureFromFile(dxdevice, mat.Kd_texture_filename.c_str(), &mat.diffuse_texture);

	AddModel(new CubeModel(dxdevice, dxdevice_context));
	((CubeModel*)h_models[0])->SetMaterial(mat);
	AddModel(new CubeModel(dxdevice, dxdevice_context));
	((CubeModel*)h_models[1])->SetMaterial(mat);
	AddModel(new CubeModel(dxdevice, dxdevice_context));
	((CubeModel*)h_models[2])->SetMaterial(mat);
#endif // !Trojan

#ifdef Sponza
	sponza = new OBJModel("crytek-sponza/sponza.obj", dxdevice, dxdevice_context);
#endif // Sponza

#ifdef Sphere
	AddModel(new OBJModel("sphere/sphere.obj", dxdevice, dxdevice_context));
#endif // Sphere
}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	InputHandler* input_handler)
{
	// Basic camera control
	if (input_handler->IsKeyPressed(Keys::Up) || input_handler->IsKeyPressed(Keys::W))
		camera->move({ 0.0f, 0.0f, -camera_vel * dt });
	if (input_handler->IsKeyPressed(Keys::Down) || input_handler->IsKeyPressed(Keys::S))
		camera->move({ 0.0f, 0.0f, camera_vel * dt });
	if (input_handler->IsKeyPressed(Keys::Right) || input_handler->IsKeyPressed(Keys::D))
		camera->move({ camera_vel * dt, 0.0f, 0.0f });
	if (input_handler->IsKeyPressed(Keys::Left) || input_handler->IsKeyPressed(Keys::A))
		camera->move({ -camera_vel * dt, 0.0f, 0.0f });
	if (input_handler->IsKeyPressed(Keys::Space))
		camera->move({ 0.0f, camera_vel * dt, 0.0f });
	if (input_handler->IsKeyPressed(Keys::Ctrl))
		camera->move({ 0.0f, -camera_vel * dt, 0.0f });

	if (input_handler->IsKeyPressed(Keys::One))
	{
		HRESULT hr;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		ASSERT(hr = dxdevice->CreateSamplerState(&samplerDesc, &samplerState));
		dxdevice_context->PSSetSamplers(0, 1, &samplerState);
	}
	if (input_handler->IsKeyPressed(Keys::Two))
	{
		HRESULT hr;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		ASSERT(hr = dxdevice->CreateSamplerState(&samplerDesc, &samplerState));
		dxdevice_context->PSSetSamplers(0, 1, &samplerState);
	}
	if (input_handler->IsKeyPressed(Keys::Three))
	{
		HRESULT hr;
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		ASSERT(hr = dxdevice->CreateSamplerState(&samplerDesc, &samplerState));
		dxdevice_context->PSSetSamplers(0, 1, &samplerState);
	}

	float sensitivity = 0.1f;

	long mouseX = input_handler->GetMouseDeltaX();
	long mouseY = input_handler->GetMouseDeltaY();

	camera->rotate(vec3f(-mouseY, -mouseX, 0) * sensitivity * dt);

	lightPosition = vec4f(std::cos(time) * 10, 2, std::sin(time) * 10, 1);
	lightPosition = vec4f(0, 100, 0, 1);

	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	// Quad model-to-world transformation
	Mquad = mat4f::translation(0, -6, -5) *			// No translation
		mat4f::rotation(-PI / 2, 1.0f, 0.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(100.0, 100.0, 100.0);				// Scale uniformly to 150%

#ifdef Trojan
	Mtrojan = mat4f::translation(0, -1, -15) *
		mat4f::rotation(0, 0, 1, 0) *
		mat4f::scaling(0.5, 0.5, 0.5);
#endif // Trojan

#ifdef Cubes
	// Cubes position update.
	float speed = 2;
	h_models[0]->position = vec3f(std::cos(time * speed) * 0, 0, -10);
	h_models[0]->eulerAngles = vec3f(0, 0, -angle * 0.2f);
	h_models[0]->scale = vec3f(1, 1, 1);

	speed = -0.5f;
	h_models[1]->position = vec3f(std::cos(time * speed), std::sin(time * speed), 0) * 4;
	h_models[1]->eulerAngles = vec3f(0, 0, -angle * 0.5f);
	h_models[1]->scale = vec3f(0.7f, 0.7f, 0.7f);

	speed = 2;
	h_models[2]->position = vec3f(std::cos(time * speed), std::sin(time * speed), 0) * 2;
	h_models[2]->eulerAngles = vec3f(0, 0, angle * 0.8f);
	h_models[2]->scale = vec3f(0.3f, 0.3f, 0.3f);

	// Cube model-to-world transformation.
	mat4f currentTranslation;
	mat4f currentRotation;

	mat4f hierarchicalTranslation = mat4f_identity;
	mat4f hierarchicalRotation = mat4f_identity;

	for (size_t i = 0; i < h_models.size(); ++i)
	{
		currentTranslation = mat4f::translation(h_models[i]->position);
		currentRotation = mat4f::rotation(h_models[i]->eulerAngles.x, 1, 0, 0) *
			mat4f::rotation(h_models[i]->eulerAngles.y, 0, 1, 0) *
			mat4f::rotation(h_models[i]->eulerAngles.z, 0, 0, 1);

		Mh_models[i] = hierarchicalTranslation * currentTranslation *
			hierarchicalRotation * currentRotation *
			mat4f::scaling(h_models[i]->scale);

		hierarchicalTranslation = hierarchicalTranslation * currentTranslation;
		hierarchicalRotation = hierarchicalRotation * currentRotation;
	}
#endif // !Trojan

#ifdef Sponza
	// Sponza model-to-world transformation
	Msponza = mat4f::translation(0, -2, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%
#endif // Sponza

#ifdef Sphere
	h_models[0]->position = vec3f(0, 0, -10);
	h_models[0]->eulerAngles = vec3f(0, 0, 0);
	h_models[0]->scale = vec3f(1, 1, 1);

	mat4f rotation = mat4f::rotation(h_models[0]->eulerAngles.x, 1, 0, 0) *
		mat4f::rotation(h_models[0]->eulerAngles.y, 0, 1, 0) *
		mat4f::rotation(h_models[0]->eulerAngles.z, 0, 0, 1);

	Mh_models[0] = mat4f::translation(h_models[0]->position) * rotation * mat4f::scaling(h_models[0]->scale);
#endif // Sphere

	// Increase the amount of time passed.
	time += dt;
	// Increment the rotation angle.
	angle += angle_vel * dt;

	// Print fps
	fps_cooldown -= dt;
	if (fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
//		printf("fps %i\n", (int)(1.0f / dt));
		fps_cooldown = 2.0;
	}
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	dxdevice_context->VSSetConstantBuffers(0, 1, &transformation_buffer);
	// Bind light_Buffer to slot b0 of the PS
	dxdevice_context->PSSetConstantBuffers(0, 1, &light_Buffer);
	dxdevice_context->PSSetConstantBuffers(1, 1, &phong_Buffer);

	// Obtain the matrices needed for rendering from the camera
	Mview = camera->get_WorldToViewMatrix();
	Mproj = camera->get_ProjectionMatrix();

	UpdateLightBuffer(lightPosition, camera->position.xyz1());

	auto phongLambda = [this](vec4f ka, vec4f kd, vec4f ks, float s) { UpdatePhongBuffer(ka, kd, ks, s); };

	// Load matrices + the Quad's transformation to the device and render it
	UpdateTransformationBuffer(Mquad, Mview, Mproj);
	quad->Render(phongLambda);

#ifdef Trojan
	// Load matricies + Trojan's transformation to the device and render it
	UpdateTransformationBuffer(Mtrojan, Mview, Mproj);
	UpdatePhongBuffer(vec4f(0.0f, 0.0f, 0.3f, 1), vec4f(0.8f, 0.0f, 0.8f, 1), vec4f(1.0f, 0.5f, 1.0f, 1.0f), 0.5f);
	trojan->Render();
#endif // Trojan

#ifdef Cubes
	// Load matricies + the Cube's transformation to the device and reander it
	for (size_t i = 0; i < h_models.size(); ++i)
	{
		UpdateTransformationBuffer(Mh_models[i], Mview, Mproj);
		h_models[i]->Render(phongLambda);
	}
#endif // !Trojan

#ifdef Sponza
	// Load matrices + Sponza's transformation to the device and render it
	UpdateTransformationBuffer(Msponza, Mview, Mproj);
	sponza->Render(phongLambda);
#endif // Sponza

#ifdef Sphere
	UpdateTransformationBuffer(Mh_models[0], Mview, Mproj);
	UpdatePhongBuffer(vec4f(0.0f, 0.0f, 0.3f, 1), vec4f(0.8f, 0.0f, 0.8f, 1), vec4f(1.0f, 0.5f, 1.0f, 1.0f), 200);
	h_models[0]->Render();
#endif // Sphere
}

void OurTestScene::Release()
{
	SAFE_DELETE(quad);
	SAFE_DELETE(sponza);
	SAFE_DELETE(camera);

	SAFE_RELEASE(transformation_buffer);
	SAFE_RELEASE(light_Buffer);
	SAFE_RELEASE(phong_Buffer);
	// + release other CBuffers
}

void OurTestScene::WindowResize(int window_width, int window_height)
{
	if (camera)
		camera->aspect = float(window_width) / window_height;

	Scene::WindowResize(window_width, window_height);
}

void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
	MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBuffer_desc.ByteWidth = sizeof(TransformationBuffer);
	MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBuffer_desc.MiscFlags = 0;
	MatrixBuffer_desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&MatrixBuffer_desc, nullptr, &transformation_buffer));
}

void OurTestScene::UpdateTransformationBuffer(mat4f ModelToWorldMatrix, mat4f WorldToViewMatrix, mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrix_buffer_ = (TransformationBuffer*)resource.pData;
	matrix_buffer_->ModelToWorldMatrix = ModelToWorldMatrix;
	matrix_buffer_->WorldToViewMatrix = WorldToViewMatrix;
	matrix_buffer_->ProjectionMatrix = ProjectionMatrix;
	dxdevice_context->Unmap(transformation_buffer, 0);
}

void OurTestScene::InitLightBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC positionBuffer_Desc = { 0 };
	positionBuffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	positionBuffer_Desc.ByteWidth = sizeof(LightBuffer);
	positionBuffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	positionBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	positionBuffer_Desc.MiscFlags = 0;
	positionBuffer_Desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&positionBuffer_Desc, nullptr, &light_Buffer));
}

void OurTestScene::UpdateLightBuffer(vec4f lightPosition, vec4f cameraPosition)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(light_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	LightBuffer* positionBuffer = (LightBuffer*)resource.pData;
	positionBuffer->lightPosition = lightPosition;
	positionBuffer->cameraPosition = cameraPosition;
	dxdevice_context->Unmap(light_Buffer, 0);
}

void OurTestScene::InitPhongBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC phongBuffer_Desc = { 0 };
	phongBuffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	phongBuffer_Desc.ByteWidth = sizeof(PhongBuffer);
	phongBuffer_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	phongBuffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	phongBuffer_Desc.MiscFlags = 0;
	phongBuffer_Desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&phongBuffer_Desc, nullptr, &phong_Buffer));
}

void OurTestScene::UpdatePhongBuffer(vec4f ambient, vec4f diffuse, vec4f specular, float shininess)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(phong_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	PhongBuffer* materialBuffer = (PhongBuffer*)resource.pData;
	materialBuffer->ambientColor = ambient;
	materialBuffer->diffuseColor = diffuse;
	materialBuffer->specularColor = specular;
	materialBuffer->shininess = shininess;
	dxdevice_context->Unmap(phong_Buffer, 0);
}