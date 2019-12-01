/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-18     tanek        first implementation
 */

#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>

#define LED_1_INDEX                 50  // PC10  

int main(void)
{
    rt_kprintf(" start main task! %d \n", __LINE__);
    //rt_pin_mode(LED_1_INDEX, PIN_MODE_OUTPUT);
		//rt_pin_write(LED_1_INDEX, PIN_LOW);
	
    while(1)
    {
		//		rt_pin_write(LED_1_INDEX, PIN_LOW);
			rt_kprintf(" start main task! %d \n", __LINE__);
        rt_thread_mdelay(1000);
      //  rt_pin_write(LED_1_INDEX, PIN_HIGH);
        rt_thread_mdelay(1000);
    }

}
