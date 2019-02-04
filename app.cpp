/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#ifdef HAVE_MPI
#  include <mpi.h>
#endif
#include "Utils/utils.h"

using namespace std;


App *App::sm_app = NULL;

App::App(const char *name)
{
  if (sm_app != NULL)
    throw Exception("Multiple instances of the App class");
  sm_app = this;

  strcpy(m_name, name);

  strcpy(m_wdir, FS::curDir());
  strcpy(m_inDir, m_wdir);
  strcpy(m_outDir, m_wdir);
}

App::~App()
{
  sm_app = NULL;
}

void App::setWDir(const char *v)
{
  strcpy(m_wdir, v);
}

void App::setInDir(const char *v)
{
  strcpy(m_inDir, v);
}

void App::setOutDir(const char *v)
{
  strcpy(m_outDir, v);
}


void App::initLog()
{
  char path[MAXPATHLENGTH];
  
  sprintf(path, "%s%clog_%s%c%d.log",
          wdir(), FS::pathSep(), name(), FS::pathSep(), rank());

  m_logFile = fopen(path, "wb");
  if (m_logFile == NULL)
    throw Exception("Unable to open the log file for writing"); 
}

void App::log(const char *txt, ...)
{
  if (m_logFile != NULL)
  {
    static char buffer[1024];

    va_list aptr;
    va_start(aptr, txt);
    vsprintf(buffer, txt, aptr);
    va_end(aptr);

    fprintf(m_logFile, "%s" ENDL, buffer);
    fflush(m_logFile);
  }
}

void App::deinitLog()
{
  if (m_logFile != NULL)
  {
    fclose(m_logFile);
    m_logFile = NULL;
  }
}


int App::run(int argc, char *argv[])
{
  int retCode = 0;
  
  bool ready = false;

  char ldir[MAXPATHLENGTH];
  sprintf(ldir, "log_%s", name());
  FS::makeDir(wdir(), ldir);
  
  initLog();

  unsigned int seed = (unsigned int) time(NULL);
  srand(seed);
  log("Random seeded with %X", seed);
  log("Our rank is %d", rank());

  try
  {
    retCode = main();
  }
  catch (const Exception &e)
  {
    log("Unhandled exception: %s", e.message());
    retCode = -1;
  }

  deinitLog();

  return retCode;
}




MPIApp *MPIApp::sm_mpiApp = NULL;

MPIApp::MPIApp(const char *name, IDataPool *dp) :
  App(name)
{
#ifdef HAVE_MPI
  if (sm_mpiApp != NULL)
    throw Exception("Multiple instances of the MPIApp class");
  sm_mpiApp = this;

  m_dp = dp;
  m_mpiRank = 0;
#else
  throw Exception("Rebuild the Utils library with HAVE_MPI macro "
                  "defined to use MPIApp");
#endif
}

int MPIApp::run(int argc, char *argv[])
{
#ifdef HAVE_MPI
  int retCode = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &m_mpiRank);

  initMPITypes();

  if (m_mpiRank != 0)
  {
    int tmp = 0;
    MPI_Status s;
    MPI_Recv(&tmp, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
  }
  else
  {
    char ldir[MAXPATHLENGTH];
    sprintf(ldir, "log_%s", name());
    FS::makeDir(wdir(), ldir);

    int mpiSize, tmp = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
    for (int i = 1; i < mpiSize; i++)
      MPI_Ssend(&tmp, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
  }
  
  initLog(); // call after MPI starting routines to have a rank

  unsigned int seed = ((unsigned int) time(NULL)) * (m_mpiRank + 1);
  srand(seed);
  log("Random seeded with %X", seed);
  log("Our rank is %d", m_mpiRank);

  try
  {
    if (m_mpiRank == 0)
      retCode = main();
    else
      retCode = mainMPI();
  }
  catch (const Exception &e)
  {
    log("Unhandled exception: %s", e.message());
    retCode = -1;
  }

  deinitMPITypes();

  MPI_Finalize();

  deinitLog();

  return retCode;
#else
  throw Exception("Rebuild the Utils library with HAVE_MPI macro "
                  "defined to use MPIApp");
#endif
}

int MPIApp::main()
{
#ifdef HAVE_MPI
  int mpiSize;
  MPI_Status s;
  MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

  bool ready = m_dp->checkFS();
  log("Main ready state is %s", ready ? "true" : "false");

  int *tasks = new int [mpiSize];
  auto_del<int> del_tasks(tasks, true);
  for (int i = 0; i < mpiSize; i++)
    tasks[i] = -1;

  if (ready)
  {
    int msg = MSG(MSG_QUIT, 0);
    for (int i = 1; i < mpiSize; i++)
    {
      MPI_Recv(&msg, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
      MPI_Ssend(&msg, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
  }
  else
  {
    m_dp->resetIndex();

    int procsFinished = 0;
    while (procsFinished < mpiSize - 1)
    {
      int msg;
      MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &s);
    
      int src = s.MPI_SOURCE;
      if (src > 0 && src < mpiSize)
      {
        int cmd = MSG_CMD(msg);

        if (cmd == WMSG_RESULTS)
        {
          int ix = tasks[src];
          receiveResults(ix, src, MSG_DATA(msg));
          tasks[src] = -1;
        }
        if (cmd == WMSG_READY)
        {
          log("Process %d is ready", src);

          int ix = m_dp->nextIndex();
          if (ix < 0 || !m_dp->createDirs(ix))
          {
            int msg = MSG(MSG_QUIT, 0);
            MPI_Ssend(&msg, 1, MPI_INT, src, 0, MPI_COMM_WORLD);
            procsFinished ++;
          }
          else
          {
            tasks[src] = ix;
            int msg = MSG(MSG_RUN, ix);
            MPI_Ssend(&msg, 1, MPI_INT, src, 0, MPI_COMM_WORLD);
          }
        }
      }
      else
        break;
    }

    ready = mpiSize > 1;
  }

  log("Returning from runMainMPI");

  return 0;
#else
  throw Exception("Rebuild the Utils library with HAVE_MPI macro "
                  "defined to use MPIApp");
#endif
}

int MPIApp::mainMPI()
{
#ifdef HAVE_MPI
  while (true)
  {
    MPI_Status s;
    int msg = MSG(WMSG_READY, 0);
    MPI_Ssend(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Recv(&msg, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &s);

    int cmd = MSG_CMD(msg);
    if (cmd != MSG_RUN)
      break;
    int ix = MSG_DATA(msg);
    calcIndex(ix);
    sendResults(ix);
  }

  return 0;
#else
  throw Exception("Rebuild the Utils library with HAVE_MPI macro "
                  "defined to use MPIApp");
#endif
}
