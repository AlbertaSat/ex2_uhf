
#ifndef __BL_UTIL_H__
#define __BL_UTIL_H__


#include "sys_common.h"

/**
 *
 *  @brief  	This function is responsible for enabling the RTI module
 *				and initializing the compare up counter 0 with the supplied
 *				argument.
 *  @param		compareUpCounter -  value with which the compare up counter 0
 *									should be initialized
 *  @return    	None
 */
void UTIL_initRTI(uint32_t compareUpCounter);

/**
 *
 *  @brief  	This function is responsible for starting the RTI timer.
 *  @param   	None
 *  @return    	None
 */
void UTIL_startTimer(void);

/**
 *
 *  @brief  	This function is responsible for stopping the RTI timer.
 *  @param  	None
 *  @return    	None
 *
 */
void UTIL_stopTimer(void);

/**
 *
 *  @brief  	This function is responsible for issuing a soft reset to the
 *				Hercules device.
 *  @param		None
 *  @return    	None
 *
 */
void UTIL_reset(void);


#endif // __BL_UTIL_H__
