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
};

class CoinBlock : public Block
{
	static std::shared_ptr<Model>COIN_MODEL;
	static std::shared_ptr<Model>EMPTY_COIN_MODEL;

	void OnInit()override {}
	void OnUpdate()override {}
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override {}
public:
	CoinBlock();

	bool IsDead()override { return false; }
};

class SlotBlock : public Block
{
	static std::shared_ptr<Model>MODEL;

	//�X���b�g�}�V���|�C���^
	std::weak_ptr<SlotMachine>m_slotMachinePtr;

	void OnInit()override;
	void OnUpdate()override;
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override;
public:
	SlotBlock(const std::shared_ptr<SlotMachine>& arg_slotMachine);

	bool IsDead()override;
};