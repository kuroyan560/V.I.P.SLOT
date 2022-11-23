#include<windows.h>
#include"KuroEngine.h"
#include"InGameScene.h"
#include"TitleScene.h"
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
//Windows�A�v���ł̃G���g���[�|�C���g�imain�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
	//�G���W���ݒ�=====================
	EngineOption engineOption;
	engineOption.m_windowName = "V.I.P.SLOT";
	engineOption.m_windowSize = { 1280,720 };
	engineOption.m_iconPath = nullptr;
	engineOption.m_backBuffClearColor = Color(0, 0, 0, 0);
	engineOption.m_useHDR = false;
	engineOption.m_frameRate = 60;

	//===============================

	KuroEngine engine;

	//�G���W���N��
	engine.Initialize(engineOption);

	//�V�[�����X�g=====================

	std::vector<BaseScene*>sceneList =
	{
		new TitleScene(),
		new InGameScene(),
		new OutGameScene()
	};
	int awakeScene = 0;	//�J�n���̃X�e�[�W�ԍ�

	//================================

	//�G���W���ɃV�[�����X�g��n��
	engine.SetSceneList(sceneList, awakeScene);

	//�ÓI�`��N���X������
	BasicDraw::Awake(engineOption.m_windowSize.Float());

	bool winEnd = false;

	//���[�v
	while (1)
	{
		//���b�Z�[�W������H
		MSG msg{};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	//�L�[���̓��b�Z�[�W�̏���
			DispatchMessage(&msg);	//�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
			if (msg.message == WM_QUIT)
			{
				//�E�B���h�E������ꂽ
				winEnd = true;
				break;
			}
		}

		//�I�����b�Z�[�W������ / �V�[���}�l�[�W���̏I���@�Ń��[�v�𔲂���
		if (winEnd || engine.End())
		{
			break;
		}

		engine.Update();
		engine.Draw();

		//�ÓI�N���X�������iDirty�t���O�n�j
		Transform::DirtyReset();
		Transform2D::DirtyReset();

		DrawFunc2D::CountReset();
		DrawFunc2D_Shadow::CountReset();
		DrawFunc2D_FillTex::CountReset();
		DrawFunc2D_Mask::CountReset();
		DrawFunc2D_Color::CountReset();

		DrawFunc3D::CountReset();

		DrawFuncBillBoard::CountReset();

		BasicDraw::CountReset();
	}

	return 0;
}