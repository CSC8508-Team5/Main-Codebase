/*
do some common tasks of this project

Comments and queries to: Dawei Wang, Group 5
*/

#pragma once

#include "glad\glad.h"
#include "../../Common/stb/stb_image.h"
#include <iostream>
#include <string>

#include "../../Common/Vector3.h"

class DW_Tools
{
public:
	static GLuint LoadTexture(char const* path);
	static float RandomFloatBetween(float low, float high);

	static NCL::Maths::Vector3 ComputeVelocity(const NCL::Maths::Vector3& u, const NCL::Maths::Vector3& a, float t);
	static NCL::Maths::Vector3 ComputeEuler(const NCL::Maths::Vector3& s, const NCL::Maths::Vector3& u, const NCL::Maths::Vector3& a, float t);
};

