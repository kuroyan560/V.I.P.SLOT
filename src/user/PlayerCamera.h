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
	float m_dist = 5.0f;		//�v���C���[�Ƃ̋���
	bool m_mirrorX = false;		//X���̓~���[
	bool m_mirrorY = false;		//Y���̓~���[

	//�ʒu�̊p�x
	Angle m_posAngle;

	//�J�����̍���
	float m_height;

	//���b�N�I���Ώ�
	std::weak_ptr<ActPoint>m_rockOnPoint;
	//���b�N�I���\�ȃJ�������W�Ƃ̋����̏���i�RD�j
	float m_canRockOnDist3D;
	//���b�N�I���\�ȉ�ʒ����Ƃ̋����̏���i�QD�j
	float m_canRockOnDist2D;
	//���b�N�I�����e�p�x
	Angle m_rockOnAngleRange;
	//���b�N�I���Ə��e�N�X�`��
	std::shared_ptr<TextureBuffer>m_reticleTex;

	//���b�N�I����Ɍq���铖���蔻�����
	std::shared_ptr<CollisionLine>m_rockOnColLine;
	std::shared_ptr<Collider>m_rockOnCollider;

	//���b�N�I���Ώېݒ�
	void SetRockOnTarget(std::shared_ptr<ActPoint> Target);
	//���W�v�Z
	void CalculatePos(const Transform& Player);
	//���b�N�I���Ώۂɍ��킹�ăJ�����𓮂���
	void RockOnTargeting(Vec3<float>PlayerPos);
	//���b�N�I�������蔻��p�����̃R�[���o�b�N�֐�
	void OnCollision(const Vec3<float>& Inter, std::weak_ptr<Collider> OtherCollider)override
	{
		//���b�N�I���Ώۂ̃R���C�_�[�Ȃ疳��
		if (this->m_rockOnCollider->CompareID(OtherCollider))return;
		//�����ɎՂ�ꂽ�烍�b�N�I������
		SetRockOnTarget(nullptr);
	}

public:
	PlayerCamera();
	void Init(const Transform& Player);
	void Update(const Transform& Player, Vec2<float> InputVec);
	void Draw(Camera& NowCam);

	//�v���C���[���ʂɃJ���������킹��
	void LookAtPlayersFront(const Transform& Player);

	//���b�N�I��
	void RockOn(const Transform& Player);

	//�Q�b�^
	std::shared_ptr<Camera>GetCam() { return m_cam; }
	const Angle& GetPosAngle() { return m_posAngle; }

	//imgui�f�o�b�O�@�\
	void OnImguiDebug()override;
};

