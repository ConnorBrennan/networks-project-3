#include "includes.h"

// ***************************************************************************
// * ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose
// *
// * These are the functions you need to fill in.
// ***************************************************************************


// ***************************************************************************
// * Because of the way the simulator works you will likey need global variables
// * You can define those here.
// ***************************************************************************
int ASeq;
int BSeq;
int AAck;
int BAck;
int ACount;
int BCount;
std::vector<struct pkt> ABuf;
std::vector<struct pkt> BBuf;

// ***************************************************************************
// * The following routine will be called once (only) before any other
// * entity A routines are called. You can use it to do any initialization
// ***************************************************************************
void A_init() {
    ASeq = 0;
    AAck = 0;
    ACount = 0;
}

// ***************************************************************************
// * The following rouytine will be called once (only) before any other
// * entity B routines are called. You can use it to do any initialization
// ***************************************************************************
void B_init() {
    BSeq = 0;
    BAck = 0;
    BCount = 0;
}

// ***************************************************************************
// * Called from layer 5, passed the data to be sent to other side 
// ***************************************************************************
int A_output(struct msg message) {
    std::cout << "Layer 4 on side A has recieved a message from the application that should be sent to side B: "
              << message << std::endl;
    struct pkt packet;
    packet.seqnum = 0;
    packet.acknum = 0;
    packet.checksum = FletcherChecksum(message.data);
    for(int i = 0; i < 20; i++){
        packet.payload[i] = message.data[i];
    }
    simulation->tolayer3(A,packet);
    return (1); /* Return a 0 to refuse the message */
}


// ***************************************************************************
// * Called from layer 3, when a packet arrives for layer 4 on side A
// ***************************************************************************
void A_input(struct pkt packet) {
    std::cout << "Layer 4 on side A has recieved a packet sent over the network from side B:" << packet << std::endl;
}


// ***************************************************************************
// * Called from layer 5, passed the data to be sent to other side
// ***************************************************************************
int B_output(struct msg message) {
    std::cout << "Layer 4 on side B has recieved a message from the application that should be sent to side A: "
              << message << std::endl;

    return (1); /* Return a 0 to refuse the message */
}


// ***************************************************************************
// // called from layer 3, when a packet arrives for layer 4 on side B 
// ***************************************************************************
void B_input(struct pkt packet) {
    std::cout << "Layer 4 on side B has recieved a packet from layer 3 sent over the network from side A:" << packet
              << std::endl;
    
    if(packet.checksum==0){
        //Ack logic here
        while(BBuf.size()>0){
            if(BBuf.at(0).seqnum < packet.acknum){
                BBuf.erase(BBuf.begin());
            }
            else{
                break;
            }
        }

        for(int i = 0; i < BBuf.size(); i++){
            simulation->tolayer3(B, BBuf.at(i));
        }
    }

    else{
        if(packet.seqnum == BAck){
            struct msg message;
            for(int i = 0; i < 20; i++){
                message.data[i] = packet.payload[i];
            }
            int calcChecksum = FletcherChecksum(message.data);

            std::cout << "Checksum was " << packet.checksum << " calculated checksum was " << calcChecksum;

            if(calcChecksum==packet.checksum){
                simulation->tolayer5(B,message);
            }

            BCount++;
            BAck = packet.seqnum + 20;
            if(BCount == 10){
                struct pkt ackpack;
                ackpack.acknum = packet.seqnum + 20;
                ackpack.seqnum = packet.seqnum;
                ackpack.checksum = 0;
                
                for(int i = 0; i < 20; i++){
                    ackpack.payload[i] = ' ';
                }
                
                simulation->tolayer3(A,ackpack);
            }
        }
        else{
            std::cout << "Expected sequence number was" << BAck << " recieved sequence number was " << packet.seqnum;
        }
    }
}


// ***************************************************************************
// * Called when A's timer goes off 
// ***************************************************************************
void A_timerinterrupt() {
    std::cout << "Side A's timer has gone off." << std::endl;
}

// ***************************************************************************
// * Called when B's timer goes off 
// ***************************************************************************
void B_timerinterrupt() {
    std::cout << "Side B's timer has gone off." << std::endl;
}

int FletcherChecksum(char* payload){
    int sum1 = 0;
    int sum2 = 0;

    for(int i = 0; i<20; i++){
        sum1 = (sum1 + payload[i]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2*256) + sum1;
}