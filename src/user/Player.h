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
	//�C���X�^���X�������P�ɐ������邽�߂̃t���O
	static bool s_instanced;

	//�v���C���[����J�����i�v���C���[�����ς�����x�ɁA�J�����𐶐����Ȃ��čςނ悤�Ɂj
	static std::shared_ptr<PlayerCamera> s_camera;

	//HP�Q�[�W�g�摜
	static std::shared_ptr<TextureBuffer>s_hpGaugeFrameTex;
	//HP�Q�[�W�摜
	static std::shared_ptr<TextureBuffer>s_hpGaugeTex;
	//HP�`��ʒu
	static Vec2<float>s_hpDrawPos;
	//HP�`��X�P�[��
	static Vec2<float>s_hpDrawExp;

	//�A�r���e�B�V�X�e��
	static std::shared_ptr<AbilitySystem> s_abilitySys;
	//����A�r���e�B�i�񕜁j
	static std::shared_ptr<HealAbility>s_healAbility;
	//����A�r���e�B�i�����G�C���j
	static std::shared_ptr<PrecisionAimAbility>s_precisionAimAbility;

	//�ÓI�����o������
	static void StaticInit();

public:
	//�J�����L�[
	static const std::string s_cameraKey;
	//�J�����|�C���^
	static std::shared_ptr<Camera> GetCam()
	{
		if (!s_camera)s_camera = std::make_shared<PlayerCamera>();
		return s_camera->GetCam();
	}

private:
	//�X�e�[�^�X�Ǘ�
	PlayerStatus m_statusMgr;
	//�X�e�[�^�X�̃g���K�[�����m���Ď��s���鏈���̂܂Ƃ�
	void OnStatusTriggerUpdate(const Vec3<float>& InputMoveVec);

	//���f��
	std::shared_ptr<ModelObject>m_model;

	//�R���C�_�[
	std::vector<std::shared_ptr<Collider>>m_colliders;	//�z��
	//���Ƃ̔��蓝���N���X
	std::shared_ptr<ColliderWithFloor>m_colliderWithFloor;

	//�ړ���
	//Vec3<float>m_move;

	//�ʏ�ړ��̑���
	float m_moveSpeed = 0.6f;

	//����ړ��̑���
	float m_runSpeed = 2.0f;

/*--- �W�����v�֘A ---*/
	//�W�����v��
	float m_jumpPower = 1.0f;

/*--- ��� ---*/
	//�������̋L�^�p
	Vec3<float>m_dodgeMoveVec;
	//����̃C�[�W���O�p�����[�^
	EasingParameter m_dodgeEasingParameter;
	//����t���[���v���p
	int m_dodgeFrame = 0;
	//����ɂ�����t���[����
	int m_dodgeFrameNum = 30;
	//����
	float m_dodgePower = 0.5f;

/*--- �}�[�L���O ---*/
	//�}�[�L���O�V�X�e��
	MarkingSystem m_markingSystem;
	//�}�[�L���O���������A����X�p��
	int m_markingSpan = 10;
	//�}�[�L���O���Ԍv���p
	int m_markingTimer = 0;
	//�}�[�L���O���̈ړ��̑���
	float m_whileMarkingMoveSpeed = 0.2f;
	//�ő�}�[�L���O��
	//int m_maxMarkingCount = 10;
	int m_maxMarkingCount = 200;

/*--- ���b�V�� ---*/
	//���b�V���V�X�e��
	RushSystem m_rushSystem;
	//�P�����b�V���̃`���[�W���̈ړ��̑���
	float m_whileChargeMoveSpeed = 0.2f;

/*--- �P�����b�V�� ---*/
	int m_singleRushTimer = 0;
	int m_singleRushChargeTime = 60;

/*--- �R�[���o�b�N ---*/
	//�����߂�
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

/*--- �A�j���[�V���� ---*/
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
		"Attack_",	//�U���A�j���[�V�����͕�������̂Ő擪�����񂾂�
		"",
		"",
	};
	const std::string& GetAnimName(const ANIM_TYPE& Type);

/*--- HP ---*/
	//���݂�HP
	int m_hp;
	//���݂̍ő�HP
	int m_maxHp = 100;

/*--- ���̑� ---*/
	//�U�������N���X
	PlayerAttack m_attack;
	//�P�t���[���O�ɍU�����͂�����
	bool m_oldAttackInput;

public:
	Player();
	~Player() { s_instanced = false; }
	void Init();
	void Update(UsersInput& Input, ControllerConfig& Controller, const float& Gravity, const Vec2<float>& WinSize);
	void Draw(Camera& Cam, LightManager& LigManager, std::shared_ptr<CubeMap>AttachCubeMap = nullptr);
	void DrawHUD(Camera& Cam, const Vec2<float>& WinSize);
	std::weak_ptr<ModelObject>GetModelObj() { return m_model; }

	//imgui�f�o�b�O�@�\
	void ImguiDebug();

	//���b�V���V�X�e���̃Q�b�^
	const RushSystem& GetRushSys()const { return m_rushSystem; }
	//Transform�̃Q�b�^
	const Transform& GetTransform()const;
};