#include <include/tune.h>

CParser::CParser()
{
    sScommIp = NULL;
//    sOutDir = NULL;
//    sFileNameBase = NULL;
    sPassword = NULL;
    sLogFile = NULL;
    ScommPort = 10001;
    ServerPort = 0;
    sRootDir = NULL;
//    rotation = 1;
    nTimeUpdateMin = 0;
    fDaemon = false;
}

CParser::~CParser()
{
    if(sScommIp) free(sScommIp);
//    if(sOutDir) free(sOutDir);
//    if(sFileNameBase) free(sFileNameBase);
    if(sPassword) free(sPassword);
    if(sLogFile) free(sLogFile);
    if(sRootDir) free(sRootDir);

}

int CParser::CheckDec(const char *str)
{
    int i;
    for (i = 0; i < (int)strlen(str); i++)
        if (str[i] > '9' || str[i] < '0')
            return 0;
    return 1;
}

int CParser::CheckIp(const char * ipstr)
{

    if (!strcmp(ipstr,"localhost"))
    {
        return 1;
    }

    int len = strlen(ipstr);

    if (len>6 && len<16)
    {
        char p=0,dot=0,s[len+1];
	int i = 0;
        memset(s,0,len+1);
        while(i<=len)
        {
            if(i==len)
            {
                if(dot!=3) return 0;
                if((atoi(s)<0) || (atoi(s)>255)) return 0;
                break;
            }
            if (ipstr[i] == '.')
            {
                dot++;
                if((atoi(s)<0) || (atoi(s)>255) || p == 0) return 0;
                memset(s,0,p);
                i++;
                p=0;
            }
            if (ipstr[i]> '9' || ipstr[i]< '0')return 0;
            s[(int)p++]=ipstr[(int)i++];
        }
    }
    else return 0;
    return 1;
}


int CParser::ParseCStringParams (int argc, char *argv[])
{
    if (argc == 2 &&  !strcmp(argv[1],"-h"))
    {
        PrintHelp();
        return -1;
    }
    int i;
    for( i = 1; i<argc; i++)
    {
        if(!strcmp(argv[i],"-scommip"))
        {
            i++;
            if (i < argc)
            {
                if(CheckIp(argv[i]))
                {
                    sScommIp = (char*)malloc(strlen(argv[i])+1);
                    if (!sScommIp)
                    {
                        printf("Out of memory!\n");
                        return -1;
                    }
                    strcpy(sScommIp, argv[i]);
                }
                else
                {
                    printf("Scomm ip address error!\n");
                    PrintHelp();
                    return -1;
                }
            }
        }
        else if(!strcmp(argv[i],"-password"))
        {
            i++;
            if (i < argc)
            {
                sPassword = (char*)malloc(strlen(argv[i])+1);
                if (!sPassword)
                {
                        printf("Out of memory!\n");
                        return -1;
                }
                strcpy(sPassword, argv[i]);
            }
        }
        else if(!strcmp(argv[i],"-rootdir"))
        {
            i++;
            if (i < argc)
            {
                sRootDir = (char*)malloc(strlen(argv[i])+1);
                if (!sRootDir)
                {
                        printf("Out of memory!\n");
                        return -1;
                }
                strcpy(sRootDir, argv[i]);
            }
        }


        else if(!strcmp(argv[i],"-scommport"))
        {
            i++;
            if (i < argc)
            {
                if (CheckDec(argv[i]))
                {
                    ScommPort = atoi(argv[i]);
                }
            }
        }
        else if(!strcmp(argv[i],"-logfile"))
        {
            i++;
            if (i < argc)
            {
                sLogFile = (char*)malloc(strlen(argv[i])+1);
                if (!sLogFile)
                {
                        printf("Out of memory!\n");
                        return -1;
                }
                strcpy(sLogFile, argv[i]);
            }
        }

        else if(!strcmp(argv[i],"-serverport"))
        {
            i++;
            if (i < argc)
            {
                if (CheckDec(argv[i]))
                {
                    ServerPort = atoi(argv[i]);
                }
            }
        }
        else if(!strcmp(argv[i],"-d"))
        {
            fDaemon = true;
        }
    }
    return 1;
}

int CParser::FillMainParams (void)
{
    if (sScommIp)
    {
        printf("Scomm ip address:%s\n", sScommIp);
    }
    else
    {
        printf("No scomm ip address!\n");
        PrintHelp();
        return -1;
    }

    if (!sPassword)
    {
        sPassword = (char*)malloc(strlen("default")+1);
        if (!sPassword)
        {
            printf("Out of memory!\n");
            return -1;
        }
        strcpy(sPassword, "default");
    }
    printf("Password:%s\n",sPassword);

    if(!ScommPort) ScommPort = 10001;
    printf("Scomm port:%d\n",ScommPort);

    if(!ServerPort) ServerPort = ScommPort + 1;
    printf("Server port:%d\n", ServerPort);

    if (fDaemon) printf("Daemon mode!\n");
    return 1;
}

void CParser::PrintHelp()
{
printf("\nNAME\n\
\ttune - subscriber manager ATS M-200\n\
\nSYNOPSIS\n\
\ttune -scommip X.X.X.X [-scommport N] [-serverport P]\n\
\t[-password pppppp] [-d]\n\n\
\tX.X.X.X - scomm IP address. X = 0..255\n\
\tN - scomm TCP port. N = 0..65535 (default N = 10001)\n\
\tP - tune server TCP port. P = 0..65535, P != N (default P = N+1)\n\
\tpppppp - password for ATS binary mode. p = 0..9 (default pppppp = 100100)\n\
\t-d - daemon mode.\n\
\nEXAMPLES\n\
\tUsing in daemon mode:\n\
\t[MTA@Vasia /home]./tune -scommip localhost -d\n");
}
