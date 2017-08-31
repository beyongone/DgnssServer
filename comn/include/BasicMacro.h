/*
 * TBasicMacro.h
 *
 *  Created on: 2015年5月26日
 *      Author: chuanjiang.zh@qq.com
 */

#ifndef INCLUDE_BASICMACRO_H_
#define INCLUDE_BASICMACRO_H_


#ifndef     SAFE_DELETE
#define     SAFE_DELETE(p)          if((p) != NULL) {delete (p); (p) = NULL;}
#endif

#ifndef     SAFE_DELETE_ARRAY
#define     SAFE_DELETE_ARRAY(p)    if((p) != NULL) {delete[] (p);(p) = NULL;}
#endif

#ifndef     ARRAY_SIZE
#define     ARRAY_SIZE(x)           (sizeof(x)/sizeof((x)[0]))
#endif

#ifndef     CheckPointer
#define     CheckPointer(p,ret)     {if( (p) == NULL) return (ret);}
#endif

#ifndef     CheckCondition
#define     CheckCondition(p,ret)       {if(!(p)) return (ret);}
#endif




#endif /* INCLUDE_BASICMACRO_H_ */
