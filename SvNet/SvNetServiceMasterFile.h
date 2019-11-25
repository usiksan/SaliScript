#ifndef SVNETSERVICEMASTERFILE_H
#define SVNETSERVICEMASTERFILE_H

#include "SvNetService.h"

class SvNetServiceMasterFile : public SvNetService
  {
    Q_OBJECT

  public:
    SvNetServiceMasterFile();

    //Return current master file service. If needed it build service
    static SvNetServiceMasterFile *get();
  };

#endif // SVNETSERVICEMASTERFILE_H
