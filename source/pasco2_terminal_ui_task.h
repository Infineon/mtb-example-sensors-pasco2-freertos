/******************************************************************************
** File name: pasco2_terminal_ui.h
**
** Description: This file contains the function prototypes and constants used
**   in pasco2_terminal_ui.c.
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

/* Header file for local task */
#include "pasco2_task.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
/* Name of the pasco2 terminal ui task */
#define PASCO2_TERMINAL_UI_TASK_NAME "PASCO2 TERMINAL UI"
/* Stack size for pasco2 presence terminal ui */
#define PASCO2_TERMINAL_UI_TASK_STACK_SIZE (2048)
/* Priority number for pasco2 terminal ui */
#define PASCO2_TERMINAL_UI_TASK_PRIORITY (CY_RTOS_PRIORITY_BELOWNORMAL)

/*******************************************************************************
 * Functions
 *******************************************************************************/
void pasco2_terminal_ui_task(cy_thread_arg_t arg);
