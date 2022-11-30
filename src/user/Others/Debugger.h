#pragma once
#include<string>
#include<vector>
#include"ImguiApp.h"
//imguiデバッグのインターフェース
class Debugger
{
private:
	//割り当てる識別番号
	static int s_id;
	//起動中のデバッガ配列
	static std::vector<Debugger*>s_debuggerArray;

public:
	virtual ~Debugger() {}
	//デバッグ機構表示
	static void Draw();
	//デバッガ登録
	static void Register(std::vector<Debugger*>arg_debuggerArray)
	{
		s_debuggerArray = arg_debuggerArray;
	}
	//デバッガ登録解除
	static void ClearRegister() { s_debuggerArray.clear(); }

private:
	//識別番号
	int m_id;
	//アクティブ状態
	bool m_active = false;

protected:
	//imguiウィンドウ名
	std::string m_title;
	Debugger(std::string arg_title) :m_title(arg_title), m_id(s_id++) {}

	//imguiの項目 Begin ~ End 間に呼び出す処理
	virtual void OnImguiItems() = 0;
};