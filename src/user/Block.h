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
	//叩かれた回数
	int m_hitCount;

	//初期化処理で呼び出される
	virtual void OnInit() = 0;
	//更新処理で呼び出される
	virtual void OnUpdate() = 0;
	//描画処理で呼び出される
	virtual void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam) = 0;

	//叩かれた瞬間呼び出される
	virtual void OnHitTrigger() = 0;

public:
	virtual ~Block() {}
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(Transform& arg_transform, std::weak_ptr<LightManager>&arg_lightMgr, std::weak_ptr<Camera>&arg_cam);

	//叩かれた瞬間呼ぶ処理
	void HitTrigger();

	//死亡判定
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

	//スロットマシンポインタ
	std::weak_ptr<SlotMachine>m_slotMachinePtr;

	void OnInit()override;
	void OnUpdate()override;
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override;
public:
	SlotBlock(const std::shared_ptr<SlotMachine>& arg_slotMachine);

	bool IsDead()override;
};