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

	//�����蔻��p��
	//N�U��
	std::shared_ptr<CollisionSphere>m_neutralColSphere;
	std::shared_ptr<Collider>m_neutralCol;
	//�R���C�_�[
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

	//�X�e�[�^�X�I���v���^�C�}�[
	std::array<int, STATUS_NUM>m_finishInterval = { 90,90,90,90 };

	//�X�e�[�^�X�I���v���p�^�C�}�[
	Timer m_timer;

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
	void Update(const TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//imgui�f�o�b�O
	void AddImguiDebugItem();

	//������
	void Throw(Vec2<float>arg_vec);

	//�U������
	bool IsActive() { return m_status != HAND; }
};