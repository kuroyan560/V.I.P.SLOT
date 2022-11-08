#include "YoYo.h"
#include"Collision.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"TimeScale.h"
#include<magic_enum.h>
#include"Importer.h"
#include"Object.h"

YoYo::YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr, Transform* arg_playerTransform)
{
	//モデルオブジェクト生成
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "yoyo.glb");

	//コライダー生成
	m_collider = std::make_shared<Collider>();

	/*
		[トランスフォーム親子関係]
		プレイヤー ← 攻撃方向指定 ← ヨーヨー ← 各ボーン ← 当たり判定用
	*/
	m_vecTransform.SetParent(arg_playerTransform);
	m_modelObj->m_transform.SetParent(&m_vecTransform);
	m_modelObj->m_animator->SetParentTransform(m_modelObj->m_transform);
	m_collider->SetParentTransform(&m_modelObj->m_animator->GetBoneTransform("Bone"));

	//コライダー登録
	arg_collisionMgr.lock()->Register(m_collider);

}


void YoYo::Awake(float arg_colSphereRadius)
{
	//当たり判定用球生成
	std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
	m_sphereCol = std::make_shared<CollisionSphere>(arg_colSphereRadius, Vec3<float>(0.0f, 0.0f, 0.0f));
	colPrimitiveArray.emplace_back(m_sphereCol);

	//コライダー設定
	m_collider->Generate("YoYo", "Player_Attack", colPrimitiveArray);
}

void YoYo::Init()
{
	//アニメーション初期化
	m_modelObj->m_animator->Reset();

	//コライダーオフに
	m_collider->SetActive(false);

	//手に持ってる状態
	m_status = HAND;
}

void YoYo::Update(const TimeScale& arg_timeScale)
{
	//アニメーション更新
	m_modelObj->m_animator->Update();

	//手に持ってる状態なら何もしない
	if (m_status == HAND)return;

	//手に戻った状態に遷移
	if (m_timer.UpdateTimer(arg_timeScale.GetTimeScale()))
	{
		m_collider->SetActive(false);
		m_status = HAND;
	}
}

#include"DrawFunc3D.h"
void YoYo::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock());
}

#include"imguiApp.h"
void YoYo::AddImguiDebugItem()
{
	ImGui::Text("Status : { %s }", std::string(magic_enum::enum_name(m_status)).c_str());
	ImGui::Text("TimeRate : { %f }", m_timer.GetTimeRate());
}

void YoYo::Throw(Vec2<float>arg_vec)
{
	//if (arg_vec.IsZero())
	//{
	//	m_status = NEUTRAL;
	//}
	//else
	{
		static const Vec2<float>LEFT_VEC = { -1.0f,0.0f };
		static const Vec2<float>RIGHT_VEC = { 1.0f,0.0f };

		//スティック方向から攻撃左右方向選択
		const auto lefAngle = KuroMath::GetAngleAbs(arg_vec, LEFT_VEC);
		const auto rightAngle = KuroMath::GetAngleAbs(arg_vec, RIGHT_VEC);

		if (lefAngle < rightAngle)
		{
			//左向き
			m_vecTransform.SetRotate(XMMatrixIdentity());
		}
		else
		{
			//右向き
			m_vecTransform.SetRotate(Angle(0), Angle(180), Angle(0));
		}

		if (m_status == HAND)
		{
			m_status = THROW_0;
			m_collider->SetActive(true);
			m_modelObj->m_animator->Play("Attack_0", false, false);
		}
		else if (m_status == THROW_0)
		{
			m_status = THROW_1;
			m_modelObj->m_animator->Play("Attack_1", false, false);
		}
		else if (m_status == THROW_1)
		{
			m_status = THROW_2;
			m_modelObj->m_animator->Play("Attack_2", false, false);
		}

		//デバッグ用
		else if (m_status == THROW_2)
		{
			m_status = THROW_0;
			m_modelObj->m_animator->Play("Attack_0", false, false);
		}
	}

	m_timer.Reset(m_finishInterval[m_status]);
}
