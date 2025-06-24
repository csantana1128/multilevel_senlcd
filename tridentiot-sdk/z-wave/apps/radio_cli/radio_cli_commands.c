/// ***************************************************************************
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2024 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

/**
 * @file radio_cli_commands.c
 * @brief Z-Wave radio test CLI command implementation
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUGPRINT // NOSONAR
#include "DebugPrint.h"
#include "DebugPrintConfig.h"

#include <embedded_cli.h>
#include <zpal_radio_utils.h>
#include "cli_uart_interface.h"
#include "radio_cli_app.h"
#include "radio_cli_commands.h"

/****************************************************************************/
/*                               PRIVATE DATA                               */
/****************************************************************************/
#define MAX_CHANNELS 4

static uint8_t payloadbuffer[200];

radio_cli_tx_frame_config_t frame = {.payload_buffer = payloadbuffer,
                                     .payload_length = 0,
                                     .channel = 0,
                                     .lbt = true,
                                     .delay = DEFAULT_TX_DELAY_MS,
                                     .power = 0,};

// Function prototypes for the CLI commands
void cli_zw_init(EmbeddedCli *cli, char *args, void *context);
void cli_zw_region_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_region_list(EmbeddedCli *cli, char *args, void *context);
void cli_zw_homeid_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_nodeid_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_payload_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_tx_channel_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_tx_power_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_tx_power_index_list(EmbeddedCli *cli, char *args, void *context);
void cli_zw_tx(EmbeddedCli *cli, char *args, void *context);
void cli_zw_status_get(EmbeddedCli *cli, char *args, void *context);
void get_version_handler(EmbeddedCli *cli, char *args, void *context);
void cli_zw_rx_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_tx_set_lbt(EmbeddedCli *cli, char *args, void *context);
void cli_stats_get(EmbeddedCli *cli, char *args, void *context);
void cli_stats_clear(EmbeddedCli *cli, char *args, void *context);
void cli_zw_rx_channel_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_tx_delay_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_tx_config_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_tx_beam(EmbeddedCli *cli, char *args, void *context);
void cli_zw_tx_max_power_set(EmbeddedCli *cli, char *args, void *context);

void cli_zw_radio_tx_continues_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_radio_rf_debug_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_radio_rf_debug_reg_setting_list(EmbeddedCli *cli, char *args, void *context);
void cli_zw_radio_calibration_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_radio_rssi_get(EmbeddedCli *cli, char *args, void *context);
void cli_zw_radio_rssi_get_all(EmbeddedCli *cli, char *args, void *context);
void cli_zw_radio_rssi_config_set(EmbeddedCli *cli, char *args, void *context);
void cli_zw_reset(EmbeddedCli *cli, char *args, void *context);
void cli_zw_script_entry(EmbeddedCli *cli, char *args, void *context);
void cli_zw_script_run(EmbeddedCli *cli, char *args, void *context);
void cli_zw_wait(EmbeddedCli *cli, char *args, void *context);
void cli_zw_radio_timestamp(EmbeddedCli *cli, char *args, void *context);
void cli_zw_dump(EmbeddedCli *cli, char *args, void *context);
void cli_zw_cal_xtal(EmbeddedCli *cli, char *args, void *context);

// List of CLI commands
CliCommandBinding cli_command_list[] =
{
  {"zw-init", "zw-init - Initialize the Z-Wave PHY layer", false, NULL, cli_zw_init},
  {"zw-region-set", "zw-region-set <region> - Set the desired Z-Wave region (0-103)", true, NULL, cli_zw_region_set},
  {"zw-region-list", "zw-region-list - Lists current region and all supported regions", false, NULL, cli_zw_region_list},
  {"zw-homeid-set", "zw-homeid-set <homeID> - Set the desired Z-Wave HomeID in hex", true, NULL, cli_zw_homeid_set},
  {"zw-nodeid-set", "zw-nodeid-set <nodeID> - Set the desired Z-Wave nodeID in decimal", true, NULL, cli_zw_nodeid_set},
  {"zw-tx-payload-set", "zw-tx-payload-set <b1> <b2> .. <bn> - Set the frame payload in hex byte, if less than 9 bytes then uses default payloads", true, NULL, cli_zw_payload_set},
  {"zw-tx-channel-set", "zw-tx-channel-set <channel> - Set transmit channel (0-3 according to region)", true, NULL, cli_zw_tx_channel_set},
  {"zw-tx-power-set", "zw-tx-power-set <power> [powerindex] - Set transmit power in dBm (-20-+20), if optional powerindex is specified then this will be used instead", true, NULL, cli_zw_tx_power_set},
  {"zw-tx-power-index-list", "zw-tx-power-index-list - list dynamic tx power to radio power_index conversion table", false, NULL, cli_zw_tx_power_index_list},
  {"zw-tx-lbt-set", "zw-tx-lbt-set <level> - Set lbt level in dBm (-127-0), 0 equals lbt is turned off", true, NULL, cli_zw_tx_set_lbt},
  {"zw-stats-get", "zw-stats-get [0/1] - Get network statistics and optionaly extended statistics", true, NULL, cli_stats_get},
  {"zw-stats-clear", "zw-stats-clear [0/1] - Clear network statistics or optionaly only clear tx_time statistics", true, NULL, cli_stats_clear},
  {"zw-tx-delay-set", "zw-tx-delay-set - Set delay inbetween repeated frame transmits (2-65535) ms", true, NULL, cli_zw_tx_delay_set},
  {"zw-tx-config-set", "zw-tx-config-set <option> <on/of>- Set tx option on off, supported options: fail-crc", true, NULL, cli_zw_tx_config_set},
  {"zw-rx-channel-set",  "zw-rx-channel-set <on/off> <channel> - Set on/off to use fixed Rx channel (0-3)", true, NULL, cli_zw_rx_channel_set},
  {"zw-tx-beam", "zw-tx-beam <repeats> <destid> [ack] - Transmit <repeats> Beam+frame to <destid> and optionally wait for ack", true, NULL, cli_zw_tx_beam},
  {"zw-tx-max-power-set", "zw-tx-max-power-set <14/20> - Set Tx max power (14/20) dBm", true, NULL, cli_zw_tx_max_power_set},
  {"zw-radio-tx-continues-set", "zw-radio-tx-continues_set <on/off> - Set transmit unmodulated carrier on or off", true, NULL, cli_zw_radio_tx_continues_set},
  {"zw-radio-rf-debug-set", "zw-radio-rf-debug-set <on/off> - Set RF state gpio out on/off", true, NULL, cli_zw_radio_rf_debug_set},
  {"zw-radio-calibration-set", "zw-radio-calibration-set <boardno> - Set 0 for default setting or use 9-12, 72, 74-78 or B1-B8 for predefined RF calibration setting", true, NULL, cli_zw_radio_calibration_set},
  {"zw-radio-rf-debug-reg-setting-list", "zw-radio-rf-debug-reg-setting-list [0/1] - List selected radio reg settings and optionaly all radio reg settings", true, NULL, cli_zw_radio_rf_debug_reg_setting_list},
  {"zw-radio-rssi-get",  "zw-radio-rssi-get <channel> [count [delay]] - Get radio RSSI on channel, optionaly count times with optionally delay ms inbetween (defaults to 1000ms)", true, NULL, cli_zw_radio_rssi_get},
  {"zw-radio-rssi-get-all",  "zw-radio-rssi-get-all [count [delay]] - Get radio RSSI on all channels in current region, optionaly count times with optionally delay ms inbetween (defaults to 1000ms)", true, NULL, cli_zw_radio_rssi_get_all},
  {"zw-radio-rssi-config-set",  "zw-radio-rssi-config-set <sample_freq> <average_count> - Set radio RSSI sample frequency sample_freq and average_count samples\n\t\t\t\tused for generating RSSI average received when doing rssi get. Valid only when doing Rx channel scanning", true, NULL, cli_zw_radio_rssi_config_set},
  {"tx", "tx <repeat> [wait ack] - Send <repeat> frames and optionally wait for an ack", true, NULL, cli_zw_tx},
  {"rx", "rx <on/off> - Set the receiver on or off", true, NULL, cli_zw_rx_set},
  {"timestamp", "timestamp <on/off> - enable/disable timestamp on RX and TX printout - Default is no timestamp", true, NULL, cli_zw_radio_timestamp},
  {"reset", "reset - reset radio_cli firmware", false, NULL, cli_zw_reset},
  {"script", "script <command>\n\t\tstart [1-5] - start active or specified script entry,\n\t\tstop - stop running script,\n\t\tautoon/autooff [1-5] - enable/disable active or specified script run on startup,\n\t\tlist [1-5] - list all or specified script,\n\t\tclear [1-5] - clear active or specified script", true, NULL, cli_zw_script_entry},
  {"run", "run [1-5] - run active script or specified script", true, NULL, cli_zw_script_run},
  {"wait", "wait <time> - wait time milliseconds before doing scriptline transitioning - valid range 1-86400000 ms", true, NULL, cli_zw_wait},
  {"status",  "status - Get status", false, NULL, cli_zw_status_get},
  {"dump", "dump <ft/mp> - Dump flash sector", true, NULL, cli_zw_dump},
  {"cal-xtal", "cal-xtal <try/store> <cal value> - Set crystal calibration to value (0-63)", true, NULL, cli_zw_cal_xtal},
  {"version",  "version - Get version", false, NULL, get_version_handler},
  {"", "", false, NULL}
};

/****************************************************************************/
/*                                 FUNCTIONS                                */
/****************************************************************************/

/*
 * Validate a range and print an error if out of range
 */
bool validate_integer_range(int value, int min, int max, uint8_t parameter)
{
  if ((value > max) || (value < min ))
  {
    if (parameter)
    {
      cli_uart_printf("Parameter %u must be in the range (%i..%i)\n", parameter, min, max);
    }
    return false;
  }
  return true;
}

/*
 * Check the number of arguments for a command and print error message
 */
bool check_argument_count(uint8_t count, uint8_t required, char* parms)
{
  if (count == required)
  {
    return true;
  }
  else if (count < required)
  {
    cli_uart_printf("** Missing argument(s) %s\n", parms);
  }
  else
  {
    cli_uart_print("** Wrong number of arguments\n");
  }
  return false;
}

void cli_zw_status_get(EmbeddedCli *cli, char *args, void *context)
{
  cli_radio_status_get(&frame);
}

/*
 * Handler for version command
 */
void get_version_handler(EmbeddedCli *cli, char *args, void *context)
{
  cli_radio_version_print();
}

/*
 * Handler for Z-Wave init function
 */
void cli_zw_init(EmbeddedCli *cli, char *args, void *context)
{
  if (REGION_UNDEFINED == cli_radio_region_current_get())
  {
    cli_uart_printf("** Undefined region, use %s to set the region\n", cli_command_list[1].name);
  }
  else
  {
    cli_radio_setup(cli_radio_region_current_get());
    cli_uart_printf("Z-Wave Radio initialized to Region %s (%i), Tx channel %i\n", cli_radio_region_current_description_get(), cli_radio_region_current_get(), frame.channel);
  }
  cli_radio_script_state_transition_event();
}

/*
 * Handler for Z-Wave z-wave region list function
 */
void cli_zw_region_list(EmbeddedCli *cli, char *args, void *context)
{
  cli_radio_region_list(cli_radio_region_current_get());
}

/*
 * Handler for Z-Wave region set function
 */
void cli_zw_region_set(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "Region"))
  {
    const char * arg = embeddedCliGetToken(args, 1); // args are counted from 1 (not from 0)
    zpal_radio_region_t new_region = atoi(arg);

    if (false == cli_radio_change_region(new_region))
    {
      cli_uart_print("** Changing region failed\n");
    }
    else
    {
      uint8_t region_channel_count = cli_radio_region_channel_count_get();
      frame.channel = (region_channel_count - 1 < frame.channel) ? 0 : frame.channel;
    }
    cli_uart_printf("Region %i, Tx channel %i\n", cli_radio_region_current_get(), frame.channel);
  }
  else
  {
    cli_radio_region_list(cli_radio_region_current_get());
  }
  cli_radio_script_state_transition_event();
}

/*
 * Handler for Z-Wave homeID set function
 */
void cli_zw_homeid_set(EmbeddedCli *cli, char *args, void *context)
{
  uint32_t homeid;

  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "HomeID"))
  {
    homeid = (uint32_t)strtoul(args, NULL, 16);
    cli_uart_printf("Setting homeID to %08X\n", homeid);
    cli_radio_set_homeid(homeid);
  }
  cli_radio_script_state_transition_event();
}

/*
 * Handler for Z-Wave nodeID set function
 */
void cli_zw_nodeid_set(EmbeddedCli *cli, char *args, void *context)
{
  node_id_t node_id;

  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "nodeID"))
  {
    const char * arg = embeddedCliGetToken(args, 1); // args are counted from 1 (not from 0)
    node_id = atoi(arg);

    if (validate_integer_range(node_id, 1, 239, 0) || validate_integer_range(node_id, 256, 1024, 0))
    {
      cli_uart_printf("Setting nodeID to %u\n", node_id);
      cli_radio_set_nodeid(node_id);
    }
    else
    {
      cli_uart_print("NodeID must be in the range (1..239) or (256..1024)\n");
    }
  }
  cli_radio_script_state_transition_event();
}

// Frame payload must be at least 9 bytes
#define MIN_PAYLOAD_LENGTH 9

/*
 * Handler for Z-Wave Tx payload set function
 */
void cli_zw_payload_set(EmbeddedCli *cli, char *args, void *context)
{
  const char* string_byte;
  uint8_t count = embeddedCliGetTokenCount(args);

  frame.payload_length = count;

  if (count >= MIN_PAYLOAD_LENGTH)
  {
    cli_uart_print("Setting payload ");
    for (int i=1; i<=count;i++)
    {
      string_byte = embeddedCliGetToken(args, i);

      frame.payload_buffer[i-1] = (uint8_t)strtoul(string_byte, NULL, 16);
      cli_uart_printf("%02x",frame.payload_buffer[i-1]);
    }
    cli_uart_printf("\n");
  }
  else
  {
    cli_uart_printf("** Payload must be at least %u bytes\n", MIN_PAYLOAD_LENGTH);
    cli_uart_printf("** Using default payload\n");
    cli_radio_set_payload_default(&frame);
  }
  cli_radio_script_state_transition_event();
}

/**
 * Handler for setting Tx max power command
 */
void cli_zw_tx_max_power_set(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t max_tx_power;

  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "14/20"))
  {
    const char * arg = embeddedCliGetToken(args, 1); // args are counted from 1 (not from 0)
    max_tx_power = atoi(arg);

    if ((max_tx_power == 14) || (max_tx_power == 20))
    {
      cli_uart_printf("Setting Tx max power to %udBm\n", max_tx_power);
      cli_radio_set_tx_max_power_20dbm((max_tx_power == 20));
      cli_uart_printf("NOTE: The allowed Max Tx power depends of the board layout so, make sure that the value set is supported by this board.\n");
    }
    else
    {
      cli_uart_print("Tx max power must 14 or 20\n");
    }
  }
  cli_radio_script_state_transition_event();
}

/*
 * Callback function for Z-Wave transmit command
 */
void cli_zw_tx_complete(uint16_t success, uint16_t failed, uint16_t failed_lbt)
{
  cli_uart_printf("Transmit complete, %u success, %u failed, %u lbt_failed\n", success, failed, failed_lbt);
  cli_radio_script_state_transition_event();
}

/*
 * Callback function for Z-Wave Beam transmit command
 */
void cli_zw_tx_beam_complete(uint16_t success, uint16_t failed, uint16_t failed_lbt)
{
  cli_uart_printf("Transmit Beam complete, %u success, %u failed, %u lbt_failed\n", success, failed, failed_lbt);
  cli_radio_script_state_transition_event();
}

/**
 * Handler for Z-Wave Beam transmit function
 */
void cli_zw_tx_beam(EmbeddedCli *cli, char *args, void *context)
{
  if (REGION_UNDEFINED == cli_radio_region_current_get())
  {
    cli_uart_printf("** Undefined region, use %s to set the region\n", cli_command_list[1].name);
    cli_radio_script_state_transition_event();
    return;
  }

  const char* string_u32;
  uint16_t destid;
  bool wait_ack = false;
  uint8_t arg_count = embeddedCliGetTokenCount(args);

  if (arg_count > 1 && arg_count < 4)
  {
    if (0 == frame.payload_length)
    {
      cli_uart_print("** No payload set\n");
      cli_radio_script_state_transition_event();
    }
    else if (MAX_CHANNELS == frame.channel)
    {
      cli_uart_print("** No channel set\n");
      cli_radio_script_state_transition_event();
    }
    else
    {
      string_u32 = embeddedCliGetToken(args, 1);
      frame.frame_repeat = strtoul(string_u32, NULL, 10);
      string_u32 = embeddedCliGetToken(args, 2);
      frame.destid = (uint16_t)strtoul(string_u32, NULL, 10);
      frame.wait_ack = (0 != embeddedCliFindToken(args, "ack"));
      frame.tx_callback = cli_zw_tx_beam_complete;
      if (false == cli_radio_transmit_beam_frame(&frame))
      {
        cli_uart_print("\n** Functionality not implemented **\n");
        cli_radio_script_state_transition_event();
      }
    }
  }
  else
  {
    cli_uart_print("** Invalid number of arguments\n");
    cli_radio_script_state_transition_event();
  }
}

/*
 * Handler for Z-Wave transmit function
 */
void cli_zw_tx(EmbeddedCli *cli, char *args, void *context)
{
  if (REGION_UNDEFINED == cli_radio_region_current_get())
  {
    cli_uart_printf("** Undefined region, use %s to set the region\n", cli_command_list[1].name);
    cli_radio_script_state_transition_event();
    return;
  }

  const char* string_uint32;
  bool wait_ack = false;
  uint8_t count = embeddedCliGetTokenCount(args);

  if (count > 0 && count < 3)
  {
    if (0 == frame.payload_length)
    {
      cli_uart_print("** No payload set\n");
      cli_radio_script_state_transition_event();
    }
    else if (MAX_CHANNELS == frame.channel)
    {
      cli_uart_print("** No channel set\n");
      cli_radio_script_state_transition_event();
    }
    else
    {
      string_uint32 = embeddedCliGetToken(args, 1);
      frame.frame_repeat = strtoul(string_uint32, NULL, 10);
      frame.wait_ack = (0 != embeddedCliFindToken(args, "ack"));
      frame.channel = frame.channel;
      frame.tx_callback = cli_zw_tx_complete;
      cli_radio_transmit_frame(&frame);
    }
  }
  else
  {
    cli_uart_print("** Invalid number of arguments\n");
    cli_radio_script_state_transition_event();
  }
}

/*
 * Handler for Z-Wave tx channel set function
 */
void cli_zw_tx_channel_set(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t tmp_channel;

  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "channel"))
  {
    const char * arg = embeddedCliGetToken(args, 1); // args are counted from 1 (not from 0)
    tmp_channel = atoi(arg);

    uint8_t region_channel_count = cli_radio_region_channel_count_get();
    if (0 == region_channel_count)
    {
      // If radio not initialized - we will only allow Tx channel to become 0
      region_channel_count++;
    }
    if (validate_integer_range(tmp_channel, 0, region_channel_count-1, 1))
    {
      frame.channel = tmp_channel;
      cli_uart_printf("** Using default payload\n");
      cli_radio_set_payload_default(&frame);
    }
  }
  cli_uart_printf("Tx channel %i\n", frame.channel);
  cli_radio_script_state_transition_event();
}

bool validate_radio_power_index(uint8_t power_index)
{
  bool valid = false;

  if (cli_radio_get_tx_max_power_20dbm())
  {
    if ((power_index < 201) && validate_integer_range(power_index, 76, 127, 2))
    {
      valid = true;
    }
    else if (validate_integer_range(power_index, 201, 255, 2))
    {
      valid = true;
    }
  }
  else
  {
    if ((power_index < 143) && validate_integer_range(power_index, 15, 63, 2))
    {
      valid = true;
    }
    else if (validate_integer_range(power_index, 143, 191, 2))
    {
      valid = true;
    }
  }
  return valid;
}

static void update_tx_power(char *args, uint8_t count)
{
  const char * arg = embeddedCliGetToken(args, 1);
  int8_t tmp_power = atoi(arg);

  if (validate_integer_range(tmp_power, cli_radio_get_tx_min_power(frame.channel), cli_radio_get_tx_max_power(frame.channel), 1))
  {
    frame.power = tmp_power;
    if (2 == count)
    {
      arg = embeddedCliGetToken(args, 2);
      uint8_t power_index = atoi(arg);

      power_index = validate_radio_power_index(power_index) ? power_index : 0;
      if ((0 != power_index) && cli_radio_tx_power_index_set(frame.channel, frame.power, power_index))
      {
        cli_uart_printf("Tx Power on channel %d, set to %idBm, power_index %d\n", frame.channel, frame.power, power_index);
      }
    }
    else
    {
      cli_uart_printf("Tx Power on channel %d, set to %idBm\n", frame.channel, frame.power);
    }
  }
  else
  {
    if ((tmp_power > 14) && (!cli_radio_get_tx_max_power_20dbm()))
    {
      cli_uart_printf("Tx power is above this boards allowed Max Tx power\n");
      cli_uart_printf("The allowed Max Tx power can be set with the zw-tx-max-power-set command\n");
    }
  }
}

bool radio_is_not_initialized(void)
{
  bool status = false;
  if (!cli_radio_initialized())
  {
    cli_radio_script_state_transition_event();
    status = true;
  }
  return status;
}

/*
 * Handler for Z-Wave tx channel set function
 */
void cli_zw_tx_power_set(EmbeddedCli *cli, char *args, void *context)
{
  if (radio_is_not_initialized())
  {
    return;
  }

  uint8_t count = embeddedCliGetTokenCount(args);

  if ((count >= 1) && (count < 3))
  {
    update_tx_power(args, count);
  }
  else
  {
    check_argument_count(count, 1, "power");
    cli_uart_printf("Tx Power on channel %d, set to %idBm\n", frame.channel, frame.power);
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_tx_power_index_list(EmbeddedCli *cli, char *args, void *context)
{
  if (radio_is_not_initialized())
  {
    return;
  }

  cli_radio_tx_power_index_list(frame.channel);
  cli_radio_script_state_transition_event();
}

/**
 * Handler for setting transmission delay between transmit repeats
 */
void cli_zw_tx_delay_set(EmbeddedCli *cli, char *args, void *context)
{
  uint16_t tmp_delay;

  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "delay"))
  {
    const char * arg = embeddedCliGetToken(args, 1);
    tmp_delay = atoi(arg);

    if (validate_integer_range(tmp_delay, 2, 65535, 1))
    {
      frame.delay = tmp_delay;
    }
    cli_uart_printf("Tx repeat delay %u\n", frame.delay);
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_tx_config_set(EmbeddedCli *cli, char *args, void *context)
{
  const char* tmp;
  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 2, "option, on/off"))
  {
    uint8_t option = 0;
    uint8_t enable = false;

    tmp = embeddedCliGetToken(args, 1);
    if (!strcmp(tmp, "fail-crc"))
    {
      option = 1;
    }
    else
    {
      cli_uart_print("** first argument must be a tx option - fail-crc\n");
      cli_radio_script_state_transition_event();
      return;
    }
    tmp = embeddedCliGetToken(args, 2);
    if (!strcmp(tmp, "on"))
    {
      enable = 1;
    }
    else if (!strcmp(tmp, "off"))
    {
      enable = 0;
    }
    else
    {
      cli_uart_print("** second argument must be on or off\n");
      cli_radio_script_state_transition_event();
      return;
    }
    if (0 != option)
    {
      cli_radio_tx_option_set(option, enable);
    }
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_rx_set(EmbeddedCli *cli, char *args, void *context)
{
  if (REGION_UNDEFINED == cli_radio_region_current_get())
  {
    cli_uart_printf("** Undefined region, use %s to set the region\n", cli_command_list[1].name);
    cli_radio_script_state_transition_event();
    return;
  }

  bool start_receive = false;
  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "on/off"))
  {
    const char * arg = embeddedCliGetToken(args, 1); // args are counted from 1 (not from 0)
    if (!strcmp(arg, "on"))
    {
      start_receive = true;
    }
    else if (!strcmp(arg, "off"))
    {
      start_receive = false;
      cli_uart_printf("Rx off - Received %u frames\n", cli_radio_get_rx_count());
    }
    else
    {
      cli_uart_print("** argument must be on or off\n");
      cli_radio_script_state_transition_event();
      return;
    }
    cli_radio_start_receive(start_receive);
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_tx_set_lbt(EmbeddedCli *cli, char *args, void *context)
{
  const char* tmp;
  int8_t level;
  int8_t channel;

  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 2, "channel, power"))
  {
    tmp = embeddedCliGetToken(args, 1);
    channel = atoi(tmp);
    if (validate_integer_range(channel, 0, 3, 1))
    {
      tmp = embeddedCliGetToken(args, 2);
      level = atoi(tmp);
      if (validate_integer_range(level, -127, 0, 1))
      {
        // If lbt level equal 0 then turn off LBT on transmit on specified channel
        frame.lbt = (0 != level);
        cli_radio_set_lbt_level(channel, level);
        cli_uart_printf("Setting lbt level=%idBm for channel %i, lbt %s\n",level, channel, (frame.lbt ? "on" : "off"));
      }
    }
  }
  cli_radio_script_state_transition_event();
}

void cli_stats_get(EmbeddedCli *cli, char *args, void *context)
{
  zpal_radio_network_stats_t *stats;
  stats = cli_radio_get_stats();

  if (NULL != stats)
  {
    cli_uart_print("Network statistics\n");
    cli_uart_print("----------------------\n");
    cli_uart_printf("Tx frame        = %i\n",stats->tx_frames);
    cli_uart_printf("Tx lbt errors   = %i\n",stats->tx_lbt_back_offs);
    cli_uart_printf("Rx frame        = %i\n",stats->rx_frames);
    cli_uart_printf("Rx lrc errors   = %i\n",stats->rx_lrc_errors);
    cli_uart_printf("Rx crc errors   = %i\n",stats->rx_crc_errors);
    cli_uart_printf("HomeID mismatch = %i\n",stats->rx_foreign_home_id);
    cli_uart_printf("Total Tx time 0 = %i\n",stats->tx_time_channel_0);
    cli_uart_printf("Total Tx time 1 = %i\n",stats->tx_time_channel_1);
    cli_uart_printf("Total Tx time 2 = %i\n",stats->tx_time_channel_2);
    cli_uart_printf("Total Tx time 3 = %i\n",stats->tx_time_channel_3);
    cli_uart_printf("Total Tx time 4 = %i\n",stats->tx_time_channel_4);
    cli_uart_print("----------------------\n");

    bool print_extended = false;
    uint8_t count = embeddedCliGetTokenCount(args);

    if (count == 1)
    {
      const char* tmp;
      int8_t param_print_extended;

      tmp = embeddedCliGetToken(args, 1);
      param_print_extended = atoi(tmp);
      if (validate_integer_range(param_print_extended, 0, 1, 1))
      {
        print_extended = (param_print_extended == 1);
      }
    }
    cli_radio_print_statistics(print_extended);
  }
  cli_radio_script_state_transition_event();
}

void cli_stats_clear(EmbeddedCli *cli, char *args, void *context)
{
  const char* tmp;
  int8_t clear = 0;
  if (embeddedCliGetTokenCount(args) > 0)
  {
    tmp = embeddedCliGetToken(args, 1);
    clear = atoi(tmp);
    if (!validate_integer_range(clear, 0, 1, 1))
    {
      return;
    }
  }
  cli_radio_clear_stats((clear == 1));
  cli_radio_script_state_transition_event();
}

void cli_zw_rx_channel_set(EmbeddedCli *cli, char *args, void *context)
{
  const char* tmp;
  uint8_t channel;
  int8_t enable = -1;
  uint8_t count = embeddedCliGetTokenCount(args);

  if (REGION_UNDEFINED == cli_radio_region_current_get())
  {
    cli_uart_printf("** Undefined region, use %s to set the region\n", cli_command_list[1].name);
    return;
  }

  if (check_argument_count(count, 2, "on/off, channel"))
  {
    tmp = embeddedCliGetToken(args, 2);
    channel = atoi(tmp);
    uint8_t region_channel_count = cli_radio_region_channel_count_get();
    if (validate_integer_range(channel, 0, region_channel_count-1, 1))
    {
      tmp = embeddedCliGetToken(args, 1); // args are counted from 1 (not from 0)
      if (!strcmp(tmp, "on"))
      {
        enable = 1;
      }
      else if (!strcmp(tmp, "off"))
      {
        enable = 0;
      }
      else
      {
        cli_uart_print("** argument must be on or off\n");
        cli_radio_script_state_transition_event();
        return;
      }
      if (1 == enable)
      {
        cli_uart_printf("Rx fixed channel enabled using channel %u \n", channel);
      }
      else if (0 == enable)
      {
        cli_uart_print("Rx fixed channel disabled\n");
      }
      cli_radio_set_fixed_channel(enable, channel);
    }
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_radio_tx_continues_set(EmbeddedCli *cli, char *args, void *context)
{
  if (radio_is_not_initialized())
  {
    return;
  }

  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "enable"))
  {
    const char * arg = embeddedCliGetToken(args, 1);
    bool enable;
    if (!strcmp(arg, "on"))
    {
      enable = true;
    }
    else if (!strcmp(arg, "off"))
    {
      enable = false;
    }
    else
    {
      cli_uart_print("** argument must be on or off\n");
      cli_radio_script_state_transition_event();
      return;
    }
    cli_radio_tx_continues_set(enable, &frame);
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_radio_rf_debug_set(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "on/off"))
  {
    const char * arg;
    bool rf_state_enable;

    arg = embeddedCliGetToken(args, 1);
    if (!strcmp(arg, "on"))
    {
      rf_state_enable = true;
    }
    else if (!strcmp(arg, "off"))
    {
      rf_state_enable = false;
    }
    else
    {
      cli_uart_printf("rf ** argument must be on or off %s\n", arg);
      cli_radio_script_state_transition_event();
      return;
    }

    cli_radio_rf_debug_set(rf_state_enable);
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_radio_calibration_set(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "boardno"))
  {
    const char * arg = embeddedCliGetToken(args, 1);
    int16_t boardno = atoi(arg);
    int16_t boardnoHex = 0;
    if (0 == boardno)
    {
      boardnoHex = strtol(arg, NULL, 16);
      if ((0 == boardno) && (0 != boardnoHex))
      {
        boardno = boardnoHex;
      }
    }
    if ((0 == boardno) || (validate_integer_range(boardno, 9, 12, 0) || validate_integer_range(boardno, 0xB1, 0xB8, 0) || ((73 != boardno) && validate_integer_range(boardno, 72, 78, 0))))
    {
      cli_radio_calibration_set((uint8_t)boardno);
    }
    else
    {
      cli_uart_printf("boardno must be 0, 9-12, 72, 74-78 or in the B1-B8 range - entered %s\n", arg);
      cli_radio_script_state_transition_event();
      return;
    }
  }
  else
  {
    cli_radio_calibration_list();
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_radio_rf_debug_reg_setting_list(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t count = embeddedCliGetTokenCount(args);
  bool listallreg = false;

  if (count == 1)
  {
    const char * arg = embeddedCliGetToken(args, 1);
    int16_t listreg = atoi(arg);
    if (validate_integer_range(listreg, 0, 1, 0))
    {
      listallreg = (listreg == 1);
    }
  }
  cli_radio_rf_debug_reg_setting_list(listallreg);
  cli_radio_script_state_transition_event();
}

void cli_zw_radio_rssi_get(EmbeddedCli *cli, char *args, void *context)
{
  if (radio_is_not_initialized())
  {
    return;
  }

  uint8_t count = embeddedCliGetTokenCount(args);
  uint8_t channel = 0;
  uint32_t repeats = 1;
  uint32_t delay = 1000;

  if (count >= 1)
  {
    const char * arg = embeddedCliGetToken(args, 1);
    uint32_t value = strtoul(arg, NULL, 10);
    if (validate_integer_range(value, 0, cli_radio_region_channel_count_get() - 1, 0))
    {
      channel = (uint8_t)value;
    }
    if (count >= 2)
    {
      arg = embeddedCliGetToken(args, 2);
      value = strtoul(arg, NULL, 10);
      if (value)
      {
        repeats = value;
      }
    }
    if (count >= 3)
    {
      arg = embeddedCliGetToken(args, 3);
      value = strtoul(arg, NULL, 10);
      if (value)
      {
        delay = value;
      }
    }
  }
  cli_radio_rssi_get(channel, repeats, delay);
  cli_radio_script_state_transition_event();
}

void cli_zw_radio_rssi_get_all(EmbeddedCli *cli, char *args, void *context)
{
  if (radio_is_not_initialized())
  {
    return;
  }

  uint8_t count = embeddedCliGetTokenCount(args);
  uint32_t repeats = 1;
  uint32_t delay = 1000;

  if (count >= 1)
  {
    const char * arg = embeddedCliGetToken(args, 1);
    uint32_t value = strtoul(arg, NULL, 10);
    if (value)
    {
      repeats = value;
    }
    if (count >= 2)
    {
      arg = embeddedCliGetToken(args, 2);
      value = strtoul(arg, NULL, 10);
      if (value)
      {
        delay = value;
      }
    }
  }

  cli_radio_rssi_get_all(repeats, delay);
  cli_radio_script_state_transition_event();
}

void cli_zw_radio_rssi_config_set(EmbeddedCli *cli, char *args, void *context)
{
  if (radio_is_not_initialized())
  {
    return;
  }

  uint8_t count = embeddedCliGetTokenCount(args);
  uint16_t sample_frequency = 0;
  uint8_t sample_count_average = 0;

  if (count == 2)
  {
    const char * arg = embeddedCliGetToken(args, 1);
    sample_frequency = (uint16_t)atoi(arg);
    if (validate_integer_range(sample_frequency, 0, 65535, 0))
    {
      arg = embeddedCliGetToken(args, 2);
      sample_count_average = atoi(arg);
      if (!validate_integer_range(sample_count_average, 0, 255, 0))
      {
        sample_count_average = 0;
      }
    }
    cli_radio_rssi_config_set(sample_frequency, sample_count_average);
  }
  else
  {
    cli_uart_print("Usage: zw-radio-rssi-config-set <sample_frequency> <sample_count_average>\n\n");
  }
  cli_radio_rssi_config_get(&sample_frequency, &sample_count_average);
  cli_uart_printf("Current RSSI sample configuration: sample_frequency %u, sample_count_average %u\n", sample_frequency, sample_count_average);
  cli_radio_script_state_transition_event();
}

void cli_zw_reset(EmbeddedCli *cli, char *args, void *context)
{
  cli_radio_reset();
}

void cli_zw_script_entry(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t count = embeddedCliGetTokenCount(args);
  int8_t script_number = -1;

  if (count)
  {
    const char * arg;
    radio_cli_script_cmd_t script_state_request = 0;

    arg = embeddedCliGetToken(args, 1);
    if (!strcmp(arg, "start"))
    {
      script_state_request = SCRIPT_START;
    }
    else if (!strcmp(arg, "stop"))
    {
      script_state_request = SCRIPT_STOP;
    }
    else if (!strcmp(arg, "autoon"))
    {
      script_state_request = SCRIPT_AUTORUN_ON;
    }
    else if (!strcmp(arg, "autooff"))
    {
      script_state_request = SCRIPT_AUTORUN_OFF;
    }
    else if (!strcmp(arg, "list"))
    {
      script_state_request = SCRIPT_LIST;
    }
    else if (!strcmp(arg, "clear"))
    {
      script_state_request = SCRIPT_CLEAR;
    }
    else
    {
      cli_uart_printf(" ** argument must be start, stop, autoon, autooff, list or clear - entered %s\n", arg);
      return;
    }
    if (count == 2)
    {
      arg = embeddedCliGetToken(args, 2);
      int number = atoi(arg);
      if (validate_integer_range(number, 1, 5, 1))
      {
        script_number = number;
      }
      else
      {
        return;
      }
    }
    cli_radio_script(script_state_request, script_number);
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_script_run(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t count = embeddedCliGetTokenCount(args);
  int8_t script_number = -1;

  if (count)
  {
    const char * arg;

    arg = embeddedCliGetToken(args, 1);
    int number = atoi(arg);
    if (validate_integer_range(number, 0, 5, 1))
    {
      script_number = number;
    }
  }
  cli_radio_script(SCRIPT_RUN, script_number);
  cli_radio_script_state_transition_event();
}

void cli_zw_wait(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "time ms"))
  {
    const char * arg;
    arg = embeddedCliGetToken(args, 1);
    uint32_t waittime_ms = atol(arg);
    if (validate_integer_range(waittime_ms, 1, 86400000, 0))
    {
      cli_radio_wait(waittime_ms);
      return;
    }
    else
    {
      cli_uart_print("Wait time in ms must in the range (1..86400000)\n");
    }
  }
  cli_radio_script_state_transition_event();
}

void cli_zw_radio_timestamp(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "on/off"))
  {
    const char * arg;
    bool timestamp_enable = false;

    arg = embeddedCliGetToken(args, 1);
    if (!strcmp(arg, "on"))
    {
      timestamp_enable = true;
    }
    else if (!strcmp(arg, "off"))
    {
      timestamp_enable = false;
    }
    else
    {
      cli_uart_printf("** argument must be on or off - %s\n", arg);
      cli_uart_printf("\nTimestamp currently %s on Rx and Tx dump\n", cli_radio_timestamp_get() ? "Enabled" : "Disabled");
      cli_radio_script_state_transition_event();
      return;
    }

    cli_radio_timestamp_set(timestamp_enable);
  }
  else
  {
    cli_uart_printf("\nTimestamp currently %s on Rx and Tx dump\n", cli_radio_timestamp_get() ? "Enabled" : "Disabled");
  }
  cli_radio_script_state_transition_event();
}

static EmbeddedCli *cmd_cli;

void cli_command_execute(char * cmdStr, size_t length)
{
  if (NULL != cmdStr)
  {
    size_t i = 0;
    while (i < length)
    {
      embeddedCliReceiveChar(cmd_cli, cmdStr[i++]);
    }
    embeddedCliReceiveChar(cmd_cli, '\n');
  }
  embeddedCliProcess(cmd_cli);
}

/*
 * Add all defined commands to the CLI
 */
void cli_commands_init(EmbeddedCli *cli)
{
  uint16_t command = 0;
  cmd_cli = cli;
  while (cli_command_list[command].binding != NULL)
  {
    embeddedCliAddBinding(cli, cli_command_list[command]);
    command++;
  }
}

void cli_zw_dump(EmbeddedCli *cli, char *args, void *context)
{
  uint8_t count = embeddedCliGetTokenCount(args);

  if (check_argument_count(count, 1, "Sector"))
  {
    const char * tmp = embeddedCliGetToken(args, 1); // args are counted from 1 (not from 0)
    if (!strcmp(tmp, "ft"))
    {
      cli_uart_print("\nDumping FT sector (Security page 0)\n");
      cli_system_dumpft();
    }
    else if (!strcmp(tmp, "mp"))
    {
      cli_uart_print("\nDumping MP sector\n");
      cli_system_dumpmp();
    }
    else if (!strcmp(tmp, "uft"))
    {
      cli_uart_print("\nDumping user FT sector (Security Page 1, offset 512.)\n");
      cli_system_dumpuft();
    }

  }
}

typedef enum {
  read = 0,
  store,
  try
} XTAL_CMD;

static void xtal_internal_handler(char *args, XTAL_CMD xtal_cmd)
{
  uint16_t xtal_cal = 0xffff;

  if ((xtal_cmd == store) || (xtal_cmd == try))
  {
    uint8_t arg_count = embeddedCliGetTokenCount(args);

    if (arg_count == 3)
    {
      const char* string_u16 = embeddedCliGetToken(args, 3);
      xtal_cal = (uint16_t)strtoul(string_u16, NULL, 10);

      if (!validate_integer_range(xtal_cal, 0, 63, 2))
      {
        cli_uart_printf("** Invalid xtal value %u\n", xtal_cal);
        return;
      }
    }
    else
    {
      cli_uart_print("** xtal value missing\n");
      return;
    }
  }

  if (xtal_cmd == try)
  {
    cli_calibration_change_xtal(xtal_cal);
  }
  else if (xtal_cmd == store)
  {
    cli_calibration_store_xtal_sec_reg(xtal_cal);
  }
  else if (xtal_cmd == read)
  {
    cli_calibration_read_xtal_sec_reg(&xtal_cal);
    cli_uart_printf("\nxtal trim value %u (0x%02X)\n", xtal_cal, xtal_cal);
  }
  else
  {
    cli_uart_print("** Invalid parameter\n");
  }
}

static void xtal_token_handler(char *args, XTAL_CMD xtal_cmd)
{
  uint16_t xtal_cal = 0xffff;

  if ((xtal_cmd == store) || (xtal_cmd == try))
  {
    uint8_t arg_count = embeddedCliGetTokenCount(args);

    if (arg_count == 2)
    {
      const char* string_u16 = embeddedCliGetToken(args, 2);
      xtal_cal = (uint16_t)strtoul(string_u16, NULL, 10);

      if (!validate_integer_range(xtal_cal, 0, 63, 2))
      {
        cli_uart_printf("** Invalid xtal value %u\n", xtal_cal);
        return;
      }
    }
    else
    {
      cli_uart_print("** xtal value missing\n");
      return;
    }
  }

  if (xtal_cmd == try)
  {
    cli_calibration_change_xtal(xtal_cal);
  }
  else if (xtal_cmd == store)
  {
    cli_calibration_store_xtal(xtal_cal);
  }
  else if (xtal_cmd == read)
  {
    cli_calibration_read_xtal(&xtal_cal);
    cli_uart_printf("\nxtal trim value %u (0x%02X)\n", xtal_cal, xtal_cal);
  }
  else
  {
    cli_uart_print("** Invalid parameter\n");
  }
}

void cli_zw_cal_xtal(EmbeddedCli *cli, char *args, void *context)
{
  XTAL_CMD xtal_cmd = read;
  bool internal = false;

  uint8_t arg_count = embeddedCliGetTokenCount(args);

  for (uint8_t i = 0; i < arg_count; i++)
  {
    const char * arg = embeddedCliGetToken(args, i + 1);
    if (!strcmp(arg, "store"))
    {
      xtal_cmd = store;
    }
    else if (!strcmp(arg, "read"))
    {
      xtal_cmd = read;
    }
    else if (!strcmp(arg, "try"))
    {
      xtal_cmd = try;
    }
    else if (!strcmp(arg, "internal"))
    {
      internal = true;
    }
  }

  if (internal)
  {
    xtal_internal_handler(args, xtal_cmd);
  }
  else
  {
    xtal_token_handler(args, xtal_cmd);
  }
  cli_radio_script_state_transition_event();
}
