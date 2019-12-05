#include "SvNetServiceFileMaster.h"

#if 0
SvNetServiceFileMaster::SvNetServiceFileMaster()
  {

  }




SvNetServiceFileMaster *SvNetServiceFileMaster::get()
  {
  static SvNetServiceFileMaster *master;
  if( master == nullptr ) {
    //build new master
    master = new SvNetServiceFileMaster();
    }
  return master;
  }
#endif
