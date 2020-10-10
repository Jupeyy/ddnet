#include "bezier.h"

CCubicBezier CCubicBezier::With(float Start, float StartDerivative, float EndDerivative, float End)
{
	return CCubicBezier(Start, Start + StartDerivative / 3, End - EndDerivative / 3, End);
}

// f(t) = (1-t)³ m_A + 3(1-t)²t m_B + 3(1-t)t² m_C + t³ m_D
float CCubicBezier::Evaluate(float t) const
{
	return (1 - t) * (1 - t) * (1 - t) * m_A + 3 * (1 - t) * (1 - t) * t * m_B + 3 * (1 - t) * t * t * m_C + t * t * t * m_D;
}

// f(t) = 3(1-t)²(m_B-m_A) + 6(1-t)t(m_C-m_B) + 3t²(m_D-m_C)
float CCubicBezier::Derivative(float t) const
{
	return 3 * (1 - t) * (1 - t) * (m_B - m_A) + 6 * (1 - t) * t * (m_C - m_B) + 3 * t * t * (m_D - m_C);
}
