/*
 * clustal version strings, maybe dependent on build state
 * as controlled by compile.h.
 *
 * Created: 17-01-07,Nigel Brown(EMBL).
 *
 * Changes:
 *
 * 31-01-07,Nigel Brown(EMBL): fixed _CLU_VERSION_STRING_ typo.
 * 27-06-07,Nigel Brown(EMBL): public release 2.0.
 */
#ifndef VERSION_H
#define VERSION_H

#include "clustalW/clustalw_version.h"
//AW: not needed anymore #include "compile.h"

#define CLUSTALX_NAME "ClustalX"
/* version number between X and W are same for now */
#define CLUSTALX_VERSION CLUSTALW_VERSION

#endif
//VERSION_H
