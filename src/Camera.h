
//
//  Camera.h
//
//	Basic camera class
//

#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "vec\vec.h"
#include "vec\mat.h"

using namespace linalg;

class Camera
{
public:
	// Aperture attributes
	float vfov, aspect;	
	
	// Clip planes in view space coordinates
	// Evrything outside of [zNear, zFar] is clipped away on the GPU side
	// zNear should be > 0
	// zFar should depend on the size of the scene
	// This range should be kept as tight as possibly to improve
	// numerical precision in the z-buffer
	float zNear, zFar;	
						
	vec3f position;
	vec3f rotation;

	Camera(
		float vfov,
		float aspect,
		float zNear,
		float zFar):
		vfov(vfov), aspect(aspect), zNear(zNear), zFar(zFar)
	{
		position = { 0.0f, 0.0f, 0.0f };
		rotation = { 0.0f, 0.0f, 0.0f };
	}

	// Move to an absolute position
	//
	void moveTo(const vec3f& p)
	{
		position = p;
	}

	// Move relatively
	//
	void move(const vec3f& v)
	{
		mat4f TR = mat4f::translation(position) *
			mat4f::rotation(rotation.y, 0, 1, 0) *
			mat4f::rotation(rotation.x, 1, 0, 0);

		vec4f fwd = TR * vec4f(v.x, v.y, v.z, 0);

		position += vec3f(fwd.x, fwd.y, fwd.z);
	}

	void rotate(const vec3f& r)
	{
		float deg2Rad = fPI / 180.0f;
		float toCompare = 89 * deg2Rad;

		rotation.x += r.x;
		if (rotation.x > toCompare)
			rotation.x = toCompare;
		else if (rotation.x < -toCompare)
			rotation.x = -toCompare;

		rotation.y = std::fmod(rotation.y + r.y, fPI * 2);
	}

	// Return World-to-View matrix for this camera
	//
	mat4f get_WorldToViewMatrix()
	{
		// Assuming a camera's position and rotation is defined by matrices T(p) and R,
		// the View-to-World transform is T(p)*R (for a first-person style camera).
		//
		// World-to-View then is the inverse of T(p)*R;
		//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
		// Since now there is no rotation, this matrix is simply T(-p)

		mat4f rotMat = mat4f::rotation(rotation.y, 0, 1, 0) * mat4f::rotation(rotation.x, 1, 0, 0);
		rotMat.transpose();
		return rotMat * mat4f::translation(-position);
	}

	// Matrix transforming from View space to Clip space
	// In a performance sensitive situation this matrix should be precomputed
	// if possible
	//
	mat4f get_ProjectionMatrix()
	{
		return mat4f::projection(vfov, aspect, zNear, zFar);
	}
};

#endif