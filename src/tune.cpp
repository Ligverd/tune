
/***************************************************************************
 *   Copyright (C) 2007 by PAX MAKSIM  *
 *   pax@m-200.com, maksim@m-200.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <config.h>
#include <include/cfg.h>
#include <sys/errno.h>
#include <include/tune.h>
#include <include/client.h>

CSMPClient smp(0);

BYTE mas_mod[MAX_MOD];

int timerA;
int timerB;
int timerC;

bool runTimerA;
bool runTimerB;
bool runTimerC;

struct itimerval real_timer;
struct sigaction sact;

int Scomm_fd = 0;
int Server_fd = 0;
int Client_fd[MAX_CLIENT];

pthread_t Server_tid;
pthread_t terminal_tid;

char tfs_file_name[128];        //…Õ— ∆¡ Ã¡ tfs
char txt_file_name[128];        //…Õ— ∆¡ Ã¡ txt
int fd_tfs_file;                // handle Œ¡ œ‘À“Ÿ‘Ÿ  tfs ∆¡ Ã
int fd_txt_file;                // handle Œ¡ œ‘À“Ÿ‘Ÿ  txt ∆¡ Ã

// Configuration variable
CParser Parser;

int filedes[2];



TSHORT_LISTABON m_listabon;

void get_time_str(char *tm_str, tm T)
{
    sprintf(tm_str, "[%02d-%02d-%04d %02d:%02d:%02d]", T.tm_mday,
            T.tm_mon + 1, (T.tm_year + 1900), T.tm_hour, T.tm_min, T.tm_sec);
}

bool StrToLog(const char *str)
{
    int fd;
    bool fret = true;


    if (Parser.sLogFile != NULL)
        if ((fd = open(Parser.sLogFile, O_RDWR | O_CREAT | O_APPEND, 0666)) < 0)
	{
    	    printf("Can't write to log file:%s\n", Parser.sLogFile);
    	    return false;
        }
    if (write(fd, str, strlen(str)) < 0)
        fret = false;
    close(fd);

    return fret;
}

void Loger(const char *str)
{
    char time_str[22];
    char *logstr = new char[10000];
    time_t itime;
    tm T;

    time(&itime);
    localtime_r(&itime, &T);
    printf("%s", str);
    get_time_str(time_str, T);
    sprintf(logstr, "%s %s", time_str, str);
    StrToLog(logstr);
    delete logstr;
}

                                                                              

bool make_nonblock(int sock)
{
/*    int sock_opt;

    if ((sock_opt = fcntl(sock, F_GETFL, O_NONBLOCK)) < 0)
    {
        return false;
    }
    if ((sock_opt = fcntl(sock, F_SETFL, sock_opt | O_NONBLOCK)) < 0)
    {
        return false;
    }
*/    
    return true;
}

int Login_ethernet(const char *ip, in_addr_t port)
{
    struct sockaddr_in sock_addr;
    struct hostent *host;

    int fd, sock_opt;

    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        Loger("Can't create scomm socket!\n");
        return -1;
    }

    if (!make_nonblock(fd))
    {
        close(fd);
        Loger("Can't make scomm socket nonblock!\n");
        return -1;
    }

    sock_opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt))
        < 0)
    {
        Loger("Can't set scomm socket option SO_REUSEADDR!\n");
    }

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);

    if (inet_aton(ip, &sock_addr.sin_addr) < 0)
    {
        close(fd);
        Loger("Scomm ip error!\n");
        return -1;
    }

    host = gethostbyname(ip);
    if (!host)
    {
        Loger("Error resolve server!\n");
        return -1;
    }
    memcpy(&sock_addr.sin_addr, host->h_addr_list[0],
           sizeof(sock_addr.sin_addr));

    int state = 0;
    struct timeval tout;

    tout.tv_sec = 20;
    tout.tv_usec = 0;
    fd_set wset, rset;

    FD_ZERO(&wset);
    FD_SET(fd, &wset);
    rset = wset;
    int n;

    if (connect(fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            close(fd);
            printf("Connection to scomm Error!\n");
            return -1;
        }
        state = 1;
        printf("Connection in process...\n");
    }
    if (state)
    {
        n = select(fd + 1, &rset, &wset, NULL, &tout);
        if (n < 0 || (FD_ISSET(fd, &wset) && FD_ISSET(fd, &rset)))
        {
            close(fd);
            printf("Connection to scomm error!\n");
            return -1;
        }
        if (!n)
        {
            close(fd);
            printf("Time out of connection to scomm\n");
            return -1;
        }
    }
    char log[50];

    sprintf(log, "Successfuly connected to:%s\n\n", ip);
    Loger(log);
    return fd;
}

int Create_server_point(in_addr_t port)
{
    struct sockaddr_in server_addr;
    int fd, sock_opt;

    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        Loger("Can't create tune server socket!\n");
        return -1;
    }

    if (!make_nonblock(fd))
    {
        close(fd);
        Loger("Can't make tune server socket nonblock!\n");
        return -1;
    }

    sock_opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt))
        < 0)
    {
        printf("Can't set tune server socket option SO_REUSEADDR!\n");
    }

    sock_opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &sock_opt, sizeof(sock_opt))
        < 0)
    {
        Loger("Can't set tune server socket option SO_KEEPALIVE!\n");
    }

#ifdef TCP_OPT
    sock_opt = 60;
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &sock_opt, sizeof(sock_opt))
        < 0)
    {
        Loger("Can't set tune server socket option TCP_KEEPIDLE!\n");
    }

    sock_opt = 60;
    if (setsockopt
        (fd, IPPROTO_TCP, TCP_KEEPINTVL, &sock_opt, sizeof(sock_opt)) < 0)
    {
        Loger("Can't set tune server socket option TCP_KEEPINTVL!\n");
    }
#endif

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    char log[64];

    if (bind(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        close(fd);
        sprintf(log, "Error binding server with port:%d\n", port);
        Loger(log);
        return -1;
    }

    if (listen(fd, 1) < 0)
    {
        close(fd);
        Loger("Listen tune server socket error!\n");
        return -1;
    }
    sprintf(log, "tune server listening port:%d\n",
            ntohs(server_addr.sin_port));
    Loger(log);
    return fd;
}


void set_timer_a(int timer)
{
    timerA = timer;
}

void set_timer_b(int timer)
{
    timerB = timer;
}

void set_timer_c(int timer)
{
    timerC = timer;
}

void sig_SIGPIPE_hndlr(int signo)
{
    Loger("Signal SIGPIPE received!\n");
}

bool fRecur = false;

void sig_SIGTERM_hndlr(int signo)
{
    if (!fRecur)
    {
        fRecur = true;
        Loger("Terminating process...\n");
        close(Scomm_fd);
        close(Server_fd);
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            close(Client_fd[i]);
        }
    }
    exit(signo);
}

void sig_SIGALRM_hndlr(int signo)
{
    if (timerA > 0)
    {
        timerA--;
        if (timerA == 0)
        {
            runTimerA = true;
        }
    }

    if (timerB > 0)
    {
        timerB--;
        if (timerB == 0)
        {
            runTimerB = true;
        }
    }

    if (timerC > 0)
    {
        timerC--;
        if (timerC == 0)
        {
            runTimerC = true;
        }
    }
    real_timer.it_value.tv_sec = 0;
    real_timer.it_value.tv_usec = 100000;
    setitimer(ITIMER_REAL, &real_timer, NULL);
}

#define CONF_STR_BUFFER_SIZE    300000

#define MEM_CONF_SIZE                           0x14000
#define MEM_CONF_SIZE_EX                        MEM_CONF_SIZE+CONF_STR_BUFFER_SIZE

BYTE *m_pConfBuffer;
DWORD m_BufferOffset;


void BeginReadConfig(unsigned char m_nMod, DWORD offset)
{
    //
    //
    BYTE buf[sizeof(CNetMessage) + 4];
    CNetMessage *mes = (CNetMessage *) buf;

    mes->dst.nMod = m_nMod;
    mes->nMessage = NET_MES_CONFBACK_GET;
    *(DWORD *) mes->pData() = offset;

    smp.SendPacket((BYTE *) mes, (unsigned short) mes->nHead() + 4);
//    SetTimer(TIMER_WAIT_CONFGET,m_AT_ReadConf,NULL);
}

void FillListAbon(CCfg * pCfg)
{

    TShortAbonInfo tmpAb;

    std::string tmpnum;
    CCfgModule *pMod = pCfg->module;
    CCfgProfil *pProfil = pCfg->profil;

    for (int i = 0; i < pCfg->numModule; i++)
    {
        CCfgSlot *pSlot = pMod[i].slot;

        int count = 0;
        int j2;

        for (j2 = 0; j2 < pMod[i].numSlot; j2++)
        {
            count += pSlot[j2].numPort;
        }
        std::string * mas_num = new std::string[count];
        if (mas_num == NULL)
            printf("error\n");

        for (j2 = 0; j2 < count; j2++)
            mas_num[j2] = "";

        for (int i1 = pMod[i].place; i1 < pMod[i].place + pMod[i].amount;
             i1++)
        {
            count = 0;
            // Parsim slot
            int j;

            for (j = 0; j < pMod[i].numSlot; j++)
            {
                CCfgPort *pPort = pSlot[j].port;


                if (j != 0)
                    count += pSlot[j - 1].numPort;
                for (int j1 = pSlot[j].place;
                     j1 < pSlot[j].place + pSlot[j].amount; j1++)
                {


                    for (int k = 0; k < pSlot[j].numPort; k++)
                    {

                        if (pProfil[pPort[k].prof].type ==
                            (SIGNALLING_EXT + 0x100))
                        {
                            if (mas_num[count + k] == "")
                                mas_num[count + k] = pPort[k].numberA;

                            std::string beg_str = mas_num[count + k];

                            int mode = 0;

                            if (beg_str.find("+") != std::string::npos)
                            {
                                mode = 1;
                                beg_str.erase(beg_str.length() - 1);
                            }
                            else if (beg_str.find("-") != std::string::npos)
                            {
                                mode = 2;
                                beg_str.erase(beg_str.length() - 1);
                            }
                            beg_str = "1" + beg_str;
                            DWORD beg_num = (DWORD) atoi(beg_str.data());
                            int l;

                            for (l = 0; l < pPort[k].amount; l++)
                            {
                                tmpAb.nMod = i1;
                                tmpAb.nSlot = j1 + 1;
                                tmpAb.nPort = pPort[k].place + l + 1;

                                if (mode != 2)
                                    tmpAb.tuneID = beg_num + l;
                                else
                                    tmpAb.tuneID = beg_num - l;
                                m_listabon.push_back(tmpAb);
//                                printf("nMod %d, mSlot %d, nPort %d, tuneID %d\n", tmpAb.nMod, tmpAb.nSlot, tmpAb.nPort, tmpAb.tuneID);
                            }
                            char eraseStr[100];

                            if (mode == 1)
                                sprintf(eraseStr, "%d+",
                                        atoi((const char *) beg_str.data()) +
                                        l);
                            else if (mode == 2)
                                sprintf(eraseStr, "%d-",
                                        atoi((const char *) beg_str.data()) -
                                        l);
                            strcpy(eraseStr, eraseStr + 1);
                            mas_num[count + k] = eraseStr;
                        }
                    }
                }
            }
            // Parsim PCM
            CCfgPcm *pPcm = pMod[i].pcm;

            for (j = 0; j < pMod[i].numPcm; j++)
            {
                CCfgPort *pPort = pPcm[j].port;

                for (int k = 0; k < pPcm[j].numPort; k++)
                {
                    if (pProfil[pPort[k].prof].type ==
                        (SIGNALLING_EXT + 0x100))
                    {
                        std::string sTmp = pPort[k].numberA;
//                        if((sTmp.data())[0]== (char)"+")
                        if (sTmp.find('+', 1) != std::string::npos)
                            sTmp.erase(sTmp.length() - 1);
                        sTmp = "1" + sTmp;
                        for (int iPcm = pPcm[j].place;
                             iPcm < pPcm[j].place + pPcm[j].amount; iPcm++)
                        {
                            for (int iPort = pPort[k].place;
                                 iPort < pPort[k].place + pPort[k].amount;
                                 iPort++)
                            {
                                tmpAb.nMod = i1;
                                tmpAb.nSlot = iPcm + 1;
                                tmpAb.nPort = iPort;
                                DWORD dwID = atoi(sTmp.data());

                                tmpAb.tuneID = dwID;
                                dwID++;
                                sprintf((char *) sTmp.data(), "%ld", dwID);
                                m_listabon.push_back(tmpAb);
                            }
                        }
                    }
                }
            }
            // Parsim AIR
            CCfgAir *pAir = pMod[i].air;

            for (j = 0; j < pMod[i].numAir; j++)
            {
                CCfgPort *pPort = pAir[j].port;

                for (int k = 0; k < pAir[j].numPort; k++)
                {
                    if (pProfil[pPort[k].prof].type ==
                        (SIGNALLING_EXT + 0x100))
                    {
                        std::string sTmp = pPort[k].numberA;
                        if (sTmp.find('+', 1) != std::string::npos)
//                        if((sTmp.data())[0]== (char)"+")
                            sTmp.erase(sTmp.length() - 1);
                        sTmp = "1" + sTmp;
                        for (int iAir = pAir[j].place;
                             iAir < pAir[j].place + pAir[j].amount; iAir++)
                        {
                            for (int iPort = pPort[k].place;
                                 iPort < pPort[k].place + pPort[k].amount;
                                 iPort++)
                            {
                                tmpAb.nMod = i1;
                                tmpAb.nSlot = iAir + 1;
                                tmpAb.nPort = iPort + 1;
                                DWORD dwID = atoi(sTmp.data());

                                tmpAb.tuneID = dwID;
                                dwID++;
//                                sTmp.Format("%d",dwID);
                                sprintf((char *) sTmp.data(), "%u", (unsigned int)dwID);
                                m_listabon.push_back(tmpAb);
                            }
                        }
                    }
                }
            }


        }
        delete[]mas_num;
    }


}

void HandlingConfig()
{

    CAll *pCAll = (CAll *) m_pConfBuffer;

    if (!pCAll->checkControlSum())
    {
        printf("Error control sum\n");
        return;
    }
    pCAll->decodeCfg();


    CCfg *pCfg = pCAll->cfg[0]; // ÕÛÊÌ‡ˇ ÍÓÌÙË„‡

    printf("Fill list is start\n");
    FillListAbon(pCfg);
    printf("Load config complite\n");

}

extern void parse_command(char *id, char *command);

void OnMessage(CSMPMessage * mes)
{
    switch (mes->mes)
    {
      case SMPMESCLIENT_CONNECT_OK:
//            EventTCPUp();
          smp.SwitchBinary(Parser.sPassword);
          break;
      case SMPMESCLIENT_CONNECT_ERROR:
          break;
      case SMPMESCLIENT_CLOSE:
      {
          for (int i = 0; i < MAX_MOD; i++)
              mas_mod[i] = 255;
//                EventTCPDown();
      }
          break;
      case SMPMESCLIENT_BINARYMODE_ER:
      {
//            EventTCPDown();
          Loger("Invalid password!\n");
          sig_SIGTERM_hndlr(1);
      }
          break;

      case SMPMESCLIENT_BINARYMODE_OK:
      {
          Loger("BINARYMODE_OK\n");
          // Â”Ã… œÀ ‘œ
          printf("Connect Scomm is ok\n");

//          set_timer_b(TIMER_SEND_REGISTER_MONITOR_VALUE);

          CNetMessage xmes;

          xmes.dst.nMod = 0xFF;
          xmes.nMessage = NET_MES_HELLO;
          xmes.un.multi.set.Full();

          uc buf[sizeof(CNetMessageBody)];
          uc *p = xmes.encode(buf);

          smp.SendPacket(buf, p - buf);

      }
          break;

      case SMPMESCLIENT_PACKET:
      {
          CNetMessage *net = (CNetMessage *) mes->data;

          switch (net->nMessage)
          {


            case NET_MES_TESTEXT_TAKE:
            {
                write(filedes[1], net, mes->len);

            };
                break;

            case NET_MES_TUNE_SETOK:
            {
                printf("Tune set ok\n");

            };
                break;

            case NET_MES_TUNE_GETOK:
            {
                printf("Tune get ok\n");

                TTuneData *tune = (TTuneData *) (net->pData() + 5);

                write(filedes[1], tune, sizeof(TTuneData));
//      char out[300];

//      sprintf(out, "print use=%s\naon=%s\ncat=%d\nredirect=%s\nalarm=%s\npincode=%s\npriv=%d\n", tune->ext.fUse ? "On" : "Off", tune->ext.sAON, tune->ext.nCatAON, tune->ext.sRedirect, tune->ext.sAlarm, tune->ext.sPinCode, tune->ext.nPrivLevel);
//      parse_command(out);

            };
                break;
            case NET_MES_HELLO:
            {
                if (m_pConfBuffer == NULL)      // «‡˘ËÚ‡ ÓÚ ÌÂÒÍÓÎ¸ÍËı hello
                {
                    printf("Hello from module %d\n", net->src.nMod);
                    m_pConfBuffer = (BYTE *) malloc(MEM_CONF_SIZE_EX);
                    m_BufferOffset = 0;
                    BeginReadConfig(net->src.nMod, m_BufferOffset);
                    printf("Wait load configuration\n");
                }
            };
                break;

            case NET_MES_CONFBACK_TAKE:
            {


                DWORD dataSize = *(DWORD *) (net->pData() + 4); //

                if (dataSize == 0)
                {               //
                    //
                    printf("Handle is start\n");
                    HandlingConfig();
                    delete mes;

                    return;

                }
                //
                if (m_BufferOffset != *(DWORD *) (net->pData()))
                {
                    free(m_pConfBuffer);
                    printf("Error read config\n");
                    delete mes;

                    return;
                }

                memcpy(m_pConfBuffer + m_BufferOffset, (net->pData() + 8),
                       dataSize);

                m_BufferOffset += dataSize;
                BeginReadConfig(net->src.nMod, m_BufferOffset);

//                SetTimer(TIMER_WAIT_CONFGET, m_AT_ReadConf, NULL);

            };
                break;
            default:
            {
//                printf("unsupported mes %x message %x module %d\n", mes->mes,
//                       net->nMessage, net->src.nMod);
            }

          }

      }

    }
    delete mes;
}



void timer_a()
{
    printf("Timer A is ok\n");
}

void timer_b()
{
    printf("Timer B is ok\n");
}

void timer_c()
{
    printf("Timer A is ok\n");
}



void Reinit_connection()
{
    close(Scomm_fd);
    sact.sa_handler = SIG_IGN;
    sigaction(SIGALRM, &sact, NULL);
    while ((Scomm_fd = Login_ethernet(Parser.sScommIp, Parser.ScommPort)) < 0)
    {
        sleep(5);
    }
    set_timer_b(-1);
    smp.OnConnect();
    sact.sa_handler = sig_SIGALRM_hndlr;
    sigaction(SIGALRM, &sact, NULL);
    real_timer.it_value.tv_sec = 0;
    real_timer.it_value.tv_usec = 100000;
    setitimer(ITIMER_REAL, &real_timer, NULL);

}



int max(int x, int y)
{
    return x > y ? x : y;
}

void *Server_ptread(void *arg)
{
    char log[80];
    char sClient_ip[MAX_CLIENT][16];

    pthread_detach(pthread_self());
    fd_set fds;
    struct timeval tv;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int retval;
    int i, maxfd;

    while (1)
    {
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        for (i = 0; i < MAX_CLIENT; i++)
        {
            if (Client_fd[i] > 0)
            {
                FD_SET(Client_fd[i], &fds);
                maxfd = max(maxfd, Client_fd[i]);
            }
        }
        if (Server_fd)
            FD_SET(Server_fd, &fds);

        if ((retval =
             select(max(Server_fd, maxfd) + 1, &fds, NULL, NULL, &tv)) > 0)
        {
            if (FD_ISSET(Server_fd, &fds) > 0)
            {
                int clnt_fd =
                    accept(Server_fd, (struct sockaddr *) &client_addr,
                           &client_addr_size);

                if (clnt_fd < 0)
                {
                    Loger("Server: Client accept error!\n");
                }
                else
                {
                    for (i = 0; i < MAX_CLIENT; i++)
                        if (!Client_fd[i])
                            break;
                    if (i < MAX_CLIENT) // Esli kanal s ATS ne rabochij
                    {
                        if (make_nonblock(clnt_fd) < 0)
                        {
                            close(clnt_fd);
                            sprintf(log,
                                    "Server: Can't make client socket nonblock! IP:%s\n",
                                    inet_ntoa(client_addr.sin_addr));
                            Loger(log);
                        }
                        else
                        {
                            Client_fd[i] = clnt_fd;
                            strcpy(sClient_ip[i],
                                   inet_ntoa(client_addr.sin_addr));
                            sprintf(log,
                                    "Server: Client%d connected! IP:%s\n", i,
                                    sClient_ip[i]);
                            Loger(log);

//                            write(Client_fd[i], "I_AM_TUNE\0",
//                                  strlen("I_AM_TUNE") + 1);

                        }
                    }
                    else        //Esli kanal rabochij dobavliajem novogo klienta
                    {
                        close(clnt_fd);
                        sprintf(log,
                                "Server: Client%d accept denied! IP:%s\n", i,
                                inet_ntoa(client_addr.sin_addr));
                        Loger(log);
                    }
                }
            }
            for (i = 0; i < MAX_CLIENT; i++)
            {
                if (FD_ISSET(Client_fd[i], &fds) > 0)
                {
                    unsigned char temp_buff[1000];
                    int size = read(Client_fd[i], temp_buff, sizeof(temp_buff));

                    if (size > 0)       // if rcv data from client socket
                    {
                        static char input_string[1000];
                        static int pos = 0;

                        strncpy(input_string + pos, (char *) temp_buff, size);
                        pos += size;
                        bool fInputOk = false;
                        int k;

                        for (k = 0; k < pos; k++)
                        {
                            if (input_string[k] == 10
                                || input_string[k] == 13)
                            {
                                input_string[k] = 0;
                                fInputOk = true;
                                break;
                            }

                        }
                        if (!fInputOk)
                            continue;


                        int oldstdout = dup(1);

                        if (dup2(Client_fd[i], 1) < 0)
                        {

                            printf("%s\n", strerror(errno));
                            continue;
                        }
                        char id[100];

                        sprintf(id, "IP %s", sClient_ip[i]);
                        parse_command(id, input_string);
                        if (dup2(oldstdout, 1) < 0)
                        {
                            printf("%s\n", strerror(errno));
                        }
                        close(oldstdout);
                        memset(input_string, 0, sizeof(input_string));
                        pos = 0;

                    }
                    else if ((size == 0) || ((size < 0) && (errno != EINTR)))
                    {
                        sprintf(log, "Server: Client%d disconnected! IP:%s\n",
                                i, sClient_ip[i]);
                        Loger(log);
                        close(Client_fd[i]);
                        Client_fd[i] = 0;
                    }
                }
            }
        }
        else if (retval < 0 && errno != EINTR)  //Esli oshibka ne sviazana s prepivanijem signalom
        {
            Loger("Server internal error!\n");
        }
    }
    close(Server_fd);
    return NULL;
}


int waitdata(BYTE * data, int data_len)
{
    int res;
    fd_set rset;

    struct timeval tv;

    int counter = 0;

    do
    {
        FD_ZERO(&rset);         // Set Zero
        FD_SET(filedes[0], &rset);

        tv.tv_sec = 10;
        tv.tv_usec = 0;

        while (1)
        {
            if ((res = (select(filedes[0] + 1, &rset, NULL, NULL, &tv))) > 0)
            {
                int size = read(filedes[0], data, data_len);
                return size;
            }
            if (res < 0 && errno == EINTR)
                continue;
            break;
        }
        counter++;
        if (counter >= 3)
            return -1;
    } while (1);

    return -1;
}

#define MAX_DEFINE_DVO 28

struct DVO
{
    char *name;
    BYTE pos;
};

const DVO dvo[MAX_DEFINE_DVO] = { {(char *)"cmn_tone", BEXT_CMN_TONE},
{(char *)"cmn_outcome", BEXT_CMN_OUTCOME},
{(char *)"cmn_income", BEXT_CMN_INCOME},
{(char *)"cmn_external", BEXT_CMN_EXTERNAL},
{(char *)"cmn_payservice", BEXT_CMN_PAYSERVICE},
{(char *)"cmn_8xxx", BEXT_CMN_8xxx},
{(char *)"cmn_10xxx", BEXT_CMN_10xxx},
{(char *)"cmn_aon", BEXT_CMN_AON},
{(char *)"cmn_82xxx", BEXT_CMN_82xxx},
{(char *)"enb_flash", BEXT_ENB_FLASH},
{(char *)"enb_redirect", BEXT_ENB_REDIRECT},
{(char *)"enb_alarm", BEXT_ENB_ALARM},
{(char *)"enb_pincode", BEXT_ENB_PINCODE},
{(char *)"enb_autodial", BEXT_ENB_AUTODIAL},
{(char *)"enb_intercept", BEXT_ENB_INTERCEPT},
{(char *)"enb_notification", BEXT_ENB_NOTIFICATION},
{(char *)"enb_evilcall", BEXT_ENB_EVILCALL},
{(char *)"enb_callerid", BEXT_ENB_CALLERID},
{(char *)"dvo_redirect_always", BEXT_DVO_REDIRECT_ALWAYS},
{(char *)"dvo_redirect_busy", BEXT_DVO_REDIRECT_BUSY},
{(char *)"dvo_redirect_silence", BEXT_DVO_REDIRECT_SILENCE},
{(char *)"dvo_alarm", BEXT_DVO_ALARM},
{(char *)"dvo_redirect_hotcall", BEXT_DVO_REDIRECT_HOTCALL},
{(char *)"dvo_notification", BEXT_DVO_NOTIFICATION},
{(char *)"dvo_nodisturb", BEXT_DVO_NODISTURB},
{(char *)"dvo_pincode", BEXT_DVO_PINCODE},
{(char *)"dvo_clir", BEXT_DVO_CLIR},
{(char *)"dvo_pincodetwo", BEXT_DVO_PINCODETWO}
};


TSHORT_LISTABON::const_iterator find_abon(DWORD int_id)
{
    TSHORT_LISTABON::const_iterator it;
    for (it = m_listabon.begin(); it != m_listabon.end(); it++)
    {
        if ((*it).tuneID == int_id)
            return it;
    }
    return (TSHORT_LISTABON::const_iterator) NULL;
}

void set_tune(int int_id, TTuneData * tune)
{
//    printf("it = %x\n", it);
//    printf("it.nmod = %x\n", (*it).nMod);
    // ŒÚÔ‡‚ÎˇÂÏ Í‡ÚÓ˜ÍÛ ‚ ÒÚ‡ÌˆË˛
    TSHORT_LISTABON::const_iterator it = find_abon(int_id);

    BYTE buf[sizeof(CNetMessage) + 400];
    CNetMessage *mes = (CNetMessage *) buf;

    mes->nMessage = NET_MES_TUNE_SET;
    mes->dst.nMod = (*it).nMod;
    *(DWORD *) (mes->pData()) = (*it).tuneID;
    *(mes->pData() + 4) = sizeof(TTuneData);
    memcpy((mes->pData() + 5), tune, sizeof(TTuneData));

    smp.SendPacket((BYTE *) mes, mes->nHead() + 4 + 1 + sizeof(TTuneData));

}

bool get_tune(int &int_id, TModule &mod_id, char *number, TTuneData *tune, int len)
{
    char str_id[100];

//    int int_id;

    sprintf(str_id, "1%s", number);
    sscanf(str_id, "%d", &int_id);
    TSHORT_LISTABON::const_iterator abon;

    if ((abon = find_abon(int_id)) != (TSHORT_LISTABON::const_iterator) NULL)
    {
        BYTE buf[sizeof(CNetMessage) + 4];
        CNetMessage *mes = (CNetMessage *) buf;

        mes->nMessage = NET_MES_TUNE_GET;
        mod_id = (*abon).nMod;
        mes->dst.nMod = (*abon).nMod;
        *(DWORD *) (mes->pData()) = (*abon).tuneID;
        smp.SendPacket((BYTE *) mes, mes->nHead() + 4);
        return waitdata((BYTE *)tune, len) >= 0;
    }
    printf("subscriber %s not found in configuration\n", number);
    return false;

}


DWORD round(DWORD value)
{
    if (value == 0)
        return value;

    DWORD v = value;

    int count = 0;

    while (true)
    {
        ++count;
        v /= 10;
        if (v == 0)
            break;
    }
    DWORD divisor = 1;

    for (int i = 0; i < count - 2; i++)
        divisor *= 10;

    return value - value % divisor;
}

DWORD correct(DWORD Uab, DWORD value, bool kilo)
{
    if (kilo)
        value *= 1000;

    const int len1 = 4;
    const int len2 = 28;

    DWORD r[len1][len2] = {
        {
           87,  170,  250,  330,  410,   490,   560,   630,   700,   770,  1400,  2600,  3100,  3700,
         4200, 4900, 5500, 6200, 6900, 12700, 17000, 20000, 22000, 24000, 26000, 27000, 29000, 30000
        },
        {
          104,  203,  301,  398,  495,   593,   690,   787,   882,   979,  1900,  2900,  3600,  4400,
         5200, 6100, 7100, 8000, 9610, 17000, 23000, 28000, 33000, 38000, 42000, 46000, 50000, 54000
        },
        {
          111,  218,  326,  433,   533,   652,   758,   851,   974,  1100,  2100,  3100,  3900,  4800,
         5800, 6900, 7900, 8970, 10200, 19400, 27000, 34000, 42000, 50000, 56000, 63000, 69000, 74000
        },
        {
          118,  229,  347,  459,   575,   690,   804,   923,  1040,  1170,  2500,  3200,  4100,  5030,
         6100, 7200, 8400, 9600, 10800, 20000, 29000, 39000, 49000, 59000, 68000, 77000, 85000, 92000
        }
    };

    DWORD u[len1] = { 49, 57, 62, 64 };
    DWORD rreal[len2] = 
    {
         100,  200,  300,  400,   500,   600,   700,   800,   900,  1000,  2000,  3000,  4000,   5000,
        6000, 7000, 8000, 9000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000
    };

    int index1 = len1;

    for (int i = 0; i < len1; i++)
    {
        if (Uab < u[i])
        {
            index1 = i;
            break;
        }
    }

    int realValue[2] = { -1, -1 };

    for (int j = 1; j >= 0; j--)
    {
        if (index1 >= 0 && index1 < 4)
        {
            int index2 = len2;
	    int i = 0;
            for (i = 0; i < len2; i++)
            {
                if (value < r[index1][i])
                {
                    index2 = i;
                    break;
                }
            }
            if (index2 == 0)
                realValue[j] = 0;
            else if (index2 == len2)
                realValue[j] = 666666;
            else
            {
                int diff =
                    100 * (r[index1][i] - value) / (r[index1][i] -
                                                    r[index1][i - 1]);
                realValue[j] =
                    rreal[i] - diff * (rreal[i] - rreal[i - 1]) / 100;
            }
        }

        --index1;
    }

    DWORD ret = 0;

    if (realValue[0] != -1)
    {
        if (realValue[1] != -1)
        {
            int diff =
                100 * (Uab - u[index1 + 1]) / (u[index1 + 2] - u[index1 + 1]);
            ret = realValue[0] - diff * (realValue[0] - realValue[1]) / 100;
        }
        else
            ret = realValue[0];
    }
    else
    {
        if (realValue[1] != -1)
            ret = realValue[1];
        else
            Loger("ERROR CORRECT");
    }

    return round(kilo ? ret / 1000 : ret);
}


int get_measure(DWORD & int_id, TModule & mod_id, char *number, short Uab, BYTE * data,
                 int len)
{
    char str_id[100];

    sprintf(str_id, "1%s", number);
    int_id = atoi(str_id);
    TSHORT_LISTABON::const_iterator abon;

    if ((abon = find_abon(int_id)) != (TSHORT_LISTABON::const_iterator) NULL)
    {
        int pos = 0;

        // –ó–∞–ø—Ä–∞—à–∏–≤–∞–µ–º –ö–§–°
        BYTE buf[sizeof(CNetMessage) + sizeof(DWORD)];

        memset(buf, 0, sizeof(buf));
        CNetMessage *mes = (CNetMessage *) buf;

        mes->nMessage = NET_MES_TESTEXT_GET;
        mod_id = (*abon).nMod;
        mes->dst.nMod = (*abon).nMod;
        *(DWORD*)(mes->pData()) = 0;
        smp.SendPacket((BYTE *) mes, mes->nHead() + sizeof(DWORD));
        int size = waitdata(data, len);

        if (size < 0)
            return -1;

        mes = (CNetMessage *) data;

        pos = sizeof(DWORD);
        BYTE l_ReturnCode = *(BYTE*)&mes->pData()[pos];
        pos += sizeof(BYTE);
        pos += sizeof(BYTE);
        pos += sizeof(BYTE);
        pos += sizeof(BYTE);
        pos += sizeof(BYTE);
        Uab = *(short*)&mes->pData()[pos];
                                                                        
	if (l_ReturnCode != 0)
    	    return -2;

        // –ò–∑–º–µ—Ä—è–µ–º –∞–±–æ–Ω–µ–Ω—Ç–∞
        memset(buf, 0, sizeof(buf));
        mes = (CNetMessage *) buf;

        mes->nMessage = NET_MES_TESTEXT_GET;
        mod_id = (*abon).nMod;
        mes->dst.nMod = (*abon).nMod;
        *(DWORD*)(mes->pData()) = (*abon).tuneID;

        smp.SendPacket((BYTE *) mes, mes->nHead() + sizeof(DWORD));

        size = waitdata(data, len);
        if (size < 0)
            return -1;


        return 0;
    }
    return -3;
}

void parse_command(char *id, char *command)
{

    if (strlen(command) == 0)
    {
        printf(">\n");
        return;

    }
    char str[1000];

    sprintf(str, "CMD from %s %s\n", id, command);
    Loger(str);
    if (!strcmp(command, "help"))
    {
        printf("list - list subscribers\n"
               "gettune - gettune subscriber\n"
               "settune - settune subscriber\n"
               "reload - reload configuration\n"
		"izmab - measuare subscriber line\n");               
        return;
    }

    if (!strncmp(command, "izmab", strlen("izmab")))
    {
        char number[100];

        if (sscanf(command + strlen("izmab"), "%s", number) == -1)
        {
            sprintf(str, "RESULS CMD from %s %s\n", id, "izmab need number");
            Loger(str);
            return;
        }
        DWORD int_id;
        TModule mod_id;

        BYTE data[500];
	short Uab = 0;
	int size = 0;
        if ((size = get_measure(int_id, mod_id, number, Uab, data, sizeof(data))) >= 0)
        {
            char str_id[20];
	    int pos = 0;

	    CNetMessage* in_net = (CNetMessage*)data;
            DWORD id = *(DWORD*)&in_net->pData()[pos]; pos += sizeof(DWORD);
            sprintf(str_id, "%u", (unsigned int)id);
            strcpy(str_id, str_id + 1);
            
            BYTE returnCode = *(BYTE*)&in_net->pData()[pos]; pos += sizeof(BYTE);
            
            char str_MAL[5];
            BYTE MAL = *(BYTE*)&in_net->pData()[pos]; pos += sizeof(BYTE);
            sprintf(str_MAL, "%.3d", MAL);
            
            char str_Slot[5];
            BYTE Slot = *(BYTE*)&in_net->pData()[pos]; pos += sizeof(BYTE);
            sprintf(str_Slot, "%.3d", Slot);
            
            char str_Hole[5];
            BYTE Hole = *(BYTE*)&in_net->pData()[pos]; pos += sizeof(BYTE);
            sprintf(str_Hole, "%.3d", Hole);

    	    char str_Uab[10];
            short l_Uab = *(short*)&in_net->pData()[pos]; pos += sizeof(short);
            (l_Uab /= 2) *= 2;
            sprintf(str_Uab, "%d–í", l_Uab);
            
	    char str_Uag[10];
            short l_Uag = *(short*)&in_net->pData()[pos]; pos += sizeof(short);
            (l_Uag /= 2) *= 2;
            sprintf(str_Uag, "%d–í", l_Uag);
            
	    char str_Ubg[10];
            short l_Ubg = *(short*)&in_net->pData()[pos]; pos += sizeof(short);
            (l_Ubg /= 2) *= 2;
            sprintf(str_Ubg, "%d–í", l_Ubg);

//	    printf("TTT1 %s %s %s\n", str_Uab, str_Uag, str_Ubg);
	    
	    char str_Rab[10];
            DWORD l_Rab = *(WORD*)&in_net->pData()[pos]; pos += sizeof(WORD);
//           printf("l_Rab %x\n", l_Rab);
            // –ö–û–º
            if (l_Rab & 0x8000)
            {
    		l_Rab = correct(Uab, l_Rab & 0x7fff, true);
    		sprintf(str_Rab, "%u–∫–û–º", (unsigned int)l_Rab);
            }
            else                // –ü—Ä–æ—Å—Ç–æ –û–ú—ã
            {
		l_Rab = correct(Uab, l_Rab, false);
    		sprintf(str_Rab, "%u–û–º", (unsigned int)l_Rab);
            }

	    char str_Rag[10];
            DWORD l_Rag = *(WORD*)&in_net->pData()[pos]; pos += sizeof(WORD);
//           printf("l_Rag %x\n", l_Rag);
            // –ö–û–º
            if (l_Rag & 0x8000)
            {
    		l_Rag = correct(Uab, l_Rag & 0x7fff, true);
    		sprintf(str_Rag, "%u–∫–û–º", (unsigned int)l_Rag);
            }
            else                // –ü—Ä–æ—Å—Ç–æ –û–ú—ã
            {
		l_Rag = correct(Uab, l_Rag, false);
    		sprintf(str_Rag, "%u–û–º", (unsigned int)l_Rag);
            }

	    char str_Rbg[10];
	    DWORD l_Rbg = *(WORD*)&in_net->pData()[pos]; pos += sizeof(WORD);
            // –ö–û–º
            if (l_Rbg & 0x8000)
            {
    		l_Rbg = correct(Uab, l_Rbg & 0x7fff, true);
    		sprintf(str_Rbg, "%u–∫–û–º", (unsigned int)l_Rbg);
            }
            else                // –ü—Ä–æ—Å—Ç–æ –û–ú—ã
            {
		l_Rbg = correct(Uab, l_Rbg, false);
    		sprintf(str_Rbg, "%u–û–º", (unsigned int)l_Rbg);
            }
//	    printf("TTT2 %s %s %s\n", str_Rab, str_Rag, str_Rbg);

	    
            // –°—Ç–∞—Ä—à–∞—è —á–∞—Å—Ç—å –µ–º–∫–æ—Å—Ç–∏
            char str_Cab[20];
            WORD l_Cab = *(WORD*)&in_net->pData()[pos]; pos += sizeof(WORD);
            if (l_Cab == 0xffee)
            {
                // –û—à–∏–±–∫–∞
                sprintf(str_Cab, "---");
            }
            else
            {
                double l_dCab = l_Cab;
                l_dCab *= 0.1;
                sprintf(str_Cab, "%4.2f –º–∫–§", l_dCab);
            }

            // –ú–ª–∞–¥—à–∞—è —á–∞—Å—Ç—å –µ–º–∫–æ—Å—Ç–∏
            WORD l_Cabmini = *(WORD*)&in_net->pData()[pos]; pos += sizeof(WORD);

            if ((l_Cabmini >= 0) && (l_Cabmini <= 1000))
            {
                double l_dCab = (int) (l_Cab * 0.1) + l_Cabmini * 0.001;
		sprintf(str_Cab, "%4.2f –º–∫–§", l_dCab);
            }

	    char out[200];
	    switch (returnCode)
	    {
		// –í—Å–µ –æ–∫
		case 0:
		{
		    if (l_Uab > 0 || l_Uag > 0 ||l_Ubg > 0)
		    {
		        //                N  S  H  UabUagUbgRabRagRbgCabSt Res
			sprintf(out, "%s %s %s %s: %s %s %s %s %s %s %s %s %s\n", str_id, str_MAL, str_Slot, str_Hole, str_Uab, str_Uag, str_Ubg, \
				"---", "---", "---", "---", "---", "–ü–æ—Å—Ç–æ—Ä–æ–Ω–Ω–µ–µ –Ω–∞–ø—Ä—è–∂–µ–Ω–∏–µ –Ω–∞ –ª–∏–Ω–∏–∏");
		    }
		    else
		    {
			sprintf(out, "%s %s %s %s: %s %s %s %s %s %s %s %s %s\n", str_id, str_MAL, str_Slot, str_Hole, str_Uab, str_Uag, str_Ubg, \
				str_Rab, str_Rag, str_Rbg, str_Cab, "–°–≤–æ–±–æ–¥–µ–Ω", "OK");
		    }
		    
		}; break;
		
		case 1:
		{
			sprintf(out, "%s %s %s %s: %s %s %s %s %s %s %s %s %s\n", str_id, str_MAL, str_Slot, str_Hole, "", "", "", \
				"", "", "", "", "", "–ê–±–æ–Ω–µ–Ω—Ç –Ω–µ –Ω–∞–π–¥–µ–Ω");

		}; break;


		case 2:
		{
			sprintf(out, "%s %s %s %s: %s %s %s %s %s %s %s %s %s\n", str_id, str_MAL, str_Slot, str_Hole, "", "", "", \
				"", "", "", "", "", "–û—à–∏–±–∫–∞ –∏–∑–º–µ—Ä–µ–Ω–∏—è");

		}; break;

		case 3:
		{
			sprintf(out, "%s %s %s %s: %s %s %s %s %s %s %s %s %s\n", str_id, str_MAL, str_Slot, str_Hole, "", "", "", \
				"", "", "", "", "–ó–∞–Ω—è—Ç", "–®–ª–µ–π—Ñ –∑–∞–º–∫–Ω—É—Ç");

		}; break;

		case 4:
		{
			sprintf(out, "%s %s %s %s: %s %s %s %s %s %s %s %s %s\n", str_id, str_MAL, str_Slot, str_Hole, "", "", "", \
				"", "", "", "", "–ë–µ–∑–æ—Ç–±–æ–π–Ω–æ–µ", "–û—à–∏–±–∫–∞ –∏–∑–º–µ—Ä–µ–Ω–∏—è");

		}; break;


		case 5:
		{
			sprintf(out, "%s %s %s %s: %s %s %s %s %s %s %s %s %s\n", str_id, str_MAL, str_Slot, str_Hole, str_Uab, str_Uag, str_Ubg, \
				str_Rab, str_Rag, str_Rbg, str_Cab, "–ë–µ–∑–æ—Ç–±–æ–π–Ω–æ–µ", "OK");
		}; break;

		case 6:
		{
			sprintf(out, "%s %s %s %s: %s %s %s %s %s %s %s %s %s\n", str_id, str_MAL, str_Slot, str_Hole, "", "", "", \
				"", "", "", "", "–ù–µ—Å–æ–≤–º–µ—Å—Ç–∏–º–æ—Å—Ç—å –∏–∑–º–µ—Ä–∏—Ç–µ–ª–µ–π", "–û—à–∏–±–∫–∞ –∏–∑–º–µ—Ä–µ–Ω–∏—è");

		}; break;
	    }
            Loger(out);

        }
        else
        {
	    switch (size)
	    {
		case -1:  sprintf(str, "RESULS CMD from %s %s\n", id, "error read get_measure"); break;
		case -2:  sprintf(str, "RESULS CMD from %s %s\n", id, "error meter not found"); break;
		case -3:  sprintf(str, "RESULS CMD from %s %s\n", id, "subscriber not found"); break;
	    }
    
            Loger(str);
        }
        return;
    }

    if (!strcmp(command, "list"))
    {
        if (m_listabon.size() == 0)
        {
            sprintf(str, "RESULS CMD from %s %s\n", id, "error subscriber not found or not load config");
            Loger(str);

        }
        else
        {
            TSHORT_LISTABON::const_iterator it;
            int i = 0;

            for (it = m_listabon.begin(); it != m_listabon.end(); it++)
            {
                i++;
                printf("%d nMod = %d nSlot = %d nPort %d nID %d\n", i,
                       (*it).nMod, (*it).nSlot, (*it).nPort,
                       (int) (*it).tuneID);
            }
            sprintf(str, "RESULS CMD from %s %s\n", id, "list ok");
            Loger(str);
        }
        return;
    }


    if (!strncmp(command, "gettune", strlen("gettune")))
    {
        char number[100];

        if (sscanf(command + strlen("gettune"), "%s", number) == -1)
        {
            sprintf(str, "RESULS CMD from %s %s\n", id,
                    "gettune need number");
            Loger(str);
            return;
        }
        int int_id;
        TModule mod_id;

        TTuneData tune;

        if (get_tune(int_id, mod_id, number, &tune, sizeof(TTuneData)))
        {
            printf("number=%s\n"
                   "use=%s\n"
                   "aon=%s\n"
                   "cat=%d\n"
                   "redirect=%s\n"
                   "alarm=%s\n"
                   "pincode=%s\n"
                   "priv=%d\n"
                   "module=%d\n"
                   "slot=%d\n"
                   "port=%d\n",
                   number,
                   tune.ext.fUse ? "on" : "off",
                   tune.ext.sAON,
                   tune.ext.nCatAON,
                   tune.ext.sRedirect,
                   tune.ext.sAlarm, tune.ext.sPinCode, tune.ext.nPrivLevel, mod_id, tune.ext.nSlot + 1, tune.ext.nHole + 1);

            for (int i = 0; i < MAX_DEFINE_DVO; i++)
                printf("%s=%s\n", dvo[i].name,
                       tune.ext.bFlags.GetBit(dvo[i].pos) ? "on" : "off");
            sprintf(str, "RESULS CMD from %s %s\n", id, "gettune ok");
            Loger(str);

        }
        else
        {
            sprintf(str, "RESULS CMD from %s %s\n", id, "error read tune");
            Loger(str);
        }
        return;

        return;
    }


    if (!strncmp(command, "settune", strlen("settune")))
    {

        char number[100];
        char str_id[100];

        if (sscanf(command + strlen("settune"), "%s", number) == -1)
        {
            sprintf(str, "RESULS CMD from %s %s\n", id,
                    "settune need number");
            Loger(str);
            return;
        }
        sprintf(str_id, "1%s", number);
        TTuneData tune;
        int int_id;
        TModule mod_id;

        if (get_tune(int_id, mod_id, number, &tune, sizeof(TTuneData)))
        {
            // “ÛÚ Ô‡ÒËÏ ÍÓÏ‡Ì‰ÌÛ˛ ÒÚÓÍÛ Ë Á‡ÔÓÎÌˇÂÏ Í‡ÚÓ˜ÍÛ ‡·ÓÌÂÌÚ‡
            int res;
            char tocken[100];
            char *pos = command;

            while ((res = sscanf(pos, "%s", tocken)) > 0)
            {
                if (!strncmp(tocken, "use", strlen("use")))
                {
                    char yes_no[20];

                    sscanf(tocken + strlen("use"), "=%s", yes_no);
                    if (!strcmp(yes_no, "yes") || !strcmp(yes_no, "on"))
                        tune.ext.fUse = true;
                    if (!strcmp(yes_no, "no") || !strcmp(yes_no, "off"))
                        tune.ext.fUse = false;

                }
                if (!strncmp(tocken, "aon", strlen("aon")))
                {
                    char aon[10];

                    sscanf(tocken + strlen("aon"), "=%s", aon);
                    strncpy(tune.ext.sAON, aon, 8);

                }

                if (!strncmp(tocken, "cat", strlen("cat")))
                {
                    int cat;

                    sscanf(tocken + strlen("cat"), "=%d", &cat);
                    tune.ext.nCatAON = cat;
                }
                if (!strncmp(tocken, "redirect", strlen("redirect")))
                {
                    char redirect[10];

                    sscanf(tocken + strlen("redirect"), "=%s", redirect);
                    strncpy(tune.ext.sRedirect, redirect, 12);
                }

                if (!strncmp(tocken, "alarm", strlen("alarm")))
                {
                    char alarm[10];

                    sscanf(tocken + strlen("alarm"), "=%s", alarm);
                    strncpy(tune.ext.sAlarm, alarm, 6);
                }

                if (!strncmp(tocken, "pincode", strlen("pincode")))
                {
                    char pincode[10];

                    sscanf(tocken + strlen("pincode"), "=%s", pincode);
                    strncpy(tune.ext.sPinCode, pincode, 5);
                }
                if (!strncmp(tocken, "priv", strlen("priv")))
                {
                    int priv;

                    sscanf(tocken + strlen("priv"), "=%d", &priv);
                    tune.ext.nPrivLevel = priv;
                }


                for (int i = 0; i < MAX_DEFINE_DVO; i++)
                {
                    if (!strncmp(tocken, dvo[i].name, strlen(dvo[i].name)))
                    {
                        char yes_no[20];

                        sscanf(tocken + strlen(dvo[i].name), "=%s", yes_no);
                        if (!strcmp(yes_no, "yes") || !strcmp(yes_no, "on"))
                            tune.ext.bFlags.WriteBit(dvo[i].pos, true);
                        if (!strcmp(yes_no, "no") || !strcmp(yes_no, "off"))
                            tune.ext.bFlags.WriteBit(dvo[i].pos, false);

                    }
                }


                pos += strlen(tocken) + 1;      // 1 Ì‡ ‡Á‰ÂÎËÚÂÎ¸
            }
            TSHORT_LISTABON::const_iterator it = find_abon(int_id);

            set_tune(int_id, &tune);
            // ŒÚÔ‡‚ÎˇÂÏ Í‡ÚÓ˜ÍÛ ‚ ÒÚ‡ÌˆË˛
            sprintf(str, "RESULS CMD from %s %s\n", id, "settune ok");
            Loger(str);

        }
        else
        {
            sprintf(str, "RESULS CMD from %s %s\n", id, "error read tune");
            Loger(str);
        }
        return;

        return;
    }

    if (!strncmp(command, "reload", strlen("reload")))
    {
        printf("reload\n");
        return;
    }

    printf("unknown command %s\n", command);

}

void *terminal_ptread(void *arg)
{
    int retval;

    pthread_detach(pthread_self());
    fd_set fds;
    struct timeval tv;
    char input_string[1000];

    memset(input_string, 0, sizeof(input_string));
    while (1)
    {
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(0, &fds);        // 1 fileDiscriptor - input

        if ((retval = select(0 + 1, &fds, NULL, NULL, &tv)) > 0)
        {
            if (FD_ISSET(0, &fds) > 0)
            {
                unsigned char temp_buff[1000];
                int size = read(0, temp_buff, sizeof(temp_buff));

                strncpy(input_string, (char *) temp_buff, size - 1);
                parse_command((char *)"CONSOLE", input_string);
                memset(input_string, 0, sizeof(input_string));
            }
        }
    }
    return NULL;

}

int main(int argc, char *argv[])
{
    if (Parser.ParseCStringParams(argc, argv) < 0)
    {
        exit(1);
    }

    printf
        ("<-------------------------------Tune v%s---------------------------->\n", VERSION);
    if (Parser.FillMainParams() < 0)
    {
        exit(1);
    }
    printf
        ("<---------------------------------------------------------------------->\n");

    StrToLog("Tune started....\n");

    if ((Server_fd = Create_server_point(Parser.ServerPort)) < 0)
    {
        Loger("Error create server point!\r\n");
        Server_fd = 0;
    }


    for (int i = 0; i < MAX_CLIENT; i++)
    {
        Client_fd[i] = 0;
    }

    timerA = timerB = timerC = -1;
    runTimerA = false;
    runTimerB = false;
    runTimerC = false;

    for (int i = 0; i < MAX_MOD; i++)
        mas_mod[i] = 255;

    if (pipe(filedes) < 0)
        return -1;

    if (Parser.fDaemon)
        daemon(0, 0);


    while ((Scomm_fd = Login_ethernet(Parser.sScommIp, Parser.ScommPort)) < 0)
    {
        sleep(5);
    }


    smp.OnConnect();

    if (Server_fd)
    {
        if (pthread_create(&Server_tid, NULL, &Server_ptread, NULL) != 0)
        {
            Loger("Can't create server thread!\n");
        }
    }

    if (!Parser.fDaemon)
        if (pthread_create(&terminal_tid, NULL, &terminal_ptread, NULL) != 0)
        {
            Loger("Can't create terminal thread!\n");
        }

    sigfillset(&sact.sa_mask);
    sact.sa_flags = 0;
    sact.sa_handler = sig_SIGTERM_hndlr;
    sigaction(SIGINT, &sact, NULL);
    sigaction(SIGTERM, &sact, NULL);

    sigemptyset(&sact.sa_mask);
    sact.sa_handler = sig_SIGPIPE_hndlr;
    sigaction(SIGPIPE, &sact, NULL);

    sact.sa_handler = sig_SIGALRM_hndlr;
    //sact.sa_flags |= SA_RESTART;
    sigaction(SIGALRM, &sact, NULL);

    real_timer.it_value.tv_sec = 0;
    real_timer.it_value.tv_usec = 100000;
    setitimer(ITIMER_REAL, &real_timer, NULL);

    struct timeval tv;
    fd_set fds;
    int retval;

    time_t UpdateT1;

    time(&UpdateT1);

    while (1)
    {
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(Scomm_fd, &fds);
        if ((retval = select(Scomm_fd + 1, &fds, NULL, NULL, &tv)) > 0)
        {
            char buffer[1024];
            int size = read(Scomm_fd, buffer, 1024);

            if (size > 0)       // if rcv data from tty
            {
                smp.OnReceive(buffer, size);
            }
            else
            {
                if (!size || (size < 0 && errno != EINTR))
                {
                    Loger("Scomm socket closed!\n");
                    smp.OnClose();
                }
            }
        }
        else if (retval < 0 && errno != EINTR)  //Esli oshibka ne sviazana s prepivanijem signalom
        {
            Loger("Internal error!\n");
            smp.OnClose();
        }

        if (!smp.fOpenConnection)
        {
            Loger("Reconnecting...\n");
            Reinit_connection();
        }

        if (runTimerA)
        {
            runTimerA = false;
            timer_a();
        }
        if (runTimerB)
        {
            runTimerB = false;
            timer_b();
        }
        if (runTimerC)
        {
            runTimerC = false;
            timer_c();
        }
    }
    return EXIT_SUCCESS;
}


void CTuneFlags::SetBit(int num)
{
    base |= (1 << num);
}

void CTuneFlags::ResetBit(int num)
{
    base &= (0xFFFFFFFFUL - (1 << num));
}

bool CTuneFlags::GetBit(int num)
{
    return (base & (1 << num)) ? true : false;
}
