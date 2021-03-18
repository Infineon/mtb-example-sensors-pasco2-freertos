/******************************************************************************
** File Name:   pasco2_task.h
**
** Description: This file contains the function prototypes and constants used
**   in pasco2_task.c.
**
** Related Document: See README.md
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

#pragma once

/* Header file includes */
#include "cyabs_rtos.h"
#include "cycfg.h"
#include "mtb_pasco2.h"

/* Header file for library */

/*******************************************************************************
 * Macros
 ******************************************************************************/

/* Name of the pas co2 task */
#define PASCO2_TASK_NAME "CO2 SENSOR TASK"
/* Stack size for the co2 sensor task */
#define PASCO2_TASK_STACK_SIZE (1024 * 4)
/**< Priority number for the co2 sensor task */
#define PASCO2_TASK_PRIORITY (CY_RTOS_PRIORITY_BELOWNORMAL)
/* Delay time after each call to Ifx_RadarSensing_Process */
#define PASCO2_PROCESS_DELAY (10000)
/*******************************************************************************
 * Global Variables
 *******************************************************************************/
extern mtb_pasco2_context_t mtb_pasco2_context;

/*******************************************************************************
 * Functions
 *******************************************************************************/

void pasco2_task(cy_thread_arg_t arg);
void pasco2_enable_internal_logging(bool enable_logging);
void pasco2_display_ppm(bool enable_output);
cy_rslt_t pasco2_terminal_mutex_get(cy_time_t timeout_ms);
cy_rslt_t pasco2_terminal_mutex_release(void);
