#include "SvNetServiceFileMaster.h"

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
