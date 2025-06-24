/// ****************************************************************************
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2023 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

#include <Assert.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <timers.h>
#include <zpal_radio.h>
#include <zpal_power_manager.h>
#include <zpal_power_manager_private.h>
#include <lpm.h>
#include <comm_subsystem_drv.h>
//#define DEBUGPRINT // NOSONAR
#include <DebugPrint.h>
#include <cmsis_gcc.h>
#include "chip_define.h"
#ifdef TR_PLATFORM_T32CZ20
#include "sysctrl.h"
#endif
#define ASSERT_ZPAL_PM_TYPE_T(t) assert(t == ZPAL_PM_TYPE_USE_RADIO || t == ZPAL_PM_TYPE_DEEP_SLEEP)


static uint8_t active_locks[2];
static zpal_pm_mode_t current_mode = ZPAL_PM_MODE_RUNNING;
static zpal_pm_mode_t allowed_mode = ZPAL_PM_MODE_SHUTOFF;

const uint32_t zpal_magic_number = 0x5B9E684D;

typedef struct
{
  uint32_t magic_number;  /*< The magic_number field is used to check the validity of the handle pointer passed to the power_manager APIs,
                               Since the handle type used for the power_manager API is a pointer for void.
                               handle pointer is typecasted to pointer for zpal_pm_power_lock_t inside the power_manager APIs
                               When the zpal_pm_register is called, a unique value is assigned to the magic_number field.*/
  zpal_pm_type_t type;
  bool active;
  bool forever;
  TimerHandle_t timer;
  StaticTimer_t timer_buffer;
} zpal_pm_power_lock_t;


static void timer_callback( TimerHandle_t xTimer )
{
  zpal_pm_handle_t handle = (zpal_pm_handle_t) pvTimerGetTimerID(xTimer);
  zpal_pm_cancel(handle);
}

void zpal_pm_enter_sleep(TickType_t sleep_ticks)
{
  if ((allowed_mode > current_mode) && (0 < sleep_ticks))
  {
    zpal_zw_pm_event_handler(current_mode, allowed_mode);
    current_mode = allowed_mode;
    if (allowed_mode == ZPAL_PM_MODE_SLEEP)
    {
      __DSB();
      __WFI();
      __ISB();
      return;
    }
    else if (allowed_mode == ZPAL_PM_MODE_DEEP_SLEEP)
    {
      Lpm_Set_Low_Power_Level(LOW_POWER_LEVEL_SLEEP0);    //Sleep
    }
    else if (allowed_mode == ZPAL_PM_MODE_SHUTOFF)
    {
      Lpm_Set_Low_Power_Level(LOW_POWER_LEVEL_SLEEP2);    //Deep Sleep
      Lpm_Set_Sram_Sleep_Deepsleep_Shutdown(0x3F);        // keep the top 16kb SRAM powered on
#ifdef TR_PLATFORM_T32CZ20
      Lpm_Sub_System_Low_Power_Mode(COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP);     //if no load fw can call the function (Let subsystem enter sleep mode),  if load fw don't call the function
#endif
    }
    Lpm_Enter_Low_Power_Mode();
  }
}

void zpal_pm_exit_sleep(void)
{
  current_mode = ZPAL_PM_MODE_RUNNING;
  zpal_zw_pm_event_handler(allowed_mode, ZPAL_PM_MODE_RUNNING);
 }

zpal_pm_handle_t zpal_pm_register(zpal_pm_type_t type)
{
  ASSERT_ZPAL_PM_TYPE_T(type);
  zpal_pm_power_lock_t *lock = pvPortMalloc(sizeof(zpal_pm_power_lock_t));

  if (NULL == lock) {
    ASSERT(false);
    return NULL;
  }

  lock->magic_number = zpal_magic_number;
  lock->type = type;
  lock->active = false;
  lock->forever = false;

  lock->timer = xTimerCreateStatic( "",
                                    1,
                                    pdFALSE,
                                    lock,
                                    timer_callback,
                                    &lock->timer_buffer);

  ASSERT(lock->timer != NULL);

  DPRINTF("zpal_pm_register, handle: %p, type: %d\n", lock, type);
  return (zpal_pm_handle_t)lock;
}

void zpal_pm_stay_awake(zpal_pm_handle_t handle, uint32_t timeout_ms)
{
  if (NULL == handle)
  {
    return;
  }
  zpal_pm_power_lock_t *lock = (zpal_pm_power_lock_t *)handle;
  ASSERT(lock->magic_number == zpal_magic_number);
  DPRINTF("zpal_pm_stay_awake, handle: %p, timeout: %u, type: %d\n", handle, timeout_ms, lock->type);
  enter_critical_section();
  if (!lock->active)
  {
    active_locks[lock->type]++;
    DPRINTF("active_locks[%d] = %d\n", lock->type, active_locks[lock->type]);

    lock->active = true;
  }
  if (timeout_ms)
  {
    // Set timeout and start timer
    BaseType_t res = xTimerChangePeriod(lock->timer, pdMS_TO_TICKS(timeout_ms), pdMS_TO_TICKS(100));
    ASSERT(res == pdPASS);
    lock->forever = false;
  }
  else
  {
    BaseType_t res = xTimerStop(lock->timer, pdMS_TO_TICKS(100));
    ASSERT(res == pdPASS);
    lock->forever = true;
  }

  if (active_locks[ZPAL_PM_TYPE_DEEP_SLEEP] > 0)
  {
    allowed_mode = ZPAL_PM_MODE_DEEP_SLEEP;
  }
  if (active_locks[ZPAL_PM_TYPE_USE_RADIO] > 0)
  {
    allowed_mode = ZPAL_PM_MODE_SLEEP;
  }
  leave_critical_section();
}

void zpal_pm_cancel(zpal_pm_handle_t handle)
{
  if (NULL == handle)
  {
    return;
  }
  zpal_pm_power_lock_t *lock = (zpal_pm_power_lock_t *)handle;
  ASSERT(lock->magic_number == zpal_magic_number);
  DPRINTF("zpal_pm_cancel, handle: %p, timer: %p, active: %d, type: %d\n", handle, lock->timer, lock->active, lock->type);

  if (!lock || !lock->active)
  {
    return;
  }
  enter_critical_section();
  BaseType_t res = xTimerStop(lock->timer, pdMS_TO_TICKS(100));
  ASSERT(res == pdPASS);

  lock->forever = false;
  lock->active = false;

  if (active_locks[lock->type] > 0)
  {
    active_locks[lock->type]--;
    DPRINTF("active_locks[%d] = %d\n", lock->type, active_locks[lock->type]);
  }

  if (active_locks[ZPAL_PM_TYPE_USE_RADIO] == 0)
  {
    allowed_mode = ZPAL_PM_MODE_DEEP_SLEEP;

    if (active_locks[ZPAL_PM_TYPE_DEEP_SLEEP] == 0)
    {
      allowed_mode = ZPAL_PM_MODE_SHUTOFF;
    }
  }
  leave_critical_section();
}

void zpal_pm_cancel_all(void)
{
}
