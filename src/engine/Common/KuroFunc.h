#pragma once
#include<string>
#include"Vec.h"
#include"Angle.h"
#include"KuroMath.h"
#include<memory>

namespace KuroFunc
{
	bool LoadData(FILE* Fp, void* Data, const size_t& Size, const int& ElementNum);
	bool SaveData(FILE* Fp, const void* Data, const size_t& Size, const int& ElementNum);

	std::wstring GetWideStrFromStr(const std::string& Str);
	std::wstring GetWideStrFromStr(const char* Str);

	std::string GetExtension(const std::string& Path);

	void GetDivideStr(const std::string& Str, std::string* Dir, std::string* FileName);

	Vec2<float> ConvertWorldToScreen(Vec3<float> WorldPos,
		const Matrix& ViewMat, const Matrix& ProjMat, const Vec2<float>&WinSize);
	Vec3<float> ConvertScreenToWorld(Vec2<float> ScreenPos, float Z,
		const Matrix& ViewMat, const Matrix& ProjMat, const Vec2<int>&WinSize);

	//���S���W����ʓ��ɉf���Ă��邩
	bool InScreen(Vec2<float>ScreenPos, Vec2<float> WinSize);
	bool InScreen(Vec3<float> WorldPos, const Matrix& ViewMat, const Matrix& ProjMat, Vec2<float> WinSize);

	bool ExistFile(const std::string FilePass);

	//�x�W�G�p
	float GetYFromXOnBezier(float x, const Vec2<float>& a, const Vec2<float>& b, uint8_t n);
	//�x�W�G(����_�w��)
	float GetBezierFromControlPoint(float t, int ControlPointNum, float* ControlPointArray);

	//����
	int GetRand(int Min, int Max);
	int GetRand(int Max);
	float GetRand(float Min, float Max);
	Vec2<float> GetRand(Vec2<float> Min, Vec2<float> Max);
	Vec3<float> GetRand(Vec3<float> Min, Vec3<float> Max);
	float GetRand(float Max);
	Vec2<float> GetRand(Vec2<float> Max);
	Vec3<float> GetRand(Vec3<float> Max);
	//���������_��
	int GetRandPlusMinus();
	//��̒l����O�㗐���Z�o
	int GetRandFromCenter(int CenterNum, int Range);


	//�w�肵�����̐��������o��
	int GetSpecifiedDigitNum(int From, int Digit);
	//�������Z�o
	int GetDigit(int Num);

	//�����擾
	int GetNumSign(int Num);
	int GetNumSign(float Num);

	//�f�[�^�ǂݍ���
	void LoadData(FILE* Fp, std::string DataName, void* Data, size_t Size, int ElementNum);
	//�f�[�^�ۑ�
	void SaveData(FILE* Fp, std::string DataName, const void* Data, size_t Size, int ElementNum);


}