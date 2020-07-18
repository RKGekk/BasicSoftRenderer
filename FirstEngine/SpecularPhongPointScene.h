#pragma once

#include <DirectXMath.h>

#include "Scene.h"
#include "Wall.h"
#include "GraphicsClass.h"
#include "ZBuffer.h"
#include "SpecularPhongPointPipeline.h"
#include "ColorIntegers.h"
#include "IndexedTriangleList.h"
#include "Plane.h"

class SpecularPhongPointScene : public Scene {
	
public:

	struct Wall {
		const TextureClass*	pTex;
		IndexedTriangleList	model;
		DirectX::XMFLOAT4X4	world;
	};

	SpecularPhongPointScene(GraphicsClass& gfx) : pZb(std::make_shared<ZBuffer>(gfx.GetWidth(), gfx.GetHeight())), pipeline(gfx, pZb, gfx.GetTexture()->GetTexture("soft")), Scene("phong point shader scene free mesh") {

		// Set the initial position of the camera.
		m_Camera.SetPosition(0.0f, 0.0f, -1.0f);
		m_Camera.SetLens(hfov, aspect_ratio, 1.0f, 1000.0f);

		// load walls
		DirectX::XMFLOAT4X4 wallWorld;
		DirectX::XMMATRIX wallTrans = DirectX::XMMatrixTranslation(0.0f, 0.0f, width / 2.0f);
		DirectX::XMStoreFloat4x4(&wallWorld, wallTrans);
		walls.push_back({
			&gfx.GetTexture()->GetTexture("stone01.tga"),
			Plane::GetSkinnedNormals(20, 20, width, width, tScaleWall),
			wallWorld
		});
	}

	virtual void Update(float dt) override {
		t += dt;
	}

	virtual void Draw() override {

		pipeline.BeginFrame();

		DirectX::XMMATRIX view = m_Camera.GetViewMatrix();
		DirectX::XMVECTOR l_posXM = DirectX::XMLoadFloat4(&l_pos);
		DirectX::XMFLOAT4 lightPos(l_pos.x, l_pos.y, l_pos.z, 1.0f);
		DirectX::XMStoreFloat4(&lightPos, DirectX::XMVector4Transform(l_posXM, view));

		pipeline.effect.ps.SetLightPosition(DirectX::XMFLOAT3(lightPos.x, lightPos.y, lightPos.z));
		pipeline.effect.ps.SetAmbientLight(l_ambient);
		pipeline.effect.ps.SetDiffuseLight(l);
		for (const auto& w : walls) {
			DirectX::XMFLOAT4X4 worldView;

			DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&w.world);
			DirectX::XMStoreFloat4x4(&worldView, DirectX::XMMatrixMultiply(world, view));

			pipeline.effect.vs.BindWorldView(worldView);
			pipeline.effect.vs.BindProjection(m_Camera.GetProjectionMatix4x4());
			pipeline.effect.ps.BindTexture(*w.pTex);
			pipeline.Draw(w.model);
		}
	}

private:
	float t = 0.0f;

	// scene params
	static constexpr float width = 4.0f;
	static constexpr float height = 1.75f;

	// light
	DirectX::XMFLOAT4 l_pos		= { 0.0f,	0.0f,	0.0f,	1.0f	};
	DirectX::XMFLOAT3 l			= { 1.0f,	1.0f,	1.0f	};
	DirectX::XMFLOAT3 l_ambient	= { 0.35f,	0.35f,	0.35f	};

	// pipelines
	std::shared_ptr<ZBuffer> pZb;
	SpecularPhongPointPipeline pipeline;

	// fov
	static constexpr float aspect_ratio = 1.33333f;
	static constexpr float hfov = 85.0f;
	static constexpr float vfov = hfov / aspect_ratio;
	// camera stuff
	CameraClass m_Camera;
	
	// wall stuff
	static constexpr float tScaleCeiling = 0.5f;
	static constexpr float tScaleWall = 0.65f;
	static constexpr float tScaleFloor = 0.65f;
	
	std::vector<Wall> walls;

	// ripple stuff
	static constexpr float sauronSize = 0.6f;
};