//
//  Model.cpp
//
//  CJ Gribel 2016, cjgribel@gmail.com
//

#include "Model.h"

QuadModel::QuadModel(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context)
	: Model(dxdevice, dxdevice_context)
{
	// Vertex and index arrays
	// Once their data is loaded to GPU buffers, they are not needed anymore
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	// Populate the vertex array with 4 vertices
	Vertex v0, v1, v2, v3;
	v0.Pos = { -0.5, -0.5f, 0.0f };
	v0.Normal = { 0, 0, 1 };
	v0.TexCoord = { 0, 0 };
	v1.Pos = { 0.5, -0.5f, 0.0f };
	v1.Normal = { 0, 0, 1 };
	v1.TexCoord = { 0, 1 };
	v2.Pos = { 0.5, 0.5f, 0.0f };
	v2.Normal = { 0, 0, 1 };
	v2.TexCoord = { 1, 1 };
	v3.Pos = { -0.5, 0.5f, 0.0f };
	v3.Normal = { 0, 0, 1 };
	v3.TexCoord = { 1, 0 };
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	// Populate the index array with two triangles
	// Triangle #1
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);
	// Triangle #2
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	// Vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0 };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = (UINT)(vertices.size()*sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &vertices[0];
	// Create vertex buffer on device using descriptor & data
	const HRESULT vhr = dxdevice->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);
	SETNAME(vertex_buffer, "VertexBuffer");
    
	//  Index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0 };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = (UINT)(indices.size()*sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	const HRESULT ihr = dxdevice->CreateBuffer(&ibufferDesc, &idata, &index_buffer);
	SETNAME(index_buffer, "IndexBuffer");
    
	nbr_indices = (unsigned int)indices.size();
	material = new Material();
}


void QuadModel::Render(std::function<void(vec4f, vec4f, vec4f, float)> phongBufferUpdate) const
{
	// Bind our vertex buffer
	const UINT32 stride = sizeof(Vertex); //  sizeof(float) * 8;
	const UINT32 offset = 0;
	dxdevice_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Bind our index buffer
	dxdevice_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	if (material)
	{
		if (phongBufferUpdate)
			phongBufferUpdate(material->Ka.xyz1(), material->Kd.xyz1(), material->Ks.xyz1(), 200);

		if (material->diffuse_texture && material->diffuse_texture.texture_SRV)
			dxdevice_context->PSSetShaderResources(0, 1, &material->diffuse_texture.texture_SRV);
	}

	// Make the drawcall
	dxdevice_context->DrawIndexed(nbr_indices, 0, 0);
}


CubeModel::CubeModel(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context)	: Model(dxdevice, dxdevice_context)
{
	// Vertex and index arrays
	// Once their data is loaded to GPU buffers, they are not needed anymore
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	// Populate the vertex array with 24 vertices
	Vertex vbbl0, vbbl1, vbbl2, vbbr0, vbbr1, vbbr2, vtbl0, vtbl1, vtbl2, vtbr0, vtbr1, vtbr2, vbfl0, vbfl1, vbfl2, vbfr0, vbfr1, vbfr2, vtfl0, vtfl1, vtfl2, vtfr0, vtfr1, vtfr2;
	// Bottom Back Left:
	vbbl0.Pos = vbbl1.Pos = vbbl2.Pos = { -0.5f, -0.5f, -0.5f };
	vbbl0.Normal = { 0, 0, -1 };
	vbbl0.TexCoord = { 0, 1 };

	vbbl1.Normal = { -1, 0, 0 };
	vbbl1.TexCoord = { 0, 0 };

	vbbl2.Normal = { 0, -1, 0 };
	vbbl2.TexCoord = { 1, 1 };

	// Bottom Back Right:
	vbbr0.Pos = vbbr1.Pos = vbbr2.Pos = { 0.5f, -0.5f, -0.5f };
	vbbr0.Normal = { 0, 0, -1 };
	vbbr0.TexCoord = { 0, 0 };

	vbbr1.Normal = { 1, 0, 0 };
	vbbr1.TexCoord = { 0, 1 };

	vbbr2.Normal = { 0, -1, 0 };
	vbbr2.TexCoord = { 1, 0 };

	// Top Back Left:
	vtbl0.Pos = vtbl1.Pos = vtbl2.Pos = { -0.5f, 0.5f, -0.5f };
	vtbl0.Normal = { 0, 0, -1 };
	vtbl0.TexCoord = { 1, 1 };

	vtbl1.Normal = { -1, 0, 0 };
	vtbl1.TexCoord = { 1, 0 };

	vtbl2.Normal = { 0, 1, 0 };
	vtbl2.TexCoord = { 1, 0 };

	// Top Back Right:
	vtbr0.Pos = vtbr1.Pos = vtbr2.Pos = { 0.5f, 0.5f, -0.5f };
	vtbr0.Normal = { 0, 0, -1 };
	vtbr0.TexCoord = { 1, 0 };

	vtbr1.Normal = { 1, 0, 0 };
	vtbr1.TexCoord = { 1, 1 };

	vtbr2.Normal = { 0, 1, 0 };
	vtbr2.TexCoord = { 1, 1 };

	// Bottom Front Left:
	vbfl0.Pos = vbfl1.Pos = vbfl2.Pos = { -0.5f, -0.5f, 0.5f };
	vbfl0.Normal = { 0, 0, 1 };
	vbfl0.TexCoord = { 0, 0 };

	vbfl1.Normal = { -1, 0, 0 };
	vbfl1.TexCoord = { 0, 1 };

	vbfl2.Normal = { 0, -1, 0 };
	vbfl2.TexCoord = { 0, 1 };

	// Bottom Front Right:
	vbfr0.Pos = vbfr1.Pos = vbfr2.Pos = { 0.5f, -0.5f, 0.5f };
	vbfr0.Normal = { 0, 0, 1 };
	vbfr0.TexCoord = { 0, 1 };

	vbfr1.Normal = { 1, 0, 0 };
	vbfr1.TexCoord = { 0, 0 };

	vbfr2.Normal = { 0, -1, 0 };
	vbfr2.TexCoord = { 0, 0 };

	// Top Front Left:
	vtfl0.Pos = vtfl1.Pos = vtfl2.Pos = { -0.5f, 0.5f, 0.5f };
	vtfl0.Normal = { 0, 0, 1 };
	vtfl0.TexCoord = { 1, 0 };

	vtfl1.Normal = { -1, 0, 0 };
	vtfl1.TexCoord = { 1, 1 };

	vtfl2.Normal = { 0, 1, 0 };
	vtfl2.TexCoord = { 0, 0 };

	// Top Front Right:
	vtfr0.Pos = vtfr1.Pos = vtfr2.Pos = { 0.5f, 0.5f, 0.5f };
	vtfr0.Normal = { 0, 0, 1 };
	vtfr0.TexCoord = { 1, 1 };

	vtfr1.Normal = { 1, 0, 0 };
	vtfr1.TexCoord = { 1, 0 };

	vtfr2.Normal = { 0, 1, 0 };
	vtfr2.TexCoord = { 0, 1 };

	vertices.push_back(vbbl0); // 0.
	vertices.push_back(vbbl1); // 1.
	vertices.push_back(vbbl2); // 2.

	vertices.push_back(vbbr0); // 3.
	vertices.push_back(vbbr1); // 4.
	vertices.push_back(vbbr2); // 5.

	vertices.push_back(vtbl0); // 6.
	vertices.push_back(vtbl1); // 7.
	vertices.push_back(vtbl2); // 8.

	vertices.push_back(vtbr0); // 9.
	vertices.push_back(vtbr1); // 10.
	vertices.push_back(vtbr2); // 11.

	vertices.push_back(vbfl0); // 12.
	vertices.push_back(vbfl1); // 13.
	vertices.push_back(vbfl2); // 14.

	vertices.push_back(vbfr0); // 15.
	vertices.push_back(vbfr1); // 16.
	vertices.push_back(vbfr2); // 17.

	vertices.push_back(vtfl0); // 18.
	vertices.push_back(vtfl1); // 19.
	vertices.push_back(vtfl2); // 20.

	vertices.push_back(vtfr0); // 21.
	vertices.push_back(vtfr1); // 22.
	vertices.push_back(vtfr2); // 23.

	// Populate the index array with 12 triangles (2 for each side)
	// Triangle #1
	indices.push_back(3);
	indices.push_back(0);
	indices.push_back(6);
	// Triangle #2
	indices.push_back(3);
	indices.push_back(6);
	indices.push_back(9);
	// Triangle #3
	indices.push_back(14);
	indices.push_back(2);
	indices.push_back(17);
	// Triangle #4
	indices.push_back(17);
	indices.push_back(2);
	indices.push_back(5);
	// Triangle #5
	indices.push_back(15);
	indices.push_back(21);
	indices.push_back(18);
	// Triangle #6
	indices.push_back(15);
	indices.push_back(18);
	indices.push_back(12);
	// Triangle #7
	indices.push_back(8);
	indices.push_back(20);
	indices.push_back(11);
	// Triangle #8
	indices.push_back(11);
	indices.push_back(20);
	indices.push_back(23);
	// Triangle #9
	indices.push_back(16);
	indices.push_back(4);
	indices.push_back(10);
	// Triangle #10
	indices.push_back(16);
	indices.push_back(10);
	indices.push_back(22);
	// Triangle #11
	indices.push_back(13);
	indices.push_back(19);
	indices.push_back(7);
	// Triangle #12
	indices.push_back(13);
	indices.push_back(7);
	indices.push_back(1);

	// Vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0 };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = (UINT)(vertices.size() * sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &vertices[0];
	// Create vertex buffer on device using descriptor & data
	const HRESULT vhr = dxdevice->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);
	SETNAME(vertex_buffer, "VertexBuffer");

	//  Index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0 };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	const HRESULT ihr = dxdevice->CreateBuffer(&ibufferDesc, &idata, &index_buffer);
	SETNAME(index_buffer, "IndexBuffer");

	nbr_indices = (unsigned int)indices.size();
	material = new Material();
}

void CubeModel::Render(std::function<void(vec4f, vec4f, vec4f, float)> phongBufferUpdate) const
{
	// Bind our vertex buffer
	const UINT32 stride = sizeof(Vertex); //  sizeof(float) * 8;
	const UINT32 offset = 0;
	dxdevice_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Bind our index buffer
	dxdevice_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	if (material)
	{
		if (phongBufferUpdate)
			phongBufferUpdate(material->Ka.xyz1(), material->Kd.xyz1(), material->Ks.xyz1(), 200);

		if (material->diffuse_texture && material->diffuse_texture.texture_SRV)
			dxdevice_context->PSSetShaderResources(0, 1, &material->diffuse_texture.texture_SRV);
	}

	// Make the drawcall
	dxdevice_context->DrawIndexed(nbr_indices, 0, 0);
}


OBJModel::OBJModel(
	const std::string& objfile,
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context)
	: Model(dxdevice, dxdevice_context)
{
	// Load the OBJ
	OBJLoader* mesh = new OBJLoader();
	mesh->Load(objfile);

	// Load and organize indices in ranges per drawcall (material)

	std::vector<unsigned> indices;
	unsigned int i_ofs = 0;

	for (auto& dc : mesh->drawcalls)
	{
		// Append the drawcall indices
		for (auto& tri : dc.tris)
			indices.insert(indices.end(), tri.vi, tri.vi + 3);

		// Create a range
		unsigned int i_size = (unsigned int)dc.tris.size() * 3;
		int mtl_index = dc.mtl_index > -1 ? dc.mtl_index : -1;
		index_ranges.push_back({ i_ofs, i_size, 0, mtl_index });

		i_ofs = (unsigned int)indices.size();
	}

	// Vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0 };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = (UINT)(mesh->vertices.size()*sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &(mesh->vertices)[0];
	// Create vertex buffer on device using descriptor & data
	HRESULT vhr = dxdevice->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);
	SETNAME(vertex_buffer, "VertexBuffer");
    
	// Index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0 };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = (UINT)(indices.size()*sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	HRESULT ihr = dxdevice->CreateBuffer(&ibufferDesc, &idata, &index_buffer);
	SETNAME(index_buffer, "IndexBuffer");
    
	// Copy materials from mesh
	append_materials(mesh->materials);

	// Go through materials and load textures (if any) to device
	std::cout << "Loading textures..." << std::endl;
	for (auto& mtl : materials)
	{
		HRESULT hr;

		// Load Diffuse texture
		//
		if (mtl.Kd_texture_filename.size()) {

			hr = LoadTextureFromFile(
				dxdevice,
				mtl.Kd_texture_filename.c_str(), 
				&mtl.diffuse_texture);
			std::cout << "\t" << mtl.Kd_texture_filename 
				<< (SUCCEEDED(hr) ? " - OK" : "- FAILED") << std::endl;
		}

		// + other texture types here - see Material class
		// ...
	}
	std::cout << "Done." << std::endl;

	SAFE_DELETE(mesh);
}


void OBJModel::Render(std::function<void(vec4f, vec4f, vec4f, float)> phongBufferUpdate) const
{
	// Bind vertex buffer
	const UINT32 stride = sizeof(Vertex);
	const UINT32 offset = 0;
	dxdevice_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Bind index buffer
	dxdevice_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Iterate drawcalls
	for (auto& irange : index_ranges)
	{
		// Fetch material
		const Material& mtl = materials[irange.mtl_index];

		if (phongBufferUpdate)
			phongBufferUpdate(mtl.Ka.xyz1(), mtl.Kd.xyz1(), mtl.Ks.xyz1(), 5);

		// Bind diffuse texture to slot t0 of the PS
		dxdevice_context->PSSetShaderResources(0, 1, &mtl.diffuse_texture.texture_SRV);
		// + bind other textures here, e.g. a normal map, to appropriate slots

		// Make the drawcall
		dxdevice_context->DrawIndexed(irange.size, irange.start, 0);
	}
}

OBJModel::~OBJModel()
{
	for (auto& material : materials)
	{
		SAFE_RELEASE(material.diffuse_texture.texture_SRV);

		// Release other used textures ...
	}
}