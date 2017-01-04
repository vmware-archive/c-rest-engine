/*
 * Copyright © 2012-2015 VMware, Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the ~@~\License~@~]); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ~@~\AS IS~@~] BASIS, without
 * warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "includes.h"

#ifndef WIN32
#include <getopt.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
#endif

uint32_t                         useFile = 0;
uint32_t                         cbIndex = 0;

#define MAX_ARGUMENTS 10
#define MAX_ARG_LEN 20
#define MAX_COMMAND_LEN 100

int     exitLoop = 0;

void printHelp(void)
{
    printf("\nUsage:\n");
    printf("config     [OPTIONS...]              Provide Configuration params to REST engine\n");
    printf("restengine [OPTIONS...]              Initialize/shutdown the REST engine\n");
    printf("endpoint   [OPTIONS...]              Register/Deregister the REST point\n");
    printf("server     [OPTIONS...]              Start/Stop the server\n");
    printf("help                                 See this message\n");
    printf("exit                                 Exit this test client\n\n");

    printf("Report any bug to kaushikk@vmware.com\n\n");
}

void printConfigHelp(void)
{
    printf("\nUsage:\n");
    printf("config [OPTIONS...]                  Provide Configuration params to REST engine\n");
    printf("-f,  --FILE <File path>              Read Configuration from file\n");
    printf("-p,  --port <Port number>            Server port on which server will be listening\n");
    printf("-k,  --sslKeyPath <File path>        SSL key file path for secure communication\n");
    printf("-c,  --sslCertPath <File path>       SSL certificate file path for secure communication\n");
    printf("-C,  --clientCnt <Number>            Maximum count of client supported,defaults to 5\n");
    printf("-W,  --workerThrCnt <Number>         Maximum count of worker threads,defaults to 5\n");
    printf("-e,  --exit                          Exit the test client\n");
    printf("-h,  --help                          See this message\n\n");
}

void printServerHelp(void)
{
    printf("\nUsage:\n");
    printf("server [OPTIONS...]                  Start/stop the REST server\n");
    printf("-s,  --start                         Start the REST server\n");
    printf("-S,  --stop                          Stop the REST server\n");
    printf("-e,  --exit                          Exit the test client\n");
    printf("-h,  --help                          See this message\n\n");
}

void printEndPointHelp(void)
{
    printf("\nUsage:\n");
    printf("endpoint [OPTIONS...]                Register/deregister the REST endpoint\n");
    printf("-r,  --registerURI <End Point URI>   Register EndPoint\n");
    printf("-d,  --deregisterURI <End Point URI> Deregister EndPoint\n");
    printf("-e,  --exit                          Exit the test client\n");
    printf("-h,  --help                          See this message\n\n");
}

void printRestEngineHelp(void)
{
    printf("\nUsage:\n");
    printf("restengine [OPTIONS...]              Init/Shutdown the REST engine\n");
    printf("-i,  --init                          Init the REST engine\n");
    printf("-s,  --shutdown                      Shutdown the REST engine\n");
    printf("-e,  --exit                          Exit the test client\n");
    printf("-h,  --help                          See this message\n\n");
}


uint32_t
VmRegisterHandler(
    char*                            URI,
    uint32_t                         index
    )
{
    uint32_t                         dwError = 0;
    /**** Call helper in global with resource specific pointer ****

    VmAppStoreEndpoint(
        index,
        &VmHandlePackageWrite,
        &VmHandlePackageRead,
        &VmHandlePackageUpdate,
        &VmHandlePackageDelete
        );
    */
     VmAppStoreEndpoint(
        index,
        &VmHandleEchoData,
        &VmHandleEchoData,
        &VmHandleEchoData,
        &VmHandleEchoData
        );
        

 

     dwError = VmRESTRegisterHandler(
                  URI,
                  &(gVmEndPointHandler[index]),
                  NULL
                  );
     BAIL_ON_VMREST_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

#ifndef WIN32


void readOptionConfig(int argc, char *argv[])
{
    int                              c = 0;
    int                              option_index = 0;
  
    if (argc <= 1)
    {
        printf("Wrong usage: No options specified\n");
        return;
    }

    while (1)
    {
        struct option long_options[] =
        {
            {"FILE",             required_argument,       0, 'f'},
            {"port",             required_argument,       0, 'p'},
            {"logfile",          required_argument,       0, 'l'},
            {"sslKeyPath",       required_argument,       0, 'k'},
            {"sslCertPath",      required_argument,       0, 'c'},
            {"clientCnt",        required_argument,       0, 'C'},
            {"workerThrCnt",     required_argument,       0, 'W'},
            {"exit",             no_argument,             0, 'e'},
            {"help",             no_argument,             0, 'h'},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "f:p:l:k:c:C:W:eh", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                {
                    break;
                }
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                {
                    printf (" with arg %s", optarg);
                }
                printf ("\n");
                break;

            case 'f':
                printf ("Reading Config from File %s\n", optarg);
                strncpy(restdConfig.configFile,optarg, (MAX_CONFIG_PARAMS_LEN - 1));
                useFile = 1;
                break;

            case 'p':
                printf ("Server will be listening on port %s\n", optarg);
                strncpy(restdConfig.port,optarg, (MAX_CONFIG_PARAMS_LEN -1));
                break;

            case 'l':
                printf ("Log file location %s\n", optarg);
                strncpy(restdConfig.debugLogFile,optarg, (MAX_CONFIG_PARAMS_LEN -1));                
                break;
            case 'k':
                printf ("SSL Key file %s\n", optarg);
                strncpy(restdConfig.sslKey,optarg, (MAX_CONFIG_PARAMS_LEN - 1));
                break;

            case 'c':
                printf ("SSL Certificate File %s\n", optarg);
                strncpy(restdConfig.sslCert,optarg, (MAX_CONFIG_PARAMS_LEN -1));
                break;

            case 'C':
                printf ("Maximum Client %s\n", optarg);
                strncpy(restdConfig.clientCnt,optarg, (MAX_CONFIG_PARAMS_LEN - 1));
                break;
            case 'W':
                printf ("Maximum worker thread spawned %s\n", optarg);
                strncpy(restdConfig.workerThCnt,optarg, (MAX_CONFIG_PARAMS_LEN - 1));
                break;
            case 'h':
                printConfigHelp();
                break;
            case 'e':
                printf ("Exiting tool ....\n");
                exitLoop = 1;
                break;
            case '?':
                break;

            default:
                abort ();
         }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf ("%s ", argv[optind++]);
        }
        putchar ('\n');
    }
    optind = 1;
}

void readOptionEndPoint(int argc, char *argv[])
{
    int                              c = 0;
    int                              option_index = 0;
    uint32_t                         dwError = 0;

    if (argc <= 1)
    {
        printf("Wrong usage: No options specified\n");
        return;
    }

    while (1)
    {
        struct option long_options[] =
        {
            {"registerURI",       required_argument, 0, 'r'},
            {"deregisterURI",     required_argument, 0, 'd'},
            {"exit",              no_argument,       0, 'e'},
            {"help",              no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "r:d:eh", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                {
                    break;
                }
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                {
                    printf (" with arg %s", optarg);
                }
                printf ("\n");
                break;

            case 'r':
                printf ("Registering REST Endpoint for URI %s\n", optarg);
                dwError = VmRegisterHandler(
                              optarg,
                              cbIndex++
                              );
                if (!dwError)
                {
                    printf("REST endpoint %s registered successfully\n", optarg);
                }
                else
                {
                    printf ("ERROR: Registering REST Endpoint for URI %s failed, Error code %u\n", optarg, dwError);
                    exitLoop = 1;
                }
                
                break;

            case 'd':
                printf ("Deregistering REST Endpoint for URI %s\n", optarg);
                dwError = VmRESTUnRegisterHandler(
                              optarg
                              );
                if (!dwError)
                {
                    printf("REST endpoint %s removed successfully\n", optarg);
                }
                else
                {
                    printf ("ERROR: removing REST Endpoint for URI %s failed, Error code %u\n", optarg, dwError);
                    exitLoop = 1;
                }
                break;

            case 'h':
                printEndPointHelp();
                break;

            case 'e':
                printf ("Exiting tool ....\n");
                exitLoop = 1;
                break;

            case '?':
                break;

            default:
                abort ();
         }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf ("%s ", argv[optind++]);
        }
        putchar ('\n');
    }
    optind = 1;
}


void readOptionRestEngine(int argc, char *argv[])
{
    int                              c = 0;
    int                              option_index = 0;
    uint32_t                         dwError = 0;
    PREST_CONF                       pConfig = NULL;

    if (argc <= 1)
    {
        printf("Wrong usage: No options specified\n");
        return;
    }

    while (1)
    {
        struct option long_options[] =
        {
            {"init",              no_argument,       0, 'i'},
            {"shutdown",          no_argument,       0, 's'},
            {"exit",              no_argument,       0, 'e'},
            {"help",              no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "iseh", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                {
                    break;
                }
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                {
                    printf (" with arg %s", optarg);
                }
                printf ("\n");
                break;

            case 'i':
                printf ("Initializing Rest Engine ....\n");
                if (useFile == 1)
                {
                    pConfig = NULL;
                }
                else
                {
                     pConfig = (PREST_CONF)malloc(sizeof(REST_CONF));

                     pConfig->pSSLCertificate = restdConfig.sslCert;
                     pConfig->pSSLKey = restdConfig.sslKey;
                     pConfig->pServerPort = restdConfig.port;
                     pConfig->pDebugLogFile = restdConfig.debugLogFile;
                     pConfig->pClientCount = restdConfig.clientCnt;
                     pConfig->pMaxWorkerThread = restdConfig.workerThCnt;
                }
                dwError = VmRESTInit(
                              pConfig,
                              restdConfig.configFile
                              );
                if (!dwError)
                {
                    printf ("Initialized Rest Engine Successfully ....\n");
                }
                else
                {
                    printf ("ERROR: Unable to initialize Rest Engine, Error code %u ....\n",dwError);
                    exitLoop = 1;
                }

                if (pConfig)
                {
                    free(pConfig);
                }
                break;

            case 's':
                printf ("Shutdown Rest Engine ....\n");
                VmRESTShutdown();
                printf ("Shutdown Rest Engine Successful ....\n");
                break;

            case 'h':
                printRestEngineHelp();
                break;

            case 'e':
                printf ("Exiting tool ....\n");
                exitLoop = 1;
                break;

            case '?':
                break;

            default:
                abort ();
         }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf ("%s ", argv[optind++]);
        }
        putchar ('\n');
    }
    optind = 1;
}

void readOptionServer(int argc, char *argv[])
{
    int                              c = 0;
    int                              option_index = 0;
    uint32_t                         dwError = 0;

    if (argc <= 1)
    {
        printf("Wrong usage: No options specified\n");
        return;
    }

    while (1)
    {
        struct option long_options[] =
        {
            {"start",             no_argument,       0, 's'},
            {"stop",              no_argument,       0, 'S'},
            {"exit",              no_argument,       0, 'e'},
            {"help",              no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "sShe", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                {
                    break;
                }
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                {
                    printf (" with arg %s", optarg);
                }
                printf ("\n");
                break;

            case 's':
                printf ("Starting Server ....\n");
                dwError = VmRESTStart(
                              );
                if (!dwError)
                {
                    printf ("Started server successfully ....\n");
                }
                else
                {
                    printf ("ERROR: Unable to start server, Error code %u ....\n",dwError);
                    exitLoop = 1;
                }

                break;

            case 'S':
                printf ("Stoping Server ....\n");
                dwError = VmRESTStop(
                              );
                if (!dwError)
                {
                    printf ("Stopped server successfully ....\n");
                }
                else
                {
                    printf ("ERROR: Unable to stop server, Error code %u ....\n",dwError);
                    exitLoop = 1;
                }
                break;

            case 'h':
                printServerHelp();
                break;

            case 'e':
                printf ("Exiting tool ....\n");
                exitLoop = 1;
                break;

            case '?':
                break;

            default:
                abort ();
         }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf ("%s ", argv[optind++]);
        }
        putchar ('\n');
    }
    optind = 1;
}
#endif


void init_command(char* buffer, char** myargv)
{
    for (int i =0; i < MAX_ARGUMENTS; i++)
    {
        myargv[i][0] = '\0';
    }
    memset(buffer,'\0', MAX_COMMAND_LEN);
}

/// Executes single command, returns true even though there is an error, false if command is 'exit'

bool execute_command( char* buffer, char** myargv )
{
    int i = 0;
    char* ptr = strtok(buffer, " ");
    while(ptr != NULL)
    {
        strcpy(myargv[i++],ptr);
        ptr = strtok(NULL, " ");

        if (i == MAX_ARGUMENTS)
        {
            printf("\nERROR: Maximum 10 arguments supported from CLI\n");
            return true;
        }
    }

    char *temp =myargv[i-1];
    int len =strlen(temp);

    *(temp + len -1) = '\0';

    if (myargv[0] != NULL)
    {
        if (strcmp(myargv[0], "server") == 0)
        {
            readOptionServer(i, myargv);
        }
        else if (strcmp(myargv[0], "config") == 0)
        {
            readOptionConfig(i, myargv);
        }
        else if (strcmp(myargv[0], "endpoint") == 0)
        {
            readOptionEndPoint(i, myargv);
        }
        else if (strcmp(myargv[0], "restengine") == 0)
        {
            readOptionRestEngine(i, myargv);
        }
        else if ((strcmp(myargv[0], "--help") == 0) || (strcmp(myargv[0], "help") == 0))
        {
            printHelp();
        }
        else if (strcmp(myargv[0], "exit") == 0)
        {
            printf("Exiting CLI .....\n");
            return false;
        }
        else
        {
            len = 0;
            if (strlen(myargv[0]) > 0)
            {
                printf("\n%s not a valid command.\nType \'--help\' to see list of available commands and options\n\n", myargv[0]);
            }
        }

    }
    
    init_command(buffer, myargv);

    return true;
}


bool string_ends_with(const char* str, const char* suffix)
{
  int str_len = strlen(str);
  int suffix_len = strlen(suffix);

  return (str_len >= suffix_len) && (0 == strcmp(str + (str_len-suffix_len), suffix));
}


/// Runs all commands in file, returns true, unless the 'exit' command is encountered, in which case returns false right away

bool run_command_file( char* filepath, char* buffer, char** myargv )
{
    bool exitval = true;
    FILE* fr = fopen(filepath, "rt");
    if (fr != NULL)
    {
        printf("@ Running command file: %s\n", filepath );
        while(exitval == true && fgets(buffer, MAX_COMMAND_LEN, fr) != NULL)
        {
            printf("> %s", buffer );
            if ( !execute_command(buffer, myargv) )
            {
                exitval = false;
                break;
            }
        }
        fclose(fr);
        printf("@ Done running command file: %s\n", filepath );
    }
    else
        printf("@ Cannot open command file %s\n", filepath);
    return exitval;
}


#if 1
#ifdef WIN32
#include "..\transport\win\includes.h"
#endif
int main(int argc, char *argv[])
{
    uint32_t                         dwError = 0;

    printf("Bingo: %u\n", dwError);

#ifdef WIN32 
    dwError = VmRESTInit(NULL,"c:\\tmp\\restconfig.txt");
#else
    dwError = VmRESTInit(NULL,"/tmp/restconfig.txt");
#endif
    VmRegisterHandler("/v1/pkg", 0);

    VmRESTStart();

#ifdef WIN32 
       Sleep(20000);
#else
       while(1)
       {
           usleep(10);
       }
#endif
    
    dwError = VmRESTStop();

    dwError = VmRESTUnRegisterHandler("/v1/pkg");

    VmRESTShutdown();

    
return dwError;

}

#endif
#if 0
int
main (int argc, char *argv[])
{
  int    retval = 0;
  int    i;
  char   buffer[MAX_COMMAND_LEN+1];
  char** myargv = malloc(MAX_ARGUMENTS * sizeof(char *));
  for (i =0; i < MAX_ARGUMENTS; i++)
  {
      myargv[i] = (char*)malloc(MAX_ARG_LEN);
  }

  init_command(buffer, (char**)myargv);

  if( argc >=4 )
      if( !run_command_file(argv[3], buffer, myargv) )
            goto clean;

#ifndef WIN32
  if( argc >= 2)
  {
    int    length;
    int    fd;
    int    wd;
    char   eventbuffer[EVENT_BUF_LEN];
    char   filename[1024];
    time_t lastmodified;
    struct stat attr;
    char   lastevent_name[80] = { 0 };
    time_t lastevent_time;
    time ( &lastevent_time );

    /*creating the INOTIFY instance*/
    fd = inotify_init();

    if ( fd < 0 )
    {
        printf( "@ Error in inotify_init\n" );
        retval = 1;
        goto clean;
    }
    wd = inotify_add_watch( fd, argv[1], IN_MODIFY );



    if( argc >= 3 )
        printf("@ Monitoring %s directory for command files with %s extension\n", argv[1], argv[2]);
    else
        printf("@ Monitoring %s directory for command files\n", argv[1]);

    while( exitLoop == 0  )
    {
        length = read( fd, eventbuffer, EVENT_BUF_LEN );

        if ( length < 0 )
        {
            printf( "@ Error in reading file/directory event\n" );
            break;
        }
        i = 0;
        while( i < length )
        {
            struct inotify_event* event = (struct inotify_event*) &eventbuffer[ i ];
            if ( event->len > 0 && (event->mask & IN_MODIFY) && !(event->mask & IN_ISDIR) )
            {
                if( argc == 2 || string_ends_with(event->name, argv[2]) )
                {
                    strcpy(filename, argv[1]);
                    strcat(filename, "/");
                    strcat(filename, event->name);
                    stat(filename, &attr);
                    lastmodified = attr.st_mtime;
                    if( lastevent_time != lastmodified || strcmp(lastevent_name, event->name) != 0)
                    {
                        lastevent_time = lastmodified;
                        strcpy(lastevent_name, event->name);
                        if( !run_command_file( filename, buffer, myargv ) )
                        {
                            exitLoop = 1;
                            break;
                        }
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }
    inotify_rm_watch( fd, wd );
   /*closing the INOTIFY instance*/
    close( fd );
    goto clean;
  }
#endif

  while( exitLoop == 0 )
  {
      printf("VMREST_TEST_CLI > ");
      fgets(buffer, MAX_COMMAND_LEN, stdin);
      if ( !execute_command(buffer, myargv) )
          break;
  }

clean:
  for (i =0; i < MAX_ARGUMENTS; i++)
  {
      free(myargv[i]);
  } 
  free(myargv);

  return retval;
}

#endif


