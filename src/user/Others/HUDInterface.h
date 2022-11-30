#pragma once
class HUDInterface
{
	virtual void OnDraw2D() = 0;
public:
	virtual ~HUDInterface() {}
	static bool s_draw;

	void Draw2D()
	{
		if (!s_draw)return;
		OnDraw2D();
	}
};