/*
 * Base64.h
 *
 *  Created on: 2010-11-12
 *      Author: terry
 */

#ifndef BASE64_H_
#define BASE64_H_


namespace comn
{

class  Base64
{
public:

	static int decode(unsigned char *out, int out_size, const char *in, int in_size);

	static char* encode(char *out, int out_size, const unsigned char *in, int in_size);

};


}

#endif /* BASE64_H_ */
