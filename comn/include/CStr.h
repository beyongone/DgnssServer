/*    file: CStr.h
 *    desc:
 *   
 * created: 2013-08-14 10:53:16
 *  author: zhengchuanjiang
 * company: 
 */ 


#if !defined CSTR_H_
#define CSTR_H_

#include <string.h>
#include <string>
////////////////////////////////////////////////////////////////////////////
namespace comn
{

class  CStr
{
public:
    static const size_t npos;

public:
    CStr();

    explicit CStr(const char* str);

    CStr(const char* str, size_t length);

    CStr(const CStr& str);

    void assign(const CStr& str);
    void assign(const char* str);

    const CStr& operator = (const CStr& str);
    const CStr& operator = (const char* str);

    bool operator < (const CStr& str) const;
    bool operator == (const CStr& str) const;



    bool empty() const;

    void reset(const char* str, size_t length);
    void reset(const CStr& str);
    void reset();

    void clear();

    bool advance(size_t step);

    const char* c_str() const;

    size_t size() const;

    size_t length() const;

    const char* begin() const;

    const char* end() const;

    char operator [](size_t idx) const;

    
    int compare(const char* str, size_t length) const;
    int icompare(const char* str, size_t length) const;

    int compare(const char* str) const;

    int icompare(const char* str) const;

    int compare(const CStr& str) const;

    int icompare(const CStr& str) const;

    bool equal(const CStr& str) const;
	bool equal(const char* str) const;

    bool iequal(const CStr& str) const;



    size_t find(char ch, size_t pos = 0) const;

    size_t find(const char* str, size_t length, size_t pos) const;

    size_t find(const char* str, size_t pos = 0) const;


    size_t rfind(char ch, size_t pos = npos) const;

    size_t rfind(const char* str, size_t length, size_t pos) const;
    
    size_t rfind(const char* str, size_t pos = npos) const;


    CStr substr(size_t pos, size_t n = npos) const;
    CStr mid(size_t pos, size_t endPos) const;

    bool isNumber() const;

    bool startWith(const char* str, size_t length) const;
    bool startWith(const char* str) const;
    
    bool endWith(const char* str, size_t length) const;
    bool endWith(const char* str) const;
 

    size_t split(char sp, CStr& head, CStr& tail) const;
    size_t split(const char* sp, CStr& head, CStr& tail) const;

    bool extract(char spBegin, char spEnd, CStr& str) const;
    bool extract(const char* spBegin, const char* spEnd, CStr& str) const;

    void toValue(short& number) const;
    void toValue(unsigned short& number) const;

    void toValue(int& number) const;
    void toValue(unsigned int& number) const;

    void toValue(long& number) const;
    void toValue(unsigned long& number) const;

    void toValue(float& number) const;

    void toValue(double& number) const;


protected:
    const char* m_ptr;
    size_t m_length;

};

//////////////////////////////////////////////////////////////////////////
class  CharToken
{
public:
	CharToken(const char* str, size_t length, char sp);

	bool next(CStr& rawstr);

private:
	CStr  m_src;
	char    m_sp;
};


class  StrToken
{
public:
	StrToken(const char* str, size_t length, const char* sp);

	bool next(CStr& rawstr);

private:
	CStr  m_src;
	CStr  m_sp;
};

////////////////////////////////////////////////////////////////////////////


}
////////////////////////////////////////////////////////////////////////////
#endif //CSTR_H_

