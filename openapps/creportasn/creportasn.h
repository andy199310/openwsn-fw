#ifndef __CREPORTASN_H
#define __CREPORTASN_H

/**
\addtogroup AppUdp
\{
\addtogroup creportasn
\{
*/
#include "opencoap.h"
//=========================== define ==========================================

//=========================== typedef =========================================

typedef struct {
   coap_resource_desc_t desc;
   opentimers_id_t      timerId;
   bool                 isEmergency;
   uint16_t             creportasn_sequence;
   uint16_t             lastSuccessLeft;
   uint16_t             errorCounter;
   uint16_t             lastCallbackSequence;

} creportasn_vars_t;

//=========================== variables =======================================

//=========================== prototypes ======================================

void creportasn_init(void);


/**
\}
\}
*/

#endif
