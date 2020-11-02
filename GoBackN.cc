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
    ASeq = 1;
    AAck = 1;
    ACount = 0;
    simulation->starttimer(A, 50);
}

// ***************************************************************************
// * The following rouytine will be called once (only) before any other
// * entity B routines are called. You can use it to do any initialization
// ***************************************************************************
void B_init() {
    BSeq = 1;
    BAck = 1;
    BCount = 0;
    simulation->starttimer(B, 50);
}

// ***************************************************************************
// * Called from layer 5, passed the data to be sent to other side 
// ***************************************************************************
int A_output(struct msg message) {
    std::cout << "Layer 4 on side A has recieved a message from the application that should be sent to side B: "
              << message << std::endl;

    if(ABuf.size() >= 10){
        return 0;
    }
    struct pkt packet;
    packet.seqnum = ASeq;
    packet.acknum = 0;
    packet.checksum = FletcherChecksum(message.data);
    for(int i = 0; i < 20; i++){
        packet.payload[i] = message.data[i];
    }
    simulation->tolayer3(A,packet);
    ABuf.push_back(packet);

    ASeq+=20;

    return (1); /* Return a 0 to refuse the message */
}


// ***************************************************************************
// * Called from layer 3, when a packet arrives for layer 4 on side A
// ***************************************************************************
void A_input(struct pkt packet) {
    std::cout << "Layer 4 on side A has recieved a packet sent over the network from side B:" << packet << std::endl;

    if(packet.checksum==-1){
        //Ack logic here
        simulation->stoptimer(A);
        while(ABuf.size()>0){
            if(ABuf.at(0).seqnum < packet.acknum){
                ABuf.erase(ABuf.begin());
                std::cout << "Erased acked item from abuffer" << std::endl;
            }
            else{
                break;
            }
        }

        int ABufSize = ABuf.size();

        for(int i = 0; i < ABufSize; i++){
            std::cout << "Retransmitting packet " << ABuf.at(i).seqnum << std::endl;
            simulation->tolayer3(A, ABuf.at(i));
            simulation->starttimer(A, 50);
        }
        
    }

    else{
        if(packet.seqnum == AAck){
            struct msg message;
            for(int i = 0; i < 20; i++){
                message.data[i] = packet.payload[i];
            }
            int calcChecksum = FletcherChecksum(message.data);

            std::cout << "Checksum was " << packet.checksum << " calculated checksum was " << calcChecksum << std::endl;

            if(calcChecksum==packet.checksum){
                simulation->tolayer5(A,message);
            }

            ACount++;
            AAck = packet.seqnum + 20;
            if(ACount == 10){
                struct pkt ackpack;
                ackpack.acknum = packet.seqnum + 20;
                ackpack.seqnum = packet.seqnum;
                ackpack.checksum = -1;
                
                for(int i = 0; i < 20; i++){
                    ackpack.payload[i] = ' ';
                }
                
                simulation->tolayer3(A,ackpack);
            }
        }
        else{
            std::cout << "Expected sequence number was " << AAck << " recieved sequence number was " << packet.seqnum << std::endl;
        }
    }
}


// ***************************************************************************
// * Called from layer 5, passed the data to be sent to other side
// ***************************************************************************
int B_output(struct msg message) {
    std::cout << "Layer 4 on side B has recieved a message from the application that should be sent to side A: "
              << message << std::endl;

    if(BBuf.size() >= 10){
        return 0;
    }
    struct pkt packet;
    packet.seqnum = BSeq;
    packet.acknum = 0;
    packet.checksum = FletcherChecksum(message.data);
    for(int i = 0; i < 20; i++){
        packet.payload[i] = message.data[i];
    }
    simulation->tolayer3(B,packet);
    BBuf.push_back(packet);

    BSeq+=20;

    return (1); /* Return a 0 to refuse the message */

}
// ***************************************************************************
// // called from layer 3, when a packet arrives for layer 4 on side B 
// ***************************************************************************
void B_input(struct pkt packet) {
    std::cout << "Layer 4 on side B has recieved a packet from layer 3 sent over the network from side A:" << packet
              << std::endl;
    
    if(packet.checksum==-1){
        //Ack logic here
        simulation->stoptimer(B);
        while(BBuf.size()>0){
            if(BBuf.at(0).seqnum < packet.acknum){
                BBuf.erase(BBuf.begin());
                std::cout << "Erased acked item from bbuffer" << std::endl;
            }
            else{
                break;
            }
        }

        int BBufSize = BBuf.size();

        //BSeq = BBuf.at(0).seqnum;

        for(int i = 0; i < BBufSize; i++){
            std::cout << "Retransmitting packet " << BBuf.at(i).seqnum << std::endl;
            simulation->tolayer3(B, BBuf.at(i));
            simulation->starttimer(B, 50);
        }
        
    }

    else{
        if(packet.seqnum == BAck){
            struct msg message;
            for(int i = 0; i < 20; i++){
                message.data[i] = packet.payload[i];
            }
            int calcChecksum = FletcherChecksum(message.data);

            std::cout << "Checksum was " << packet.checksum << " calculated checksum was " << calcChecksum << std::endl;

            if(calcChecksum==packet.checksum){
                simulation->tolayer5(B,message);
            }

            BCount++;
            BAck = packet.seqnum + 20;
            if(BCount == 10){
                struct pkt ackpack;
                ackpack.acknum = packet.seqnum + 20;
                ackpack.seqnum = packet.seqnum;
                ackpack.checksum = -1;
                
                for(int i = 0; i < 20; i++){
                    ackpack.payload[i] = ' ';
                }
                
                simulation->tolayer3(B,ackpack);
            }
        }
        else{
            std::cout << "Expected sequence number was " << BAck << " recieved sequence number was " << packet.seqnum << std::endl;
        }
    }
}


// ***************************************************************************
// * Called when A's timer goes off 
// ***************************************************************************
void A_timerinterrupt() {
    std::cout << "Side A's timer has gone off." << std::endl;

        int ABufSize = ABuf.size();

        for(int i = 0; i < ABufSize; i++){
            std::cout << "Retransmitting packet " << ABuf.at(i).seqnum << std::endl;
            simulation->tolayer3(A, ABuf.at(i));
            simulation->stoptimer(A);
            simulation->starttimer(A, 50);
        }
}

// ***************************************************************************
// * Called when B's timer goes off 
// ***************************************************************************
void B_timerinterrupt() {
    std::cout << "Side B's timer has gone off." << std::endl;

        int BBufSize = BBuf.size();

        //BSeq = BBuf.at(0).seqnum;

        for(int i = 0; i < BBufSize; i++){
            std::cout << "Retransmitting packet " << BBuf.at(i).seqnum << std::endl;
            simulation->tolayer3(B, BBuf.at(i));
            simulation->starttimer(B, 50);
        }

    
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