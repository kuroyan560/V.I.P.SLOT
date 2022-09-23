#pragma once
#include"Angle.h"
#include<memory>
#include"ImguiDebugInterface.h"
#include"Vec.h"
#include"Collider.h"
class Camera;
class Transform;
class ActPoint;

struct PlayerCamera : public ImguiDebugInterface, public CollisionCallBack
{
private:
	std::shared_ptr<Camera>m_cam;
	float m_dist = 5.0f;		//プレイヤーとの距離
	bool m_mirrorX = false;		//X入力ミラー
	bool m_mirrorY = false;		//Y入力ミラー

	//位置の角度
	Angle m_posAngle;

	//カメラの高さ
	float m_height;

	//ロックオン対象
	std::weak_ptr<ActPoint>m_rockOnPoint;
	//ロックオン可能なカメラ座標との距離の上限（３D）
	float m_canRockOnDist3D;
	//ロックオン可能な画面中央との距離の上限（２D）
	float m_canRockOnDist2D;
	//ロックオン許容角度
	Angle m_rockOnAngleRange;
	//ロックオン照準テクスチャ
	std::shared_ptr<TextureBuffer>m_reticleTex;

	//ロックオン先に繋がる当たり判定線分
	std::shared_ptr<CollisionLine>m_rockOnColLine;
	std::shared_ptr<Collider>m_rockOnCollider;

	//ロックオン対象設定
	void SetRockOnTarget(std::shared_ptr<ActPoint> Target);
	//座標計算
	void CalculatePos(const Transform& Player);
	//ロックオン対象に合わせてカメラを動かす
	void RockOnTargeting(Vec3<float>PlayerPos);
	//ロックオン当たり判定用線分のコールバック関数
	void OnCollision(const Vec3<float>& Inter, std::weak_ptr<Collider> OtherCollider)override
	{
		//ロックオン対象のコライダーなら無視
		if (this->m_rockOnCollider->CompareID(OtherCollider))return;
		//何かに遮られたらロックオン解除
		SetRockOnTarget(nullptr);
	}

public:
	PlayerCamera();
	void Init(const Transform& Player);
	void Update(const Transform& Player, Vec2<float> InputVec);
	void Draw(Camera& NowCam);

	//プレイヤー正面にカメラを合わせる
	void LookAtPlayersFront(const Transform& Player);

	//ロックオン
	void RockOn(const Transform& Player);

	//ゲッタ
	std::shared_ptr<Camera>GetCam() { return m_cam; }
	const Angle& GetPosAngle() { return m_posAngle; }

	//imguiデバッグ機能
	void OnImguiDebug()override;
};

