#ifndef __CSCHEDULE_H
#define __CSCHEDULE_H

/**
\addtogroup AppCoAP
\{
\addtogroup cschedule
\{
*/
#include "opencoap.h"
//=========================== define ==========================================

//=========================== typedef =========================================

typedef struct {
   coap_resource_desc_t desc;
} cschedule_vars_t;

//=========================== variables =======================================

//=========================== prototypes ======================================

void cschedule__init(void);

/**
\}
\}
*/


#endif
