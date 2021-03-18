/* *****************************************************************************
** File Name:   main.c
**
** Description: This is the source code for the PSoC 6 MCU: PASCO2 Free RTOS
**              example for ModusToolbox.
**
** Related Document: See README.md
**
**
** ===========================================================================

** Copyright (C) 2021 Infineon Technologies AG. All rights reserved.
** ===========================================================================
**
** ===========================================================================
** Infineon Technologies AG (INFINEON) is supplying this file for use
** exclusively with Infineon's sensor products. This file can be freely
** distributed within development tools and software supporting such
** products.
**
** THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
** OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
** INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON
** WHATSOEVER.
** ===========================================================================
*/

/* Header file includes */
#include "cy_pdl.h"
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"

/* Header file for local task */
#include "pasco2_task.h"
#include "pasco2_terminal_ui_task.h"

/*******************************************************************************
 * Global Variables
 *******************************************************************************/

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 * This is the main function for CM4 CPU. It creates and starts a freeRTOS task
 * to handle PAS CO2 sensor configuration and data processing.
 *
 * Parameters:
 *  none
 *
 * Return:
 *  int
 *
 *******************************************************************************/
int main(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port. */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("=====================================================\r\n"
           "Connected Sensor Kit: PAS CO2 Application on FreeRTOS\r\n"
           "=====================================================\r\n");

    printf("For more PSoC 6 MCU projects, "
           "visit our code examples repositories:\r\n\r\n");

    printf("https://github.com/cypresssemiconductorco/\r\n\r\n"
           "Code-Examples-for-ModusToolbox-Software\r\n\r\n");

    /* Create PAS CO2 task */
    cy_thread_t ifx_pasco2_task;
    result = cy_rtos_create_thread(&ifx_pasco2_task,
                                   pasco2_task,
                                   PASCO2_TASK_NAME,
                                   NULL,
                                   PASCO2_TASK_STACK_SIZE,
                                   PASCO2_TASK_PRIORITY,
                                   (cy_thread_arg_t)NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Create PAS CO2 terminal UI task */
    cy_thread_t ifx_pasco2_terminal_task;
    result = cy_rtos_create_thread(&ifx_pasco2_terminal_task,
                                   pasco2_terminal_ui_task,
                                   PASCO2_TERMINAL_UI_TASK_NAME,
                                   NULL,
                                   PASCO2_TERMINAL_UI_TASK_STACK_SIZE,
                                   PASCO2_TERMINAL_UI_TASK_PRIORITY,
                                   (cy_thread_arg_t)NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Start the FreeRTOS scheduler. */
    vTaskStartScheduler();

    /* Should never get here. */
    CY_ASSERT(0);
}
/* [] END OF FILE */
