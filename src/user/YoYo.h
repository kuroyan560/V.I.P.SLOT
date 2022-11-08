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
	};
	std::array<StatusParameter, STATUS_NUM>m_statusParams;

	//�\�����
	bool m_previousInput = false;

	//�X�e�[�^�X�I���v���p�^�C�}�[
	Timer m_timer;

	//���݂̍U�������f�\��
	bool CanInterrupt()
	{
		return (float)m_statusParams[(int)m_status].m_interruptInterval < m_timer.GetElaspedTime();
	}

public:

	YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr, Transform* arg_playerTransform);

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
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//imgui�f�o�b�O
	void AddImguiDebugItem();

	//����
	void Throw(float arg_vecX);

	//N�U��
	void Neutral();

	//�U������
	bool IsActive() { return m_status != HAND; }
};