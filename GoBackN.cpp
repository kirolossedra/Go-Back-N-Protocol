

#include <iostream>
#include <vector>
#include <unistd.h>
using namespace std;

#include "GoBackN.h"


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
    frame s; /* scratch variable */


    (&s)->ack = buffer[frame_nr];	/* insert packet into frame */
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



/*******************************************************************************
 *                              Main Porgram                                   *
 *******************************************************************************/

int main()
{

	seq_nr next_frame_to_send;
	event_type event;



	/*Variables Declaration*/
	int frames_no , transmission_no  = 0;

	char _frame;

	vector<char> buffer , frames ;

	int error_index , latestAck = 0 , frame_ind;


	/*Getting frames values from the console*/
	cout<<"Enter the number of frames"<<endl ;
	cin>>frames_no ;


	cout<<"Enter the frame values"<<endl ;

	for(int i = 0 ; i < frames_no ; i++) {

		cout<<"Frame "<<i+1<<" : ";

		cin>>_frame;
		frame newFrame;
		to_physical_layer(&newFrame);
		start_timer(newFrame.seq);
		wait_for_event(&event);


		frames.push_back(_frame);


	}


	/*For each frame to be sent: */

	for(frame_ind = 0 ; frame_ind < (int)frames.size() ;  transmission_no++  ) {

		/*A random error can happen in receiving the frame with a possibility of 10%*/
		int error = rand() % ERROR_NUMBER + 0;

		/*Case 1 : No error will occur*/
		if(error != ERROR_VALUE) {

			/*Frame Transmission*/
			cout<<"Sending Frame "<<frame_ind+1<<endl;
			cout<<"Frame Data : "<<frames[frame_ind]<<endl;
			delay_ms(DELAY_AMOUNT);

			/*Acknowledgement Transmission depending on the sender's window size*/
			if(frame_ind == latestAck + SENDER_WINDOW_SIZE-1 ) {
				latestAck++ ;
				cout<<"Frame "<<latestAck<<" Acknowledged"<<endl;
				delay_ms(DELAY_AMOUNT);

			}

			buffer.push_back(frames[frame_ind++]);

		}


		/*Case 2 : An error will occur while receiving the frame*/

		else{


			/*Keeping Track of the error location*/
			error_index = frame_ind ;

			/*Sending frames until the end of the window*/
			for(int i = frame_ind ; i<error_index+BUFFER_SIZE+1 && i<frames_no ; i++) {

				transmission_no++;

				cout<<"Sending Frame "<<i+1<<endl;
				cout<<"Frame Data : "<<frames[i]<<endl;
				delay_ms(DELAY_AMOUNT);


				/*Acknowledging frames until the error frame*/
				if (error_index > latestAck )
				{




					cout << "Acknowledgment for Frame " << ++latestAck << endl;

				}


			}

			/*Initiating retransmission of the window*/
			frame_ind = error_index ;
			cout<<"Frame "<<frame_ind+1<<" is Not Received"<<endl;
			cout<<"Retransmitting Window"<<endl;

		}
	}

	/*After sending all frames, the remaining acknowledgement are sent back*/
	while(latestAck < frame_ind) {
		latestAck++;
		cout<<"Frame "<<latestAck<<" Acknowledged"<<endl;
		delay_ms(DELAY_AMOUNT);
	}




	return 0;
}
