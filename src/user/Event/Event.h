#pragma once
#include<memory>
class TimeScale;
class Camera;

class Event
{
	static Event* s_nowEvent;

public:
	const bool m_collision;

protected:
	Event(bool arg_collision) :m_collision(arg_collision) {}

	virtual void OnStart() = 0;
	virtual void OnUpdate(TimeScale& arg_timeScale) = 0;
	virtual void OnFinish() = 0;
	virtual bool End() = 0;
	virtual std::shared_ptr<Camera> GetMainCam() { return nullptr; }
	virtual std::shared_ptr<Camera> GetSubCam() { return nullptr; }

	void Update(TimeScale& arg_timeScale)
	{
		OnUpdate(arg_timeScale);
	}

public:
	void Start()
	{
		if (s_nowEvent != nullptr)return;
		s_nowEvent = this;
		OnStart();
	}

	static void StaticInit() { s_nowEvent = nullptr; }
	static void StaticUpdate(TimeScale& arg_timeScale)
	{
		if (s_nowEvent == nullptr)return;

		s_nowEvent->Update(arg_timeScale);

		if (s_nowEvent->End()) 
		{
			s_nowEvent->OnFinish();
			s_nowEvent = nullptr;
		}
	}

	static bool CollisionFlg() 
	{
		if (s_nowEvent == nullptr)return true;
		return s_nowEvent->m_collision;
	}
	static std::shared_ptr<Camera>GetMainEventCamera()
	{
		if (s_nowEvent == nullptr)return nullptr;
		return s_nowEvent->GetMainCam();
	}
	static std::shared_ptr<Camera>GetSubEventCamera()
	{
		if (s_nowEvent == nullptr)return nullptr;
		return s_nowEvent->GetSubCam();
	}
};