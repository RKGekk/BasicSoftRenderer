#pragma once

#include <DirectXMath.h>

class CameraClass {
public:
	CameraClass();
	CameraClass(const CameraClass&) = delete;
	CameraClass& operator=(const CameraClass&) = delete;
	~CameraClass();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	DirectX::XMFLOAT3 GetPosition3f() const;
	DirectX::XMVECTOR GetPosition() const;
	DirectX::XMFLOAT3 GetRotation3f() const;
	DirectX::XMVECTOR GetRotation() const;

	DirectX::XMVECTOR GetRight()const;
	DirectX::XMFLOAT3 GetRight3f()const;
	DirectX::XMVECTOR GetUp()const;
	DirectX::XMFLOAT3 GetUp3f()const;
	DirectX::XMVECTOR GetLook()const;
	DirectX::XMFLOAT3 GetLook3f()const;

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// Get View/Proj matrices.
	DirectX::XMFLOAT4X4 GetViewMatrix4x4() const;
	DirectX::XMMATRIX GetViewMatrix()const;

	DirectX::XMMATRIX GetProjectionMatix()const;
	DirectX::XMFLOAT4X4 GetProjectionMatix4x4()const;

	DirectX::XMMATRIX GetViewProjectionMatrix()const;

	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);

	void Render();

	// Set frustum.
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
	DirectX::XMFLOAT3 m_Look;

	DirectX::XMFLOAT4X4 m_View;
	DirectX::XMFLOAT4X4 m_Projection;

	float m_NearZ;
	float m_FarZ;
	float m_Aspect;
	float m_FovY;

	float m_NearWindowHeight;
	float m_FarWindowHeight;
};