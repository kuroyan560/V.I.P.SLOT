#pragma once
#include<string>
#include"PlayerCamera.h"
#include<memory>
#include<vector>
#include"PlayerAttack.h"
#include"Collider.h"
#include"PlayerStatus.h"
#include"KuroMath.h"
#include"MarkingSystem.h"
#include"RushSystem.h"
#include"AbilitySystem.h"
class ModelObject;
class Camera;
class UsersInput;
class ControllerConfig;
class LightManager;
class CubeMap;
class TextureBuffer;
class HealAbility;
class PrecisionAimAbility;
class ColliderWithFloor;

class Player
{
private:
	//インスタンス生成を１個に制限するためのフラグ
	static bool s_instanced;

	//プレイヤー操作カメラ（プレイヤーが作り変えられる度に、カメラを生成しなくて済むように）
	static std::shared_ptr<PlayerCamera> s_camera;

	//HPゲージ枠画像
	static std::shared_ptr<TextureBuffer>s_hpGaugeFrameTex;
	//HPゲージ画像
	static std::shared_ptr<TextureBuffer>s_hpGaugeTex;
	//HP描画位置
	static Vec2<float>s_hpDrawPos;
	//HP描画スケール
	static Vec2<float>s_hpDrawExp;

	//アビリティシステム
	static std::shared_ptr<AbilitySystem> s_abilitySys;
	//常備アビリティ（回復）
	static std::shared_ptr<HealAbility>s_healAbility;
	//常備アビリティ（精密エイム）
	static std::shared_ptr<PrecisionAimAbility>s_precisionAimAbility;

	//静的メンバ初期化
	static void StaticInit();

public:
	//カメラキー
	static const std::string s_cameraKey;
	//カメラポインタ
	static std::shared_ptr<Camera> GetCam()
	{
		if (!s_camera)s_camera = std::make_shared<PlayerCamera>();
		return s_camera->GetCam();
	}

private:
	//ステータス管理
	PlayerStatus m_statusMgr;
	//ステータスのトリガーを感知して実行する処理のまとめ
	void OnStatusTriggerUpdate(const Vec3<float>& InputMoveVec);

	//モデル
	std::shared_ptr<ModelObject>m_model;

	//コライダー
	std::vector<std::shared_ptr<Collider>>m_colliders;	//配列
	//床との判定統括クラス
	std::shared_ptr<ColliderWithFloor>m_colliderWithFloor;

	//移動量
	//Vec3<float>m_move;

	//通常移動の速さ
	float m_moveSpeed = 0.6f;

	//走り移動の速さ
	float m_runSpeed = 2.0f;

/*--- ジャンプ関連 ---*/
	//ジャンプ力
	float m_jumpPower = 1.0f;

/*--- 回避 ---*/
	//回避方向の記録用
	Vec3<float>m_dodgeMoveVec;
	//回避のイージングパラメータ
	EasingParameter m_dodgeEasingParameter;
	//回避フレーム計測用
	int m_dodgeFrame = 0;
	//回避にかかるフレーム数
	int m_dodgeFrameNum = 30;
	//回避力
	float m_dodgePower = 0.5f;

/*--- マーキング ---*/
	//マーキングシステム
	MarkingSystem m_markingSystem;
	//マーキング長押し時、判定スパン
	int m_markingSpan = 10;
	//マーキング時間計測用
	int m_markingTimer = 0;
	//マーキング中の移動の速さ
	float m_whileMarkingMoveSpeed = 0.2f;
	//最大マーキング回数
	//int m_maxMarkingCount = 10;
	int m_maxMarkingCount = 200;

/*--- ラッシュ ---*/
	//ラッシュシステム
	RushSystem m_rushSystem;
	//単発ラッシュのチャージ中の移動の速さ
	float m_whileChargeMoveSpeed = 0.2f;

/*--- 単発ラッシュ ---*/
	int m_singleRushTimer = 0;
	int m_singleRushChargeTime = 60;

/*--- コールバック ---*/
	//押し戻し
	class PushBackColliderCallBack : public CollisionCallBack
	{
		Player* m_parent;
		void OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)override;
	public:
		PushBackColliderCallBack(Player* Parent) :m_parent(Parent) {}
		CollisionCallBack* Clone()override
		{
			return new PushBackColliderCallBack(m_parent);
		}
	}m_pushBackColliderCallBack;

/*--- アニメーション ---*/
	enum struct ANIM_TYPE
	{
		WAIT,
		MOVE,
		RUN,
		JUMP,
		ATTACK,
		GUARD,
		DODGE,
		NUM
	};
	const std::array<std::string, static_cast<int>(ANIM_TYPE::NUM)>m_animName =
	{
		"Wait",
		"Run",
		"Run",
		"",
		"Attack_",	//攻撃アニメーションは複数あるので先頭文字列だけ
		"",
		"",
	};
	const std::string& GetAnimName(const ANIM_TYPE& Type);

/*--- HP ---*/
	//現在のHP
	int m_hp;
	//現在の最大HP
	int m_maxHp = 100;

/*--- その他 ---*/
	//攻撃処理クラス
	PlayerAttack m_attack;
	//１フレーム前に攻撃入力したか
	bool m_oldAttackInput;

public:
	Player();
	~Player() { s_instanced = false; }
	void Init();
	void Update(UsersInput& Input, ControllerConfig& Controller, const float& Gravity, const Vec2<float>& WinSize);
	void Draw(Camera& Cam, LightManager& LigManager, std::shared_ptr<CubeMap>AttachCubeMap = nullptr);
	void DrawHUD(Camera& Cam, const Vec2<float>& WinSize);
	std::weak_ptr<ModelObject>GetModelObj() { return m_model; }

	//imguiデバッグ機能
	void ImguiDebug();

	//ラッシュシステムのゲッタ
	const RushSystem& GetRushSys()const { return m_rushSystem; }
	//Transformのゲッタ
	const Transform& GetTransform()const;
};