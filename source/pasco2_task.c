/*****************************************************************************
** File name: pasco2_task.c
**
** Description: This file uses PASCO2 library APIs to demonstrate the use of
** CO2 sensor.
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
#include <stdio.h>

/* Header file includes */
#include "cybsp.h"
#include "cyhal.h"

/* Header file for local task */
#include "pasco2_task.h"

/* Output pin for sensor PSEL line */
#define MTB_PASCO2_PSEL (P5_3)
/* Pin state to enable I2C channel of sensor */
#define MTB_PASCO2_PSEL_I2C_ENABLE (0U)
/* Output pin for PAS CO2 Wing Board power switch */
#define MTB_PASCO2_POWER_SWITCH (P10_5)
/* Pin state to enable power to sensor on PAS CO2 Wing Board*/
#define MTB_PASCO2_POWER_ON (1U)

/* Output pin for PAS CO2 Wing Board LED OK */
#define MTB_PASCO2_LED_OK (P9_0)
/* Output pin for PAS CO2 Wing Board LED WARNING  */
#define MTB_PASCO2_LED_WARNING (P9_1)

/* Pin state for PAS CO2 Wing Board LED off. */
#define MTB_PASCO_LED_STATE_OFF (0U)
/* Pin state for PAS CO2 Wing Board LED on. */
#define MTB_PASCO_LED_STATE_ON (1U)

/* I2C bus frequency */
#define I2C_MASTER_FREQUENCY (100000U)
/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/* CO2 driver context */
mtb_pasco2_context_t mtb_pasco2_context;

static volatile bool log_internal = false;
static volatile bool display_ppm = true;

static cy_mutex_t terminal_print_mutex;

#define conditional_log(...)                                                                                           \
    if (log_internal)                                                                                                  \
    {                                                                                                                  \
        printf(__VA_ARGS__);                                                                                           \
    }

/*******************************************************************************
 * Function Name: pasco2_terminal_mutex_get
 *******************************************************************************
 * Summary:
 *   Get mutex
 *
 * Parameters:
 *   timeout_ms: Maximum number of milliseconds to wait while attempting to
 *   get mutex
 *
 * Return:
 *   Status of mutex request
 *******************************************************************************/
cy_rslt_t pasco2_terminal_mutex_get(cy_time_t timeout_ms)
{
    return (cy_rtos_get_mutex(&terminal_print_mutex, timeout_ms));
}

/*******************************************************************************
 * Function Name: pasco2_terminal_mutex_release
 *******************************************************************************
 * Summary:
 *  Set mutex
 *
 * Parameters:
 *   none
 *
 * Return:
 *   Status of mutex request
 *******************************************************************************/
cy_rslt_t pasco2_terminal_mutex_release(void)
{
    return (cy_rtos_set_mutex(&terminal_print_mutex));
}

/*******************************************************************************
 * Function Name: pasco2_enable_internal_logging
 *******************************************************************************
 * Summary:
 *  enable/disable debug info for CO2 sensor
 *
 * Parameters:
 *   enable_logging: value for logging flag
 *
 *
 * Return:
 *   Status of mutex request
 *******************************************************************************/
void pasco2_enable_internal_logging(bool enable_logging)
{
    if (enable_logging)
    {
        printf("Enable additional diagnostic logging\r\n\r\n");
    }
    else
    {
        printf("Disable additional diagnostic logging\r\n\r\n");
    }
    log_internal = enable_logging;
}

/*******************************************************************************
 * Function Name: pasco2_display_ppm
 *******************************************************************************
 * Summary:
 *   Enables serial printing of CO2 PPM value
 *
 * Parameters:
 *   enable_output: enables printing of CO2 value
 *
 * Return:
 *   system time in ms
 *******************************************************************************/
void pasco2_display_ppm(bool enable_output)
{
    display_ppm = enable_output;
}

/*******************************************************************************
 * Function Name: pasco2_task
 *******************************************************************************
 * Summary:
 *   Initializes context object of PASCO2 library, sets default parameters values
 *   for sensor and continuously acquire data from sensor.
 *
 * Parameters:
 *   arg: thread
 *
 * Return:
 *   none
 *******************************************************************************/
void pasco2_task(cy_thread_arg_t arg)
{
    cy_rslt_t result;
    /* I2C variables */
    cyhal_i2c_t cyhal_i2c;
    /* initialize i2c library*/
    cyhal_i2c_cfg_t i2c_master_config = {CYHAL_I2C_MODE_MASTER,
                                         0 /* address is not used for master mode */,
                                         I2C_MASTER_FREQUENCY};

    result = cyhal_i2c_init(&cyhal_i2c, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    result = cyhal_i2c_configure(&cyhal_i2c, &i2c_master_config);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* initialize mutex for terminal print */
    result = cy_rtos_init_mutex(&terminal_print_mutex);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the User LED on CYSBSYSKIT-DEV-01 and turn it on to show initialization of PAS CO2 Wing Board */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_ON);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize and enable PAS CO2 Wing Board power switch */
    cyhal_gpio_init(MTB_PASCO2_POWER_SWITCH, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO2_POWER_ON);
    /* Initialize and enable PAS CO2 Wing Board I2C channel communication*/
    cyhal_gpio_init(MTB_PASCO2_PSEL, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO2_PSEL_I2C_ENABLE);
    /* Initialize the LEDs on PAS CO2 Wing Board */
    cyhal_gpio_init(MTB_PASCO2_LED_OK, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO_LED_STATE_OFF);
    cyhal_gpio_init(MTB_PASCO2_LED_WARNING, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, MTB_PASCO_LED_STATE_OFF);

    vTaskDelay(2000);

    /* Initialize PAS CO2 sensor with default parameter values */
    result = mtb_pasco2_init(&mtb_pasco2_context, &cyhal_i2c);
    if (result != CY_RSLT_SUCCESS)
    {
        /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
        printf("\x1b[2J\x1b[;H");
        if (CY_RSLT_GET_CODE(result) == MTB_PASCO2_SENSOR_NOT_FOUND)
        {
            printf("****************** "
                   "PAS CO2 Wing Board not found "
                   "****************** \r\n\n");
        }
        else
        {
            printf("An unexpected occurred during initialization of CO2 sensor\r\n");
        }
        CY_ASSERT(0);
    }
    /* Turn off User LED on CYSBSYSKIT-DEV-01 to indicate successful initialization of CO2 Wing Board */
    cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
    /* Turn on status LED on PAS CO2 Wing Board to indicate normal operation */
    cyhal_gpio_write(MTB_PASCO2_LED_OK, MTB_PASCO_LED_STATE_ON);
    for (;;)
    {
        uint16_t ppm = 0;

        /* Read CO2 value from sensor */
        result = mtb_pasco2_get_ppm(&mtb_pasco2_context, &ppm);
        pasco2_terminal_mutex_get(0);

        if (result == CY_RSLT_SUCCESS)
        {
            /* New CO2 value is successfully read from sensor and print it to serial console */
            if (display_ppm)
            {
                printf("CO2 PPM Level: %d\r\n", ppm);
            }
            /* Turn-off warning LED*/
            cyhal_gpio_write(MTB_PASCO2_LED_WARNING, MTB_PASCO_LED_STATE_OFF);

            vTaskDelay(PASCO2_PROCESS_DELAY);
            continue;
        }
        else if (CY_RSLT_GET_TYPE(result) == CY_RSLT_TYPE_INFO)
        {
            /* Sensor gave other information than CO2 value */
            if (CY_RSLT_GET_CODE(result) == MTB_PASCO2_PPM_PENDING)
            {
                /* New value is not available yet */
                conditional_log("CO2 PPM value is not ready\r\n");
            }
            else if (CY_RSLT_GET_CODE(result) == MTB_PASCO2_SENSOR_BUSY)
            {
                /* Sensor is busy in internal processing */
                conditional_log("CO2 sensor is busy\r\n");
            }
            else
            {
                conditional_log("An unexpected occurred when accessing the CO2 sensor\r\n");
            }
            /* Turn-Off warning LED */
            cyhal_gpio_write(MTB_PASCO2_LED_WARNING, MTB_PASCO_LED_STATE_OFF);
            /* Sensor is polled in 1 second again */
            vTaskDelay(1000);
        }
        else if (CY_RSLT_GET_TYPE(result) == CY_RSLT_TYPE_WARNING)
        {
            /* Sensor gave a warning regarding over-voltage, temperature, or communication problem */
            switch (CY_RSLT_GET_CODE(result))
            {
                case MTB_PASCO2_VOLTAGE_ERROR:
                    /* Sensor detected over-voltage problem */
                    conditional_log("CO2 Sensor Over-Voltage Error\r\n");
                    break;
                case MTB_PASCO2_TEMPERATURE_ERROR:
                    /* Sensor detected temperature problem */
                    conditional_log("CO2 Sensor Temperature Error\r\n");
                    break;
                case MTB_PASCO2_COMMUNICATION_ERROR:
                    /* Sensor detected communication problem with MCU */
                    conditional_log("CO2 Sensor Communication Error\r\n");
                    break;
                default:
                    conditional_log("Unexpected error\r\n");
                    break;
            }
            /* Turn-On warning LED to indicate warning to user from sensor */
            cyhal_gpio_write(MTB_PASCO2_LED_WARNING, MTB_PASCO_LED_STATE_ON);
        }
        pasco2_terminal_mutex_release();
    }
}
