#pragma once
#include<functional>
#include"Vec.h"
#include<vector>

#include<DirectXMath.h>
using namespace DirectX;
using Matrix = XMMATRIX;

bool operator!=(const Matrix& lhs, const Matrix& rhs);
bool operator==(const Matrix& lhs, const Matrix& rhs);

#include"Angle.h"

enum EASE_CHANGE_TYPE
{
    In, Out, InOut, OutIn, EASE_CHANGE_TYPE_NUM
};
enum EASING_TYPE
{
    Liner,Quad, Cubic, Quart, Quint, Sine, Exp, Circ, Elastic, Back, Bounce, EASING_TYPE_NUM
};

class KuroMath
{
private:
    static float LinerFunc(float t, float totaltime, float min, float max);

    static float QuadIn(float t, float totaltime, float min, float max);
    static float QuadOut(float t, float totaltime, float min, float max);
    static float QuadInOut(float t, float totaltime, float min, float max);
    static float QuadOutIn(float t, float totaltime, float min, float max);

    static float CubicIn(float t, float totaltime, float min, float max);
    static float CubicOut(float t, float totaltime, float min, float max);
    static float CubicInOut(float t, float totaltime, float min, float max);
    static float CubicOutIn(float t, float totaltime, float min, float max);

    static float QuartIn(float t, float totaltime, float min, float max);
    static float QuartOut(float t, float totaltime, float min, float max);
    static float QuartInOut(float t, float totaltime, float min, float max);
    static float QuartOutIn(float t, float totaltime, float min, float max);

    static float QuintIn(float t, float totaltime, float min, float max);
    static float QuintOut(float t, float totaltime, float min, float max);
    static float QuintInOut(float t, float totaltime, float min, float max);
    static float QuintOutIn(float t, float totaltime, float min, float max);

    static float SineIn(float t, float totaltime, float min, float max);
    static float SineOut(float t, float totaltime, float min, float max);
    static float SineInOut(float t, float totaltime, float min, float max);
    static float SineOutIn(float t, float totaltime, float min, float max);

    static float ExpIn(float t, float totaltime, float min, float max);
    static float ExpOut(float t, float totaltime, float min, float max);
    static float ExpInOut(float t, float totaltime, float min, float max);
    static float ExpOutIn(float t, float totaltime, float min, float max);

    static float CircIn(float t, float totaltime, float min, float max);
    static float CircOut(float t, float totaltime, float min, float max);
    static float CircInOut(float t, float totaltime, float min, float max);
    static float CircOutIn(float t, float totaltime, float min, float max);

    static float ElasticIn(float t, float totaltime, float min, float max);
    static float ElasticOut(float t, float totaltime, float min, float max);
    static float ElasticInOut(float t, float totaltime, float min, float max);
    static float ElasticOutIn(float t, float totaltime, float min, float max);

    static float BackIn(float t, float totaltime, float min, float max);
    static float BackOut(float t, float totaltime, float min, float max);
    static float BackInOut(float t, float totaltime, float min, float max);
    static float BackOutIn(float t, float totaltime, float min, float max);

    static float BounceIn(float t, float totaltime, float min, float max);
    static float BounceOut(float t, float totaltime, float min, float max);
    static float BounceInOut(float t, float totaltime, float min, float max);
    static float BounceOutIn(float t, float totaltime, float min, float max);

    //ラムダ式のための型
    using EasingFunction = std::function<float(float t, float totaltime, float min, float max)>;
    static EasingFunction easing[EASE_CHANGE_TYPE_NUM][EASING_TYPE_NUM];

    static float GetSplineSection(const float& p0, const float& p1, const float& p2, const float& p3, const float& t);
public:
    static float Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, float Min, float Max);
    static Vec2<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, Vec2<float> Min, Vec2<float> Max);
    static Vec3<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, Vec3<float> Min, Vec3<float> Max);
    static Vec4<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, Vec4<float> Min, Vec4<float> Max);
    
    static float Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, float Min, float Max);
    static Vec2<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, Vec2<float> Min, Vec2<float> Max);
    static Vec3<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, Vec3<float> Min, Vec3<float> Max);
    static Vec4<float> Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, Vec4<float> Min, Vec4<float> Max);
    
    static float Lerp(float Min, float Max, float Rate)
    {
        return Ease(In, Liner, Rate, Min, Max);
    }
    static Vec2<float> Lerp(Vec2<float> Min, Vec2<float> Max, float Rate)
    {
        return Ease(In, Liner, Rate, Min, Max);
    }
    static Vec3<float> Lerp(Vec3<float> Min, Vec3<float> Max, float Rate)
    {
        return Ease(In, Liner, Rate, Min, Max);
    }
    static Vec4<float> Lerp(Vec4<float> Min, Vec4<float> Max, float Rate)
    {
        return Ease(In, Liner, Rate, Min, Max);
    }

    static float Lerp(float Min, float Max, float T, float TotalTime)
    {
        return Ease(In, Liner, T, TotalTime, Min, Max);
    }
    static Vec2<float> Lerp(Vec2<float> Min, Vec2<float> Max, float T, float TotalTime)
    {
        return Ease(In, Liner, T, TotalTime, Min, Max);
    }
    static Vec3<float> Lerp(Vec3<float> Min, Vec3<float> Max, float T, float TotalTime)
    {
        return Ease(In, Liner, T, TotalTime, Min, Max);
    }
    static Vec4<float> Lerp(Vec4<float> Min, Vec4<float> Max, float T, float TotalTime)
    {
        return Ease(In, Liner, T, TotalTime, Min, Max);
    }

    static float GetSpline(const float& T, const int& P1Idx, const std::vector<float>& Array, bool Loop = false);
    static float GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<float>& Array, bool Loop = false);
    static Vec2<float> GetSpline(const float& T, const int& P1Idx, const std::vector<Vec2<float>>& Array, bool Loop = false);
    static Vec2<float> GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<Vec2<float>>& Array, bool Loop = false);
    static Vec3<float> GetSpline(const float& T, const int& P1Idx, const std::vector<Vec3<float>>& Array, bool Loop = false);
    static Vec3<float> GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<Vec3<float>>& Array, bool Loop = false);
    static Vec4<float> GetSpline(const float& T, const int& P1Idx, const std::vector<Vec4<float>>& Array, bool Loop = false);
    static Vec4<float> GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<Vec4<float>>& Array, bool Loop = false);

    //x,y,zの角度から回転行列を生成
    static Matrix RotateMat(const Angle& X, const Angle& Y, const Angle& Z);
    //軸と角度を指定して回転行列を生成
    static Matrix RotateMat(const Vec3<float>& Axis, const float& Radian);
    //aをbに向かせる回転行列を求める
    static Matrix RotateMat(const Vec3<float>& VecA, const Vec3<float>& VecB);

    //2Dベクトルを回転させる
    static Vec2<float>RotateVec2(const Vec2<float>& Vec, const float& Radian);

    static Vec2<float> GetCenterVec2(Vec2<float> From, Vec2<float> To);
    static Vec3<float> GetCenterVec3(Vec3<float> From, Vec3<float> To);

    //２点を通る直線の傾き
    static float GetLineSlope(Vec2<float> From, Vec2<float> To);
    //1つのベクトルから角度を取得
    static Angle GetAngle(Vec2<float>Vec);
    //２つのベクトルの角度差を取得
    static Angle GetAngleAbs(Vec2<float>From, Vec2<float>To);

    //Vec3に行列を反映
    static Vec3<float>TransformVec3(const Vec3<float>& Value, const Matrix& Mat);
    //回転軸を指定して回転
    static Vec3<float>TransformVec3(const Vec3<float>& Value, const Vec3<float>& Axis, const Angle& Angle);

    /// <summary>
    /// 指定した値が範囲内においてどの位置にいるか
    /// </summary>
    /// <param name="arg_min">範囲下限（下回る場合は0）</param>
    /// <param name="arg_max">範囲上限（上回る場合は1）</param>
    /// <param name="arg_val">対象の値</param>
    /// <returns>0.0f ~ 1.0f</returns>
    static float GetRateInRange(float arg_min, float arg_max, float arg_val);
    static float GetRateInRange(int arg_min, int arg_max, int arg_val);
};

struct EasingParameter
{
    EASE_CHANGE_TYPE m_changeType = In;
    EASING_TYPE m_easeType = Quad;

    float Calculate(float T, float TotalTime, float Min, float Max)
    {
        return KuroMath::Ease(m_changeType, m_easeType, T, TotalTime, Min, Max);
    }
    float Calculate(float Rate, float Min, float Max)
    {
        return KuroMath::Ease(m_changeType, m_easeType, Rate, Min, Max);
    }
    void ImguiDebug(const std::string& Tag);
};