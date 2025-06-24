/// ***************************************************************************
/// SPDX-License-Identifier: LicenseRef-TridentMSLA
/// SPDX-FileCopyrightText: 2025 Trident IoT, LLC <https://www.tridentiot.com>
/// ***************************************************************************

/***********************************************************************************************************************
 * @file     tr_uart_blocking.c
 * @version
 * @brief    blocking uart driver file
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
*    INCLUDES
**********************************************************************************************************************/

#ifdef TR_PLATFORM_T32CZ20
#include "uart_drv.h"
#include "cm33.h"
#endif
#ifdef TR_PLATFORM_ARM
#include <stdint.h>
#include "uart_drv.h"
#include "cm3_mcu.h"

#include "project_config.h"
#endif

#define  Enter_Critical_Section()       enter_critical_section()
#define  Leave_Critical_Section()       leave_critical_section()
/***********************************************************************************************************************
 *    CONSTANTS AND DEFINES
 **********************************************************************************************************************/
#define MAX_NUMBER_OF_UART        3

#define UART0_BASE_CLK            16


// UART flags
#define UART_FLAG_INITIALIZED        (1U << 0)
#define UART_FLAG_POWERED            (1U << 1)
#define UART_FLAG_CONFIGURED         (UART_FLAG_POWERED | UART_FLAG_INITIALIZED)

/*private structure for uart driver used.*/
/***********************************************************************************************************************
*    TYPEDEFS
 **********************************************************************************************************************/
// UART Transfer Information (Run-Time)
typedef struct _UART_TRANSFER_INFO
{
    uint8_t                *rx_buf;        // Pointer to in data buffer
    uint8_t                *tx_buf;        // Pointer to out data buffer
    uint32_t                rx_num;        // Total number of data to be received
    uint32_t                tx_num;        // Total number of data to be send
    uint32_t                rx_cnt;        // Number of data received
    uint32_t                tx_cnt;        // Number of data sent
} uart_transfer_info_t;

/* Remark: Please don't change uart_rx_status_t structure bit setting
 *
 */
typedef struct
{

    uint8_t resv1       : 3;               // Reserver bits.
    uint8_t rx_overflow : 1;               // Receive data overflow detected (cleared on start of next receive operation)
    uint8_t rx_break    : 1;               // Break detected on receive (cleared on start of next receive operation)
    uint8_t rx_framing_error : 1;          // Framing error detected on receive (cleared on start of next receive operation)
    uint8_t rx_parity_error  : 1;          // Parity error detected on receive (cleared on start of next receive operation)
    uint8_t resv2       : 1;               // Reserver bits.

} uart_rx_status_t;


typedef union
{
    uart_rx_status_t   uart_rx_status;
    uint32_t           init_byte;
} status_byte;

// UART Information (Run-Time)
typedef struct _UART_INFO
{

    uart_event_handler_t     cb_event;      // Event callback

    void                     *p_context;

    uart_transfer_info_t     xfer;          // Transfer information

    status_byte              status;        // UART　status flags

    /* "send_active" and "rx_busy" is single byte to avoid
       "race condition" issue */
    uint8_t                  send_active;   // TX Send active flag
    uint8_t                  rx_busy;       // Receiver busy flag

    uint8_t                  flags;         // UART driver flags
} uart_info_t;


typedef struct
{

    UART_T          *uart;       /*based hardware address*/

    uart_info_t     *info;       // Run-Time Information

    /*IRQ number*/
    IRQn_Type       irq_num;     // UART IRQ Number

    uint8_t         uart_id;     // uart id, 0 for uart0, 1 for uart1

    /*DMA information*/
    uint8_t         dma_tx;        // DMA channel for uart tx, if 0 doesn't use DMA for tx
    uint8_t         dma_rx;        // DMA channel for uart rx, if 0 doesn't use DMA for rx

    uint8_t         user_def_recv;    // user has itself recv function handler..

} uart_handle_t;


static uart_info_t uart0_info = {0};
static uart_info_t uart1_info = {0};
static uart_info_t uart2_info = {0};


#ifdef TR_PLATFORM_ARM

static const uart_handle_t  m_uart_handle[3] =
{
    {   /*UART0 instatnce*/
        UART0,
        &uart0_info,
        Uart0_IRQn,
        0,

#if (MODULE_ENABLE(UART0_USER_HANDLE_RECV_RX))
        1,        // 1 for enable RX DMA, 0 for PIO DMA...
        0,        // 0 for PIO RX.
        1         // 1 for using user RECV Rx Handler.

#else
        0,        // 1 for enable TX DMA
        1,        // 1 for enable RX DMA
        0         // Enable RX DMA can not call user user defined RECV handler.
#endif

    },
    {   /*UART1 instatnce*/
        UART1,
        &uart1_info,
        Uart1_IRQn,
        1,
        1,
        1,
        0
    },
    {   /*UART2 instatnce*/
        UART2,
        &uart2_info,
        Uart2_IRQn,
        2,
        1,                   // DMA channel for uart tx, if 0 doesn't use DMA for tx
        1,                   // DMA channel for uart tx, if 0 doesn't use DMA for tx
        0
    }
};

#else /*TR_PLATFORM_ARM*/

#define USE_DMA     1
static const uart_handle_t  m_uart_handle[3] =
{
    {   /*UART0 instatnce*/
        UART0,
        &uart0_info,
        Uart0_IRQn,
        0,

#ifdef  UART0_SECURE_EN
#ifdef USE_DMA
        0,
        1,
#else
        0,
        0,
#endif
#else
        0,   /*For non-secure, it can not use DMA address in secure*/
        0,
#endif
        0
    },
    {   /*UART1 instatnce*/
        UART1,
        &uart1_info,
        Uart1_IRQn,
        1,
#if (USE_DMA==1) & (TX_DMA_RX_PIO==0)
        //DMA
        0,
        1,
        0
#elif TX_DMA_RX_PIO==1      //for Uart Receive data wakeup exampels
        //PIO
        0,
        0,
        1
#endif

    },
    {   /*UART2 instatnce*/
        UART2,
        &uart2_info,
        Uart2_IRQn,
        2,

#if USE_DMA
        //DMA
        0,
        1,
#else
        //PIO
        0,
        0,
#endif

        0
    }
};
#endif /*TR_PLATFORM_ARM*/
/***********************************************************************************************************************
 *    GLOBAL FUNCTIONS
 **********************************************************************************************************************/
/**
* @brief uart initinal function
* @details config uart buadrate,stop bit, parity bit,interrupt, fifo size,dma mode
* @param[in] uart_id
* @param[in] p_config
* @param[in] uart_event_handler_t uart interrupt call back function
* @return None
*/
uint32_t tr_uart_init_blocking(uint32_t uart_id,
                        uart_config_t const   *p_config,
                        uart_event_handler_t  event_handler)
{
    const uart_handle_t  *uart_dev;
    uart_info_t          *uart_info;
    UART_T               *uart;

    uint32_t             cval, ier_value, fcr_value;

    if ((uart_id >= MAX_NUMBER_OF_UART) || (p_config == NULL))
    {
        return STATUS_INVALID_PARAM;
    }

    uart_info = m_uart_handle[uart_id].info;

    if (uart_info->flags & UART_FLAG_INITIALIZED)
    {
        // Driver is already initialized
        // Please uninit the uart then init if APP wants to resetting
        return STATUS_INVALID_REQUEST;
    }

    uart_dev = &m_uart_handle[uart_id];
    uart = uart_dev->uart;

    // Initialize UART Run-time Resources

    uart_info->cb_event = event_handler;
    uart_info->p_context = p_config->p_context;

    /*Set rx_busy, rx_overflow, rx_break, rx_framing_error,
      and rx_parity_error as 0．　
      It also set tx_send_active as 0 */
    uart_info->status.init_byte = 0;
    uart_info->send_active = 0;
    uart_info->rx_busy = 0;

    /*set uart config*/

    // Clear UART IRQ
    NVIC_ClearPendingIRQ(uart_dev->irq_num);
#ifdef TR_PLATFORM_ARM
// Enable UART peripheral clock
    enable_perclk((UART0_BASE_CLK + uart_dev->uart_id));
#endif
#ifdef TR_PLATFORM_T32CZ20
    /*RT584: Disable uart enable first */
    uart->UART_EN = UART_DISABLE;

    /*clear FIFO, REMAKR: FCR is write-only*/
    uart->FCR = FCR_DEFVAL;     /*reset FIFO*/

    // Disable interrupts
    uart->IER = 0;

    /*RT584 set baudrate has itself setting register.*/

    /*set baudrate*/

    uart->DLX = p_config->baudrate & 0xFFFF ;
    uart->FDL = (p_config->baudrate >> UART_BR_FRCT_SHIFT) & 0xFF;

    if (p_config->baudrate & ENABLE_LSM)
    {
        uart->LSM = 1;
    }
    else
    {
        uart->LSM = 0;
    }
    /*bits mode only use two bits.*/
    cval = p_config->databits & 0x03;
#endif
#ifdef TR_PLATFORM_ARM
/*clear FIFO, REMAKR: FCR is write-only*/
    uart->FCR = 0;
    uart->FCR = FCR_DEFVAL;     /*reset FIFO*/

    // Disable interrupts
    uart->IER = 0;

    /*bits mode only use two bits.*/
    cval = p_config->databits & 0x03;

    /*set baudrate*/
    uart->LCR = (LCR_DLAB | cval);
    /*set baudrate*/
    uart->DLL = p_config->baudrate & 0xFF ;
    uart->DLM = p_config->baudrate >> 8;
    uart->LCR = (cval);

#endif


    /*set stop bits*/
    if (p_config->stopbit == UART_STOPBIT_TWO)
    {
        cval |= LCR_STOP;
    }

    /*set parity*/
    if (p_config->parity & PARENB)
    {
        cval |= LCR_PARITY;
    }
    if (!(p_config->parity & PARODD))
    {
        cval |= LCR_EPAR;
    }
    if (p_config->parity & CMSPAR)
    {
        cval |= LCR_SPAR;
    }

    uart->LCR = cval;

    uart->MCR = 0;         /*Initial default modem control register.*/
#ifdef TR_PLATFORM_ARM
/*init native DMA architecture setting*/
    uart->xDMA_IER = 0;         /*disable xDMA interrupt*/
#endif

    /*init native DMA architecture setting*/

    uart->xDMA_RX_ENABLE = xDMA_Stop;
    uart->xDMA_TX_ENABLE = xDMA_Stop;

    uart->xDMA_TX_LEN = 0;
    uart->xDMA_RX_LEN = 0;
#ifdef TR_PLATFORM_ARM
    uart->xDMA_INT_STATUS = xDMA_ISR_RX | xDMA_ISR_TX;      /*write 1 to clear*/
    uart->xDMA_IER = xDMA_IER_TX | xDMA_IER_RX ;    /*Enable DMA interrupt here*/
#if MODULE_ENABLE(SUPPORT_UART1_FLOWCNTL)
    if ((uart_id == 1) && (p_config->hwfc))
    {
        /*only uart1 support HW flow control pin*/
        uart->MCR = MCR_ENABLE_CTS;     /*Enable hardware CTS to block transmit data*/
        /*when CTS change, and rx line status error, interrupt event generated */
        uart->IER = UART_IER_RLSI | UART_IER_MSI;
    }
    else
#endif
        /*rx line status error, interrupt event generated*/
        uart->IER = UART_IER_RLSI;

    if (uart_dev->user_def_recv)
    {
        uart->FCR = (FCR_FIFO_EN | FCR_TRIGGER_1);      /*FCR Trigger 1, so RX interrrupt can happen ASAP. */
        uart->IER |= UART_IER_RDI;                            /*default enable RX interrupt for user recv handler mode*/
    }
    else
    {
        uart->FCR = (FCR_FIFO_EN | FCR_TRIGGER_4);
    }

#endif
#ifdef TR_PLATFORM_T32CZ20
    /*clear interrupt flag if exists... write 1 clear*/
    uart->ISR = (ISR_RDA_INTR | ISR_THRE_INTR | ISR_RX_TIMEOUT | ISR_DMA_RX_INTR | ISR_DMA_TX_INTR);

    /* RT584 clear all LSR error if exists*/
    uart->LSR = UART_LSR_BRK_ERROR_BITS;

    ier_value = UART_IER_RLSI;      /* should we set this status error */

    fcr_value = 0;

    if ((m_uart_handle[uart_id].dma_rx) && (uart_dev->user_def_recv != 1))
    {
        /* enable dma for rx*/
        ier_value |= UART_IER_DMA_RX_INTR;
        fcr_value = FCR_DMA_SELECT;
    }

    if ((uart_id == 1) && (p_config->hwfc))
    {
        /*only uart1 support HW flow control pin*/
        uart->MCR = MCR_HW_AUTOFLOW_EN;    /*Enable hardware CTS to block transmit data*/
        fcr_value |= FCR_RTS_LEVEL_14;     /*14 bytes will set pin RTSn=1*/
        /*when CTS change interrupt event generated */
        ier_value |= UART_IER_MSI ;        /*modem status change interrrupt*/
    }

    if (uart_dev->user_def_recv)
    {
        fcr_value |= FCR_TRIGGER_1;     /*FCR Trigger 1, so RX interrrupt can happen ASAP. */
    }
    else
    {
        fcr_value |=  FCR_TRIGGER_8;
    }

    uart->FCR = fcr_value;
    uart->IER = ier_value;
#endif /*TR_PLATFORM_T32CZ20 */
    NVIC_SetPriority(uart_dev->irq_num, p_config->interrupt_priority);
    NVIC_EnableIRQ(uart_dev->irq_num);

    /*this function auto turn-on the uart device power.*/
    uart_info->flags = (UART_FLAG_POWERED | UART_FLAG_INITIALIZED);
#ifdef TR_PLATFORM_T32CZ20
    /*RT584: uart enable after all setting is ready */
    uart->UART_EN = UART_ENABLE;
#endif
    return STATUS_SUCCESS;
}

/**
* @brief uart uninitinal function
* @details uninitinal UART
* @param[in] uart_id UART idiention
* @return STATUS_SUCCESS
*/
static uint32_t _uart_power(uint32_t uart_id, uint32_t enable)
{
    const uart_handle_t  *uart_dev;
    uart_info_t          *uart_info;
    UART_T               *uart;

    if (uart_id >= MAX_NUMBER_OF_UART)
    {
        return STATUS_INVALID_PARAM;
    }

    uart_dev = &m_uart_handle[uart_id];
    uart_info = m_uart_handle[uart_id].info;

    /*
     * FIXME: this function is used in CMSIS uart architecture driver... but
     * in current desgin uart_power( id, UART_POWER_ON) has been replace
     * by uart_init(...), so system can forget call this function for UART_POWER_ON
     * call uart_init(...) directly.
     */

    uart = uart_dev->uart;

    if (enable)
    {
        /*power-on*/
        if (uart_info->flags & UART_FLAG_POWERED)
        {
            /*already power-on*/
            return STATUS_SUCCESS;
        }
#ifdef TR_PLATFORM_ARM
        // Enable UART peripheral clock
        enable_perclk((UART0_BASE_CLK + uart_dev->uart_id));
        /*clear FIFO, REMAKR: FCR is write-only*/
        uart->FCR = 0;
#endif
        /*clear FIFO, REMAKR: FCR is write-only*/
        uart->FCR |= FCR_DEFVAL;     /*reset FIFO*/

        /*Remark: we don't change IER setting here if initial*/

        // Clear driver variables
        uart_info->status.init_byte = 0;
        uart_info->send_active = 0;
        uart_info->rx_busy = 0;

        NVIC_ClearPendingIRQ(uart_dev->irq_num);

        if (uart_info->flags & UART_FLAG_INITIALIZED)
        {
            // Clear pending UART interrupts in NVIC
            NVIC_EnableIRQ(uart_dev->irq_num);
        }

        uart_info->flags |= UART_FLAG_POWERED;


    }
    else
    {
#ifdef TR_PLATFORM_ARM
        uart->xDMA_IER = 0;         /*disable xDMA interrupt*/
        uart->xDMA_RX_ENABLE = xDMA_Stop;
        uart->xDMA_TX_ENABLE = xDMA_Stop;
        uart->xDMA_INT_STATUS = xDMA_ISR_RX | xDMA_ISR_TX;      /*write 1 to clear*/

        /*reset uart FIFO*/
        uart->FCR =  0;
        uart->FCR =  FCR_CLEAR_XMIT | FCR_CLEAR_RCVR;

        // Disable UART peripheral clock
        disable_perclk((UART0_BASE_CLK + uart_dev->uart_id));

#endif
#ifdef TR_PLATFORM_T32CZ20
        /*power-off*/
        uart->UART_EN = UART_DISABLE;

        // Disable UART IRQ,
        NVIC_DisableIRQ(uart_dev->irq_num);

        uart->xDMA_RX_ENABLE = xDMA_Stop;
        uart->xDMA_TX_ENABLE = xDMA_Stop;

        /*reset FCR receive FIFO and transmit FIFO*/
        uart->FCR |=  FCR_CLEAR_XMIT | FCR_CLEAR_RCVR;
#endif
        // Clear driver variables
        uart_info->status.init_byte = 0;
        uart_info->send_active = 0;
        uart_info->rx_busy = 0;

        uart_info->flags &= ~UART_FLAG_POWERED;

        // Clear pending UART interrupts in NVIC
        NVIC_ClearPendingIRQ(uart_dev->irq_num);
    }

    return STATUS_SUCCESS;
}

/**
* @brief uart uninitinal function
* @details uninitinal UART
* @param[in] uart_id UART idiention
* @return STATUS_SUCCESS
*/
uint32_t tr_uart_uninit_blocking(uint32_t uart_id)
{
    uart_info_t          *uart_info;
#ifdef TR_PLATFORM_T32CZ20
    UART_T               *uart;
#endif
    if (uart_id >= MAX_NUMBER_OF_UART)
    {
        return STATUS_INVALID_PARAM;
    }
#ifdef TR_PLATFORM_T32CZ20
    uart = m_uart_handle[uart_id].uart;
    uart->IER = 0;    /*disable all interrupt*/
#endif
    /*clear all interrupt if there pending.*/

    /*auto turn off the power*/
    _uart_power(uart_id, UART_POWER_OFF);

    uart_info = m_uart_handle[uart_id].info;

    // Reset UART status flags
    uart_info->flags = 0;

    return STATUS_SUCCESS;
}

/**
* @brief uart Transfer function (DMA Architecture)
* @details uninitinal UART
* @param[in] uart_id UART idiention
* @param[in] p_data data buffer point
* @param[in] length data length
* @return STATUS_SUCCESS
*/
uint32_t tr_uart_tx_blocking(uint32_t uart_id,
                 uint8_t const *p_data,
                 uint32_t length)
{

  const uart_handle_t *uart_dev;
  uart_info_t *uart_info;
  UART_T *uart;
  int32_t val;

  if (uart_id >= MAX_NUMBER_OF_UART)
  {
    return STATUS_INVALID_PARAM;
  }

  if ((p_data == NULL) || (length == 0) || (length > 65535))
  {
    // Invalid parameters
    return STATUS_INVALID_PARAM;
  }

  uart_dev = &m_uart_handle[uart_id];
  uart_info = uart_dev->info;
  uart = uart_dev->uart;

  if ((uart_info->flags & UART_FLAG_CONFIGURED) != UART_FLAG_CONFIGURED)
  {
    // UART is not configured (mode not selected)
    return STATUS_NO_INIT;
  }

  if (uart_info->send_active != 0)
  {
    // Send is not completed yet
    return STATUS_EBUSY;
  }

  // Set Send active flag
  uart_info->send_active = 1;

  // Save transmit buffer info
  uart_info->xfer.tx_buf = (uint8_t *)p_data;
  uart_info->xfer.tx_num = length;
  uart_info->xfer.tx_cnt = 0;

  uart = uart_dev->uart;
  Enter_Critical_Section();
  if (uart->LSR & UART_LSR_THRE)
  {
    while (uart_info->xfer.tx_num != uart_info->xfer.tx_cnt)
    {
      val = 16;
      while ((val--) && (uart_info->xfer.tx_cnt != uart_info->xfer.tx_num))
      {
        uart->THR = uart_info->xfer.tx_buf[uart_info->xfer.tx_cnt++];
      }
      while (1)
      {
        if ( uart->LSR & UART_LSR_THRE)
        {
          break;
        }
      }
    }
    while ((uart->LSR & UART_LSR_TEMT) == 0) {};
  }
  // Clear TX busy flag
  uart_info->send_active = 0;
  Leave_Critical_Section();
  return STATUS_SUCCESS;
}

bool tr_uart_trx_complete(uint32_t uart_id)
{
    const uart_handle_t  *uart_dev;
    UART_T               *uart;
    bool trx_done = FALSE;

    uart_dev = &m_uart_handle[uart_id];
    uart = uart_dev->uart;

    if (((m_uart_handle[uart_id].info->send_active) == 0) && ((uart->LSR & UART_LSR_TEMT) != 0))
    {
        trx_done = TRUE;
    }

    return trx_done;
}


