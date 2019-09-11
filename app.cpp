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

  m_logFile = NULL;

  m_args = new StrCmdArgMap();

  strcpy(m_name, name);

  strcpy(m_wdir, FS::curDir());
  strcpy(m_inDir, m_wdir);
  strcpy(m_outDir, m_wdir);

  sm_app = this;
}

App::~App()
{
  sm_app = NULL;

  delete m_args;
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
          wdir(), FS::pathSep(), name(), FS::pathSep(), mpiRank());

  m_logFile = fopen(path, "wb");
  if (m_logFile == NULL)
    throw Exception("Unable to open the log file for writing"); 
}

void App::log(const char *txt, ...) const
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
  log("Our rank is %d of %d processes", mpiRank(), mpiSize());

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

inline bool App::hasArg(const char *name) const
{
  return m_args->find(name) != m_args->end();
}
inline std::string App::argValue(const char *name) const
{
  return (*m_args)[name].m_value;
}
inline double App::argNum(const char *name) const
{
  return (*m_args)[name].m_num;
}

void App::parseArgs(int argc, char *argv[])
{
  m_args->clear();

  for (int i = 0; i < argc; i++)
  {
    CmdArg arg;
    arg.parse(argv[i]);
    if (arg.m_name.length() > 0)
      (*m_args)[arg.m_name] = arg;
  }

  if (hasArg("wdir"))
    setWDir(argValue("wdir").c_str());
  else
    throw Exception("Working directory must be specified with command argument -wdir=<path>");

  if (hasArg("indir"))
    setInDir(argValue("indir").c_str());
  else
    setInDir(wdir());

  if (hasArg("outdir"))
    setOutDir(argValue("outdir").c_str());
  else
    setOutDir(wdir());

  if (!FS::fsEntryExist(wdir()) ||
      !FS::fsEntryExist(outDir()) ||
      !FS::fsEntryExist(inDir()))
    throw Exception("Incorrect working, output, or input directory specified");
}



const char *IDataPool::indexLogStr(int ix)
{
  static char ret[100];
  sprintf(ret, "Index %d", ix);
  return ret;
}


#ifdef HAVE_MPI

class MPISendMgr : public IMPISendMgr
{
  int m_dst;
public:
  MPISendMgr(int dst) : m_dst(dst)
  {
  }
  virtual int SendResultsMsgData(int msgData)
  {
    int msg = MSG(WMSG_RESULTS, msgData);
    return MPI_Ssend(&msg, 1, MPI_INT, m_dst, 0, MPI_COMM_WORLD);
  }
  virtual int Send(const void *buf, int count, void *pDatatype)
  {
    MPI_Datatype* pdt = (MPI_Datatype*)pDatatype;
    return MPI_Ssend(buf, count, *pdt, m_dst, 0, MPI_COMM_WORLD);
  }
};

class MPIRecvMgr : public IMPIRecvMgr
{
  int m_src;
public:
  MPIRecvMgr(int src, int resultsMsgData) : m_src(src)
  {
    m_resultsMsgData = resultsMsgData;
  }
  virtual int Recv(void *buf, int count, void *pDatatype)
  {
    MPI_Status s;
    MPI_Datatype* pdt = (MPI_Datatype*)pDatatype;
    return MPI_Recv(buf, count, *pdt, m_src, 0, MPI_COMM_WORLD, &s);
  }
};

class MPIDbgSendRecvMgr : public IMPISendMgr, public IMPIRecvMgr
{
  struct Buffer
  {
    char *data;
    int size;
  };
  std::queue<Buffer> m_buffers;

public:
  inline bool hasData() const
  {
    return m_buffers.size() > 0;
  }
  virtual int SendResultsMsgData(int msgData)
  {
    m_resultsMsgData = msgData;
    return MPI_SUCCESS;
  }
  virtual int Send(const void *buf, int count, void* pDatatype)
  {
    int sz;
    MPI_Datatype* pdt = (MPI_Datatype*)pDatatype;
    MPI_Type_size(*pdt, &sz);
    sz *= count;

    Buffer b = { new char[sz], sz };
    m_buffers.push(b);
    memcpy(b.data, buf, sz);
    return MPI_SUCCESS;
  }
  virtual int Recv(void *buf, int count, void* pDatatype)
  {
    int sz;
    MPI_Datatype* pdt = (MPI_Datatype*)pDatatype;
    MPI_Type_size(*pdt, &sz);
    sz *= count;

    Buffer b = m_buffers.front();
    if (b.size != sz)
      throw Exception("MPI read calls should be for the same amount of bytes "
                      "as previous MPI write calls in DEBUG emulation runs");
    memcpy(buf, b.data, sz);
    delete[] b.data;
    m_buffers.pop();
    return MPI_SUCCESS;
  }
};

#endif


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
  MPI_Comm_size(MPI_COMM_WORLD, &m_mpiSize);

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

    int tmp = 1;
    for (int i = 1; i < m_mpiSize; i++)
      MPI_Ssend(&tmp, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
  }

  initLog(); // call after MPI starting routines to have a rank

  unsigned int seed = ((unsigned int) time(NULL)) * (m_mpiRank + 1);
  srand(seed);
  log("Random seeded with %X", seed);
  log("Our rank is %d of %d processes", m_mpiRank, m_mpiSize);

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
    log("Aborting MPI from process %d", m_mpiRank);
    MPI_Abort(MPI_COMM_WORLD, -1);
    // the code below should not execute as MPI_Abort does not return
    log("Failed to abort MPI from process %d", m_mpiRank);
    return -1;
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
  MPI_Status s;

  bool ready = m_dp->checkFS();
  log("Main ready state is %s", ready ? "true" : "false");

  int *tasks = new int[m_mpiSize];
  auto_del<int> del_tasks(tasks, true);
  for (int i = 0; i < m_mpiSize; i++)
    tasks[i] = -1;

  std::list<int> tasksSyncing;
  m_dp->resetIndex();

  if (m_mpiSize > 1)
  { // real MPI scenario
#ifdef _DEBUG
    MessageBoxA(0, "Master MPI", "Master MPI", MB_OK);
#endif

    int procsFinished = 0;
    while (procsFinished < m_mpiSize - 1)
    {
      int msg;
      MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
               MPI_COMM_WORLD, &s);

      int src = s.MPI_SOURCE;
      if (src > 0 && src < m_mpiSize)
      {
        int cmd = MSG_CMD(msg);

        if (cmd == WMSG_RESULTS)
        {
          int ix = tasks[src];
          MPIRecvMgr mgr(src, MSG_DATA(msg));
          receiveResults(mgr, ix);
          log("Received results for %s from process %d",
              m_dp->indexLogStr(ix), src);
        }
        if (cmd == WMSG_READY)
        {
          if (tasks[src] < 0)
            log("Process %d is ready, no previous tasks", src);
          else
            log("Process %d is ready, previous task was %s",
                src, m_dp->indexLogStr(tasks[src]));

          tasks[src] = -1;

          tasksSyncing.push_back(src);

          while (!iterationSync() && tasksSyncing.size() > 0)
          {
            int task = tasksSyncing.front();
            tasksSyncing.pop_front();
            int ix = m_dp->nextIndex();
            if (ix < 0)
            {
              int msg = MSG(MSG_QUIT, 0);
              log("Quitting process %d", task);
              MPI_Ssend(&msg, 1, MPI_INT, task, 0, MPI_COMM_WORLD);
              procsFinished++;
            }
            else
            {
              if (!m_dp->createDirs(ix))
                throw Exception("Unable to create an output directory");

              tasks[task] = ix;
              int msg = MSG(MSG_RUN, ix);
              log("Running %s on process %d", m_dp->indexLogStr(ix), task);
              MPI_Ssend(&msg, 1, MPI_INT, task, 0, MPI_COMM_WORLD);
            }
          }
        }
      }
      else
        break;
    }
  }
  else
  { // emulate MPI in a single process/thread for debugging
    while (true)
    {
      // let the master process aggregate results
      // as we only have one worker process this should never return true
      if (iterationSync())
        throw Exception("Unexpected operation");

      int ix = m_dp->nextIndex();
      if (ix < 0)
        break;

      if (!m_dp->createDirs(ix))
        throw Exception("Unable to create an output directory");

      MPIDbgSendRecvMgr sendRecvMgr;
      calcIndex(ix);
      sendResults(sendRecvMgr, ix);
      if (sendRecvMgr.hasData())
        receiveResults(sendRecvMgr, ix);
    }
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
  MPISendMgr sendRecvMgr(0);
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
    sendResults(sendRecvMgr, ix);
  }

  return 0;
#else
  throw Exception("Rebuild the Utils library with HAVE_MPI macro "
                  "defined to use MPIApp");
#endif
}
