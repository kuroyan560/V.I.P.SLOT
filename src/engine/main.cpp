#include<windows.h>
#include"KuroEngine.h"
#include"InGameScene.h"
#include"TitleScene.h"
#include"GameOverScene.h"
#include"OutGameScene.h"
#include"Transform.h"
#include"Transform2D.h"
#include"Color.h"

#include"DrawFunc2D.h"
#include"DrawFunc2D_Shadow.h"
#include"DrawFunc2D_FillTex.h"
#include"DrawFunc2D_Mask.h"
#include"DrawFunc2D_Color.h"

#include"DrawFunc3D.h"

#include"DrawFuncBillBoard.h"

#include"BasicDraw.h"


#ifdef _DEBUG
int main()
#else
//Windowsアプリでのエントリーポイント（main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
	//エンジン設定=====================
	EngineOption engineOption;
	engineOption.m_windowName = "V.I.P.SLOT";
	engineOption.m_windowSize = { 1280,720 };
	engineOption.m_iconPath = nullptr;
	engineOption.m_backBuffClearColor = Color(0, 0, 0, 0);
	engineOption.m_useHDR = false;
	engineOption.m_frameRate = 60;

	//===============================

	KuroEngine engine;

	//エンジン起動
	engine.Initialize(engineOption);

	//シーンリスト=====================

	std::map<std::string, BaseScene*>sceneList =
	{
		{"Title",new TitleScene()},
		{"InGame",new InGameScene()},
		{"GameOver",new GameOverScene()},
		{"OutGame",new OutGameScene()},
	};
	std::string awakeScene = "Title";	//開始時のシーンキー

	//================================

	//エンジンにシーンリストを渡す
	engine.SetSceneList(sceneList, awakeScene);

	//静的描画クラス初期化
	BasicDraw::Instance()->Awake(engineOption.m_windowSize.Float());

	bool winEnd = false;

	//ループ
	while (1)
	{
		//メッセージがある？
		MSG msg{};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	//キー入力メッセージの処理
			DispatchMessage(&msg);	//プロシージャにメッセージを送る
			if (msg.message == WM_QUIT)
			{
				//ウィンドウが閉じられた
				winEnd = true;
				break;
			}
		}

		//終了メッセージが来た / シーンマネージャの終了　でループを抜ける
		if (winEnd || engine.End())
		{
			break;
		}

		engine.Update();
		engine.Draw();

		//静的クラス初期化（Dirtyフラグ系）
		Transform::DirtyReset();
		Transform2D::DirtyReset();

		DrawFunc2D::CountReset();
		DrawFunc2D_Shadow::CountReset();
		DrawFunc2D_FillTex::CountReset();
		DrawFunc2D_Mask::CountReset();
		DrawFunc2D_Color::CountReset();

		DrawFunc3D::CountReset();

		DrawFuncBillBoard::CountReset();

		BasicDraw::Instance()->CountReset();
	}

	return 0;
}