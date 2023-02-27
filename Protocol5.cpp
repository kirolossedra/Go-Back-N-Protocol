#include <iostream>
#include "Protocol5.h"

using namespace std;

/*********************************Global Variables*******************************************/

bool NetworkLayer = false;
bool TimeOutFlag = false;

packet NetworkLayer_Buffer[8];




/***********************************Functions***********************************************/
/* Wait for an event to happen; return its type in event. */
void wait_for_event(event_type *event)
{
	if(NetworkLayer)
	{
		*event = network_layer_ready;
	}
	else if(TimeOutFlag)
	{
		*event =timeout;
	}

	else
	{
		*event = frame_arrival;
	}

}

/* Fetch a packet from the network layer for transmission on the channel. */
void from_network_layer(packet *p)
{


}

/* Deliver information from an inbound frame to the network layer. */
void to_network_layer(packet *p)
{

}

/* Go get an inbound frame from the physical layer and copy it to r. */
void from_physical_layer(frame *r)
{

}

/* Pass the frame to the physical layer for transmission. */
void to_physical_layer(frame *s)
{

}

/* Start the clock running and enable the timeout event. */
void start_timer(seq_nr k)
{

}

/* Stop the clock and disable the timeout event. */
void stop_timer(seq_nr k)
{

}

/* Start an auxiliary timer and enable the ack_timeout event. */
void start_ack_timer(void)
{

}

/* Stop the auxiliary timer and disable the ack_timeout event. */
void stop_ack_timer(void)
{

}

/* Allow the network layer to cause a network_layer_ready event. */
void enable_network_layer(void)
{
	cout<<"Network Layer enabled"<<endl;
	NetworkLayer = true;


}

/* Forbid the network layer from causing a network_layer_ready event. */
void disable_network_layer(void)
{
	cout<<"Network Layer disabled"<<endl;
	NetworkLayer = false;

}



static void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{
    /* Construct and send a data frame. */
    frame s;	/* scratch variable */
    s.info = buffer[frame_nr];	/* insert packet into frame */
    s.seq = frame_nr;	/* insert sequence number into frame */
    s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);	/* piggyback ack */
    to_physical_layer(&s);	/* transmit the frame */
    start_timer(frame_nr);	/* start the timer running */
}

static bool between(seq_nr a, seq_nr b, seq_nr c)
{
    /* Return true if (a <=b < c circularly; false otherwise. */
    if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
        return(true);
    else
        return(false);
}

void protocol5(void)
{
	/* MAX_SEQ > 1; used for outbound stream */
    seq_nr next_frame_to_send;       // @sender

    /* oldest frame as yet unacknowledged */
    seq_nr ack_expected; 			 // @sender

    /* next frame expected on inbound stream */
    seq_nr frame_expected;			 // @receiver

    /* scratch variable */
    frame r;						//@receiver - I will receive this at receiver

    /* buffers for the outbound stream */
    packet buffer[MAX_SEQ+1];       // Number of packets in our window

    /* # output buffers currently in use */
    seq_nr nbuffered;               // @sender - Number of packets I stored waiting for the ack

    /* used to index into the buffer array */
    seq_nr i; 						// @sender - iterator over the number of buffered packets

    event_type event;

    /* allow network_layer_ready events */
    enable_network_layer();

    //Because we are just starting to send , all values are zero

    ack_expected = 0;	/* next ack expected inbound */
    next_frame_to_send = 0;	/* next frame going out */
    frame_expected = 0;	/* number of frame expected inbound */
    nbuffered = 0;	/* initially no packets are buffered */

    while (true) {
        wait_for_event(&event);	/* four possibilities: see event_type above */

        switch(event) {
            case network_layer_ready:	/* the network layer has a packet to send */
                /* Accept, save, and transmit a new frame. */
                from_network_layer(&buffer[next_frame_to_send]); /* fetch new packet */
                nbuffered = nbuffered + 1;	/* expand the sender's window */
                send_data(next_frame_to_send, frame_expected, buffer);	/* transmit the frame */
                inc(next_frame_to_send);	/* advance sender's upper window edge */
                break;

            case frame_arrival:	/* a data or control frame has arrived */
                from_physical_layer(&r);	/* get incoming frame from physical layer */

                if (r.seq == frame_expected) {
                    /* Frames are accepted only in order. */
                    to_network_layer(&r.info);	/* pass packet to network layer */
                    inc(frame_expected);	/* advance lower edge of receiver's window */
                }

                /* Ack n implies n - 1, n - 2, etc.  Check for this. */
                while (between(ack_expected, r.ack, next_frame_to_send)) {
                    /* Handle piggybacked ack. */
                    nbuffered = nbuffered - 1;	/* one frame fewer buffered */
                    stop_timer(ack_expected);	/* frame arrived intact; stop timer */
                    inc(ack_expected);	/* contract sender's window */
                }
                break;

            case cksum_err: ;	/* just ignore bad frames */
                break;

                //case no ack arrived after certain time

            case timeout:	/* trouble; retransmit all outstanding frames */
                next_frame_to_send = ack_expected;	/* start retransmitting here */
                for (i = 1; i <= nbuffered; i++) {
                    send_data(next_frame_to_send, frame_expected, buffer);	/* resend 1 frame */
                    inc(next_frame_to_send);	/* prepare to send the next one */
                }
        }

        // does my window have place for more packets to come from network layer?
        if (nbuffered < MAX_SEQ)
            enable_network_layer();
        else
            disable_network_layer();
    }
}

