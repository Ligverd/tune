#ifndef _CFG_H_
#define _CFG_H_

#pragma pack (1)

#include <stdlib.h>
#include <stdio.h>
#include  <include/type.h>
#include  <include/unipar.h>
#include  <string.h>

#include  <include/lib.h>

//#include  <include/logqueue.h>


#define VERSIONALLCFG 12

#define CFGHARDADDRESS 10000000

#define MAX_CFG 5

#define PAR_NONE    0
#define PAR_DWORD   1
#define PAR_STRING  2
#define PAR_BOOL    3
#define PAR_SETSTR  4
#define PAR_CHAR    5

#ifdef _WIN_VISUALCONFIGER_

#define  APPLY_BEGIN                               \
        glVisConfInd1 = 0;                         \
        bool result = false

#define  APPLY_END                                 \
        return result

class CCfgUniPar;
extern int glVisConfInd1, glVisConfInd2;
extern char *glVisConfDescr;
extern int glVisConfLevel;
extern CCfgUniPar glVisConfUnipar;

#define APPLY_BOOL(NAME, VARIABLE, DESCR, LEVEL)   \
    if (glVisConfInd1++ == glVisConfInd2)          \
    {                                              \
        glVisConfUnipar.type = PAR_BOOL;           \
        glVisConfUnipar.name = NAME;               \
        glVisConfUnipar.value.flag = VARIABLE;     \
        glVisConfDescr = DESCR;                    \
        glVisConfLevel = LEVEL;                    \
        result = true;                             \
    }

#define  APPLY_BYTE(NAME, VARIABLE, DESCR, LEVEL)  \
    if (glVisConfInd1++ == glVisConfInd2)          \
    {                                              \
        glVisConfUnipar.type = PAR_DWORD;          \
        glVisConfUnipar.name = NAME;               \
        glVisConfUnipar.value.dword = VARIABLE;    \
        glVisConfDescr = DESCR;                    \
        glVisConfLevel = LEVEL;                    \
        result = true;                             \
    }

#define  APPLY_WORD(NAME, VARIABLE, DESCR, LEVEL)  \
    if (glVisConfInd1++ == glVisConfInd2)          \
    {                                              \
        glVisConfUnipar.type = PAR_DWORD;          \
        glVisConfUnipar.name = NAME;               \
        glVisConfUnipar.value.dword = VARIABLE;    \
        glVisConfDescr = DESCR;                    \
        glVisConfLevel = LEVEL;                    \
        result = true;                             \
    }

#define  APPLY_DWORD(NAME, VARIABLE, DESCR, LEVEL) \
    if (glVisConfInd1++ == glVisConfInd2)          \
    {                                              \
        glVisConfUnipar.type = PAR_DWORD;          \
        glVisConfUnipar.name = NAME;               \
        glVisConfUnipar.value.dword = VARIABLE;    \
        glVisConfDescr = DESCR;                    \
        glVisConfLevel = LEVEL;                    \
        result = true;                             \
    }

#define  APPLY_STRING(NAME, VARIABLE, DESCR, LEVEL)\
    if (glVisConfInd1++ == glVisConfInd2)          \
    {                                              \
        glVisConfUnipar.type = PAR_STRING;         \
        glVisConfUnipar.name = NAME;               \
        glVisConfUnipar.value.str = VARIABLE;      \
        glVisConfDescr = DESCR;                    \
        glVisConfLevel = LEVEL;                    \
        result = true;                             \
    }

#define  APPLY_FIXSTRING(NAME, VARIABLE, SIZEOFVALUE, DESCR, LEVEL) \
    if (glVisConfInd1++ == glVisConfInd2)          \
    {                                              \
        glVisConfUnipar.type = PAR_STRING;         \
        glVisConfUnipar.name = NAME;               \
        glVisConfUnipar.value.str = VARIABLE;      \
        glVisConfDescr = DESCR;                    \
        glVisConfLevel = LEVEL;                    \
        result = true;                             \
    }

#define  APPLY_SET(NAME, VARIABLE, DESCR, LEVEL)   \
    if (glVisConfInd1++ == glVisConfInd2)          \
    {                                              \
        glVisConfUnipar.type = PAR_SETSTR;         \
        glVisConfUnipar.name = NAME;               \
        glVisConfUnipar.value.set = VARIABLE;      \
        glVisConfDescr = DESCR;                    \
        glVisConfLevel = LEVEL;                    \
        result = true;                             \
    }

#define  APPLY_CHAR(NAME, VARIABLE, DESCR, LEVEL)  \
    if (glVisConfInd1++ == glVisConfInd2)          \
    {                                              \
        glVisConfUnipar.type = PAR_CHAR;           \
        glVisConfUnipar.name = NAME;               \
        glVisConfUnipar.value.ch = VARIABLE;       \
        glVisConfDescr = DESCR;                    \
        glVisConfLevel = LEVEL;                    \
        result = true;                             \
    }

#else

#define  APPLY_BEGIN            \
        bool result = true;     \
        if (false)

#define  APPLY_END              \
        else                    \
            result = false;     \
        return result

#define  APPLY_BOOL(NAME, VALUE, DESCR, LEVEL)                      \
        else                                                        \
        if (!strcmp(unipar.name, NAME) && unipar.type == PAR_BOOL)  \
            VALUE = (unipar.value.flag ? true : false)

#define  APPLY_BYTE(NAME, VALUE, DESCR, LEVEL)                      \
        else                                                        \
        if (!strcmp(unipar.name, NAME) && unipar.type == PAR_DWORD) \
            VALUE = (unsigned char)unipar.value.dword

#define  APPLY_WORD(NAME, VALUE, DESCR, LEVEL)                      \
        else                                                        \
        if (!strcmp(unipar.name, NAME) && unipar.type == PAR_DWORD) \
            VALUE = (unsigned short)unipar.value.dword

#define  APPLY_DWORD(NAME, VALUE, DESCR, LEVEL)                     \
        else                                                        \
        if (!strcmp(unipar.name, NAME) && unipar.type == PAR_DWORD) \
            VALUE = (unsigned int)unipar.value.dword

#define  APPLY_STRING(NAME, VALUE, DESCR, LEVEL)                    \
        else                                                        \
        if (!strcmp(unipar.name, NAME) && unipar.type == PAR_STRING)\
            VALUE = unipar.value.str

#define  APPLY_FIXSTRING(NAME, VALUE, SIZEOFVALUE, DESCR, LEVEL)    \
        else                                                        \
        if (!strcmp(unipar.name, NAME) && unipar.type == PAR_STRING && SIZEOFVALUE > strlen(unipar.value.str)) \
            strcpy(VALUE, unipar.value.str)

#define  APPLY_SET(NAME, VALUE, DESCR, LEVEL)                       \
        else                                                        \
        if (!strcmp(unipar.name, NAME) && unipar.type == PAR_SETSTR)\
            VALUE = unipar.value.set

#define  APPLY_CHAR(NAME, VALUE, DESCR, LEVEL)                      \
        else                                                        \
        if (!strcmp(unipar.name, NAME) && unipar.type == PAR_CHAR)  \
            VALUE = unipar.value.ch

#endif

// x86 & x86_64 emulate pointer 32 bits

// not very elegant, use static property
class PtrBuffer {
public:
  static char *ptrData;
  static int   ptrShift;
};

char* PtrBuffer::ptrData = NULL;
int   PtrBuffer::ptrShift= 0;

// template pointer 32 bit  T*
template <class T>
class Ptr32 : public PtrBuffer {
public:
  DWORD pointer;

  T* operator->() {
    return (T*) (ptrData + pointer - ptrShift);
  }

  T& operator[](int i) {
    return ( (T*)(ptrData + pointer - ptrShift) )[i];
  }

  Ptr32& operator=( DWORD newPointer ) {
    pointer = newPointer + ptrShift;
  }

  void operator+=( DWORD pointerOffset ) {
    pointer += pointerOffset;
  }

  void operator-=( DWORD pointerOffset ) {
    pointer -= pointerOffset;
  }

  operator T*() {
    return (T*) (ptrData + pointer - ptrShift);
  }

};


typedef union
{
    BYTE flag;
    DWORD dword;
    Ptr32<char>  str;
    Ptr32<char>  set[];
    char ch;
} TCfgUniValue;

class CSet
{
  public:
    WORD place;                 // стартовое месторасположение:
    WORD amount;                //    Модуль / Плата / Поток / Дырка / КИ / ...
};

// ============================== PROFIL =======================

class CCfgUniPar
{
  public:
    short type;
    Ptr32<char> name;
    TCfgUniValue value;
};


class CCfgProfil
{
  public:
    short type;

    WORD numPar;
    Ptr32<CCfgUniPar> par;
};

// ============================== PORT =======================

class CCfgPort:public CSet
{
  public:
    short prof;
    short group;
    Ptr32<char> numberA;              // знак '=' впереди занчит direct направление '+','-' - с икрементом
    Ptr32<char> numberB;              // номера не могут повторяться
    Ptr32<char> prefix;               // префикс может повторяться, а может и нет
};

typedef short TCfgIdx;

class CCfgPcm:public CSet
{
  public:
    short prof;
    Ptr32<char> name;                 // !!! пока не поддерживается

    WORD numPort;
    Ptr32<CCfgPort> port;
};

class CCfgSlot:public CSet
{
  public:
    short prof;

    WORD numPort;
    Ptr32<CCfgPort> port;
};

class CCfgAir:public CSet
{
  public:
    short prof;

    WORD numPort;
    Ptr32<CCfgPort> port;
};

// ============================== ROUTING =======================


class CCfgGroup
{
  public:
    short route;
    short routeOut;
};

class CCfgDirCycle
{
  public:
    WORD numGroup;
    Ptr32<short> group;
};

class CCfgDir
{
  public:
    Ptr32<char> name;

    WORD numCycle;
    Ptr32<CCfgDirCycle> cycle;

    WORD numPar;
    Ptr32<CCfgUniPar> par;
};

class CCfgRouteRecord
{
  public:
    Ptr32<char> begin;
    Ptr32<char> end;
    Ptr32<char> trans;
    Ptr32<char> abegin;
    Ptr32<char> aend;
    Ptr32<char> atrans;
    DWORD dir;
    BYTE max;                   // если = 0 то не задан
    short profProp;
    BYTE reserved[2];
};


class CCfgRoute
{
  public:
    Ptr32<char> name;

    WORD numRec;
    Ptr32<CCfgRouteRecord> rec;
};

// ============================== MODULE =======================

class CCfgModule:public CSet
{
  public:
    short type;                 // DEV_NONE DEV_MAL DEV_MPA DEV_MPB DEV_FT
    short prof;                 // если = -1 то не задан
    Ptr32<char> name;

    WORD numPcm;
    Ptr32<CCfgPcm> pcm;

    WORD numSlot;
    Ptr32<CCfgSlot> slot;

    WORD numAir;
    Ptr32<CCfgAir> air;
};

// ============================== COMMON =======================

class CCfgCommon
{
  public:
    Ptr32<char> name;

    WORD numPar;
    Ptr32<CCfgUniPar> par;
};

// ============================== CONFIGA =======================

class CCfg
{
  public:
    WORD numModule;
    Ptr32<CCfgModule> module;         // в системе существуют по параметру PLACE

    WORD numProfil;
    Ptr32<CCfgProfil> profil;         // в системе существуют по номерам

    WORD numGroup;
    Ptr32<CCfgGroup> group;           // в системе существуют по номерам

    WORD numDir;
    Ptr32<CCfgDir> dir;               // в системе существуют по номерам

    WORD numRoute;
    Ptr32<CCfgRoute> route;           // в системе существуют по номерам

    CCfgCommon common;

    WORD numNumberProfil;
    Ptr32<char> profilNumber;         // пока не используется
};


class CAll
{
  public:
    DWORD size;
    DWORD address;

    BYTE Version;               // при каждом имзменении структыры увеличивать на 1

    short numCfg;
    Ptr32<CCfg> cfg[MAX_CFG]; // CCfg *cfg[MAX_CFG]; // @TODO

    TClock time;
    WORD nControlSum;

    // эти функции вызывают инлайн,
    // т.к. в GNU это inline глючит
    bool pro_decodeCfg(void);
    void pro_makeControlSum(void);
    bool pro_checkControlSum(void);

    // method
    inline bool decodeCfg(void)
    {
        PtrBuffer::ptrData = (char*)this;
        PtrBuffer::ptrShift= address;

        return true;
    }

    inline void makeControlSum(void)
    {
        Version = VERSIONALLCFG;
        nControlSum = 0;
        BYTE sum1 = 0, sum2 = 0;
        BYTE *data = (BYTE *) this;

        for (DWORD i = 0; i < size; i++)
        {
            sum1 ^= data[i];
            sum2 = (sum2 + data[i] + 1) & 0xFF;
        }
        nControlSum = sum1 * 0x100 + sum2;
    }

    inline bool checkControlSum(void)
    {
        if (Version != VERSIONALLCFG)
        {
            printf("Error version\n");
            return false;
        }
        if (size < sizeof(CAll) || size > 0x20000)      //128K
        {
            printf("Error size\n");
            return false;
        }
        if (numCfg < 1 || numCfg > 5)
        {
            printf("Error cfg ID\n");
            return false;
        }
//    nControlSum = 0;
        BYTE sum1 = 0, sum2 = 0;
        BYTE *data = (BYTE *) this;

        for (DWORD i = 0; i < size; i++)
        {
            sum1 ^= data[i];
            sum2 = (sum2 + data[i] + 1) & 0xFF;
        }
        sum1 ^= ((BYTE *) & nControlSum)[0];
        sum2 -= ((BYTE *) & nControlSum)[0];
        sum1 ^= ((BYTE *) & nControlSum)[1];
        sum2 -= ((BYTE *) & nControlSum)[1];
        return nControlSum == sum1 * 0x100 + sum2;
    }

};

#pragma pack ()

#endif
