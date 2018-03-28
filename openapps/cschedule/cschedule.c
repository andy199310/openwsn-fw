/**
\brief An example CoAP application.
*/

#include "opendefs.h"
#include "cschedule.h"
#include "opencoap.h"
#include "opentimers.h"
#include "openqueue.h"
#include "packetfunctions.h"
#include "openserial.h"
#include "openrandom.h"
#include "scheduler.h"
//#include "ADC_Channel.h"
#include "idmanager.h"
#include "IEEE802154E.h"

//=========================== defines =========================================

const uint8_t cschedule_path0[] = "s";

//=========================== variables =======================================

cschedule_vars_t cschedule_vars;

//=========================== prototypes ======================================

owerror_t cschedule_receive(OpenQueueEntry_t* msg,
                    coap_header_iht*  coap_header,
                    coap_option_iht*  coap_options);
void    cschedule_sendDone(OpenQueueEntry_t* msg,
                       owerror_t error);


//=========================== public ==========================================

void cschedule__init() {
   
   // prepare the resource descriptor for the /s path
   cschedule_vars.desc.path0len             = sizeof(cschedule_path0)-1;
   cschedule_vars.desc.path0val             = (uint8_t*)(&cschedule_path0);
   cschedule_vars.desc.path1len             = 0;
   cschedule_vars.desc.path1val             = NULL;
   cschedule_vars.desc.componentID          = COMPONENT_CSCHEDULE;
   cschedule_vars.desc.discoverable         = TRUE;
   cschedule_vars.desc.callbackRx           = &cschedule_receive;
   cschedule_vars.desc.callbackSendDone     = &cschedule_sendDone;
   
   
   opencoap_register(&cschedule_vars.desc);
}

//=========================== private =========================================

owerror_t cschedule_receive(OpenQueueEntry_t* msg,
                      coap_header_iht* coap_header,
                      coap_option_iht* coap_options) {
   owerror_t outcome;
   
   switch (coap_header->Code) {
      case COAP_CODE_REQ_POST:
         ;
         // Get common header
         uint8_t commonLength = msg->payload[0] >> 4;
         uint8_t entryCount = msg->payload[1];

         uint8_t entryLength = 2 + (16 - commonLength);
         uint8_t currentPointer = 2;

         uint8_t scheduleType;
         uint8_t scheduleChannelOffset;
         uint8_t scheduleSlotOffset;
         uint8_t cellType;
         open_addr_t temp_neighbor;

         // copy self address
         memset(&temp_neighbor, 0, sizeof(temp_neighbor));
         temp_neighbor.type = ADDR_64B;
         memcpy(&(temp_neighbor.addr_64b), idmanager_getMyID(ADDR_64B)->addr_64b, 8);
         
         // iterate all the entry
         for (int i=0; i<entryCount; i++){
            if (i > 0){
               currentPointer += entryLength;
            }

            scheduleType = msg->payload[currentPointer] >> 4;
            scheduleChannelOffset = msg->payload[currentPointer] & 0x0F;
            scheduleSlotOffset = msg->payload[currentPointer + 1];
            memcpy(&(temp_neighbor.addr_64b[8 - (16 - commonLength)]), &(msg->payload[currentPointer + 2]), 16 - commonLength);


            if (scheduleType >= 0b1000) {
               // remove cell
               schedule_removeActiveSlot(scheduleSlotOffset, &temp_neighbor);
               continue;
            }

            switch (scheduleType) {
               case 0b0000:
                  cellType = CELLTYPE_TX;
                  break;
               case 0b0001:
                  cellType = CELLTYPE_RX;
                  break;
               case 0b0010:
                  cellType = CELLTYPE_TXRX;
                  break;
               default:
                  continue;
            }

            schedule_addActiveSlot(
               scheduleSlotOffset,        // slot offset
               cellType,                  // type of slot
               FALSE,                     // shared?
               scheduleChannelOffset,     // channel offset
               &temp_neighbor             // neighbor
            );

         }

         
         msg->payload                     = &(msg->packet[127]);
         msg->length                      = 0;
         coap_header->Code                = COAP_CODE_RESP_CONTENT;
         
         outcome                          = E_SUCCESS;

         break;
      default:
         // return an error message
         outcome = E_FAIL;
   }
   
   return outcome;
}

void cschedule_sendDone(OpenQueueEntry_t* msg, owerror_t error) {
   openqueue_freePacketBuffer(msg);
}
