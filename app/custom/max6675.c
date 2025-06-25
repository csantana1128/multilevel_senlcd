#define DEBUGPRINT
#include "DebugPrint.h"
#include "sysctrl.h"
#include "tr_hal_spi.h"
#include "T32CZ20_spi.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "max6675.h"

static bool max6675_initialized = false;

max6675_status_t max6675_init(void)
{
    DPRINT("MAX6675: Init start\n");

    tr_hal_spi_settings_t spi_settings = SPI_CONFIG_CONTROLLER_NORMAL_MODE;

    tr_hal_status_t status = tr_hal_spi_init(MAX6675_SPI_ID, &spi_settings);
    if (status != TR_HAL_SUCCESS)
    {
        DPRINT("MAX6675: SPI init failed\n");
        return MAX6675_ERROR;
    }

    max6675_initialized = true;
    DPRINT("MAX6675: Init OK\n");
    return MAX6675_OK;
}

// Read temperature - returns temperature * 100 (so 25.75°C returns 2575)
max6675_status_t max6675_read_temperature(int *temp_x100)
{
    if (!max6675_initialized)
    {
        DPRINT("MAX6675: Not initialized\n");
        return MAX6675_ERROR;
    }

    if (temp_x100 == NULL)
    {
        return MAX6675_ERROR;
    }

    uint8_t rx_buffer[2] = {0};
    uint8_t dummy_tx[2] = {0xFF, 0xFF};
    uint16_t num_received = 0;

    DPRINT("MAX6675: Reading...\n");

    // Send dummy data to read
    tr_hal_status_t status = tr_hal_spi_raw_tx_buffer(
        MAX6675_SPI_ID,
        MAX6675_CS_INDEX,
        (char *)dummy_tx,
        2,
        true);

    if (status != TR_HAL_SUCCESS)
    {
        DPRINT("MAX6675: TX failed\n");
        return MAX6675_ERROR;
    }

    Delay_ms(1);

    // Read response
    status = tr_hal_spi_raw_rx_available_bytes(
        MAX6675_SPI_ID,
        (char *)rx_buffer,
        2,
        &num_received);

    if (status != TR_HAL_STATUS_DONE || num_received != 2)
    {
        DPRINT("MAX6675: RX failed\n");
        return MAX6675_ERROR;
    }

    // Combine bytes (MSB first)
    uint16_t raw_data = ((uint16_t)rx_buffer[0] << 8) | rx_buffer[1];

    // Check for open circuit (bit 2)
    if (raw_data & 0x0004)
    {
        DPRINT("MAX6675: Open circuit\n");
        return MAX6675_OPEN_CIRCUIT;
    }

    // Extract temperature data (bits 14-3, shift right by 3)
    uint16_t temp_data = (raw_data >> 3) & 0x0FFF;

    // Convert to temperature * 100 (each LSB = 0.25°C = 25/100°C)
    *temp_x100 = (int)temp_data * 25;

    DPRINT("MAX6675: Raw=");
    DPRINTF("%d", raw_data);
    DPRINT(" Temp=");
    DPRINTF("%d", temp_data);
    DPRINT(" Result=");
    DPRINTF("%d", *temp_x100);
    DPRINT("\n");

    return MAX6675_OK;
}

void max6675_example_usage(void)
{
    int temperature_x100;
    max6675_status_t status = max6675_read_temperature(&temperature_x100);

    if (status == MAX6675_OK)
    {

        int whole = temperature_x100 / 100;
        int fraction = temperature_x100 % 100;

        DPRINT("Temperature: ");
        DPRINTF("%d", whole);
        DPRINT(".");
        if (fraction < 10)
        {
            DPRINT("0");
        }
        DPRINTF("%d", fraction);
        DPRINT(" C\n\n");
    }
    else if (status == MAX6675_OPEN_CIRCUIT)
    {
        DPRINT("Thermocouple disconnected\n\n");
    }
    else
    {
        DPRINT("Read error\n\n");
    }
}