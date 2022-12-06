#pragma once
#include<memory>
class TimeScale;
class Camera;

//イベントクラス
class Event
{
	//進行中のイベント
	static Event* s_nowEvent;

	//イベント進行中のパラメータ
	//衝突判定フラグ
	const bool m_collision;

protected:
	//イベント中のパラメータを設定
	Event(bool arg_collision) :m_collision(arg_collision) {}

	//イベント開始時の処理
	virtual void OnStart() = 0;
	//イベントの更新処理
	virtual void OnUpdate() = 0;
	//イベント終了時の処理
	virtual void OnFinish() = 0;
	//イベント終了条件
	virtual bool End() = 0;
	//専用のメイン（近景）イベントカメラゲッタ（デフォルト：なし）
	virtual std::shared_ptr<Camera> GetMainCam() { return nullptr; }
	//専用のサブ（遠景）イベントカメラゲッタ（デフォルト：なし）
	virtual std::shared_ptr<Camera> GetSubCam() { return nullptr; }

	void Update()
	{
		OnUpdate();
	}

public:
	virtual ~Event() {}
	//イベント開始
	void Start()
	{
		if (s_nowEvent != nullptr)return;
		s_nowEvent = this;
		OnStart();
	}

	//イベントの初期化
	static void StaticInit() { s_nowEvent = nullptr; }

	//進行中のイベントの更新
	static void StaticUpdate()
	{
		if (s_nowEvent == nullptr)return;

		s_nowEvent->Update();

		if (s_nowEvent->End()) 
		{
			s_nowEvent->OnFinish();
			s_nowEvent = nullptr;
		}
	}

	//当たり判定を行うか
	static bool CollisionFlg() 
	{
		if (s_nowEvent == nullptr)return true;
		return s_nowEvent->m_collision;
	}

	//メイン（近景）イベントカメラゲッタ
	static std::shared_ptr<Camera>GetMainEventCamera()
	{
		if (s_nowEvent == nullptr)return nullptr;
		return s_nowEvent->GetMainCam();
	}
	//サブ（遠景）イベントカメラゲッタ
	static std::shared_ptr<Camera>GetSubEventCamera()
	{
		if (s_nowEvent == nullptr)return nullptr;
		return s_nowEvent->GetSubCam();
	}
};