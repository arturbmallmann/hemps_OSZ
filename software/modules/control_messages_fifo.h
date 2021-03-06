/*!\file control_messages_fifo.h
 * HEMPS VERSION - 8.0 - support for RT applications
 *
 * Distribution:  June 2016
 *
 * Created by: Luciano L. Caimi - contact: luciano.caimi@acad.pucrs.br
 *
 * Research group: GAPH-PUCRS   -  contact:  fernando.moraes@pucrs.br
 *
 * \brief This module defines the function relative to insert all control messages in a FIFO. If any send fail 
 *  the correspondent message could be reload and resend using a Source Routing strategie
 */

#ifndef CONTROL_MESSAGES_FIFO_H_
#define CONTROL_MESSAGES_FIFO_H_

#include "packet.h"

#define     CM_FIFO_LENGTH  16
#define     YES     1
#define     NO      0

typedef struct {

    ServiceHeader service_header;                //!<Copy of service header send
    unsigned int ptr_payload;                   //!<payload pointer of message send
    unsigned int payload_length;                 //!<payload lenght
    unsigned int used;                          //!<used: flag control of resend messages

} ControlMessagesFifo;


void insert_CM_FIFO(ServiceHeader *, unsigned int, unsigned int);

int search_Service(unsigned int );

int search_Target(unsigned int );

//int search_Target_Service(unsigned int, unsigned int );

void initialize_CM_FIFO();

void print_CM_FIFO();

int resend_control_message(unsigned int target);

#endif /* SOFTWARE_INCLUDE_CONTROL_MESSAGES_FIFO_ */
