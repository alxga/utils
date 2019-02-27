/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "fsutils.h"

#ifndef APP_HEADER_FILE_INCLUDED
#define APP_HEADER_FILE_INCLUDED

class LIBUTILS_API App
{
  static App *sm_app;

  char m_name[MAXPATHLENGTH];

  char m_wdir[MAXPATHLENGTH];
  char m_inDir[MAXPATHLENGTH];
  char m_outDir[MAXPATHLENGTH];

public:
  static inline App *app() { return sm_app; }

  void log(const char *txt, ...);

  inline const char *wdir() { return m_wdir; }
  void setWDir(const char *v);
  inline const char *inDir() { return m_inDir; }
  void setInDir(const char *v);
  inline const char *outDir() { return m_outDir; }
  void setOutDir(const char *v);

  virtual int rank() { return 0; }
  const char *name() { return m_name; }

  App(const char *name);
  virtual ~App();

  virtual int run(int argc, char *argv[]);

protected:
  FILE *m_logFile;
  void initLog();
  void deinitLog();

  virtual int main() = 0;
};


class IDataPool
{
public:
  virtual bool checkFS() = 0;
  virtual void resetIndex() = 0;
  virtual int nextIndex() = 0;
  virtual bool createDirs(int ix) = 0;
};

class LIBUTILS_API MPIApp : public App
{
  static MPIApp *sm_mpiApp;

  IDataPool *m_dp;
  int m_mpiRank;

protected:
  virtual int main();
  virtual int mainMPI();

  virtual void initMPITypes() {}
  virtual void deinitMPITypes() {}
  virtual void receiveResults(int ix, int src, int msgData) {}
  virtual void sendResults(int ix) {}
  virtual void calcIndex(int ix) = 0;

public:
  static inline MPIApp *mpiApp() { return sm_mpiApp; }

  MPIApp(const char *name, IDataPool *dp);

  virtual int rank() { return m_mpiRank; }

  virtual int run(int argc, char *argv[]);
};

#endif // APP_HEADER_FILE_INCLUDED
