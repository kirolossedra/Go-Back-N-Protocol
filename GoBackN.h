/*
 * GoBackN.h
 *
 *  Created on: 4 Dec 2021
 *      Author: kirol
 */

#ifndef GOBACKN_H_
#define GOBACKN_H_



#endif /* GOBACKN_H_ */
/*******************************************************************************
 *                               Definitions                                   *
 *******************************************************************************/
#define BUFFER_SIZE 5
#define  ERROR_NUMBER 10
#define ERROR_VALUE   0
#define SENDER_WINDOW_SIZE 4
#define DELAY_AMOUNT 1000
#define delay_ms(msec) usleep(msec*1000)


#define inc(k) if (k < MAX_SEQ) k = k + 1; else k = 0

/*********************************Static Configuration**************************************/

#define MAX_SEQ 7
#define MAX_PKT 1024	/* determines packet size in bytes */

/********************************* Type Definition *******************************************/


//This enumeration is used to represent the events that could happen
typedef enum
{
	frame_arrival, cksum_err, timeout, network_layer_ready
} event_type;

typedef unsigned int seq_nr;	/* sequence or ack numbers */


typedef struct
{
	unsigned char data[MAX_PKT];
} packet;	/* packet definition */

typedef enum
{
	data, ack, nak
} frame_kind;	/* frame_kind definition */

typedef struct {	/* frames are transported in this layer */
    frame_kind kind;	/* what kind of a frame is it? */
    seq_nr seq;   	/* sequence number */
    seq_nr ack;   	/* Acknowledgment number */
    packet info;  	/* the network layer packet */
} frame;



/***********************************Functions***********************************************/
/* Wait for an event to happen; return its type in event. */
void wait_for_event(event_type *event);

/* Fetch a packet from the network layer for transmission on the channel. */
void from_network_layer(packet *p);

/* Deliver information from an inbound frame to the network layer. */
void to_network_layer(packet *p);

/* Go get an inbound frame from the physical layer and copy it to r. */
void from_physical_layer(frame *r);

/* Pass the frame to the physical layer for transmission. */
void to_physical_layer(frame *s);

/* Start the clock running and enable the timeout event. */
void start_timer(seq_nr k);

/* Stop the clock and disable the timeout event. */
void stop_timer(seq_nr k);

/* Start an auxiliary timer and enable the ack_timeout event. */
void start_ack_timer(void);

/* Stop the auxiliary timer and disable the ack_timeout event. */
void stop_ack_timer(void);

/* Allow the network layer to cause a network_layer_ready event. */
void enable_network_layer(void);

/* Forbid the network layer from causing a network_layer_ready event. */
void disable_network_layer(void);
