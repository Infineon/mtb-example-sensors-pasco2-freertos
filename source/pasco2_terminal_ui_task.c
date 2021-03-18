/*****************************************************************************
** File name: pasco2_terminal_ui_task.c
**
** Description: This file implements a terminal UI to configure parameters
** for PASCO2 application.
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

/* Header file from system */
#include <ctype.h>
#include <stdlib.h>

/* Header file includes */
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"

/* Header file for local task */
#include "pasco2_task.h"
#include "pasco2_terminal_ui_task.h"

/*******************************************************************************
 * Constants
 *******************************************************************************/
#define IFX_PASCO2_VALUE_MAXLENGTH 256

/*******************************************************************************
 * Function Name: terminal_ui_menu
 ********************************************************************************
 * Summary:
 *   This function prints the available parameters configurable for CO2 sensor.
 *
 * Parameters:
 *   none
 *
 * Return:
 *   none
 *******************************************************************************/
static void terminal_ui_menu(void)
{
    // Print main menu
    pasco2_terminal_mutex_get(0);
    pasco2_display_ppm(false);
    printf("Select a setting to configure\r\n");
    printf("'p': Set the measurement period\r\n");
    printf("'i': Print additional diagnostic information if available\r\n");
    printf("\r\n");
    pasco2_display_ppm(true);
    pasco2_terminal_mutex_release();
}

/*******************************************************************************
 * Function Name: terminal_ui_info
 ********************************************************************************
 * Summary:
 *   This function prints character using which a user can see all available
 *   parameters configurable for CO2 sensor.
 *
 * Parameters:
 *   none
 *
 * Return:
 *   none
 *******************************************************************************/
static void terminal_ui_info(void)
{
    printf("Press '?' to list all CO2 sensor settings\r\n");
}

/*******************************************************************************
 * Function Name: terminal_ui_readline
 ********************************************************************************
 * Summary:
 *   This function reads values entered by a user and prints it.
 *
 * Parameters:
 *   uart_ptr: UART object
 *   line: value to be read
 *   maxlength: maximum number of characters to be read
 *
 * Return:
 *   none
 *******************************************************************************/
static void terminal_ui_readline(void *uart_ptr, char *line, int maxlength)
{
    pasco2_display_ppm(false);
    if (maxlength <= 0)
    {
        return;
    }
    int i = 0;
    uint8_t rx_value = 0;
    /* Receive character until enter has been pressed */
    while ((rx_value != '\r') && (--maxlength > 0))
    {
        cyhal_uart_getc(uart_ptr, &rx_value, 0);
        cyhal_uart_putc(uart_ptr, rx_value);
        if (isspace(rx_value))
        {
            continue;
        }
        line[i++] = rx_value;
    }
    cyhal_uart_putc(uart_ptr, '\n');
    line[i] = '\0';
    pasco2_display_ppm(true);
}

/*******************************************************************************
 * Function Name: radar_presence_terminal_ui
 ********************************************************************************
 * Summary:
 *   Continuously checks if a key has been pressed to configure CO2 sensor
 *   parameter. Displays a status message according to the user
 *   input/selection.
 *
 * Parameters:
 *   arg: thread
 *
 * Return:
 *   none
 *******************************************************************************/
void pasco2_terminal_ui_task(cy_thread_arg_t arg)
{
    terminal_ui_menu();
    char value[IFX_PASCO2_VALUE_MAXLENGTH];
    uint8_t rx_value = 0;

    /* Check if a key was pressed */
    while (cyhal_uart_getc(&cy_retarget_io_uart_obj, &rx_value, 0) == CY_RSLT_SUCCESS)
    {
        pasco2_terminal_mutex_get(0);
        switch ((char)rx_value)
        {
            // menu
            case '?':
                terminal_ui_menu();
                break;
            // measurement period
            case 'p':
            {
                printf("Enter the measurement period [10-4095]s\r\n");
                terminal_ui_readline(&cy_retarget_io_uart_obj, value, IFX_PASCO2_VALUE_MAXLENGTH);
                mtb_pasco2_config_t pas_co2_config = {
                    .measurement_period = atoi(value),
                };
                cy_rslt_t result = mtb_pasco2_set_config(&mtb_pasco2_context, &pas_co2_config);
                if (result == CY_RSLT_SUCCESS)
                {
                    printf("CO2 measurement period set to: %d\r\n\r\n", pas_co2_config.measurement_period);
                    continue;
                }
                if (CY_RSLT_GET_CODE(result) == MTB_PASCO2_CONFIGURATION_ERROR)
                {
                    printf("CO2 sensor measurement period configuration error, Valid range is [10-4095]\r\n\r\n");
                }
            }
            break;
            case 'i':
                printf("Display additional diagnostic information [y/n]?\r\n");
                terminal_ui_readline(&cy_retarget_io_uart_obj, value, IFX_PASCO2_VALUE_MAXLENGTH);
                if (strlen(value) != 1 || (value[0] != 'y' && value[0] != 'n'))
                {
                    printf("Input error, valid values are [y/n]\r\n\r\n");
                    continue;
                }
                pasco2_enable_internal_logging(value[0] == 'y');
                break;
            default:
                terminal_ui_info();
        }
        pasco2_terminal_mutex_release();
        rx_value = 0;
    }
    printf("Exiting terminal ui\r\n");
    // exit current thread (suspend)
    (void)cy_rtos_exit_thread();
}
