#include "Camera.h"
#include "Window.h"
#include <algorithm>

using namespace NCL;

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float dt) {
	//Update the mouse by how much
	pitch	-= (Window::GetMouse()->GetRelativePosition().y);
	yaw		-= (Window::GetMouse()->GetRelativePosition().x);

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw <0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	float frameSpeed = 100 * dt;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
		position.y += frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
		position.y -= frameSpeed;
	}
}

void NCL::Camera::UpdateThirdPersonCamera(CSC8503::Transform& target, Vector3 offset, float rayDistance, bool player2)
{
	if (player2)
	{
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUMPAD8))
			pitch += 0.3f;
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUMPAD2))
			pitch -= 0.3f;
	}
	else
		pitch -= (Window::GetMouse()->GetRelativePosition().y);

	pitch = std::min(pitch, 89.0f);
	pitch = std::max(pitch, -89.0f);

	float cameraDistance = 15.0f;
	cameraDistance = std::min(rayDistance, cameraDistance);

	float cameraYOffset = cameraDistance * sin(-pitch * Camera::Deg2Rad);
	float cameraXOffset = cameraDistance * cos(-pitch * Camera::Deg2Rad);
	Vector3 camerTargetPos = target.GetPosition()
		+ Vector3(0, cameraYOffset, 0)
		+ target.Backward().Normalised() * cameraXOffset;

	Matrix4 mat = Matrix4::BuildViewMatrix(camerTargetPos, target.GetPosition(), Vector3(0, 1, 0));
	Matrix4 modelMat = mat.Inverse();

	Quaternion q(modelMat);
	Vector3 angles = q.ToEuler();

	SetPosition(camerTargetPos + offset);
	SetPitch(angles.x);
	SetYaw(angles.y);
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix() const {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
};

Matrix4 Camera::BuildViewMatrixWithoutTranlation() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0));
}

Matrix4 Camera::GetRotationMatrix() const {
	return Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0));
}

Matrix4 Camera::BuildProjectionMatrix(float currentAspect) const {
	if (camType == CameraType::Orthographic) {
		return Matrix4::Orthographic(nearPlane, farPlane, right, left, top, bottom);
	}
	//else if (camType == CameraType::Perspective) {
		return Matrix4::Perspective(nearPlane, farPlane, currentAspect, fov);
	//}
}

Camera Camera::BuildPerspectiveCamera(const Vector3& pos, float pitch, float yaw, float fov, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Perspective;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane  = farPlane;

	c.fov		= fov;

	return c;
}
Camera Camera::BuildOrthoCamera(const Vector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Orthographic;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane	= farPlane;

	c.left		= left;
	c.right		= right;
	c.top		= top;
	c.bottom	= bottom;

	return c;
}