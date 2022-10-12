#pragma once
#include<memory>
class Model;
class SlotMachine;
class Transform;
class Camera;
class LightManager;

class Block
{
protected:
	//�@���ꂽ��
	int m_hitCount;

	//�����������ŌĂяo�����
	virtual void OnInit() = 0;
	//�X�V�����ŌĂяo�����
	virtual void OnUpdate() = 0;
	//�`�揈���ŌĂяo�����
	virtual void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam) = 0;

	//�@���ꂽ�u�ԌĂяo�����
	virtual void OnHitTrigger() = 0;

public:
	enum TYPE { COIN, SLOT };

	virtual ~Block() {}
	//������
	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw(Transform& arg_transform, std::weak_ptr<LightManager>&arg_lightMgr, std::weak_ptr<Camera>&arg_cam);

	//�@���ꂽ�u�ԌĂԏ���
	void HitTrigger();

	//���S����
	virtual bool IsDead() = 0;
	virtual TYPE GetType() = 0;
};

class CoinBlock : public Block
{
	void OnInit()override {}
	void OnUpdate()override {}
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override {}
public:
	CoinBlock();

	//�R�C�����������Ă��邩�i��x�ł��@����Ă�������������ĂȂ��j
	bool IsEmpty() { return m_hitCount; }

	bool IsDead()override { return false; }
	TYPE GetType()override { return COIN; }
};

class SlotBlock : public Block
{
	//�X���b�g�}�V���|�C���^
	std::weak_ptr<SlotMachine>m_slotMachinePtr;

	void OnInit()override;
	void OnUpdate()override;
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override;
public:
	SlotBlock(const std::shared_ptr<SlotMachine>& arg_slotMachine);

	bool IsDead()override;
	TYPE GetType()override { return SLOT; }

};