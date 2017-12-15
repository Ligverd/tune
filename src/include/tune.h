/***************************************************************************
                          main.h  -  description
                             -------------------
    begin                : Sun Feb 22 2004
    copyright            : (C) 2004 by maksim
    email                : maksim@m-200.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _MAIN_H
#define _MAIN_H

//#define FREE_BSD 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#ifdef FREE_BSD
    #include <sys/wait.h>
#else
    #include <sys/wait.h>
#endif

#ifdef FREE_BSD
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/queue.h>
#include <netinet/tcp_var.h>
#define TCP_KEEPIDLE TCPCTL_KEEPIDLE
#define TCP_KEEPINTVL TCPCTL_KEEPINTVL
#endif

#include "client.h"
#include "include/type.h"
#include "include/netmes.h"
//#include "include/monmessage.h"
#include "include/unipar.h"
#include "include/tune.h"
#include <list>

#include "parse_tune.h"

#define TIMER_SEND_REGISTER_MONITOR_VALUE 100

#define MAX_MOD 64

#define ROTATION_DAY 1
#define ROTATION_DECADE 2
#define ROTATION_MONTH 3
#define ROTATION_REALTIME 4

#define MAX_CLIENT 3


#define BEXT_CMN_TONE              0     // тоновый набор
#define BEXT_CMN_OUTCOME           1     // вся исходящая связь
#define BEXT_CMN_INCOME            2     // вся входящяа связь
#define BEXT_CMN_EXTERNAL          3     // внешние номера
#define BEXT_CMN_PAYSERVICE        4     // платные службы
#define BEXT_CMN_8xxx              5     // вся 8-ка, кроме бесплатной
#define BEXT_CMN_10xxx             6     // международная связь 810xxx
#define BEXT_CMN_AON               7     // выдавать АОН абоненту
#define BEXT_CMN_82xxx             8     // зоновая связь 82xxx

// флажки разрешающие ДВО

#define BEXT_ENB_FLASH            10     // постановка на удержание
#define BEXT_ENB_REDIRECT         11     // использование переадресации
#define BEXT_ENB_ALARM            12     // использование будильника
#define BEXT_ENB_INTERCEPT        13     // перехват вызова
#define BEXT_ENB_PINCODE          14     // разрешение пользоваться пин-кодом
#define BEXT_ENB_AUTODIAL         15     // разрешение пользоваться автодозвоном
#define BEXT_ENB_NOTIFICATION     16     // разрешение использования уведомлени
#define BEXT_ENB_EVILCALL         17     // разрешение использования фиксирования злономеренного вызова
#define BEXT_ENB_CALLERID         18     // использовать callerID вместо обычного аона

// флажки текущего состояния ДВО

#define BEXT_DVO_REDIRECT_ALWAYS  20     // безусловная переадресаци
#define BEXT_DVO_REDIRECT_BUSY    21     // переадресация при занятости
#define BEXT_DVO_REDIRECT_SILENCE 22     // переадресация по неотвту
#define BEXT_DVO_ALARM            23     // установлен разовый будильник
#define BEXT_DVO_REDIRECT_HOTCALL 24     // горячий вызов
#define BEXT_DVO_NOTIFICATION     25     // уведомление о входящем вызове
#define BEXT_DVO_NODISTURB        26     // не беспокоить
#define BEXT_DVO_PINCODE          27     // режим пинкода
// фложок типа абонента
#define BEXT_TYPE_SUBSCRIBER      28     // Тип абонента 0 -аналог/1 - isdn
#define BEXT_DVO_CLIR             29     // Скрывать свой номер
#define BEXT_DVO_PINCODETWO       30     // режим пинкода на всю исходящую связь

#pragma pack (1)

class CTuneFlags
{
public:
    void SetBit ( int num );
    void ResetBit ( int num );
    void WriteBit ( int num, bool val ) { val ? SetBit(num) : ResetBit(num); }
    bool GetBit ( int num );

    DWORD base;
};

typedef struct
{
    bool        fUse;                    // включен
    CTuneFlags  bFlags;                  // все флажки
    BYTE        nSlot;                   // слот
    BYTE        nHole;                   // дырка 
    char        sAON[8];                 // аон
    BYTE        nCatAON;                 // категория аон
    char        sRedirect[12];           // переадресаци
    char        sAlarm[6];               // будильник
    char        sPinCode[5];             // пинкод, используется для 8-ки если введен
    BYTE        nPrivLevel;              // уровень привелегий для вмешательства
    BYTE        nTEI;                    // для ISDN TEI
    BYTE        reserved[9];             // для сохранения размера структыры
} TTuneExt;

typedef union
{
    TTuneExt        ext;
} TTuneData;

typedef struct
{
    DWORD tuneID;
    BYTE nMod;
    BYTE nSlot;
    BYTE nPort;
} TShortAbonInfo;


typedef struct
{
    TShortAbonInfo abon;
    TTuneData card;
} TFullAbonInfo;

#pragma pack ()

typedef std::list < TShortAbonInfo > TSHORT_LISTABON;
typedef std::list < TFullAbonInfo > TFULL_LISTABON;

extern int Scomm_fd;
void OnMessage(CSMPMessage *mes);
void Loger(const char* str);
bool make_nonblock(int sock);
int max(int x, int y);
bool get_tune(int &int_id, TModule &mod_id, char *number, TTuneData *tune, int len);
void set_tune(int int_id, TTuneData *tune);
TSHORT_LISTABON::const_iterator find_abon(DWORD int_id);

extern CParser Parser;
#endif
