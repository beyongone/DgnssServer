/*
 * NumGenerator.h
 *
 *  Created on: 2015年1月15日
 *      Author: zhengchuanjiang
 */

#ifndef NUMGENERATOR_H_
#define NUMGENERATOR_H_

#include <algorithm>

namespace comn
{

template < class T >
class NumGenerator
{
public:
	NumGenerator():
		m_step(1),
		m_lower(0),
		m_upper(100),
		m_cur(m_lower)
	{
	}

	NumGenerator(T step, T lower, T upper):
		m_step(step),
		m_lower(lower),
		m_upper(upper),
		m_cur(m_lower)
	{
	}

	~NumGenerator()
	{
	}

	void setStep(T step)
	{
		m_step = step;
	}

	bool setRange(T lower, T upper)
	{
		if (upper < lower)
		{
			std::swap(lower, upper);
		}

		m_lower = lower;
		m_upper = upper;
        m_cur = m_lower;
		return true;
	}

	T getStep() const
	{
		return m_step;
	}

	T getLower() const
	{
		return m_lower;
	}

	T getUpper() const
	{
		return m_upper;
	}

	T getCurrent() const
	{
		return m_cur;
	}

    T max_size() const
    {
        return m_upper - m_lower;
    }

	T next()
	{
		m_cur += m_step;
		if (m_upper < m_cur)
		{
			m_cur = m_lower;
		}
		return m_cur;
	}

protected:
	T	m_step;
	T	m_lower;
	T	m_upper;
	T	m_cur;

};


}

#endif /* NUMGENERATOR_H_ */
