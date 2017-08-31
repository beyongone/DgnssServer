/*
 * TRandom.h
 *
 *  Created on: 2009-12-12
 *      Author: terry
 */

#ifndef TRANDOM_H_
#define TRANDOM_H_

#include <cstdlib>
#include <time.h>
/////////////////////////////////////////////////////////////////////////////
namespace comn
{

class Random
{
public:
	static void makeSeed()
	{
		generateSeed();
	}

	/// make number
	static int makeNumber()
	{
		init();
		return rand();
	}

	/// make number in range [ 0, maxNum ]
	static int makeNumber( int maxNum )
	{
		return (int)( makeScale() * maxNum + 0.5 );
	}

	/// make number in range [ lower, upper ]
	static int makeNumber( int lower, int upper )
	{
		return (int)( ( makeScale() * ( upper - lower ) + 0.5 ) + lower );
	}

	static int makePercentage()
	{
		return makeNumber( 100 );
	}

	/// return scale value in [0,1)
	static float makeScale()
	{
		init();
		float scale = rand()/float(RAND_MAX);
		return scale;
	}

private:
	Random()
	{
		generateSeed();
	}

	static void generateSeed()
	{
		srand( (unsigned int)time( NULL ) );
	}

	static void init()
	{
		static Random random;
	}

};


}
/////////////////////////////////////////////////////////////////////////////
#endif /* TRANDOM_H_ */
