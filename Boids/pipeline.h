/*
	Copyright 2010 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PIPELINE_H
#define	PIPELINE_H

#include "MathHelpers.h"

class Pipeline
{
public:
    Pipeline()
    {
        m_scale      = Vec3(1.0f, 1.0f, 1.0f);
        m_worldPos   = Vec3(0.0f, 0.0f, 0.0f);
        m_rotateInfo = Vec3(0.0f, 0.0f, 0.0f);
    }

    void Scale(float ScaleX, float ScaleY, float ScaleZ)
    {
        m_scale.x = ScaleX;
        m_scale.y = ScaleY;
        m_scale.z = ScaleZ;
    }

    void WorldPos(float x, float y, float z)
    {
        m_worldPos.x = x;
        m_worldPos.y = y;
        m_worldPos.z = z;
    }

    void Rotate(float RotateX, float RotateY, float RotateZ)
    {
        m_rotateInfo.x = RotateX;
        m_rotateInfo.y = RotateY;
        m_rotateInfo.z = RotateZ;
    }

	void SetPerspectiveProj(PersProjInfo& p)
	{
		m_persProj = p;
	}
	/*
    void SetPerspectiveProj(float FOV, float Width, float Height, float zNear, float zFar)
    {
        m_persProj.FOV    = FOV;
        m_persProj.Width  = Width;
        m_persProj.Height = Height;
        m_persProj.zNear  = zNear;
        m_persProj.zFar   = zFar;
    }
	*/
    void SetCamera(const Vec3& Pos, const Vec3& Target, const Vec3& Up)
    {
        m_camera.Pos = Pos;
        m_camera.Target = Target;
        m_camera.Up = Up;
    }

	const Matrix4* GetTrans();
	const Matrix4& GetVPTrans();
    const Matrix4& GetWorldTrans();

private:
    Vec3 m_scale;
    Vec3 m_worldPos;
    Vec3 m_rotateInfo;

	PersProjInfo m_persProj;

    struct {
        Vec3 Pos;
        Vec3 Target;
        Vec3 Up;
    } m_camera;

    Matrix4 m_transformation;
	Matrix4 m_WVPtransformation;
    Matrix4 m_WorldTransformation;
};

#endif	/* PIPELINE_H */

