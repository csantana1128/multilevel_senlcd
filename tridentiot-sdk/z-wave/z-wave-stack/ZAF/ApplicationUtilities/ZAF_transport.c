// SPDX-FileCopyrightText: Silicon Laboratories Inc. <https://www.silabs.com/>
//
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @file ZAF_transport.c
 * @brief Various transport functionality in the Z-Wave Framework.
 * @copyright 2021 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <ZAF_Common_interface.h>

//#define DEBUGPRINT
#include "DebugPrint.h"

bool
ZAF_transportSendDataAbort(void)
{
  SZwaveCommandPackage CommandPackage = {
    .eCommandType = EZWAVECOMMANDTYPE_SEND_DATA_ABORT
  };

  DPRINT("\nSendDataAbort\n");

  EQueueNotifyingStatus Status = QueueNotifyingSendToBack(ZAF_getAppHandle()->pZwCommandQueue, (uint8_t*)&CommandPackage, 500);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == Status);
  return (EQUEUENOTIFYING_STATUS_SUCCESS == Status);
}
