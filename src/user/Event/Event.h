#pragma once
#include<memory>
class TimeScale;
class Camera;

//イベントクラス
class Event
{
	//進行中のイベント
	static Event* s_nowEvent;
	//次のイベント
	static Event* s_nextEvent;

protected:
	//イベント中のパラメータを設定
	Event() {}

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
	void SetNextEvent()
	{
		if (s_nextEvent != nullptr)return;
		s_nextEvent = this;
		OnStart();
	}

	//イベントの初期化
	static void StaticInit() 
	{ 
		s_nowEvent = nullptr; 
		s_nextEvent = nullptr;
	}

	//進行中のイベントの更新
	static void StaticUpdate()
	{
		//イベント進行中でない
		if (s_nowEvent == nullptr)
		{
			//次のイベントがセットされている
			if (s_nextEvent != nullptr)
			{
				//次のイベント開始
				s_nowEvent = s_nextEvent;
				s_nowEvent->OnStart();
				s_nextEvent = nullptr;
			}
			return;
		}

		s_nowEvent->Update();

		if (s_nowEvent->End()) 
		{
			s_nowEvent->OnFinish();
			s_nowEvent = nullptr;
		}
	}

	//イベントが進行中か
	bool IsProceed()const
	{
		return s_nowEvent != nullptr && s_nowEvent == this;
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