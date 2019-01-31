/**
 *
 * \file
 *
 * \brief This module contains NMC1000 bus APIs implementation.
 *
 * Copyright (c) 2016-2017 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _NMBUS_H_
#define _NMBUS_H_

#include "../../common/include/nm_common.h"
#include "../../bus_wrapper/include/nm_bus_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 *	@fn		nm_bus_iface_init
 *	@brief	Initialize bus interface
 *	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
sint8 nm_bus_iface_init(void *);

/**
 *	@fn		nm_bus_iface_deinit
 *	@brief	Deinitialize bus interface
 *	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
sint8 nm_bus_iface_deinit(void);

/**
 *	@fn		nm_bus_reset
 *	@brief	reset bus interface
 *	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
 *	@version	1.0
 */
sint8 nm_bus_reset(void);

/**
 *	@fn		nm_bus_iface_reconfigure
 *	@brief	reconfigure bus interface
 *	@return	M2M_SUCCESS in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
sint8 nm_bus_iface_reconfigure(void *ptr);

/**
 *	@fn		nm_read_reg
 *	@brief	Read register
 *	@param [in]	u32Addr
 *				Register address
 *	@return	Register value
 */
uint32 nm_read_reg(uint32 u32Addr);

/**
 *	@fn		nm_read_reg_with_ret
 *	@brief	Read register with error code return
 *	@param [in]	u32Addr
 *				Register address
 *	@param [out]	pu32RetVal
 *				Pointer to u32 variable used to return the read value
 *	@return	ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
sint8 nm_read_reg_with_ret(uint32 u32Addr, uint32 *pu32RetVal);

/**
 *	@fn		nm_write_reg
 *	@brief	write register
 *	@param [in]	u32Addr
 *				Register address
 *	@param [in]	u32Val
 *				Value to be written to the register
 *	@return	ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
sint8 nm_write_reg(uint32 u32Addr, uint32 u32Val);

/**
 *	@fn		nm_read_block
 *	@brief	Read block of data
 *	@param [in]	u32Addr
 *				Start address
 *	@param [out]	puBuf
 *				Pointer to a buffer used to return the read data
 *	@param [in]	u32Sz
 *				Number of bytes to read. The buffer size must be >= u32Sz
 *	@return	ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
sint8 nm_read_block(uint32 u32Addr, uint8 *puBuf, uint32 u32Sz);

/**
 *	@fn		nm_write_block
 *	@brief	Write block of data
 *	@param [in]	u32Addr
 *				Start address
 *	@param [in]	puBuf
 *				Pointer to the buffer holding the data to be written
 *	@param [in]	u32Sz
 *				Number of bytes to write. The buffer size must be >= u32Sz
 *	@return	ZERO in case of success and M2M_ERR_BUS_FAIL in case of failure
 */
sint8 nm_write_block(uint32 u32Addr, uint8 *puBuf, uint32 u32Sz);

#ifdef __cplusplus
}
#endif

#endif /* _NMBUS_H_ */
