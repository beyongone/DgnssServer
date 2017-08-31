/*
 * TDiscreteArray.h
 *
 *  Created on: 2016Äê8ÔÂ7ÈÕ
 *      Author: zhengboyuan
 */

#ifndef TDISCRETEARRAY_H_
#define TDISCRETEARRAY_H_

#include <utility>
#include <map>
#include "TCriticalSection.h"


namespace comn
{

template < class T, size_t MAX_SIZE >
class DiscreteArray
{
public:
	typedef std::pair< T, bool >	Pair;

protected:
	Pair	m_elements[MAX_SIZE];
	size_t	m_size;
	size_t	m_hintIndex;
	comn::CriticalSection	m_cs;

public:

	DiscreteArray():
		m_size(),
		m_hintIndex()
	{
	}

	size_t capacity() const
	{
		return MAX_SIZE;
	}

	size_t max_size() const
	{
		return MAX_SIZE;
	}

	size_t size()
	{
		comn::AutoCritSec lock(m_cs);
		return m_size;
	}

	bool empty()
	{
		return (size() == 0);
	}

	size_t put(const T& t)
	{
		comn::AutoCritSec lock(m_cs);

		if (m_size >= MAX_SIZE)
		{
			return -1;
		}

		size_t idx = findIdle();
		if (idx == -1)
		{
			return -1;
		}

		m_hintIndex = idx;

		Pair element(t, true);
		m_elements[idx] = element;

		m_size ++;

		return idx;
	}

	bool putAt(size_t idx, const T& t)
	{
		comn::AutoCritSec lock(m_cs);

		if (m_size >= MAX_SIZE)
		{
			return false;
		}

		Pair& element = m_elements[idx];
		element.first = t;

		if (!element.second)
		{
			element.second = true;
			m_size ++;
		}

		return true;
	}

	bool isIdle(size_t idx)
	{
		comn::AutoCritSec lock(m_cs);

		if (idx >= MAX_SIZE)
		{
			return false;
		}

		return !m_elements[idx].second;
	}

	size_t findIdle()
	{
		size_t idx = m_hintIndex;
		for (size_t i = 0; i < MAX_SIZE; i ++)
		{
			if (isIdle(idx))
			{
				return idx;
			}

			idx ++;
			idx = idx % MAX_SIZE;
		}
		return -1;
	}

	void clear()
	{
		comn::AutoCritSec lock(m_cs);

		Pair element;
		for (size_t i = 0; i < MAX_SIZE; i ++)
		{
			m_elements[i] = element;
		}

		m_hintIndex = 0;
		m_size = 0;
	}

	bool remove(size_t idx, T& t)
	{
		comn::AutoCritSec lock(m_cs);

		if (idx >= MAX_SIZE)
		{
			return false;
		}

		if (!m_elements[idx].second)
		{
			return false;
		}

		Pair element;

		t = m_elements[idx].first;
		m_elements[idx] = element;

		m_size --;

		return true;
	}

	bool remove(size_t idx)
	{
		comn::AutoCritSec lock(m_cs);

		if (idx >= MAX_SIZE)
		{
			return false;
		}

		if (!m_elements[idx].second)
		{
			return false;
		}

		Pair element;
		m_elements[idx] = element;
		m_size --;
		return true;
	}

	T& getAt(size_t idx)
	{
		comn::AutoCritSec lock(m_cs);
		return m_elements[idx].first;
	}

	bool next(size_t hintIndex, T& t)
	{
		comn::AutoCritSec lock(m_cs);

		size_t idx = hintIndex % MAX_SIZE;
		for (size_t i = 0; i < MAX_SIZE; i ++)
		{
			if (isIdle(idx))
			{
				idx ++;
				idx = idx % MAX_SIZE;
			}
			else
			{
				t = m_elements[idx].first;
				return true;
			}
		}
		return false;
	}


};


}

#endif /* TDISCRETEARRAY_H_ */
