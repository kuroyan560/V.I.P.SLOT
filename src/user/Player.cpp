#include "Player.h"
#include"Object.h"
#include"DrawFunc3D.h"
#include"Collision.h"
#include"Collider.h"
#include"Model.h"
#include"ModelAnimator.h"
#include"UsersInput.h"
#include"ControllerConfig.h"
#include"magic_enum.h"
#include"GameManager.h"
#include"HealAbility.h"
#include"PrecisionAimAbility.h"
#include"ColliderWithFloor.h"

//インスタンス生成を１個に制限するためのフラグ
bool Player::s_instanced = false;
//プレイヤー操作カメラ（プレイヤーが作り変えられる度に、カメラを生成しなくて済むように）
std::shared_ptr<PlayerCamera> Player::s_camera;
//カメラのキー
const std::string Player::s_cameraKey = "PlayerCamera";
//HPゲージ枠画像
std::shared_ptr<TextureBuffer>Player::s_hpGaugeFrameTex;
//HPゲージ画像
std::shared_ptr<TextureBuffer>Player::s_hpGaugeTex;
//HP描画位置
Vec2<float>Player::s_hpDrawPos = { 239,694 };
//HP描画スケール
Vec2<float>Player::s_hpDrawExp = { 0.7f,0.55f };

//アビリティシステム
std::shared_ptr<AbilitySystem> Player::s_abilitySys;
//常備アビリティ（回復）
std::shared_ptr<HealAbility>Player::s_healAbility;
//常備アビリティ（精密エイム）
std::shared_ptr<PrecisionAimAbility>Player::s_precisionAimAbility;

void Player::StaticInit()
{
	//HPテクスチャ読み込み
	s_hpGaugeTex = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/playerHPGauge.png");
	s_hpGaugeFrameTex = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/playerHPFrame.png");

	//アビリティシステム生成
	s_abilitySys = std::make_shared<AbilitySystem>();

	//常備アビリティ（最初から所持しているアビリティ）
	s_healAbility = std::make_shared<HealAbility>();	//回復
	s_precisionAimAbility = std::make_shared<PrecisionAimAbility>();	//精密エイム
	//アビリティシステムに登録
	s_abilitySys->Register(s_healAbility);
	s_abilitySys->Register(s_precisionAimAbility);
}

Player::Player() : m_pushBackColliderCallBack(this), m_attack(GetAnimName(ANIM_TYPE::ATTACK))
{
	//静的メンバ初期化
	static bool s_init = false;
	if (!s_init)
	{
		StaticInit();
		s_init = true;
	}

	assert(!s_instanced);
	s_instanced = true;
	m_model = std::make_shared<ModelObject>("resource/user/", "PrePlayer.gltf");

/*--- コライダー生成 ---*/
	//本体
	auto bodyCol_Sphere = std::make_shared<CollisionSphere>(3.0f, Vec3<float>(0, 6, 0), &m_model->m_transform);
	auto bodyCol = Collider::Generate(
		"Player's Body Collider",
		bodyCol_Sphere,
		&m_pushBackColliderCallBack,	//押し戻しコールバック処理をアタッチ
		COLLIDER_ATTRIBUTE::PLAYER,
		COLLIDER_ATTRIBUTE::ENEMY | COLLIDER_ATTRIBUTE::TERRIAN);
	m_colliders.emplace_back(bodyCol);

	//床との当たり判定用機構
	m_colliderWithFloor = std::make_shared<ColliderWithFloor>("Player", Vec3<float>(0, 0, 0), &m_model->m_transform);
	m_colliders.emplace_back(m_colliderWithFloor->GetCollider());

	//通常攻撃用コライダー
	auto nrmAttackCol_Sphere = std::make_shared<CollisionSphere>(3.0f, Vec3<float>(0, 6, 6), &m_model->m_transform);
	auto nrmAttackCol = Collider::Generate(
		"Player's Attack Collider",
		nrmAttackCol_Sphere,
		&m_attack,
		COLLIDER_ATTRIBUTE::PLAYER_ATTACK,
		COLLIDER_ATTRIBUTE::ENEMY);
	m_colliders.emplace_back(nrmAttackCol);

	//攻撃処理に武器コライダーをアタッチ
	m_attack.Attach(m_model->m_animator, nrmAttackCol);

	//アビリティ初期化
	s_precisionAimAbility->Init();
}

void Player::OnStatusTriggerUpdate(const Vec3<float>& InputMoveVec)
{
	//現在のステータス
	{
		//待機
		if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::WAIT))
		{
			//待機アニメーション
			m_model->m_animator->speed = 1.0f;
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::WAIT), true, false);
		}
		//移動
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::MOVE))
		{
			//移動アニメーション
			m_model->m_animator->speed = 1.5f;
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::MOVE), true, false);
		}
		//ジャンプ
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::JUMP))
		{
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::JUMP), true, false);

			m_colliderWithFloor->Jump(m_jumpPower);
		}
		//ガード
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::GUARD))
		{
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::GUARD), true, false);
		}
		//回避
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::DODGE))
		{
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::DODGE), false, false);

			//回避フレーム初期化
			m_dodgeFrame = 0;
			//回避の入力方向記録
			m_dodgeMoveVec = InputMoveVec;
		}
		//ダッシュ
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::RUN))
		{
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::RUN), true, false);
		}
		//マーキング
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::MARKING))
		{
		}
		//単発ラッシュチャージ
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::CHARGE))
		{
			m_singleRushTimer = 0;
		}
		//ラッシュ
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::RUSH))
		{
			//単発ラッシュか
			bool isOneShot = m_statusMgr.CompareOldStatus(PLAYER_STATUS_TAG::CHARGE);
			//ラッシュ実行
			m_rushSystem.Excute(
				m_markingSystem,
				m_model->m_transform.GetPos(),
				isOneShot,
				static_cast<float>(m_singleRushTimer / m_singleRushChargeTime));
			//マーキング初期化
			m_markingSystem.Init();
			//ラッシュ用カメラに変更
			if(!isOneShot)GameManager::Instance()->ChangeCamera(RushSystem::s_cameraKey);
		}
		//アビリティ
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::INVOKE_ABILITY))
		{
			s_abilitySys->Invoke();
		}
	}
	//過去のステータス
	{
		//ラッシュが終了した瞬間
		if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::RUSH, true))
		{
			//ラッシュのカメラ取得
			auto rushCam = GameManager::Instance()->GetNowCamera();
			//プレイヤー用カメラに変更
			GameManager::Instance()->ChangeCamera(Player::s_cameraKey);
			//プレイヤーカメラ初期化
			//s_camera->Init(m_model->m_transform);
			//ラッシュカメラの最後の状態をプレイヤー用カメラに反映させる
			s_camera->GetCam()->SetPos(rushCam->GetPos());
			s_camera->GetCam()->SetTarget(rushCam->GetTarget());
			s_camera->LookAtPlayersFront(m_model->m_transform);
			//予測線初期化
			m_markingSystem.InitPredictionLines();
		}
	}
}

const std::string& Player::GetAnimName(const ANIM_TYPE& Type)
{
	auto& str = m_animName[static_cast<int>(Type)];
	if (str.empty())return m_animName[static_cast<int>(ANIM_TYPE::WAIT)];
	return str;
}

void Player::Init()
{
	//ステータス初期化
	m_statusMgr.Init(PLAYER_STATUS_TAG::WAIT);

	//初期位置と向き
	static Vec3<float>INIT_POS = { 0,0,-5 };
	m_model->m_transform.SetPos(INIT_POS);
	m_model->m_transform.SetRotate(XMMatrixIdentity());

	//カメラ初期化
	s_camera->Init(m_model->m_transform);

	//待機アニメーション
	m_model->m_animator->Play(GetAnimName(ANIM_TYPE::WAIT), true, false);

	//床との当たり判定機構初期化（最初は地に足をつけている）
	m_colliderWithFloor->Init();

	//回避初期化
	m_dodgeFrame = m_dodgeFrameNum;

	//攻撃処理初期化
	m_attack.Init();
	m_oldAttackInput = false;

	//マーキングシステム初期化
	m_markingSystem.Init();
	m_markingTimer = 0;

	//HP初期化
	m_hp = m_maxHp;

	//アビリティシステム初期化
	s_abilitySys->Init();
}

void Player::Update(UsersInput& Input, ControllerConfig& Controller, const float& Gravity, const Vec2<float>& WinSize)
{
	//移動量リセット
	Vec3<float>move = { 0,0,0 };

	//HP回復アビリティ更新
	s_healAbility->Update(m_maxHp, m_hp);
	//精密エイム更新
	s_precisionAimAbility->Update();

	//アビリティ選択
	s_abilitySys->Update(Input, Controller);

	PlayerParameterForStatus infoForStatus;
	infoForStatus.m_markingNum = m_markingSystem.GetSumMarkingCount();
	infoForStatus.m_maxMarking = m_markingSystem.GetSumMarkingCount() == m_maxMarkingCount;
	infoForStatus.m_onGround = m_colliderWithFloor->GetOnGround();
	infoForStatus.m_attackFinish = !m_attack.IsActive();
	infoForStatus.m_dodgeFinish = (m_dodgeFrameNum <= m_dodgeFrame);
	infoForStatus.m_rushFinish = !m_rushSystem.IsActive();
	infoForStatus.m_abilityFinish = s_abilitySys->IsNowAbilityFinish();

	//ステータスの更新
	m_statusMgr.Update(Input, Controller, infoForStatus);

	//左スティック入力レート
	auto stickL = Controller.GetMoveVec(Input);

	//移動方向
	Vec3<float>inputMoveVec = { stickL.x,0.0f,-stickL.y };

	//カメラ位置角度のオフセットからスティックの入力方向補正
	static const Angle ANGLE_OFFSET(-90);
	inputMoveVec = KuroMath::TransformVec3(inputMoveVec, KuroMath::RotateMat({ 0,1,0 }, -s_camera->GetPosAngle() + ANGLE_OFFSET)).GetNormal();

	//ステータスのトリガーを感知して、処理を呼び出す
	OnStatusTriggerUpdate(inputMoveVec);

	//連続攻撃の入力
	bool attackInput = Controller.GetHandleInput(Input, HANDLE_INPUT_TAG::ATTACK);
	//攻撃状態
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::ATTACK))
	{
		if (!m_oldAttackInput && attackInput)
		{
			//攻撃の処理はPlayerAttack内で処理
			m_attack.Attack(m_model->m_transform.GetFront());
		}
	}
	//トリガー判定用に過去の入力として記録
	m_oldAttackInput = attackInput;

	//通常移動状態
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::MOVE))
	{
		//入力による移動の処理
		move += inputMoveVec * m_moveSpeed;
	}
	//走り移動状態
	else if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::RUN))
	{
		//入力による移動の処理
		move += inputMoveVec * m_runSpeed;
	}
	//回避状態
	else if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::DODGE))
	{
		//回避速度計算
		float dodgeSpeed = m_dodgeEasingParameter.Calculate(
			static_cast<float>(m_dodgeFrame), 
			static_cast<float>(m_dodgeFrameNum), 
			m_dodgePower,
			m_runSpeed);
		//回避による移動量加算
		move += m_dodgeMoveVec * dodgeSpeed;
		//回避フレーム計測
		m_dodgeFrame++;
	}

	//マーキング状態
	bool nowMarkingInput = false;
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::MARKING))
	{
		//入力による移動の処理
		move += inputMoveVec * m_whileMarkingMoveSpeed;

		//マーキング長押しのインターバル
		if (m_markingTimer)
		{
			m_markingTimer--;
		}
		else
		{
			//マーキング実行
			m_markingTimer = m_markingSpan;
			nowMarkingInput = true;
		}
	}
	else m_markingTimer = 0;

	//ラッシュ状態
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::RUSH))
	{
		m_rushSystem.Update(m_model->m_transform, Gravity);
	}

	//単発ラッシュのチャージ状態
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::CHARGE))
	{
		//入力による移動の処理
		move += inputMoveVec * m_whileChargeMoveSpeed;
		//チャージ時間蓄積（上限を超えないようにする）
		if (m_singleRushChargeTime < ++m_singleRushTimer)m_singleRushTimer = m_singleRushChargeTime;
	}

	//無操作状態でないとき
	if (!m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::OUT_OF_CONTROL))
	{
		//ロックオン
		if (Controller.GetCameraRock(Input))s_camera->RockOn(m_model->m_transform);

		//プレイヤー追従カメラ更新
		s_camera->Update(m_model->m_transform, Controller.GetCameraVec(Input));
	}

	//攻撃処理更新
	m_attack.Update();
	//攻撃ステータスでない（攻撃が中断された場合）
	if (!m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::ATTACK))m_attack.Stop();

	//アニメーション更新
	m_model->m_animator->Update();

	//攻撃の勢いを移動量に加算
	move += m_attack.GetMomentum();

	//落下
	move.y += m_colliderWithFloor->GetFallSpeed();
	m_colliderWithFloor->AddFallSpeed(Gravity);

	//ラッシュ中
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::RUSH))
	{
		//移動量打ち消し
		move = { 0,0,0 };
		//落下速度リセット
		m_colliderWithFloor->ResetFallSpeed();
	}

	//移動量を反映させる
	auto playerPos = m_model->m_transform.GetPos();
	playerPos += move;
	m_model->m_transform.SetPos(playerPos);

	//マーキングシステム更新
	m_markingSystem.Update(*s_camera->GetCam(), WinSize, nowMarkingInput, playerPos, m_maxMarkingCount);

	//方向転換（XZ平面）
	if (move.x || move.z)
	{
		m_model->m_transform.SetLookAtRotate(playerPos + Vec3<float>(move.x, 0.0f, move.z));
	}
}

#include"BasicDraw.h"
void Player::Draw(Camera& Cam, LightManager& LigManager, std::shared_ptr<CubeMap>AttachCubeMap)
{
	//プレイヤー描画
	BasicDraw::Draw(LigManager, m_model, Cam, AttachCubeMap);

	//マーキング予測線描画
	m_markingSystem.DrawPredictionLine(Cam, m_model->m_transform.GetPos());
}

#include"DrawFunc2D_Color.h"
#include"DrawFunc2D.h"
void Player::DrawHUD(Camera& Cam, const Vec2<float>& WinSize)
{
	s_camera->Draw(Cam);

	//ラッシュシステム中はマーキングHUD非表示
	if (!m_rushSystem.IsActive())
	{
		m_markingSystem.DrawHUD(Cam, WinSize);
	}

	//HP枠描画
	DrawFunc2D::DrawRotaGraph2D(
		s_hpDrawPos,
		s_hpDrawExp,
		0.0f,
		s_hpGaugeFrameTex
	);

	//HPゲージ描画
	float hpRate = (float)m_hp / m_maxHp;
	DrawFunc2D_Color::DrawRotaGraph2D(
		s_hpDrawPos,
		s_hpDrawExp,
		0.0f,
		s_hpGaugeTex,
		Color(0, 0, 0, 0),
		{ 0.5f,0.5f },
		{ false,false },
		{ hpRate,0.0f },
		{ 1.0f,1.0f }
	);

	//アビリティシステム
	s_abilitySys->DrawHUD();

	//デバッグ用
	s_healAbility->Draw();
	s_precisionAimAbility->Draw();
}

#include"imguiApp.h"
void Player::ImguiDebug()
{
	ImGui::Begin("Player");
/*--- パラメータ表示 ---*/
	//最大HP
	if (ImGui::DragInt("MaxHp", &m_maxHp))
	{
		if (m_maxHp <= 0)m_maxHp = 1;		//０以下防止
		if (m_maxHp < m_hp)m_hp = m_maxHp;	//現在のHPの最大超過防止

	}
	//現在のHPを直接入力
	int oldHp = m_hp;
	if (ImGui::DragInt("NowHp", &m_hp))
	{
		//マイナス防止
		if (m_hp < 0)m_hp = 0;
		//最大HPを超えない
		if (m_maxHp < m_hp)m_hp = m_maxHp;
	}
	//HP描画位置
	static float s_imguiHpDrawPos[2] = { s_hpDrawPos.x,s_hpDrawPos.y };
	if (ImGui::DragFloat2("HpDrawPos", s_imguiHpDrawPos))
	{
		s_hpDrawPos = { s_imguiHpDrawPos[0],s_imguiHpDrawPos[1] };
	}
	//HP描画スケール
	static float s_imguiHpDrawExp[2] = { s_hpDrawExp.x,s_hpDrawExp.y };
	if (ImGui::DragFloat2("HpDrawExp", s_imguiHpDrawExp, 0.05))
	{
		s_hpDrawExp = { s_imguiHpDrawExp[0],s_imguiHpDrawExp[1] };
	}

	ImGui::Separator();
/*--- ステータス表示 ---*/
	//ステータス名取得
	static std::string s_nowStatusName = std::string(magic_enum::enum_name(m_statusMgr.GetNowStatus()));
	static std::string s_beforeStatusName = s_nowStatusName;

	//ステータス切り替えトリガー判定取得
	if (m_statusMgr.StatusTrigger())
	{
		s_beforeStatusName = s_nowStatusName;
		s_nowStatusName = std::string(magic_enum::enum_name(m_statusMgr.GetNowStatus()));
	}

	//表示
	ImGui::Text("NowStatus : { %s }", s_nowStatusName.c_str());
	ImGui::Text("BeforeStatus : { %s }", s_beforeStatusName.c_str());

/*--- 性能調整 ---*/
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(500, 320), ImGuiWindowFlags_NoTitleBar);
	//通常移動
	if (ImGui::TreeNode("Move"))
	{
		if (ImGui::DragFloat("Speed", &m_moveSpeed, 0.1f) && m_moveSpeed < 0)m_moveSpeed = 0.0f;	//マイナス防止
		ImGui::TreePop();
	}
	//走り移動
	if (ImGui::TreeNode("Run"))
	{
		if (ImGui::DragFloat("Speed", &m_runSpeed, 0.1f) && m_runSpeed < 0)m_runSpeed = 0.0f;	//マイナス防止
		ImGui::TreePop();
	}
	//ジャンプ力
	if (ImGui::TreeNode("Jump"))
	{
		if (ImGui::DragFloat("Power", &m_jumpPower, 0.1f) && m_jumpPower < 0)m_jumpPower = 0.0f;	//マイナス防止
		ImGui::TreePop();
	}
	//通常攻撃
	if (ImGui::TreeNode("Attack"))
	{
		m_attack.ImguiDebug();
		ImGui::TreePop();
	}

	//回避
	if (ImGui::TreeNode("Dodge"))
	{
		//回避にかかるフレーム数
		if (ImGui::DragInt("FrameNum", &m_dodgeFrameNum) && m_dodgeFrameNum < 0)m_dodgeFrameNum = 0;	//マイナス防止
		//回避力
		if (ImGui::DragFloat("Power", &m_dodgePower, 0.1f) && m_dodgePower < 0)m_dodgePower = 0;	//マイナス防止

		//回避のイージング変化（In,Out,InOut)
		//名前表示
		ImGui::Text("%s", magic_enum::enum_name(m_dodgeEasingParameter.m_changeType).data());
		//変更
		static int s_dodgeSelectEaseChange = m_dodgeEasingParameter.m_changeType;
		if (ImGui::SliderInt("EaseChangeType", &s_dodgeSelectEaseChange, 0, EASE_CHANGE_TYPE_NUM - 1))
		{
			m_dodgeEasingParameter.m_changeType = (EASE_CHANGE_TYPE)s_dodgeSelectEaseChange;
		}

		//回避のイージングタイプ
		//名前表示
		ImGui::Text("%s", magic_enum::enum_name(m_dodgeEasingParameter.m_easeType).data());
		//変更
		static int s_dodgeSelectEase = m_dodgeEasingParameter.m_easeType;
		if (ImGui::SliderInt("EaseType", &s_dodgeSelectEase, 0, EASING_TYPE_NUM - 1))
		{
			m_dodgeEasingParameter.m_easeType = (EASING_TYPE)s_dodgeSelectEase;
		}

		ImGui::TreePop();
	}
	//マーキング
	if (ImGui::TreeNode("Marking"))
	{
		//マーキング長押し時の時間計測用タイマー
		ImGui::Text("MarkingTimer : { %d }", m_markingTimer);

		//現在の総マーキング回数
		ImGui::Text("SumMarkingCount : { %d }", m_markingSystem.GetSumMarkingCount());

		//最大マーキング回数
		if (ImGui::DragInt("MaxMarkingCount", &m_maxMarkingCount) && m_maxMarkingCount < 0)m_maxMarkingCount = 0;		//マイナス防止

		//マーキング長押しスパン
		if (ImGui::DragInt("MarkingSpan", &m_markingSpan) && m_markingSpan < 0)m_markingSpan = 0;		//マイナス防止

		//マーキング中の移動速度
		if (ImGui::DragFloat("MoveSpeed", &m_whileMarkingMoveSpeed,0.1f) && m_whileMarkingMoveSpeed < 0.0f)m_whileMarkingMoveSpeed = 0.0f;		//マイナス防止

		//マーキングシステムの初期化ボタン
		if (ImGui::Button("Init - MarkingSystem"))m_markingSystem.Init();

		ImGui::TreePop();
	}
	//ラッシュ
	if (ImGui::TreeNode("Rush"))
	{
		//単発ラッシュのチャージ中の移動速度
		if (ImGui::DragFloat("MoveSpeed_oneShotCharge", &m_whileChargeMoveSpeed, 0.1f) && m_whileChargeMoveSpeed < 0.0f)m_whileChargeMoveSpeed = 0.0f;		//マイナス防止

		ImGui::Separator();
		m_rushSystem.ImguiDebug();

		ImGui::TreePop();
	}
	//単発ラッシュ
	if (ImGui::TreeNode("SingleRush"))
	{
		if (ImGui::DragInt("ChargeTotalTime", &m_singleRushChargeTime) && m_singleRushChargeTime < 0)m_singleRushChargeTime = 0;		//マイナス防止
		ImGui::TreePop();
	}

	//アビリティ
	if (ImGui::TreeNode("Ability"))
	{
		s_abilitySys->ImguiDebug();
		ImGui::TreePop();
	}

	ImGui::EndChild();

	ImGui::End();
}

void Player::PushBackColliderCallBack::OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)
{
	//現在の座標
	Vec3<float>nowPos = m_parent->m_model->m_transform.GetPos();

	//押し戻し後の座標格納先
	Vec3<float>pushBackPos = nowPos;

	//自身のコライダープリミティブ（球）を取得
	auto mySphere = (CollisionSphere*)GetAttachCollider().lock()->GetColliderPrimitive().lock().get();

	//相手のコライダープリミティブを取得
	auto otherPrimitive = OtherCollider.lock()->GetColliderPrimitive().lock();
	if (otherPrimitive->GetShape() == CollisionPrimitive::SPHERE)
	{
		//相手の衝突判定用球取得
		CollisionSphere* otherSphere = (CollisionSphere*)otherPrimitive.get();

		//それぞれの中心座標
		Vec3<float>myCenter = mySphere->GetCenter();
		Vec3<float>otherCenter = otherSphere->GetCenter();

		//押し戻し方向
		Vec3<float>pushBackVec = (myCenter - otherCenter).GetNormal();

		//押し戻し量
		float pushBackAmount = mySphere->m_radius + otherSphere->m_radius + 0.1f;
		
		//押し戻した後のコライダーの座標
		Vec3<float>pushBackColPos = otherCenter + pushBackVec * pushBackAmount;

		//押し戻し後のコライダーと現在のコライダー座標とのオフセット
		Vec3<float>colOffset = pushBackColPos - myCenter;

		//オフセットを親であるトランスフォームに適用
		pushBackPos = nowPos + colOffset;
	}
	else assert(0);	//用意していない

	//新しい座標をセット
	m_parent->m_model->m_transform.SetPos(pushBackPos);
}

const Transform& Player::GetTransform()const
{
	return m_model->m_transform;
}
