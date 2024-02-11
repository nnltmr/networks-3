#include <stdio.h>
#include <stdlib.h>
#include<string.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

 This code should be used for PA2, unidirectional or bidirectional
 data transfer protocols (from A to B. Bidirectional transfer of data
 is for extra credit and is not required).  Network properties:
 - one way network delay averages five time units (longer if there
 are other messages in the channel for GBN), but can be larger
 - packets can be corrupted (either the header or the data portion)
 or lost, according to user-defined probabilities
 - packets will be delivered in the order in which they were sent
 (although some can be lost).
 **********************************************************************/

#define BIDIRECTIONAL 0    /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct msg {
	char data[20];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt {
	int seqnum;
	int acknum;
	int checksum;
	char payload[20];
};

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
#define WINSIZE 10 //window size
#define N 1100 //buffer size

/*prototyping all functions*/
void stoptimer(int AorB);
void starttimer(int AorB, float increment);
void tolayer3(int AorB, struct pkt packet);
void tolayer5(int AorB, char datasent[]);
int chksum(struct pkt temp);
/*done prototyping*/

/*my variables*/
struct pkt sndpktA; //packet for A
struct pkt sndpktB; //packet for B
float tcount = 20.0; //timeout value or increment value
int nomessage = 1; //to keep a count of total number of messages
int nextseqno = 1; //the next seqno
int send_base = 1; //the window base
int exptseqno = 1; //the expected sequence number on receiving side
struct pkt buffer[N]; //buffer to store in coming messages
int rear = 1; //to keep track of last message in the buffer
int front = 1; //to keep track of first message in the buffer
/*done with variable declarations*/

/*Variables for analysis*/
int pktAppl_A = 0; //number of packets send from application layer of A
int pktTrans_A = 0; //number of packets send from transport layer of A
int pktAppl_B = 0; //number of packets received by application layer of B
int pktTrans_B = 0; //number of packets received by transport layer of B
/*************************/

/*function to calculate checksum*/
int chksum(struct pkt temp) {
	printf("\n-------Inside checksum----------\n");
	int chksm, i;
	chksm = 0;
	chksm += temp.acknum;
	chksm += temp.seqnum;
	for (i = 0; i < 20; i++) {
		chksm += temp.payload[i];
		//printf("%c",temp.payload[i]);
	}
	printf("\n-------Exiting Checksum--------------\n");
	return chksm;
}
/*done wit checksum function*/

/*function to add packet in queue*/
void enqueue(struct msg message) {

	if(rear>N)
	{
		printf("\n Buffer empty\n");
	}


	else

{
	strcpy(buffer[rear].payload, message.data); //adding the messages in a queue
	buffer[rear].acknum=-1;
	buffer[rear].seqnum=nomessage;
	buffer[rear].checksum=chksum(buffer[rear]);
	rear=rear+1;
}

}
/**********************************/

/*function to remove packet from queue*/
struct pkt dequeue() {

struct pkt temp;
if (rear ==front) {
	printf("\n Buffer empty\n");
	temp.seqnum=-1;


} else {

	temp = buffer[front];
	front = front + 1;


}

return temp;
}

/**************************************/



/* called from layer 5, passed the data to be sent to other side */

void A_output(message)
struct msg message; {

	enqueue(message);
			nomessage++;

	pktAppl_A++;



int i;

if (nextseqno < (send_base + WINSIZE)) {


	printf("\n-----------A_output----------\n");
	//printf("SENDING MESSAGE:%s", message.data);
	sndpktA = dequeue();

	if(sndpktA.seqnum!=-1)
	{
	printf("\nPAYLOAD:%s \nLength:%d\n", sndpktA.payload,
			strlen(sndpktA.payload));
	/*sending next sequence number packet*/

	printf("\n************************************\n");
	printf("\nSending packet \nSeq no:%d\nChecksum:%d\nGenerated "
			"Checksum:%d\n", sndpktA.seqnum, sndpktA.checksum,
			sndpktA.checksum);
	printf("\n************************************\n");
	pktTrans_A++;
	tolayer3(0, sndpktA);

	if (send_base == nextseqno) {
		printf("\nStarting the timer\n");
		starttimer(0, tcount);

	}

	nextseqno++;

	}
}
//printf("\nFinal sequence number value %d \n", nextseqno);


}

void B_output(message)
/* need be completed only for extra credit */
struct msg message; {

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
struct pkt packet; {
int chksm;
chksm = chksum(packet); //taking checksum of packet;
printf("\n------------A_input---------------\n");

if ((chksm != packet.checksum)) //corrupt ack packet or negack
{
	printf("\n************************************\n");
	printf("Received acknowledgment \nPacket:%d \nAck:%d\n"
			"Checksum:%d \nGenerated chksum:%d", packet.seqnum, packet.acknum,
			packet.checksum, chksm);
	printf("\n************************************\n");
	printf(
			"\n Packet corrupt or not in order,retransmit after timer interrupt\n");
} else {
	printf("\nPacket %d successfully transferred\n", packet.seqnum);
	printf("\n************************************\n");
	printf("Received acknowledgment \nPacket:%d \nAck:%d\n"
			"Checksum:%d", packet.seqnum, packet.acknum, packet.checksum);
	printf("\n************************************\n");

	send_base = packet.acknum + 1; //updating the base
	printf("\n New Base is %d\n", send_base);

	/*sending packets from new base*/
	if (nextseqno < (send_base + WINSIZE)) {


		printf("\n-----------A_input----------\n");
		//printf("SENDING MESSAGE:%s", message.data);
		sndpktA = dequeue();

		if(sndpktA.seqnum!=-1)
		{
		printf("\nPAYLOAD:%s \nLength:%d\n", sndpktA.payload,
				strlen(sndpktA.payload));
		/*sending next sequence number packet*/

		printf("\n************************************\n");
		printf("\nSending packet \nSeq no:%d\nChecksum:%d\nGenerated "
				"Checksum:%d\n", sndpktA.seqnum, sndpktA.checksum,
				sndpktA.checksum);
		printf("\n************************************\n");
		pktTrans_A++;
		tolayer3(0, sndpktA);

		/*checking if packet acknowledgment is base*/
		if (send_base == nextseqno) {
			printf("\nStarting the timer\n");
			starttimer(0, tcount);

		}

		nextseqno++;

		}
	}

	if (send_base == nextseqno) {
		printf("\nReceived all the packets in this window\n");
		stoptimer(0);
	} else {
		printf("\nStill receiving packets\n");
		stoptimer(0);
		starttimer(0, tcount);
	}

}
}
/* called when A's timer goes off */
void A_timerinterrupt() {

int i,temp,var;

printf("\n---------A timer interrupt------------");
/*retransmit the last packet*/
starttimer(0, tcount);

for (i = send_base; i < nextseqno; i++) {


	pktTrans_A++;
	/*creating packet*/
	sndpktA=buffer[i];


	/*packet created*/
	printf("\n************************************\n");
	printf("\nRetransmitting packet %d\n", sndpktA.seqnum);
	printf("\n************************************\n");
	tolayer3(0, sndpktA);

}
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
printf("\n INITIALIZING THE PACKET TO 0\n");
memset(sndpktA.payload, '\0', 20);
sndpktA.checksum = -1;
sndpktA.acknum = -1;

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
struct pkt packet; {

pktTrans_B++;
int chksumg;
printf("\n---------B_input:---------------\n");
chksumg = chksum(packet);
printf("\n************************************\n");
printf("Payload:%s\nSeqno:%d\n"
		"Pktchecksum:%d \ngenerated "
		"checksum:%d \nlength:%d", packet.payload, packet.seqnum,
		packet.checksum, chksumg, strlen(packet.payload));
printf("\n************************************\n");

/*checking packet for corruption or checking if its in order*/
if ((chksumg != packet.checksum) || (packet.seqnum != exptseqno)) {

	printf("\nPacket corrupted or not in correct order\n\n");
	printf("\nResending previous packet");
	printf("\nSeqno:%d \nAckno:%d \nChecksum:%d\n ", sndpktB.seqnum,
			sndpktB.acknum, sndpktB.checksum);
	tolayer3(1, sndpktB); //re send previous packet

}

else {

	sndpktB.seqnum = exptseqno;
	strcpy(sndpktB.payload, "1234");
	sndpktB.acknum = exptseqno;

	pktAppl_B++;
	tolayer5(1, packet.payload); //sending the received packet to layer 5

	printf("\nsent data to layer 5 with packet:%d and %d", packet.seqnum,
			pktAppl_B);

	sndpktB.checksum = chksum(sndpktB);
	exptseqno++;
	tolayer3(1, sndpktB); //sending acknowledgment
}

}

/* called when B's timer goes off */
void B_timerinterrupt() {
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init() {
memset(sndpktB.payload, '\0', 20);
strcpy(sndpktB.payload, "1234");
sndpktB.acknum = 0;
sndpktB.seqnum = 0;
sndpktB.checksum = chksum(sndpktB);
}

/*****************************************************************
 ***************** NETWORK EMULATION CODE STARTS BELOW ***********
 The code below emulates the layer 3 and below network environment:
 - emulates the tranmission and delivery (possibly with bit-level corruption
 and packet loss) of packets across the layer 3/4 interface
 - handles the starting/stopping of a timer, and generates timer
 interrupts (resulting in calling students timer handler).
 - generates message to be sent (passed from later 5 to 4)

 THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
 THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
 OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
 the emulator, you're welcome to look at the code - but again, you should have
 to, and you defeinitely should not have to modify
 ******************************************************************/

struct event {
float evtime; /* event time */
int evtype; /* event type code */
int eventity; /* entity where event occurs */
struct pkt *pktptr; /* ptr to packet (if any) assoc w/ this event */
struct event *prev;
struct event *next;
};
struct event *evlist = NULL; /* the event list */

 //forward declarations
void init();
void generate_next_arrival();
void insertevent(struct event*);

/* possible events: */
#define  TIMER_INTERRUPT 0
#define  FROM_LAYER5     1
#define  FROM_LAYER3     2

#define  OFF             0
#define  ON              1
#define   A    0
#define   B    1

int TRACE = 1; /* for my debugging */
int nsim = 0; /* number of messages from 5 to 4 so far */
int nsimmax = 0; /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob; /* probability that a packet is dropped  */
float corruptprob; /* probability that one bit is packet is flipped */
float lambda; /* arrival rate of messages from layer 5 */
int ntolayer3; /* number sent into layer 3 */
int nlost; /* number lost in media */
int ncorrupt; /* number corrupted by media*/

int main() {
struct event *eventptr;
struct msg msg2give;
struct pkt pkt2give;

int i, j;
char c;

init();
A_init();
B_init();

while (1) {
	eventptr = evlist; /* get next event to simulate */
	if (eventptr == NULL)
		goto terminate;
	evlist = evlist->next; /* remove this event from event list */
	if (evlist != NULL)
		evlist->prev = NULL;
	if (TRACE >= 2) {
		printf("\nEVENT time: %f,", eventptr->evtime);
		printf("  type: %d", eventptr->evtype);
		if (eventptr->evtype == 0)
			printf(", timerinterrupt  ");
		else if (eventptr->evtype == 1)
			printf(", fromlayer5 ");
		else
			printf(", fromlayer3 ");
		printf(" entity: %d\n", eventptr->eventity);
	}
	time = eventptr->evtime; /* update time to next event time */
	if (nsim == nsimmax)
		break; /* all done with simulation */
	if (eventptr->evtype == FROM_LAYER5) {
		generate_next_arrival(); /* set up future arrival */
		/* fill in msg to give with string of same letter */
		j = nsim % 26;
		for (i = 0; i < 20; i++)
			msg2give.data[i] = 97 + j;
		if (TRACE > 2) {
			printf("          MAINLOOP: data given to student: ");
			for (i = 0; i < 20; i++)
				printf("%c", msg2give.data[i]);
			printf("\n");
		}
		nsim++;
		if (eventptr->eventity == A)
			A_output(msg2give);
		else
			B_output(msg2give);
	} else if (eventptr->evtype == FROM_LAYER3) {
		pkt2give.seqnum = eventptr->pktptr->seqnum;
		pkt2give.acknum = eventptr->pktptr->acknum;
		pkt2give.checksum = eventptr->pktptr->checksum;
		memset(pkt2give.payload, '\0', 20);
		for (i = 0; i < 20; i++)
			pkt2give.payload[i] = eventptr->pktptr->payload[i];
		if (eventptr->eventity == A) /* deliver packet by calling */
			A_input(pkt2give); /* appropriate entity */
		else
			B_input(pkt2give);
		free(eventptr->pktptr); /* free the memory for packet */
	} else if (eventptr->evtype == TIMER_INTERRUPT) {
		if (eventptr->eventity == A)
			A_timerinterrupt();
		else
			B_timerinterrupt();
	} else {
		printf("INTERNAL PANIC: unknown event type \n");
	}
	free(eventptr);
}
/*Analysis part*/
float throughpt;
printf("\n\n---------------------------------------------------");
printf("\nProtocol: Go Back N\n");
printf("\n [%d] Number of packets sent from Application layer of A", pktAppl_A);
printf("\n [%d] Number of packets sent from Transport layer of A", pktTrans_A);
printf("\n [%d] Number of packets received at Transport layer of B",
		pktTrans_B);
printf("\n [%d] Number of packets received at Application layer of B",
		pktAppl_B);
printf("\n Total time:%f", time);
throughpt = pktAppl_B / time;
printf("\n Throughtput=[%f]packets/time", throughpt);
printf("\n\n---------------------------------------------------\n");
/*done with analysis*/
terminate: printf(
		" Simulator terminated at time %f\n after sending %d msgs from layer5\n",
		time, nsim);
}

void init() /* initialize the simulator */
{
int i;
float sum, avg;
float jimsrand();

printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
printf("Enter the number of messages to simulate: ");
scanf("%d", &nsimmax);
printf("Enter  packet loss probability [enter 0.0 for no loss]:");
scanf("%f", &lossprob);
printf("Enter packet corruption probability [0.0 for no corruption]:");
scanf("%f", &corruptprob);
printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
scanf("%f", &lambda);
printf("Enter TRACE:");
scanf("%d", &TRACE);

srand(9999); /* init random number generator */
sum = 0.0; /* test random number generator for students */
for (i = 0; i < 1000; i++)
	sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
avg = sum / 1000.0;
if (avg < 0.25 || avg > 0.75) {
	printf("It is likely that random number generation on your machine\n");
	printf("is different from what this emulator expects.  Please take\n");
	printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
	exit(0);
}

ntolayer3 = 0;
nlost = 0;
ncorrupt = 0;

time = 0.0; /* initialize time to 0.0 */
generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand() {
double mmm = 2147483647; /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
float x; /* individual students may need to change mmm */
x = rand() / mmm; /* x should be uniform in [0,1] */
return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival() {
double x, log(), ceil();
struct event *evptr;
//char *malloc();
float ttime;
int tempint;

if (TRACE > 2)
	printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
/* having mean of lambda        */
evptr = (struct event *) malloc(sizeof(struct event));
evptr->evtime = time + x;
evptr->evtype = FROM_LAYER5;
if (BIDIRECTIONAL && (jimsrand() > 0.5))
	evptr->eventity = B;
else
	evptr->eventity = A;
insertevent(evptr);
}

void insertevent(p)
struct event *p; {
struct event *q, *qold;

if (TRACE > 2) {
	printf("            INSERTEVENT: time is %lf\n", time);
	printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
}
q = evlist; /* q points to header of list in which p struct inserted */
if (q == NULL) { /* list is empty */
	evlist = p;
	p->next = NULL;
	p->prev = NULL;
} else {
	for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
		qold = q;
	if (q == NULL) { /* end of list */
		qold->next = p;
		p->prev = qold;
		p->next = NULL;
	} else if (q == evlist) { /* front of list */
		p->next = evlist;
		p->prev = NULL;
		p->next->prev = p;
		evlist = p;
	} else { /* middle of list */
		p->next = q;
		p->prev = q->prev;
		q->prev->next = p;
		q->prev = p;
	}
}
}

void printevlist() {
struct event *q;
int i;
printf("--------------\nEvent List Follows:\n");
for (q = evlist; q != NULL; q = q->next) {
	printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype,
			q->eventity);
}
printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(AorB)
int AorB; /* A or B is trying to stop timer */
{
struct event *q, *qold;

if (TRACE > 2)
	printf("          STOP TIMER: stopping timer at %f\n", time);
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
for (q = evlist; q != NULL; q = q->next)
	if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
		/* remove this event */
		if (q->next == NULL && q->prev == NULL)
			evlist = NULL; /* remove first and only event on list */
		else if (q->next == NULL) /* end of list - there is one in front */
			q->prev->next = NULL;
		else if (q == evlist) { /* front of list - there must be event after */
			q->next->prev = NULL;
			evlist = q->next;
		} else { /* middle of list */
			q->next->prev = q->prev;
			q->prev->next = q->next;
		}
		free(q);
		return;
	}
printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(AorB, increment)
int AorB; /* A or B is trying to stop timer */
float increment; {

struct event *q;
struct event *evptr;
//char *malloc();

if (TRACE > 2)
	printf("          START TIMER: starting timer at %f\n", time);
/* be nice: check to see if timer is already started, if so, then  warn */
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
for (q = evlist; q != NULL; q = q->next)
	if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
		printf("Warning: attempt to start a timer that is already started\n");
		return;
	}

/* create future event for when timer goes off */
evptr = (struct event *) malloc(sizeof(struct event));
evptr->evtime = time + increment;
evptr->evtype = TIMER_INTERRUPT;
evptr->eventity = AorB;
insertevent(evptr);
}

/************************** TOLAYER3 ***************/
void tolayer3(AorB, packet)
int AorB; /* A or B is trying to stop timer */
struct pkt packet; {
struct pkt *mypktptr;
struct event *evptr, *q;
//char *malloc();
float lastime, x, jimsrand();
int i;

ntolayer3++;

/* simulate losses: */
if (jimsrand() < lossprob) {
	nlost++;
	if (TRACE > 0)
		printf("          TOLAYER3: packet being lost\n");
	return;
}

/* make a copy of the packet student just gave me since he/she may decide */
/* to do something with the packet after we return back to him/her */
mypktptr = (struct pkt *) malloc(sizeof(struct pkt));
mypktptr->seqnum = packet.seqnum;
mypktptr->acknum = packet.acknum;
mypktptr->checksum = packet.checksum;
for (i = 0; i < 20; i++)
	mypktptr->payload[i] = packet.payload[i];
//    memset(mypktptr->payload,'\0',20);
//    strcpy(mypktptr->payload,packet.payload);
if (TRACE > 2) {
	printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
			mypktptr->acknum, mypktptr->checksum);
	for (i = 0; i < 20; i++)
		printf("%c", mypktptr->payload[i]);
	printf("\n");
}

/* create future event for arrival of packet at the other side */
evptr = (struct event *) malloc(sizeof(struct event));
evptr->evtype = FROM_LAYER3; /* packet will pop out from layer3 */
evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
evptr->pktptr = mypktptr; /* save ptr to my copy of packet */
/* finally, compute the arrival time of packet at the other end.
 medium can not reorder, so make sure packet arrives between 1 and 10
 time units after the latest arrival time of packets
 currently in the medium on their way to the destination */
lastime = time;
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
for (q = evlist; q != NULL; q = q->next)
	if ((q->evtype == FROM_LAYER3 && q->eventity == evptr->eventity))
		lastime = q->evtime;
evptr->evtime = lastime + 1 + 9 * jimsrand();

/* simulate corruption: */
if (jimsrand() < corruptprob) {
	ncorrupt++;
	if ((x = jimsrand()) < .75)
		mypktptr->payload[0] = 'Z'; /* corrupt payload */
	else if (x < .875)
		mypktptr->seqnum = 999999;
	else
		mypktptr->acknum = 999999;
	if (TRACE > 0)
		printf("          TOLAYER3: packet being corrupted\n");
}

if (TRACE > 2)
	printf("          TOLAYER3: scheduling arrival on other side\n");
insertevent(evptr);
}

void tolayer5(AorB, datasent)
int AorB;char datasent[20]; {
int i;
if (TRACE > 2) {
	printf("          TOLAYER5: data received: ");
	for (i = 0; i < 20; i++)
		printf("%c", datasent[i]);
	printf("\n");
}

}