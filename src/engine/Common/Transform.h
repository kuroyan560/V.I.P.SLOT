#pragma once
#include"Vec.h"
#include<memory>
#include"Angle.h"
#include"KuroFunc.h"
#include<cmath>
#include<list>

class Transform
{
	static std::list<Transform*> s_transformList;
public:
	static void DirtyReset()
	{
		for (auto& trans : s_transformList)
		{
			trans->m_frameDirty = false;
		}
	}
private:
	Transform* m_parent = nullptr;

	Matrix m_worldMat = XMMatrixIdentity();
	Matrix m_localMat = XMMatrixIdentity();
	Vec3<float>m_pos = { 0,0,0 };
	Vec3<float>m_scale = { 1,1,1 };
	Matrix m_rotate = DirectX::XMMatrixIdentity();

	bool m_frameDirty = true;
	bool m_dirty = true;

	void MatReset()
	{
		m_frameDirty = true;
		m_dirty = true;
	}

	void CalculateMat();

public:
	Transform(Transform* Parent = nullptr) {
		SetParent(Parent);
		s_transformList.emplace_back(this);
	}
	~Transform() {
		(void)s_transformList.remove_if([this](Transform* tmp) {
			return tmp == this;
			});
	}
	void SetParent(Transform* Parent) {

		m_parent = Parent;
		MatReset();
	}

	//?Q?b?^
	const Vec3<float>& GetPos()const { return m_pos; }
	const Vec3<float>& GetScale()const{ return m_scale; }
	const Vec3<Angle>& GetAngle()const {
		auto sy = m_rotate.r[0].m128_f32[2];
		auto unlocked = std::abs(sy) < 0.99999f;
		return Vec3<Angle>(
			unlocked ? std::atan2(-m_rotate.r[1].m128_f32[2], m_rotate.r[2].m128_f32[2]) : std::atan2(m_rotate.r[2].m128_f32[1], m_rotate.r[1].m128_f32[1]),
			std::asin(sy),
			unlocked ? std::atan2(-m_rotate.r[0].m128_f32[1], m_rotate.r[0].m128_f32[0]) : 0);
	}
	const XMVECTOR& GetQuaternion()const {
		return XMQuaternionRotationMatrix(m_rotate);
	}
	const Matrix& GetRotate()const { return m_rotate; }
	Vec3<float> GetFront()const {
		XMVECTOR front = XMVectorSet(0, 0, 1, 1);
		front = XMVector3Transform(front, m_rotate);
		return Vec3<float>(front.m128_f32[0],front.m128_f32[1],front.m128_f32[2]);
	}
	Vec3<float> GetRight()const {
		XMVECTOR right = XMVectorSet(1, 0, 0, 1);
		right = XMVector3Transform(right, m_rotate);
		return Vec3<float>(right.m128_f32[0], right.m128_f32[1], right.m128_f32[2]);
	}
	Vec3<float> GetUp()const {
		XMVECTOR up = XMVectorSet(0, 1, 0, 1);
		up = XMVector3Transform(up, m_rotate);
		return Vec3<float>(up.m128_f32[0], up.m128_f32[1], up.m128_f32[2]);
	}

	//?Z?b?^
	void SetPos(const Vec3<float> Pos) {
		if (m_pos == Pos)return;
		m_pos = Pos;
		MatReset();
	}
	void SetScale(const Vec3<float> Scale) { 
		if (m_scale == Scale)return;
		m_scale = Scale;
		MatReset();
	}
	void SetScale(const float Scale) {
		auto s = Vec3<float>(Scale, Scale, Scale);
		if (m_scale == s)return;
		m_scale = s;
		MatReset();
	}
	void SetRotate(const Angle& X, const Angle& Y, const Angle& Z) {
		m_rotate = KuroMath::RotateMat(X, Y, Z);
		MatReset();
	}
	void SetRotate(const XMVECTOR& Quaternion) {
		m_rotate = XMMatrixRotationQuaternion(Quaternion);
		MatReset();
	}
	void SetRotate(const Vec3<float>& Axis, const Angle& Angle) {
		m_rotate = KuroMath::RotateMat(Axis, Angle);
		MatReset();
	}
	void SetRotate(const Matrix& RotateMat) {
		m_rotate = RotateMat;
		MatReset();
	}
	void SetLookAtRotate(const Vec3<float>& Target, const Vec3<float>& UpAxis = Vec3<float>(0, 1, 0)){
		Vec3<float>z = Vec3<float>(Target - m_pos).GetNormal();
		Vec3<float>x = UpAxis.Cross(z).GetNormal();
		Vec3<float>y = z.Cross(x).GetNormal();

		Matrix rot = XMMatrixIdentity();
		rot.r[0].m128_f32[0] = x.x; rot.r[0].m128_f32[1] = x.y; rot.r[0].m128_f32[2] = x.z;
		rot.r[1].m128_f32[0] = y.x; rot.r[1].m128_f32[1] = y.y; rot.r[1].m128_f32[2] = y.z;
		rot.r[2].m128_f32[0] = z.x; rot.r[2].m128_f32[1] = z.y; rot.r[2].m128_f32[2] = z.z;

		if (rot == m_rotate)return;
		m_rotate = rot;

		MatReset();
	}
	void SetUp(const Vec3<float>& Up)
	{
		Vec3<float> defUp = { 0,1,0 };
		Matrix rot = KuroMath::RotateMat(defUp, Up);
		if (rot == m_rotate)return;
		m_rotate = rot;

		MatReset();
	}
	void SetFront(const Vec3<float>& Front)
	{
		Vec3<float>defFront = { 0,0,1 };
		Matrix rot = KuroMath::RotateMat(defFront, Front);
		if (rot == m_rotate)return;
		m_rotate = rot;

		MatReset();
	}

	//???[?J???s???Q?b?^
	const Matrix& GetLocalMat();
	//???[???h?s???Q?b?^
	const Matrix& GetWorldMat();
	//???[???h?s???i?r???{?[?h?K?p?j
	Matrix GetWorldMat(const Matrix& arg_billBoardMat);
	//Dirty?t???O?Q?b?^
	bool IsDirty()const
	{
		return m_dirty || (m_parent != nullptr && (m_parent->IsFrameDirty() || m_parent->IsDirty()));
	}
	//?t???[???P?ʂ?Dirty?t???O?Q?b?^
	bool IsFrameDirty()const
	{
		return m_frameDirty || (m_parent != nullptr && m_parent->IsFrameDirty());
	}
};