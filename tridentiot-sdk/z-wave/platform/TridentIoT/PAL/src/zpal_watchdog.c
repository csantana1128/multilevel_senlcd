/// ****************************************************************************
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2023 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

#include "zpal_watchdog.h"
#include "wdt.h"
#include "sysctrl.h"
#include "chip_define.h"

static volatile bool enabled;
static uint16_t wdt_1ms;
#if (CHIP_MODEL == CHIP_ID(RT584, RT58X_MPB)) || (CHIP_MODEL == CHIP_ID(RT584, RT58X_MPA))
static  wdt_config_mode_ex_t wdt_mode;
#else
static  wdt_config_mode_t wdt_mode;
#endif
static   wdt_config_tick_t wdt_cfg_ticks;

#define ms_to_wdt_tick(ms) (wdt_1ms * ms)
#define wdt_reset_1sec    1000
#define wdt_reset_2sec    2000
void zpal_watchdog_init(void)
{

  /*
   *Remark: We should set each field of wdt_mode.
   *Otherwise, some field will become undefined.
   *It could be BUG.
   */
#if (CHIP_MODEL == CHIP_ID(RT584, RT58X_MPB)) || (CHIP_MODEL == CHIP_ID(RT584, RT58X_MPA))
  wdt_mode.sp_prescale = WDT_PRESCALE_128;
#else
  wdt_mode.prescale = WDT_PRESCALE_128;
#endif
  switch (Get_Ahb_System_Clk())
  {
  case SYS_CLK_32MHZ:
    wdt_1ms = 250; // watchdog clk is 250 kHz
    break;
  default:
#if !defined(TR_PLATFORM_T32CZ20)
  case SYS_CLK_48MHZ:
#endif
    wdt_1ms = 375; // watchdog clk is 375 kHz
    break;
#if !defined(TR_PLATFORM_T32CZ20)
  case SYS_CLK_64MHZ:
    wdt_1ms = 500; // watchdog clk is 500 kHz
    break;
#endif
  }

  wdt_mode.int_enable = 0;             /*wdt interrupt enable field*/
  wdt_mode.reset_enable = 1;           /*wdt reset enable field*/
  wdt_mode.lock_enable = 0;            /*wdt lock enable field*/

  wdt_cfg_ticks.wdt_ticks = ms_to_wdt_tick(wdt_reset_2sec);
  wdt_cfg_ticks.int_ticks = ms_to_wdt_tick(0);
  wdt_cfg_ticks.wdt_min_ticks = ms_to_wdt_tick(0);
}

bool zpal_is_watchdog_enabled(void)
{
  return enabled;
}

void zpal_enable_watchdog(bool enable)
{
  enabled = enable;
  if (enable)
  {
#if (CHIP_MODEL == CHIP_ID(RT584, RT58X_MPB)) || (CHIP_MODEL == CHIP_ID(RT584, RT58X_MPA))
    Wdt_Start_Ex(wdt_mode, wdt_cfg_ticks, NULL); /*wdt reset time = 50ms, window min 1ms*/
#else
    Wdt_Start(wdt_mode, wdt_cfg_ticks, NULL); /*wdt reset time = 50ms, window min 1ms*/
#endif
    zpal_feed_watchdog();
  }
  else
  {
    Wdt_Stop();
  }
}

void zpal_feed_watchdog(void)
{
  if (enabled)
  {
    Wdt_Kick();
  }
}
