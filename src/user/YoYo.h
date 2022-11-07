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

	//���������
	enum THROW_VEC { LEFT, LEFT_UP, RIGHT_UP, RIGHT, THROW_VEC_NUM };

	//�J�v�Z���̌`�𐬂������蔻��p���z��
	std::vector<std::shared_ptr<CollisionSphere>>m_capsuleSphereArray;
	//�����A�Ȃ��Ăł����J�v�Z���R���C�_�[
	//���J�v�Z����CollisionPrimitive�̎������Ԃ����������Ȃ��̂ŁA�Ƃ肠�������ŁB
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
	/// <param name="arg_length">���[���[�̒���</param>
	/// <param name="arg_colSphereRadius">�����蔻��p���̔��a</param>
	void Awake(float arg_length,float arg_colSphereRadius);

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
};