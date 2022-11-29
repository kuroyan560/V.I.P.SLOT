#pragma once
#include<array>
#include<vector>
#include<memory>
#include"Transform.h"
#include"Timer.h"
class ModelObject;
class CollisionManager;
class CollisionSphere;
class Collider;
class TimeScale;
class LightManager;
class Camera;
class CollisionCallBack;
class IndividualDrawParameter;

class YoYo
{
	//���[���[���f��
	std::shared_ptr<ModelObject>m_modelObj;

	//�����w��p�g�����X�t�H�[��
	Transform m_vecTransform;

	//�R���C�_�[
	//N�U��
	std::shared_ptr<CollisionSphere>m_neutralColSphere;
	std::shared_ptr<Collider>m_neutralCol;
	//����
	std::shared_ptr<CollisionSphere>m_throwColSphere;
	std::shared_ptr<Collider>m_throwCol;

	//��ԑJ��
	enum STATUS
	{
		THROW_0,	//����_0
		THROW_1,	//����_1
		THROW_2,	//����_2
		NEUTRAL,	//N�U��
		STATUS_NUM,
		HAND
	}m_status;

	//�X�e�[�^�X���Ƃ̐ݒ荀��
	struct StatusParameter
	{
		//�Đ�����A�j���[�V������
		std::string m_animName = "";
		//�X�e�[�^�X�I������
		int m_finishInterval = 60;
		//���f���͂��\�ɂȂ鎞��
		int m_interruptInterval = 60;
		//�ő�����x�X�J���[
		Vec3<float>m_maxAccelVal = { 0,0,0 };
		//�U���̃C���^�[�o��
		int m_attackGapInterval = 0;
	};
	std::array<StatusParameter, STATUS_NUM>m_statusParams;

	//3�A���̗\�����
	bool m_tripleAttackPreviousInput = false;

	//�X�e�[�^�X�I���v���p�^�C�}�[
	Timer m_timer;

	//�U���̐����ɂ������x
	Vec3<float>m_accel;
	//�U���g���K�[���̉����x�i�����l�j
	Vec3<float>m_iniAccel;

	//�C���^�[�o���i�U���̌��j
	Timer m_attackGapInterval;
	//�C���^�[�o�����̗\����͎��
	enum PREVIOUS_INPUT { NONE_INPUT, THROW_INPUT, NEUTRAL_INPUT };
	//�C���^�[�o�����̗\�����
	PREVIOUS_INPUT m_attackGapPreviousInput = NONE_INPUT;
	//�C���^�[�o�����̗\����͂��󂯕t����^�C�~���O�i�c�莞�ԁj
	int m_canAttackGapPreviousInputLeftTime = 10;

	//���݂̍U�������f�\��
	bool CanInterrupt()
	{
		return (float)m_statusParams[(int)m_status].m_interruptInterval < m_timer.GetElaspedTime();
	}
	//�C���^�[�o���i�U���̌��j����
	bool IsAttackGapInterval(PREVIOUS_INPUT arg_input);

	//�X�e�[�^�X�̓��I�X�V���ɌĂяo��
	void StatusInit(Vec3<float>arg_accelVec);

public:
	YoYo(
		std::weak_ptr<CollisionManager>arg_collisionMgr,
		Transform* arg_playerTransform,
		std::weak_ptr<CollisionCallBack>arg_attackCallBack,
		std::weak_ptr<CollisionCallBack>arg_parryCallBack);

	/// <summary>
	/// ���[���[�̌Œ�p�����[�^�ݒ�i���������j
	/// </summary>
	/// <param name="arg_neutralColSphereRadius">�����蔻��p���̔��a�iN�U���j</param>
	/// <param name="arg_throwColSphereRadius">�����蔻��p���̔��a�i�����j</param>
	void Awake(float arg_neutralColSphereRadius, float arg_throwColSphereRadius);

	//������
	void Init();
	//�X�V
	void Update(const TimeScale& arg_timeScale, float arg_playersVecX);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam, const IndividualDrawParameter& arg_toonParam);

	//imgui�f�o�b�O
	void AddImguiDebugItem();

	/// <summary>
	/// ����
	/// </summary>
	/// <param name="arg_vecX">X���ɑ΂��铊������</param>
	/// <returns>����������</returns>
	bool Throw(float arg_vecX);

	/// <summary>
	/// N�U��
	/// </summary>
	/// <returns>����������</returns>
	bool Neutral();

	//��������
	bool IsThrow()const { return THROW_0 <= m_status && m_status <= THROW_2; }

	//�U������
	bool IsInvincible()const { return m_status == NEUTRAL; }

	//�󒆏�Ԃ��i�������x���Z���邩�j
	bool IsAir()const { return m_status == THROW_0 || m_status == THROW_1; }

	//�����̉����x�Q�b�^
	const float& GetAccelX()const { return m_accel.x; }
	const float& GetAccelY()const { return m_accel.y; }
};