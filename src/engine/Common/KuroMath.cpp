#include "KuroMath.h"
#include<cmath>

#pragma region Easing
KuroMath::EasingFunction KuroMath::easing[EASE_CHANGE_TYPE_NUM][EASING_TYPE_NUM] =
{
    {LinerFunc,QuadIn,CubicIn,QuartIn,QuintIn,SineIn,ExpIn,CircIn,ElasticIn,BackIn,BounceIn},
     {LinerFunc,QuadOut,CubicOut,QuartOut,QuintOut,SineOut,ExpOut,CircOut,ElasticOut,BackOut,BounceOut},
     {LinerFunc,QuadInOut,CubicInOut,QuartInOut,QuintInOut,SineInOut,ExpInOut,CircInOut,ElasticInOut,BackInOut,BounceInOut},
     {LinerFunc,QuadOutIn,CubicOutIn,QuartOutIn,QuintOutIn,SineOutIn,ExpOutIn,CircOutIn,ElasticOutIn,BackOutIn,BounceOutIn}
};

const float PI = 3.14159265359f;

float KuroMath::LinerFunc(float t, float totaltime, float min, float max)
{
    float rate = t / totaltime;
    if (abs(min - max) < 0.001f)return max;
    return (1 - rate) * min + rate * max;
}

float KuroMath::QuadIn(float t, float totaltime, float min, float max)
{
    max -= min;
    t /= totaltime;
    return max * t * t + min;
}

float KuroMath::QuadOut(float t, float totaltime, float min, float max)
{
    max -= min;
    t /= totaltime;
    return -max * t * (t - 2) + min;
}

float KuroMath::QuadInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return QuadIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return QuadOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::QuadOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return QuadOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return QuadIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::CubicIn(float t, float totaltime, float min, float max)
{
    max -= min;
    t /= totaltime;
    return max * t * t * t + min;
}

float KuroMath::CubicOut(float t, float totaltime, float min, float max)
{
    max -= min;
    t = t / totaltime - 1;
    return max * (t * t * t + 1) + min;
}

float KuroMath::CubicInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return CubicIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return CubicOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::CubicOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return CubicOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return CubicIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::QuartIn(float t, float totaltime, float min, float max)
{
    max -= min;
    t /= totaltime;
    return max * t * t * t * t + min;
}

float KuroMath::QuartOut(float t, float totaltime, float min, float max)
{
    max -= min;
    t = t / totaltime - 1;
    return -max * (t * t * t * t - 1) + min;
}

float KuroMath::QuartInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return QuartIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return QuartOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::QuartOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return QuartOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return QuartIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::QuintIn(float t, float totaltime, float min, float max)
{
    max -= min;
    t /= totaltime;
    return max * t * t * t * t * t + min;
}

float KuroMath::QuintOut(float t, float totaltime, float min, float max)
{
    max -= min;
    t = t / totaltime - 1;
    return max * (t * t * t * t * t + 1) + min;
}

float KuroMath::QuintInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return QuintIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return QuintOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::QuintOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return QuintOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return QuintIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::SineIn(float t, float totaltime, float min, float max)
{
    max -= min;
    return -max * cos(t * (PI * 90 / 180) / totaltime) + max + min;
}

float KuroMath::SineOut(float t, float totaltime, float min, float max)
{
    max -= min;
    return max * sin(t * (PI * 90 / 180) / totaltime) + min;
}

float KuroMath::SineInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return SineIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return SineOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::SineOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return SineOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return SineIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::ExpIn(float t, float totaltime, float min, float max)
{
    max -= min;
    return static_cast<float>(t == 0.0 ? min : max * pow(2, 10 * (t / totaltime - 1)) + min);
}

float KuroMath::ExpOut(float t, float totaltime, float min, float max)
{
    max -= min;
    return static_cast<float>(t == totaltime ? max + min : max * (-pow(2, -10 * t / totaltime) + 1) + min);
}

float KuroMath::ExpInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return ExpIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return ExpOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::ExpOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return ExpOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return ExpIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::CircIn(float t, float totaltime, float min, float max)
{
    max -= min;
    t /= totaltime;
    return -max * (sqrt(1 - t * t) - 1) + min;
}

float KuroMath::CircOut(float t, float totaltime, float min, float max)
{
    max -= min;
    t = t / totaltime - 1;
    return max * sqrt(1 - t * t) + min;
}

float KuroMath::CircInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return CircIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return CircOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::CircOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return CircOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return CircIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::ElasticIn(float t, float totaltime, float min, float max)
{
    max -= min;
    t /= totaltime;

    float s = 1.70158f;
    float p = totaltime * 0.3f;
    float a = max;

    if (t == 0) return min;
    if (t == 1) return min + max;

    if (a < abs(max))
    {
        a = max;
        s = p / 4;
    }
    else
    {
        s = p / (2 * PI) * asin(max / a);
    }

    t = t - 1;
    return static_cast<float>(-(a * pow(2, 10 * t) * sin((t * totaltime - s) * (2 * PI) / p)) + min);
}

float KuroMath::ElasticOut(float t, float totaltime, float min, float max)
{
    max -= min;
    t /= totaltime;

    float s = 1.70158f;
    float p = totaltime * 0.3f; ;
    float a = max;

    if (t == 0) return min;
    if (t == 1) return min + max;

    if (a < abs(max))
    {
        a = max;
        s = p / 4;
    }
    else
    {
        s = p / (2 * PI) * asin(max / a);
    }

    return static_cast<float>(a * pow(2, -10 * t) * sin((t * totaltime - s) * (2 * PI) / p) + max + min);
}

float KuroMath::ElasticInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return ElasticIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return ElasticOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::ElasticOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return ElasticOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return ElasticIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::BackIn(float t, float totaltime, float min, float max)
{
    float s = 1.70158f;
    max -= min;
    t /= totaltime;
    return max * t * t * ((s + 1) * t - s) + min;
}

float KuroMath::BackOut(float t, float totaltime, float min, float max)
{
    float s = 1.70158f;
    max -= min;
    t = t / totaltime - 1;
    return max * (t * t * ((s + 1) * t + s) + 1) + min;
}

float KuroMath::BackInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return BackIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return BackOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::BackOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return BackOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return BackIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::BounceIn(float t, float totaltime, float min, float max)
{
    max -= min;
    return max - BounceOut(totaltime - t, totaltime, 0, max) + min;
}

float KuroMath::BounceOut(float t, float totaltime, float min, float max)
{
    max -= min;
    t /= totaltime;

    if (t < 1.0f / 2.75f)
    {
        return max * (7.5625f * t * t) + min;
    }
    else if (t < 2.0f / 2.75f)
    {
        t -= 1.5f / 2.75f;
        return max * (7.5625f * t * t + 0.75f) + min;
    }
    else if (t < 2.5f / 2.75f)
    {
        t -= 2.25f / 2.75f;
        return max * (7.5625f * t * t + 0.9375f) + min;
    }
    else
    {
        t -= 2.625f / 2.75f;
        return max * (7.5625f * t * t + 0.984375f) + min;
    }
}

float KuroMath::BounceInOut(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return BounceIn(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return BounceOut(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::BounceOutIn(float t, float totaltime, float min, float max)
{
    if (t < totaltime / 2.0f)
    {
        return BounceOut(t, totaltime / 2.0f, min, max * 0.5f);
    }
    return BounceIn(t - totaltime / 2.0f, totaltime / 2.0f, max * 0.5f, max);
}

float KuroMath::Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, float Min, float Max)
{
    if (Min == Max)return Min;
    if (TotalTime < T)T = TotalTime;
    return easing[EaseChangeType][EasingType](T, TotalTime, Min, Max);
}

Vec2<float> KuroMath::Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, Vec2<float> Min, Vec2<float> Max)
{
    Vec2<float> result;
    result.x = Ease(EaseChangeType, EasingType, T, TotalTime, Min.x, Max.x);
    result.y = Ease(EaseChangeType, EasingType, T, TotalTime, Min.y, Max.y);
    return result;
}

Vec3<float> KuroMath::Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, Vec3<float> Min, Vec3<float> Max)
{
    Vec3<float> result;
    result.x =  Ease(EaseChangeType, EasingType, T, TotalTime, Min.x, Max.x);
    result.y =  Ease(EaseChangeType, EasingType, T, TotalTime, Min.y, Max.y);
    result.z =  Ease(EaseChangeType, EasingType, T, TotalTime, Min.z, Max.z);
    return result;
}

Vec4<float> KuroMath::Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float T, float TotalTime, Vec4<float> Min, Vec4<float> Max)
{
    Vec4<float> result;
    result.x = Ease(EaseChangeType, EasingType, T, TotalTime, Min.x, Max.x);
    result.y = Ease(EaseChangeType, EasingType, T, TotalTime, Min.y, Max.y);
    result.z = Ease(EaseChangeType, EasingType, T, TotalTime, Min.z, Max.z);
    result.w = Ease(EaseChangeType, EasingType, T, TotalTime, Min.w, Max.w);
    return result;
}

float KuroMath::Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, float Min, float Max)
{
    if (Rate < 0.0f)assert(0);
    if (1.0f < Rate)assert(0);
    return Ease(EaseChangeType, EasingType, Rate, 1.0f, Min, Max);
}

Vec2<float> KuroMath::Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, Vec2<float> Min, Vec2<float> Max)
{
    if (Rate < 0.0f)assert(0);
    if (1.0f < Rate)assert(0);
    return Ease(EaseChangeType, EasingType, Rate, 1.0f, Min, Max);
}

Vec3<float> KuroMath::Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, Vec3<float> Min, Vec3<float> Max)
{
    if (Rate < 0.0f)assert(0);
    if (1.0f < Rate)assert(0);
    return Ease(EaseChangeType, EasingType, Rate, 1.0f, Min, Max);
}

Vec4<float> KuroMath::Ease(EASE_CHANGE_TYPE EaseChangeType, EASING_TYPE EasingType, float Rate, Vec4<float> Min, Vec4<float> Max)
{
    if (Rate < 0.0f)assert(0);
    if (1.0f < Rate)assert(0);
    return Ease(EaseChangeType, EasingType, Rate, 1.0f, Min, Max);
}

#pragma endregion

#pragma region Spline

float KuroMath::GetSplineSection(const float& p0, const float& p1, const float& p2, const float& p3, const float& t)
{
    return 0.5f * ((2.0f * p1 + (-p0 + p2) * t) +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * (t * t) +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * (t * t * t));
}

float KuroMath::GetSpline(const float& T, const int& P1Idx, const std::vector<float>& Array, bool Loop)
{
    //?P?_????????????????
    if (Array.size() < 2)assert(0);

    int endIdx = static_cast<int>(Array.size()) - 1;

    int p0_idx = P1Idx - 1;
    int p1_idx = P1Idx;
    int p2_idx = P1Idx + 1;
    int p3_idx = p2_idx;

    if (p1_idx == 0)    //p1??????
    {
        if (Loop)p0_idx = endIdx;   //???[?v??????????p0??????
        else p0_idx = p1_idx;
    }
    else
    {
        p0_idx = p1_idx - 1;    //?????O
    }

    if (p2_idx == endIdx)   //p2??????
    {
        if (Loop)p3_idx = 0;    //???[?v??????????????
        else p3_idx = p2_idx;
    }
    else if (p2_idx == Array.size())
    {
        if (Loop)
        {
            p2_idx = 0;
            p3_idx = 1;
        }
        else
        {
            return Array[endIdx];
        }
    }
    else
    {
        p3_idx = p2_idx + 1;    //??????
    }

    float p0 = Array[p0_idx];
    float p1 = Array[p1_idx];
    float p2 = Array[p2_idx];
    float p3 = Array[p3_idx];

    return GetSplineSection(p0, p1, p2, p3, T);
}

float KuroMath::GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<float>& Array, bool Loop)
{
    float t = (float)Timer / TotalTime;
    return GetSpline(t, P1Idx, Array, Loop);
}

Vec2<float> KuroMath::GetSpline(const float& T, const int& P1Idx, const std::vector<Vec2<float>>& Array, bool Loop)
{
    std::vector<float>x;
    std::vector<float>y;
    for (auto& element : Array)
    {
        x.emplace_back(element.x);
        y.emplace_back(element.y);
    }
    return Vec2<float>(GetSpline(T, P1Idx, x, Loop), GetSpline(T, P1Idx, y, Loop));
}

Vec2<float> KuroMath::GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<Vec2<float>>& Array, bool Loop)
{
    float t = (float)Timer / TotalTime;
    return GetSpline(t, P1Idx, Array, Loop);
}

Vec3<float> KuroMath::GetSpline(const float& T, const int& P1Idx, const std::vector<Vec3<float>>& Array, bool Loop)
{
    std::vector<float>x;
    std::vector<float>y;
    std::vector<float>z;
    for (auto& element : Array)
    {
        x.emplace_back(element.x);
        y.emplace_back(element.y);
        z.emplace_back(element.z);
    }
    return Vec3<float>(GetSpline(T, P1Idx, x, Loop), GetSpline(T, P1Idx, y, Loop), GetSpline(T, P1Idx, z, Loop));
}

Vec3<float> KuroMath::GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<Vec3<float>>& Array, bool Loop)
{
    float t = (float)Timer / TotalTime;
    return GetSpline(t, P1Idx, Array, Loop);
}

Vec4<float> KuroMath::GetSpline(const float& T, const int& P1Idx, const std::vector<Vec4<float>>& Array, bool Loop)
{
    std::vector<float>x;
    std::vector<float>y;
    std::vector<float>z;
    std::vector<float>w;
    for (auto& element : Array)
    {
        x.emplace_back(element.x);
        y.emplace_back(element.y);
        z.emplace_back(element.z);
        w.emplace_back(element.w);
    }
    return Vec4<float>(GetSpline(T, P1Idx, x, Loop), GetSpline(T, P1Idx, y, Loop), GetSpline(T, P1Idx, z, Loop), GetSpline(T, P1Idx, w, Loop));
}

Vec4<float> KuroMath::GetSpline(const int& Timer, const int& TotalTime, const int& P1Idx, const std::vector<Vec4<float>>& Array, bool Loop)
{
    float t = (float)Timer / TotalTime;
    return GetSpline(t, P1Idx, Array, Loop);
}

#pragma endregion

#pragma region Matrix
Matrix KuroMath::RotateMat(const Angle& X, const Angle& Y, const Angle& Z)
{
    return XMMatrixRotationRollPitchYaw(X, Y, Z);
}

Matrix KuroMath::RotateMat(const Vec3<float>& Axis, const float& Radian)
{
    Vec3<float>axis = Axis;
    if (1.0f < axis.Length())axis.Normalize();
    XMVECTOR vec = XMVectorSet(axis.x, axis.y, axis.z, 1.0f);
    auto result = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(vec, Radian));
    return result;
}

Matrix KuroMath::RotateMat(const Vec3<float>& VecA, const Vec3<float>& VecB)
{
    auto a = VecA.GetNormal();
    auto b = VecB.GetNormal();
    XMVECTOR q = { 0,0,0,0 };
    auto c = b.Cross(a);
    auto d = -c.Length();

    float epsilon = 0.0002f;
    auto ip = a.Dot(b);
    if (-epsilon < d || 1.0f < ip)
    {
        if (ip < (epsilon - 1.0f))
        {
            auto a2 = Vec3<float>(-a.y, a.z, a.x);
            c = a2.Cross(a).GetNormal();
            q.m128_f32[0] = c.x;
            q.m128_f32[1] = c.y;
            q.m128_f32[2] = c.z;
            q.m128_f32[3] = 0.0f;
        }
        else
        {
            q = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    else
    {
        auto e = c.GetNormal() * sqrt(0.5f * (1.0f - ip));
        q.m128_f32[0] = e.x;
        q.m128_f32[1] = e.y;
        q.m128_f32[2] = e.z;
        q.m128_f32[3] = sqrt(0.5f * (1.0f + ip));
    }
    return XMMatrixRotationQuaternion(q);
}
Vec2<float> KuroMath::RotateVec2(const Vec2<float>& Vec, const float& Radian)
{
    Vec2<float> result;
    result.x = Vec.x * cos(Radian) - Vec.y * sin(Radian);
    result.y = Vec.y * cos(Radian) + Vec.x * sin(Radian);
    return result;
}

Vec2<float> KuroMath::GetCenterVec2(Vec2<float> From, Vec2<float> To)
{
    Vec2<float> result = To - From;
    result = result * 0.5f;
    result = From + result;
    return result;
}

Vec3<float> KuroMath::GetCenterVec3(Vec3<float> From, Vec3<float> To)
{
    Vec3<float> result = To - From;
    result = result * 0.5f;
    result = From + result;
    return result;
}

float KuroMath::GetLineSlope(Vec2<float> From, Vec2<float> To)
{
    return (To.y - From.y) / (To.x - From.x);
}

Angle KuroMath::GetAngle(Vec2<float> Vec)
{
    return Angle((float)atan2(Vec.y, Vec.x));
}

Angle KuroMath::GetAngleAbs(Vec2<float> From, Vec2<float> To)
{
    return acos(From.Dot(To));
}
Vec3<float> KuroMath::TransformVec3(const Vec3<float>& Value, const Matrix& Mat)
{
    XMVECTOR valVec = XMVectorSet(Value.x, Value.y, Value.z, 1.0f);
    valVec = XMVector4Transform(valVec, Mat);
    return Vec3<float>(valVec.m128_f32[0], valVec.m128_f32[1], valVec.m128_f32[2]);
}

Vec3<float> KuroMath::TransformVec3(const Vec3<float>& Value, const Vec3<float>& Axis, const Angle& Angle)
{
    //???]??
    Vec3<float>axis = Axis;
    if (1.0f < axis.Length())axis.Normalize();
    XMVECTOR axisVec = XMVectorSet(axis.x, axis.y, axis.z, 1.0f);

    //???]?s??????
    auto rot = XMMatrixRotationQuaternion(XMQuaternionRotationAxis(axisVec, Angle));

    //???]???????l
    XMVECTOR valueVec = XMVectorSet(Value.x, Value.y, Value.z, 1.0f);
    valueVec = XMVector4Transform(valueVec, rot);

    return Vec3<float>(valueVec.m128_f32[0], valueVec.m128_f32[1], valueVec.m128_f32[2]);
}

float KuroMath::GetRateInRange(float arg_min, float arg_max, float arg_val)
{
    float result = (arg_val - arg_min) / (arg_max - arg_min);
    result = std::clamp(result, 0.0f, 1.0f);
    return result;
}

float KuroMath::GetRateInRange(int arg_min, int arg_max, int arg_val)
{
    return GetRateInRange(static_cast<float>(arg_min), static_cast<float>(arg_max), static_cast<float>(arg_val));
}

#include<array>
#include"imguiApp.h"
#include<magic_enum.h>
void EasingParameter::ImguiDebug(const std::string& Tag)
{
    //static const std::array<std::string, EASE_CHANGE_TYPE_NUM> CHANGE_TYPE_STR = { "In","Out","InOut" };

    std::string currentChangeType = std::string(magic_enum::enum_name(m_changeType));
    if (ImGui::BeginCombo((Tag + " - EaseChangeType").c_str(), currentChangeType.c_str()))
    {
        for (int n = 0; n < EASE_CHANGE_TYPE_NUM; ++n)
        {
            bool isSelected = (m_changeType == n);
            auto newChangeType = (EASE_CHANGE_TYPE)n;
            //if (ImGui::Selectable(CHANGE_TYPE_STR[n].c_str(), isSelected))
            if (ImGui::Selectable(std::string(magic_enum::enum_name(newChangeType)).c_str(), isSelected))
            {
                m_changeType = newChangeType;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    //static const std::array<std::string, EASING_TYPE_NUM> CHANGE_TYPE_STR = { "In","Out","InOut" };
    std::string currentEaseType = std::string(magic_enum::enum_name(m_easeType));
    if (ImGui::BeginCombo((Tag + " - EaseType").c_str(), currentEaseType.c_str()))
    {
        for (int n = 0; n < EASING_TYPE_NUM; ++n)
        {
            bool isSelected = (m_easeType == n);
            auto newChangeType = (EASING_TYPE)n;
            //if (ImGui::Selectable(CHANGE_TYPE_STR[n].c_str(), isSelected))
            if (ImGui::Selectable(std::string(magic_enum::enum_name(newChangeType)).c_str(), isSelected))
            {
                m_easeType = newChangeType;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

}

#pragma endregion

