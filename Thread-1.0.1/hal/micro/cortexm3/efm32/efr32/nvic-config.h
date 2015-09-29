 /***************************************************************************//**
 * @file nvic-config.h
 * @brief NVIC Config Header
 * @version 0.01.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *

 ******************************************************************************/

/** @addtogroup nvic_config
 * Nested Vectored Interrupt Controller configuration header.
 *
 * This header defines the functions called by all of the NVIC exceptions/
 * interrupts.  The following are the nine peripheral ISRs available for
 * modification.  To use one of these ISRs, it must be instantiated
 * somewhere in the HAL.  Each ISR may only be instantiated once.  It is
 * not necessary to instantiate all or any of these ISRs (a stub will
 * be automatically generated if an ISR is not instantiated).
 *
 * - \code void halTimer1Isr(void); \endcode
 * - \code void halTimer2Isr(void); \endcode
 * - \code void halSc1Isr(void);    \endcode
 * - \code void halSc1Isr(void);    \endcode
 * - \code void halAdcIsr(void);    \endcode
 * - \code void halIrqAIsr(void);   \endcode
 * - \code void halIrqBIsr(void);   \endcode
 * - \code void halIrqCIsr(void);   \endcode
 * - \code void halIrqDIsr(void);   \endcode
 *
 * @note This file should \b not be modified.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// \b NOTE NOTE NOTE NOTE NOTE NOTE - The physical layout of this file, that
// means the white space, is CRITICAL!  Since this file is \#include'ed by
// the assembly file isr-stubs.s79, the white space in this file translates
// into the white space in that file and the assembler has strict requirements.
// Specifically, there must be white space *before* each "SEGMENT()" and there
// must be an *empty line* between each "EXCEPTION()" and "SEGMENT()".
//
// \b NOTE NOTE NOTE - The order of the EXCEPTIONS in this file is critical
// since it translates to the order they are placed into the vector table in
// cstartup.
//
// The purpose of this file is to consolidate NVIC configuration, this
// includes basic exception handler (ISR) function definitions, into a single
// place where it is easily tracked and changeable.
//
// We establish 8 levels of priority (3 bits), with 32 (5 bits) of tie break
// priority. Lower priority values are higher priorities. This means that 0
// is the highest and 7 is the lowest. The NVIC mapping is detailed below.
//

//The 'PRIGROUP' field is 3 bits within the AIRCR register and indicates the
//bit position within a given exception's 8-bit priority field to the left of
//which is the "binary point" separating the preemptive priority level (in the
//most-significant bits) from the subpriority (in the least-significant bits).
//The preemptive priority determines whether an interrupt can preempt an
//executing interrupt. The subpriority helps choose which interrupt to run if
//multiple interrupts with the same preemptive priority are active at the same
//time. If no supriority is given or there is a tie then the hardware defined
//exception number is used to break the tie.
//
// Pri.Sub  Purpose
//   0.0    faults (highest)
//   1.0    not used
//   2.0    SysTick for idling, MAC Tmr for idling
//          during TX, and management interrupt for XTAL biasing.
//   3.0    DISABLE_INTERRUPTS(), INTERRUPTS_OFF(), ATOMIC()
//   4.0    normal interrupts
//   5.0    not used
//   6.0    not used
//   7.0    debug backchannel, PendSV
//   7.31   reserved (lowest)
#define PRIGROUP_POSITION 4  // PPP.SSSSS

// Priority level used by DISABLE_INTERRUPTS() and INTERRUPTS_OFF()
// Must be lower priority than pendsv 
// NOTE!! IF THIS VALUE IS CHANGED, SPRM.S79 MUST ALSO BE UPDATED
#define INTERRUPTS_DISABLED_PRIORITY  (3 << (PRIGROUP_POSITION+1))  // READ NOTE ABOVE!!


//Exceptions with fixed priorities cannot be changed by software.  Simply make
//them 0 since they are high priorities anyways.
#define FIXED 0
//Reserved exceptions are not instatiated in the hardware.  Therefore
//exception priorities don't exist so just default them to lowest level.
#define NONE  0xFF

#ifndef SEGMENT
  #define SEGMENT()
#endif
#ifndef SEGMENT2
  #define SEGMENT2()
#endif
#ifndef PERM_EXCEPTION
  #define PERM_EXCEPTION(vectorNumber, functionName, priority) \
    EXCEPTION(vectorNumber, functionName, priority, 0)
#endif

    // SEGMENT()
    //   **Place holder required by isr-stubs.s79 to define __CODE__**
    // SEGMENT2()
    //   **Place holder required by isr-stubs.s79 to define __THUMB__**
    // EXCEPTION(vectorNumber, functionName, priorityLevel, subpriority)
    //   vectorNumber = exception number defined by hardware (not used anywhere)
    //   functionName = name of the function that the exception should trigger
    //   priorityLevel = priority level of the function
    //   supriority = Used to break ties between exceptions at the same level
    // PERM_EXCEPTION
    //   is used to define an exception that should not be intercepted by the
    //   interrupt debugging logic, or that not should have a weak stub defined.  
    //   Otherwise the definition is the same as EXCEPTION

//INTENTIONALLY INDENTED AND SPACED APART! Keep it that way!  See comment above!
    SEGMENT()
    SEGMENT2()
    PERM_EXCEPTION(1,  halEntryPoint,   FIXED)      //Reset Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(2,  halNmiIsr,            FIXED,  0)  //NMI Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(3,  halHardFaultIsr,      FIXED,  0)  //Hard Fault Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(4,  halMemoryFaultIsr,    0,      0)      //Memory Fault Handler 
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(5,  halBusFaultIsr,       0,      0)      //Bus Fault Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(6,  halUsageFaultIsr,     0,      0)      //Usage Fault Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(7,  halReserved07Isr,     NONE,   NONE)   //Reserved
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(8,  halReserved08Isr,     NONE,   NONE)   //Reserved
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(9,  halReserved09Isr,     NONE,   NONE)   //Reserved
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(10, halReserved10Isr,     NONE,   NONE)   //Reserved
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(11, halSvCallIsr,         4,      0)      //SVCall Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(12, halDebugMonitorIsr,   4,      0)      //Debug Monitor Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(13, halReserved13Isr,     NONE,   NONE)   //Reserved
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(14, halPendSvIsr,         7,      0)      //PendSV Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(15, halInternalSysTickIsr, 2,      0)      //SysTick Handler
    
    //The following handlers map to "External Interrupts 16 and above"
    //In the NVIC Interrupt registers, this corresponds to bits 16:0 with bit 0
    //being TIMER1 (exception 4) and bit 15 being IRQD (exception 15)
    SEGMENT()
    SEGMENT2()
    EXCEPTION(16, EMU_IRQHandler,             4,      0)      //Timer 1 Handler

    SEGMENT()
    SEGMENT2()
    EXCEPTION(17, FRC_PRI_IRQHandler,         4,      0)      //Timer 1 Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(18, WDOG_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(19, FRC_IRQHandler,         4,      0)      //Timer 2 Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(20, MODEM_IRQHandler,     2,      0)      //Management Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(21, RAC_SEQ_IRQHandler,       4,      0)      //BaseBand Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(22, RAC_RSM_IRQHandler,     4,      0)      //Sleep Timer Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(23, BUFC_IRQHandler,            4,      0)      //SC1 Handler
  
    SEGMENT()
    SEGMENT2()
    EXCEPTION(24, LDMA_IRQHandler,            4,      0)      //SC2 Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(25, GPIO_EVEN_IRQHandler,       4,      0)      //Security Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(26, TIMER0_IRQHandler,  3,      0)      //MAC Timer Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(27, USART0_RX_IRQHandler,    4,      0)      //MAC RX Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(28, USART0_TX_IRQHandler,            4,      0)      //ADC Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(29, ACMP0_IRQHandler,           4,      0)      //GPIO IRQA Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(30, ADC0_IRQHandler,           4,      0)      //GPIO IRQB Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(31, IDAC0_IRQHandler,           4,      0)      //GPIO IRQC Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(32, I2C0_IRQHandler,           4,      0)      //GPIO IRQD Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(33, GPIO_ODD_IRQHandler,          7,      0)      //Debug Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(34, TIMER1_IRQHandler,            4,      0)      //SC3 Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(35, USART1_RX_IRQHandler,    4,      0)      //SC4 Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(36, USART1_TX_IRQHandler,            4,      0)      //USB Handler
    
    SEGMENT()
    SEGMENT2()
    EXCEPTION(37, LEUART0_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(38, PCNT0_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(39, CMU_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(40, MSC_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(41, CRYPTO_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(42, LETIMER0_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(43, AGC_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(44, PROTIMER_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(45, RTCC_IRQHandler,      4,      0)      //RTCC Handler

    SEGMENT()
    SEGMENT2()
    EXCEPTION(46, SYNTH_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(47, CRYOTIMER_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(48, RFSENSE_IRQHandler,     NONE,   NONE)   //Reserved

    SEGMENT()
    SEGMENT2()
    EXCEPTION(49, FPUEH_IRQHandler,     NONE,   NONE)   //Reserved


#undef SEGMENT
#undef SEGMENT2
#undef PERM_EXCEPTION

#endif //DOXYGEN_SHOULD_SKIP_THIS
