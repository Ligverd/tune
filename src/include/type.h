#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>

#ifdef  ARM
    #include  <memory.h>
#else
    #ifdef __GNUC__
        #ifdef ARMLINUX
            #define ARM
            #define __ARM__
        #endif
        #include  <string.h>
    #else
        #define _WIN_
    #endif
#endif

#define DEV_NONE 0
#define DEV_MAL  1
#define DEV_MPA  2
#define DEV_MPB  3
#define DEV_ISDN 4
#define DEV_C412 5

#define DEVICE_NONE (glDevice == DEV_NONE)
#define DEVICE_MAL  (glDevice == DEV_MAL)
#define DEVICE_MPA  (glDevice == DEV_MPA)
#define DEVICE_MPB  (glDevice == DEV_MPB)
#define DEVICE_ISDN (glDevice == DEV_ISDN)
#define DEVICE_C412 (glDevice == DEV_C412)

//typedef char FLAG;

#ifdef _ATS_

//    typedef char BOOL;

    #define NULL 0

    #define LOWORD(X) ((WORD)(X))
    #define HIWORD(X) ((WORD)((X)>>16))

    #pragma pack(1)

#endif


#define NOP5 { __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop"); }

typedef uint32_t DWORD ; // 32
typedef uint16_t WORD  ; // 16
typedef unsigned char  BYTE  ; //  8
typedef unsigned char  uc;


//#define TRUE  1
//#define FALSE 0

#define VLOWORD(X) (*(WORD*)&(X))
#define VHIWORD(X) (*(((WORD*)&(X))+1))

#ifdef _WIN_

    void stub_asm( char* s );
    BYTE stub_inportb( WORD x );
    WORD stub_inportw( WORD x );
    void stub_outportb( WORD x, BYTE y );
    void stub_outportw( WORD x, WORD y );

    #define __asm(X) stub_asm(X)
    #define inportb(X) (stub_inportb(X))
    #define inportw(X) (stub_inportw(X))
    #define outportb(X, Y) stub_outportb((X), (Y))
    #define outportw(X, Y) stub_outportw((X), (Y))
    #define __inline__

    void WinTrace ( int port, char *str );

#endif

#ifdef _ATS_
#define MAKEWORD(X, Y) ((WORD)(BYTE)(X)) + (((WORD)(BYTE)(Y)) << 8)
#define LOBYTE(X) ((BYTE)(WORD)(X))
#define HIBYTE(X) ((BYTE)(((WORD)(X)) >> 8))
#endif

#ifdef _WIN_
#endif

#define GETMIN(X,Y) ((X)<=(Y)?(X):(Y))

#define MAKEDWORD(X, Y) ((DWORD)(WORD)(X)) + (((DWORD)(WORD)(Y)) << 16)

#define PRINT HT.config.SendTerminalFormat
#define SECOND sysComPort.SecondString

#ifdef _ATS_
    #define CONTROLPOINT(X)      sysSave.ControlPoint(__LINE__, (X))
    #define CONTROLPOINTNAFIG(X) sysSave.ControlPoint(__LINE__, (X))
#endif
#ifdef _WIN_
    #define CONTROLPOINT(X)      sysLoad.ControlPoint(__LINE__, (X))
    #define CONTROLPOINTDEBUG(X) sysLoad.ControlPoint(((WORD)-1), (X))
    #define CONTROLPOINTNAFIG(X) sysLoad.ControlPoint(((WORD)-2), (X))
#endif

//
// ������ ������ ����
//

typedef     short           TState;         //
typedef     unsigned short  TBlock;         // ������������� ����������� �������� (� ��������� ���������� ������)
const       TState          InvalidState            = (TState)-1;
const       TState          InvalidStateID          = (TState)-1;

// ����
typedef     char            TModule;        // �� '99
typedef     unsigned short  TSignal;        // ������������� �������
typedef     char            TPCM;
typedef     char            TTSL;
typedef     char            THiIdx;
typedef     char            TLoIdx;
typedef     char            TFalcType;
typedef     char            TLink;
typedef     char            TChannel;

typedef     unsigned char   TSignalling;
typedef     unsigned char   TUniPar;


typedef     char            TSlot;
typedef     char            THole;
typedef     char            TWide;

typedef     unsigned long   TDir;           // ����������
typedef     short           TLine;          // �����-�� �����
typedef     unsigned long   TCallID;        // ������������� ������. �����������, ����������.
typedef     unsigned long   TCallPointID;   // ������������� ����� ������. ����.
typedef     unsigned char   TCauseValue;    // ������� �����


// ���� �������� ��� ���������� ���������
const   THiIdx  HiIdx_base_Slot                         = 1;
const   THiIdx  HiIdx_base_PCM                          = 50+1;
const   THiIdx  HiIdx_base_Air                          = 70+1;
const   THiIdx  HiIdx_base_AirShadowOffset              = 15;
const   THiIdx  HiIdx_base_AirShadow                    = 70+1 + HiIdx_base_AirShadowOffset;


// ����� ��������� � CPort

// ���� �����
const   unsigned char   BlockType_Locally       = 0x00;
const   unsigned char   BlockType_Remotely      = 0x01;

// ���� ���������� �������
const   unsigned char   Block_Maintenance       = 0x00;     // TypeInd_Maintenance_oriented;
const   unsigned char   Block_Hardware          = 0x01;     // TypeInd_Hardware_failure_oriented;
const   unsigned char   Block_Incoming          = 0x02;     // TypeInd_Hardware_failure_oriented;
const   unsigned char   Block_Outgoing          = 0x03;     // TypeInd_Hardware_failure_oriented;

// ������������ ������������ (�����):
const   unsigned char   BlockMask_LocalMaintenance      = 0x01;
const   unsigned char   BlockMask_RemoteMaintenance     = 0x02;
const   unsigned char   BlockMask_LocalHardware         = 0x04;
const   unsigned char   BlockMask_RemoteHardware        = 0x08;
const   unsigned char   BlockMask_Incoming              = 0x10;
const   unsigned char   BlockMask_Outgoing              = 0x40;

const   unsigned char   BusyBlockMask = BlockMask_LocalMaintenance  |
                                        BlockMask_LocalHardware     |
                                        BlockMask_RemoteMaintenance |
                                        BlockMask_RemoteHardware    |
                                        //BlockMask_Incoming          |
                                        BlockMask_Outgoing          ;

const   unsigned char   FullBlockMask = BlockMask_LocalMaintenance  |
                                        BlockMask_LocalHardware     |
                                        BlockMask_RemoteMaintenance |
                                        BlockMask_RemoteHardware    |
                                        BlockMask_Incoming          |
                                        BlockMask_Outgoing          ;


//
inline unsigned char makeBlockMask(unsigned char type, unsigned char block) {
    return (0x01 << ((2*block) + type));
}

#endif
