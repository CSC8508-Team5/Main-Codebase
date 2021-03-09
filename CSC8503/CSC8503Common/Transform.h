#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/Matrix3.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/Quaternion.h"

#include <btBulletDynamicsCommon.h>

#include <vector>

using std::vector;

using namespace NCL::Maths;

namespace NCL {
	namespace CSC8503 {
		class Transform
		{
		public:
			Transform();
			~Transform();

			Transform& SetPosition(const Vector3& worldPos);
			Transform& SetScale(const Vector3& worldScale);
			Transform& SetOrientation(const Quaternion& newOr);
			
			Vector3 Up() const
			{
				return Vector3(matrix.GetColumn(1));
			}

			Vector3 Down() const
			{
				return Vector3(-matrix.GetColumn(1));
			}

			Vector3 Left() const
			{
				return Vector3(-matrix.GetColumn(0));
			}

			Vector3 Right() const
			{
				return Vector3(matrix.GetColumn(0));
			}

			Vector3 Forward() const
			{
				return Vector3(-matrix.GetColumn(2));
			}

			Vector3 Backward() const
			{
				return Vector3(matrix.GetColumn(2));
			}

			Vector3 GetPosition() const {
				return position;
			}

			Vector3 GetScale() const {
				return scale;
			}

			Quaternion GetOrientation() const {
				return orientation;
			}

			Matrix4 GetMatrix() const {
				return matrix;
			}
			void UpdateMatrix();

			operator btTransform()
			{
				btScalar data[16] = { 0 };
				Matrix4 mat = matrix.Inverse();
				for (int i = 0; i < 16; i++)
				{
					data[i] = mat.array[i];
				}
				btTransform bT;
				bT.setFromOpenGLMatrix(data);
				return bT;
			}
		protected:
			Matrix4		matrix;
			Quaternion	orientation;
			Vector3		position;

			Vector3		scale;
		};
	}
}

