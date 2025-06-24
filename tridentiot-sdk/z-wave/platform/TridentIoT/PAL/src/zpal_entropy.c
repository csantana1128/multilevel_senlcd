/// ****************************************************************************
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

#include <stdbool.h>
#include <string.h>
#include "zpal_entropy.h"
#include "zpal_init.h"
#include "zpal_defs.h"
#include "Assert.h"
#include "tr_platform.h"
#include "sysctrl.h"
#if defined(TR_PLATFORM_T32CZ20)
#include "trng.h"
#endif

static bool entropy_init = false;

static uint32_t entropy_seed  __attribute__((section(".ret_sram")));

// Function to get random numbers (uint32_t) from the platform-specific random number generator.
static inline void tr_get_random_number(uint32_t * p_value, uint32_t length)
{
#if defined(TR_PLATFORM_T32CZ20)
  uint32_t status = get_random_number(p_value, length);
  ASSERT(STATUS_SUCCESS == status);
#elif defined(TR_PLATFORM_ARM)
  for (uint32_t i = 0; i < length; i++)
  {
    p_value[i] = get_random_number();
  }
#endif
}

void zpal_entropy_init(void)
{
  ASSERT(!entropy_init); // Catch multiple calls to zpal_entropy_init().
  entropy_init = true;
  zpal_reset_reason_t reason =  zpal_get_reset_reason();

  if ((ZPAL_RESET_REASON_POWER_ON == reason) || (ZPAL_RESET_REASON_PIN == reason))
  {
    tr_get_random_number(&entropy_seed, 1);
  }
}

zpal_status_t zpal_get_random_data(uint8_t *data, size_t len)
{
  ASSERT(entropy_init); // Catch missing call to zpal_entropy_init().
  uint32_t rand_buf[8]; // Buffer to hold random numbers.
  uint8_t total_words = len / 4;
  while (0 != total_words)
  {
    uint8_t words = total_words;
    if (words > 8)
    {
      words = 8; // Limit to 8 words at a time.
    }
    tr_get_random_number(rand_buf, words);
    memcpy(data, rand_buf, words * 4);
    data += words * 4;
    len -= words * 4;
    total_words -= words;
  }
  if (len > 0)
  {
    tr_get_random_number(rand_buf, 1);
    memcpy(data, &rand_buf, len);
  }
  return ZPAL_STATUS_OK;
}

uint8_t zpal_get_pseudo_random(void)
{
  ASSERT(entropy_init); // Catch missing call to zpal_entropy_init().
  entropy_seed = (entropy_seed ^ 0xAA) + 0x11;  /* XOR with 170 and add 17 */
  return entropy_seed;
}
