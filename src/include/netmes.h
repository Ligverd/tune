#ifndef _NETMES_H_
#define _NETMES_H_

#include <include/type.h>

#define MAX_MODULE 64

//
#define NET_MES_REGISTER_TERMINAL       0x01    // ������ ������ 5 ���, ����� unregister
#define NET_MES_REGISTER_MONITOR        0x02    // ������ ������ 5 ���, ����� unregister
#define NET_MES_REGISTER_ALARM          0x03    // ������ ������ 5 ���, ����� unregister
//#define NET_MES_REGISTER_PREPAY ??            // ������ ������ 5 ���, ����� unregister

#define NET_MES_HELLO                   0x04

#define NET_MES_TERMINALSTRING          0x05

#define NET_MES_CONFSTARTLOAD           0x06
#define NET_MES_CONFBLOCKLOAD           0x07
#define NET_MES_CONFFINISHLOAD          0x08
#define NET_MES_CONFACCEPT              0x09
#define NET_MES_CONFERROR               0x0A

#define NET_MES_BINSTARTLOAD            0x0B
#define NET_MES_BINBLOCKLOAD            0x0C
#define NET_MES_BINFINISHLOAD           0x0D
#define NET_MES_BINACCEPT               0x0E
#define NET_MES_BINERROR                0x0F

#define NET_MES_ROUTING                 0x10
#define NET_MES_REJECTMODUL             0x11

#define NET_MES_TEST                    0x12

#define NET_MES_WIDEUPDATE              0x13

#define NET_MES_WIDEHIMESSAGE           0x14

#define NET_MES_SETNUMBER               0x15

#define NET_MES_LOGSTART                0x16
#define NET_MES_LOGEND                  0x17
#define NET_MES_LOGACCEPT               0x18
#define NET_MES_LOGBLOCK                0x19
#define NET_MES_LOGFINISH               0x1A

#define NET_MES_CONFBACK_GET            0x1B
#define NET_MES_CONFBACK_TAKE           0x1C

#define NET_MES_TUNE_SET                0x1D
#define NET_MES_TUNE_SETOK              0x1E
#define NET_MES_TUNE_SETERROR           0x1F

#define NET_MES_TUNE_GET                0x20
#define NET_MES_TUNE_GETOK              0x21
#define NET_MES_TUNE_GETERROR           0x22

#define NET_MES_CONFEXPORT              0x23

#define NET_MES_MONITOR                 0x24    // ATS->PC(tarif) ATS->ATS(sorm)

#define NET_MES_TESTEXT_GET             0x25
#define NET_MES_TESTEXT_TAKE            0x26

#define NET_MES_ALARM                   0x27    // BYTE:code ...:param  // ATS->PC(alarm)

#define NET_MES_REJECTVIRCOM            0x28

#define NET_MES_INTELLECTROUTING_MODULE 0x29

#define NET_MES_LINK_STATE              0x2A    // �������� ��������� ������ MON<->ATS
#define NET_MES_PORT_STATE              0x2B    // �������� ��������� ������ MON<->ATS

#define NET_MES_EXTRIMTARIF_GETWIDE     0x2C    // ����������������� ������ ����������� (��� � 15 ���)
#define NET_MES_EXTRIMTARIF_GETPOINT    0x2D    // �������� ������ �����������, �� ������� ������
#define NET_MES_EXTRIMTARIF_TAKE        0x2E    // �������� ����������� (��������� �����)

#define NET_MES_SORM_OVERHEAR           0x2F    // ������ �� ������������� (����:DWORD back1:DWORD back2:DWORD)
#define NET_MES_SORM_OVERLOAD           0x30    // ������ �� ��������      (����:DWORD back1:DWORD back2:DWORD)

//#define NET_MES_FREQ_START         49 // ������ ������� ��������
//#define NET_MES_FREQ_STOP          50 // ������ ������� ��������

#define NET_MES_TRANSIT_PORT            0x33    // ���������� �������� ��������� �� ������/��� � �����

#define NET_MES_SYNCHRO                 0x34

#define NET_MES_PCPHONE                 0x35

#define NET_MES_CALLBACKACTIVATE        0x36    // ����� ��������� callback

#define NET_MES_PCMSTATUS               0x37    // �������� pcmstatus �� ������

#define NET_MES_SETPASSWORD             0x38    // ��������� ������, 6 ���� ������

#define NET_MES_REGISTER_PREPAY         0x39    // PC -> ATS ������ ������ 5 ���, ����� unregister
#define NET_MES_PREPAY_NUMBER_COMPLETE  0x3A    // ATS  -> PC
#define NET_MES_PREPAY_TALK_BEGIN       0x3B    // ATS  -> PC
#define NET_MES_PREPAY_TALK_END         0x3C    // ATS  -> PC

// ������� ��� �����
#define NET_MES_UPS_DATA                0x3D    // ATS  -> PC (�������������)

#define NET_MES_SHUTDOWN                0x3E    // ������� ���������� ���
#define NET_MES_GETJURNAL               0x3F    // ������� �������

// ������� � ��������� CF
// �����  ������������ ������� ���������� ������������ �� �����
// ������� ������������ ���� ��� � ���
#define NET_MONITOR_DISK_INFO           0x40    // ATS  ->PC 1���� ������ ������ 10���

#define NET_MES_REGISTER_MONITOR_INL    0x41    // ����������� �������� �������

#define NET_MES_CONTROL_E08_COST        0x42    // �������� ��������� �08

#define NET_MES_FTP_OVER_SCOMM          0x43    // FTP ����� SComm

#define NET_MES_ELKOM                   0x44    // �������� �����

#define NET_MES_TSF_FOR_SORM            0x45    // tsf � ���� ��� ��������
#define NET_MES_REGISTER_SORM_PULT      0x46
#define NET_MES_MESS_TO_SORM            0x47
#define NET_MES_MESS_TO_PULT_CH1        0x48
#define NET_MES_MESS_TO_PULT_CH2        0x49

// ��������: ����������� ������ ������ ������������ ���������
// ���-�� ������ �� ������
// ����� ��������� ��������� ������ � ������������ ��������


#pragma pack (push, 1)

struct TNetPoint
{
    BYTE nMod; // ���� == 0xFF �� ������������� ���� == 0xFE �� ��������� ������
    char nVir; // ���� == -1 �� ��� ����� PC
} ;

class CNetSetPoint
{
public:
    BYTE bitSet[MAX_MODULE / 8];

    void Clear();
    void Full();
    void SetIndex(short ind);
    void ResetIndex(short ind);
    bool CheckIndex(short ind);
};

class CNetMessage
{
public:
    TNetPoint dst;      // [1 + 1]
    TNetPoint src;      // [1 + 1]
    short nMessage;     // [2]
    char gate;          // [1] ����������� ������������, ��� ��� ����� = -1
    union
    {
        struct
        {
            BYTE data[1];       // [1...] ������
        } single;
        struct
        {
            CNetSetPoint set;   //  [8] 8�������� ������
            BYTE data[1];       //  [1..] ������
        } multi;
    } un;

#if defined(__GNUC__) || defined(WIN32)
    CNetMessage() {
        memset(this, 0x00, sizeof(*this));
    }
    const uc* decode(const uc* stream, int& size);
    uc* encode(uc* stream);
#endif

#ifndef ARM
    short nHead( void ) { return  7 + (dst.nMod == 0xFF ? sizeof(CNetSetPoint) : 0); }
    BYTE* pData( void ) { return  dst.nMod == 0xFF ? &un.multi.data[0] : &un.single.data[0]; }
#endif

    void fillReply(CNetMessage* mes, TModule srcModule);
    void fillWide(TModule srcModule);
    void fillNear(TModule srcModule);
};

#if defined(__GNUC__) || defined(WIN32)
const   int     Max_NetMessageDataSize =  2*100 + sizeof(BYTE) + sizeof(TCallID) + sizeof(BYTE);
class  CNetMessageBody : public CNetMessage 
{
public:
    int     dataSize;
    uc      data[Max_NetMessageDataSize];

public:
    CNetMessageBody() {
        dataSize = 0;
        memset(data, 0x00, sizeof(data));
    }

    const uc* decode(const uc* stream, int size);
    uc* encode(uc* stream);
    const uc* input(const uc* stream, int size);
};
#endif


#pragma pack (pop)

#endif
