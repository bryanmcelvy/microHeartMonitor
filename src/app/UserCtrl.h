/**
 * @file    UserCtrl.h
 * @author  Bryan McElvy
 * @brief   Interface for user control module.
 */

#ifndef __USERCTRL_H__
#define __USERCTRL_H__

#include "GPIO.h"
#include "Timer.h"

/**
 * @brief Initializes the UserCtrl module and its dependencies (Timer0B and GPIO_PortF)
 * 
 */
void UserCtrl_Init();

#endif
