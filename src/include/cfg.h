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

typedef union
{
    BYTE flag;
    DWORD dword;
    char *str;
    char **set;
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
    char *name;
    TCfgUniValue value;
};


class CCfgProfil
{
  public:
    short type;

    WORD numPar;
    CCfgUniPar *par;
};

// ============================== PORT =======================

class CCfgPort:public CSet
{
  public:
    short prof;
    short group;
    char *numberA;              // знак '=' впереди занчит direct направление '+','-' - с икрементом
    char *numberB;              // номера не могут повторяться
    char *prefix;               // префикс может повторяться, а может и нет
};

typedef short TCfgIdx;

class CCfgPcm:public CSet
{
  public:
    short prof;
    char *name;                 // !!! пока не поддерживается

    WORD numPort;
    CCfgPort *port;
};

class CCfgSlot:public CSet
{
  public:
    short prof;

    WORD numPort;
    CCfgPort *port;
};

class CCfgAir:public CSet
{
  public:
    short prof;

    WORD numPort;
    CCfgPort *port;
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
    short *group;
};

class CCfgDir
{
  public:
    char *name;

    WORD numCycle;
    CCfgDirCycle *cycle;

    WORD numPar;
    CCfgUniPar *par;
};

class CCfgRouteRecord
{
  public:
    char *begin;
    char *end;
    char *trans;
    char *abegin;
    char *aend;
    char *atrans;
    DWORD dir;
    BYTE max;                   // если = 0 то не задан
    short profProp;
    BYTE reserved[2];
};


class CCfgRoute
{
  public:
    char *name;

    WORD numRec;
    CCfgRouteRecord *rec;
};

// ============================== MODULE =======================

class CCfgModule:public CSet
{
  public:
    short type;                 // DEV_NONE DEV_MAL DEV_MPA DEV_MPB DEV_FT
    short prof;                 // если = -1 то не задан
    char *name;

    WORD numPcm;
    CCfgPcm *pcm;

    WORD numSlot;
    CCfgSlot *slot;

    WORD numAir;
    CCfgAir *air;
};

// ============================== COMMON =======================

class CCfgCommon
{
  public:
    char *name;

    WORD numPar;
    CCfgUniPar *par;
};

// ============================== CONFIGA =======================

class CCfg
{
  public:
    WORD numModule;
    CCfgModule *module;         // в системе существуют по параметру PLACE

    WORD numProfil;
    CCfgProfil *profil;         // в системе существуют по номерам

    WORD numGroup;
    CCfgGroup *group;           // в системе существуют по номерам

    WORD numDir;
    CCfgDir *dir;               // в системе существуют по номерам

    WORD numRoute;
    CCfgRoute *route;           // в системе существуют по номерам

    CCfgCommon common;

    WORD numNumberProfil;
    void *profilNumber;         // пока не используется
};


class CAll
{
  public:
    DWORD size;
    DWORD address;

    BYTE Version;               // при каждом имзменении структыры увеличивать на 1

    short numCfg;
    CCfg *cfg[MAX_CFG];

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
#define DECODE(X) ( (X) ? ( (*(BYTE**)&(X)) += delta ) : 0 )
//    #define DECODE(X) {if (X)   (*(BYTE**)&(X)) += delta;  }
        int n, i, j, kk;


        DWORD delta = (DWORD) this - address;

        for (n = 0; n < numCfg; n++)
        {
            DECODE(cfg[n]);


#ifdef _ATS_
            CProcess::DI();
            char tmp[100];
              sprintformat(tmp, "TTT: file = %s line = %d", __FILE__,
                           __LINE__);
              CProcess::EI();
#endif
              DECODE(cfg[n]->module);

            for (i = 0; i < cfg[n]->numModule; i++)
            {
                DECODE(cfg[n]->module[i].name);
                DECODE(cfg[n]->module[i].pcm);
                for (j = 0; j < cfg[n]->module[i].numPcm; j++)
                {
                    DECODE(cfg[n]->module[i].pcm[j].name);
                    DECODE(cfg[n]->module[i].pcm[j].port);
                    for (kk = 0; kk < cfg[n]->module[i].pcm[j].numPort; kk++)
                    {
                        DECODE(cfg[n]->module[i].pcm[j].port[kk].numberA);
                        DECODE(cfg[n]->module[i].pcm[j].port[kk].numberB);
                        DECODE(cfg[n]->module[i].pcm[j].port[kk].prefix);
                    }
                }
                DECODE(cfg[n]->module[i].slot);
                for (j = 0; j < cfg[n]->module[i].numSlot; j++)
                {
                    DECODE(cfg[n]->module[i].slot[j].port);
                    for (kk = 0; kk < cfg[n]->module[i].slot[j].numPort; kk++)
                    {
                        DECODE(cfg[n]->module[i].slot[j].port[kk].numberA);
                        DECODE(cfg[n]->module[i].slot[j].port[kk].numberB);
                        DECODE(cfg[n]->module[i].slot[j].port[kk].prefix);
                    }
                }
                DECODE(cfg[n]->module[i].air);
                for (j = 0; j < cfg[n]->module[i].numAir; j++)
                {
                    DECODE(cfg[n]->module[i].air[j].port);
                    for (kk = 0; kk < cfg[n]->module[i].air[j].numPort; kk++)
                    {
                        DECODE(cfg[n]->module[i].air[j].port[kk].numberA);
                        DECODE(cfg[n]->module[i].air[j].port[kk].numberB);
                        DECODE(cfg[n]->module[i].air[j].port[kk].prefix);
                    }
                }
            }
            DECODE(cfg[n]->profil);
            for (i = 0; i < cfg[n]->numProfil; i++)
            {
                DECODE(cfg[n]->profil[i].par);
                for (j = 0; j < cfg[n]->profil[i].numPar; j++)
                {
                    DECODE(cfg[n]->profil[i].par[j].name);
                    if (cfg[n]->profil[i].par[j].type == PAR_STRING)
                        DECODE(cfg[n]->profil[i].par[j].value.str);
                    if (cfg[n]->profil[i].par[j].type == PAR_SETSTR)
                    {
                        DECODE(cfg[n]->profil[i].par[j].value.set);
                        for (int s = 0; cfg[n]->profil[i].par[j].value.set[s];
                             s++)
                            DECODE(cfg[n]->profil[i].par[j].value.set[s]);
                    }
                }
            }
            DECODE(cfg[n]->group);
            for (i = 0; i < cfg[n]->numGroup; i++)
            {
            }
            DECODE(cfg[n]->dir);
            for (i = 0; i < cfg[n]->numDir; i++)
            {
                DECODE(cfg[n]->dir[i].name);
                DECODE(cfg[n]->dir[i].cycle);
                for (j = 0; j < cfg[n]->dir[i].numCycle; j++)
                {
                    DECODE(cfg[n]->dir[i].cycle[j].group);
                    for (kk = 0; kk < cfg[n]->dir[i].cycle[j].numGroup; kk++)
                    {
                    }
                }
                DECODE(cfg[n]->dir[i].par);
                for (j = 0; j < cfg[n]->dir[i].numPar; j++)
                {
                    DECODE(cfg[n]->dir[i].par[j].name);
                    if (cfg[n]->dir[i].par[j].type == PAR_STRING)
                        DECODE(cfg[n]->dir[i].par[j].value.str);
                    if (cfg[n]->dir[i].par[j].type == PAR_SETSTR)
                    {
                        DECODE(cfg[n]->dir[i].par[j].value.set);
                        for (int s = 0; cfg[n]->dir[i].par[j].value.set[s];
                             s++)
                            DECODE(cfg[n]->dir[i].par[j].value.set[s]);
                    }
                }
            }
            DECODE(cfg[n]->route);
            for (i = 0; i < cfg[n]->numRoute; i++)
            {
                DECODE(cfg[n]->route[i].name);
                DECODE(cfg[n]->route[i].rec);
                for (j = 0; j < cfg[n]->route[i].numRec; j++)
                {
                    DECODE(cfg[n]->route[i].rec[j].begin);
                    DECODE(cfg[n]->route[i].rec[j].end);
                    DECODE(cfg[n]->route[i].rec[j].trans);
                    DECODE(cfg[n]->route[i].rec[j].abegin);
                    DECODE(cfg[n]->route[i].rec[j].aend);
                    DECODE(cfg[n]->route[i].rec[j].atrans);
                }
            }
            DECODE(cfg[n]->common.name);
            DECODE(cfg[n]->common.par);
            {
                for (j = 0; j < cfg[n]->common.numPar; j++)
                {
                    DECODE(cfg[n]->common.par[j].name);
                    if (cfg[n]->common.par[j].type == PAR_STRING)
                        DECODE(cfg[n]->common.par[j].value.str);
                    if (cfg[n]->common.par[j].type == PAR_SETSTR)
                    {
                        DECODE(cfg[n]->common.par[j].value.set);
                        for (int s = 0; cfg[n]->common.par[j].value.set[s];
                             s++)
                            DECODE(cfg[n]->common.par[j].value.set[s]);
                    }
                }
            }
        }
        address = (DWORD) this;
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
