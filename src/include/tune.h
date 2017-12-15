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


#define BEXT_CMN_TONE              0     // ������� �����
#define BEXT_CMN_OUTCOME           1     // ��� ��������� �����
#define BEXT_CMN_INCOME            2     // ��� �������� �����
#define BEXT_CMN_EXTERNAL          3     // ������� ������
#define BEXT_CMN_PAYSERVICE        4     // ������� ������
#define BEXT_CMN_8xxx              5     // ��� 8-��, ����� ����������
#define BEXT_CMN_10xxx             6     // ������������� ����� 810xxx
#define BEXT_CMN_AON               7     // �������� ��� ��������
#define BEXT_CMN_82xxx             8     // ������� ����� 82xxx

// ������ ����������� ���

#define BEXT_ENB_FLASH            10     // ���������� �� ���������
#define BEXT_ENB_REDIRECT         11     // ������������� �������������
#define BEXT_ENB_ALARM            12     // ������������� ����������
#define BEXT_ENB_INTERCEPT        13     // �������� ������
#define BEXT_ENB_PINCODE          14     // ���������� ������������ ���-�����
#define BEXT_ENB_AUTODIAL         15     // ���������� ������������ ������������
#define BEXT_ENB_NOTIFICATION     16     // ���������� ������������� ����������
#define BEXT_ENB_EVILCALL         17     // ���������� ������������� ������������ �������������� ������
#define BEXT_ENB_CALLERID         18     // ������������ callerID ������ �������� ����

// ������ �������� ��������� ���

#define BEXT_DVO_REDIRECT_ALWAYS  20     // ����������� ������������
#define BEXT_DVO_REDIRECT_BUSY    21     // ������������� ��� ���������
#define BEXT_DVO_REDIRECT_SILENCE 22     // ������������� �� �������
#define BEXT_DVO_ALARM            23     // ���������� ������� ���������
#define BEXT_DVO_REDIRECT_HOTCALL 24     // ������� �����
#define BEXT_DVO_NOTIFICATION     25     // ����������� � �������� ������
#define BEXT_DVO_NODISTURB        26     // �� ����������
#define BEXT_DVO_PINCODE          27     // ����� �������
// ������ ���� ��������
#define BEXT_TYPE_SUBSCRIBER      28     // ��� �������� 0 -������/1 - isdn
#define BEXT_DVO_CLIR             29     // �������� ���� �����
#define BEXT_DVO_PINCODETWO       30     // ����� ������� �� ��� ��������� �����

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
    bool        fUse;                    // �������
    CTuneFlags  bFlags;                  // ��� ������
    BYTE        nSlot;                   // ����
    BYTE        nHole;                   // �����
    char        sAON[8];                 // ���
    BYTE        nCatAON;                 // ��������� ���
    char        sRedirect[12];           // ������������
    char        sAlarm[6];               // ���������
    char        sPinCode[5];             // ������, ������������ ��� 8-�� ���� ������
    BYTE        nPrivLevel;              // ������� ���������� ��� �������������
    BYTE        nTEI;                    // ��� ISDN TEI
    BYTE        reserved[9];             // ��� ���������� ������� ���������
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
