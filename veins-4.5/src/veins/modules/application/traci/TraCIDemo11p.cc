//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "veins/modules/application/traci/TraCIDemo11p.h"
#include<math.h>

Define_Module(TraCIDemo11p);

simsignal_t TraCIDemo11p::delaySignal = registerSignal("delay");

simsignal_t TraCIDemo11p::emrgencySentSignal = registerSignal("emrgencySent");
simsignal_t TraCIDemo11p::emrgencyRcvdSignal = registerSignal("emrgencyRcvd");
simsignal_t TraCIDemo11p::emrgencyFrwrdSignal = registerSignal("emrgencyFrwrd");

void TraCIDemo11p::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        electionDelay = par("electionDelay");
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;

        //clusterHead=this->getParentModule()->getId()+1;
        sentMessage = false;
        tempUpdate = false;

        lastUpdate = 0.00;
        lastBSMSent=simTime();
        lastAdvertize=simTime();
        clusterHead = -1;
        isClusterHead = false;
        if (this->getParentModule()->getId() == 19) {
            clusterHead = 19;
            findHost()->getDisplayString().updateWith("r=20,white");
            isClusterHead = true;
            advertize(19);
        }
        else{
            findHost()->getDisplayString().updateWith("r=16,yellow");
        }

    }
}

void TraCIDemo11p::advertize(int CHid) {

    WaveShortMessage *wsm = new WaveShortMessage();
    populateWSM(wsm);
    wsm->setByteLength(1024);
    wsm->setWsmData(mobility->getRoadId().c_str());

    EV<<"test21-carid: " << this->getParentModule()->getId() <<" - road id"<<mobility->getRoadId()<<"\n";

    wsm->setMsgType(2);  //advertizing
    wsm->setClusteHead(CHid);
    wsm->setClusterSize(clusterMembers.size());
    wsm->setMyLocation(mobility->getAngleRad());
    lastAdvertize=simTime();
    sendDown(wsm);
    //scheduleAt(simTime() + 0.0001, wsm);

}

bool TraCIDemo11p::compareDirection(double dir1, double dir2) {

    /*if (dir1 >= -3.14 && dir1 <= -1.57 && dir2 >= -3.14 && dir2 <= -1.57) {
        return true;
    }
    if (dir1 >= -1.58 && dir1 <= 0 && dir2 >= -1.58 && dir2 <= 0) {
        return true;
    }
    if (dir1 >= 0 && dir1 <= 1.57 && dir2 >= 0 && dir2 <= 1.57) {
        return true;
    }
    if (dir1 >= 1.58 && dir1 <= 3.14 && dir2 >= 1.58 && dir2 <= 3.14) {
        return true;
    }
    return false;*/
    return true;

}

void TraCIDemo11p::onBSM(BasicSafetyMessage* bsm) {
    if (isClusterHead) {
        if (clusterMembers.size() > 0) {
            for (unsigned int g = 0; g < clusterMembers.size(); g++) {
                if (clusterMembers[g]->getCarId() == bsm->getSenderAddress()) {
                    if (compareDirection(bsm->getCarDirection(),
                            mobility->getAngleRad()) == true) {
                        clusterMembers[g]->setLastUpdate(simTime());

                        break;
                    } else {
                        for (unsigned int q = 0; q < clusterMembers.size();
                                q++) {
                            if (clusterMembers[q]->getCarId()== bsm->getSenderAddress()) {
                                clusterMembers.erase(clusterMembers.begin() + q);

                                //sending FIN
                                WaveShortMessage* wsmFIN = new WaveShortMessage();
                                populateWSM (wsmFIN);
                                wsmFIN->setByteLength(1024);
                                wsmFIN->setWsmData(mobility->getRoadId().c_str());
                                wsmFIN->setMsgType(11);  //type 11 FIN
                                wsmFIN->setRecipientAddress(bsm->getSenderAddress());
                                sendDown(wsmFIN);

                                break;
                            }
                        }
                    }

                }
            }
        }

        EV << "CH: direction of CM is: " << this->getParentModule()->getId()
                                          << " " << bsm->getCarDirection() << "CH: 19-1"
                                          << mobility->getAngleRad() << "\n";
        //Your application has received a beacon message from another car or RSU
        //code for handling the message goes here

    } else {

        if (clusterHead != -1) {  // it means this car is member of some cluster
            if (clusterHead == bsm->getSenderAddress()) {
                if(compareDirection(bsm->getCarDirection(), this->mobility->getAngleRad())==true){
                    lastUpdate = simTime();
                    EV << "lastUpdateCM: " << lastUpdate << "\n";
                }
                else{

                    findHost()->getDisplayString().updateWith("r=20,white");
                    clusterHead=this->getParentModule()->getId();
                    isClusterHead=true;
                    findHost()->getDisplayString().updateWith("r=20,white");
                    WaveShortMessage* wsmFIN = new WaveShortMessage();
                    populateWSM (wsmFIN);
                    wsmFIN->setWsmData(mobility->getRoadId().c_str());
                    wsmFIN->setMsgType(11);  //type 11 FIN
                    wsmFIN->setRecipientAddress(bsm->getSenderAddress());
                    sendDown(wsmFIN);
                }

            }
        }
        EV << "lastUpdateCM2: " << lastUpdate << "\n";

        EV << "CM: direction of CM is: " << this->getParentModule()->getId()
                                          << " " << bsm->getCarDirection() << " and dirc of CH: 19-1"
                                          << mobility->getAngleRad() << "\n";
    }

}

void TraCIDemo11p::onWSA(WaveServiceAdvertisment* wsa) {

    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(wsa->getTargetChannel());
        currentSubscribedServiceId = wsa->getPsid();
        if (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService((Channels::ChannelNumber) wsa->getTargetChannel(),
                    wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void TraCIDemo11p::onWSM(WaveShortMessage* wsm) {

//    if (simTime().dbl() > 5.0 ) {//replaced  wsm->getTimestamp().dbl() with creation time 02/10/18
//        emit(delaySignal, (simTime().dbl() - wsm->getCreationTime().dbl()));
//    }

    EV<<"simtime--: "<<simTime() <<" CreationTime: "<<wsm->getCreationTime() <<"\n";

    /*if(wsm->getMsgType()==9 && wsm->getCountFrwrd() < 2){
        emit(delaySignal, (simTime().dbl() - wsm->getTimestamp().dbl() + 100.00));
    }
    else{
        emit(delaySignal, (simTime().dbl() - wsm->getCreationTime().dbl()+ 100.00));
    }*/


    if (wsm->getMsgType() == 2) {
        emit(delaySignal, (simTime().dbl() - wsm->getCreationTime().dbl()));
        //at CM
        //connection intialization
        if (isClusterHead == false) {
            if (clusterHead == -1
                    && compareDirection(wsm->getMyLocation(),
                            mobility->getAngleRad())) {
                WaveShortMessage* wsm2 = new WaveShortMessage();
                populateWSM(wsm2);
                //wsm2->setWsmData(mobility->getRoadId().c_str());
                wsm2->setMsgType(3);        //rquest to CH
                wsm2->setRecipientAddress(wsm->getClusteHead());
                //wsm->setClusteHead(CHid);
                clusterHead = wsm->getClusteHead();
                findHost()->getDisplayString().updateWith("r=16,black");
                //ssend it once instead of scheduleAt

                sendDown(wsm2);

                //scheduleAt(simTime()+0.001, wsm2);
            } else {        //member of some cluster
                //try to become gateway

                /* if(clusterHead!=wsm->getSenderAddress()){
                 WaveShortMessage* wsm2 = new WaveShortMessage();
                 populateWSM(wsm2);

                 wsm2->setMsgType(7);//gateway request to CH
                 wsm2->setRecipientAddress(clusterHead);
                 wsm2->setClusterSize(wsm->getClusterSize());
                 wsm2->setGvClusterID(wsm->getSenderAddress());
                 scheduleAt(simTime()+0.001, wsm2);

                 EV<<"stop1: "<<"\n";
                 //crashing application after addition of gateway code, it works when add break ppoint nd debug

                 }*/

            }
        } else {
            if (clusterMembers.size() < 1
                    && this->getParentModule()->getId()
                    < wsm->getSenderAddress()) {
                WaveShortMessage* wsm2 = new WaveShortMessage();
                populateWSM(wsm2);
                //wsm2->setWsmData(mobility->getRoadId().c_str());
                wsm2->setMsgType(3);                //rquest to CH
                wsm2->setRecipientAddress(wsm->getClusteHead());
                //wsm->setClusteHead(CHid);
                clusterHead = wsm->getClusteHead();
                isClusterHead=false;
                findHost()->getDisplayString().updateWith("r=16,black");
                sendDown(wsm2);
                //scheduleAt(simTime()+0.001, wsm2);
            }
        }

    } else if (wsm->getMsgType() == 3) {
        //at CH
        if (isClusterHead == true) {
            bool temp = false;
            if (clusterMembers.size() > 0) {
                for (unsigned int l = 0; l < clusterMembers.size(); l++) {
                    if (clusterMembers[l]->getCarId()
                            == wsm->getSenderAddress()) {
                        temp = true;
                        break;
                    }
                }
            }
            if (temp == false) {
                CarInformation* car = new CarInformation();
                car->setCarId(wsm->getSenderAddress());
                car->setLastUpdate(simTime());
                clusterMembers.push_back(car);

                carsInRange.push_back(wsm->getSenderAddress());
            }
            temp = false;

            for (unsigned int k = 0; k < clusterMembers.size(); k++) {
                EV << "testestets: " << clusterMembers[k]->getCarId()
                                                  << "- lastUpdate: "
                                                  << clusterMembers[k]->getLastUpdate() << "\n";
            }

            for (unsigned int i = 0; i < carsInRange.size(); i++) {
                EV << "my members: " << carsInRange[i] << "\n";
            }

            WaveShortMessage* wsm2 = new WaveShortMessage();
            populateWSM(wsm2);
            //wsm2->setWsmData(mobility->getRoadId().c_str());
            wsm2->setMsgType(4);                //response
            wsm2->setRecipientAddress(wsm->getSenderAddress());

            //wsm2->setRecipientAddress(wsm->getClusteHead());
            wsm2->setClusteHead(clusterHead);

            sendDown(wsm2);
            //scheduleAt(simTime()+0.001, wsm2);
            if((lastAdvertize + 0.05)< simTime()){
                advertize(clusterHead);
            }

        }

    } else if (wsm->getMsgType() == 4) {
        //at CM
        if (isClusterHead == false) {
            clusterHead = wsm->getClusteHead();
            findHost()->getDisplayString().updateWith("r=16,black");
            lastUpdate = simTime();
            EV << "Node Number: " << this->getParentModule()->getId()
                                              << " is member of cluster: " << clusterHead << "\n";
        }
        clusterHead = wsm->getClusteHead();
        EV << "Node Number: " << this->getParentModule()->getId()
                                          << " is member of cluster: " << clusterHead << "\n";

        BasicSafetyMessage* bsm = new BasicSafetyMessage();
        populateWSM(bsm);
        bsm->setCarDirection(mobility->getAngleRad());

        EV << "mydirec: " << mobility->getAngleRad() << "\n";

        bsm->setRecipientAddress(clusterHead);

        scheduleAt(simTime() + 0.001, bsm);
    }

    else if (wsm->getMsgType() == 7) {
        //at CH
        if (isClusterHead == true) {
            if (gateways.size() < 1) {
                GatewayModel* gm = new GatewayModel();
                gm->setNumNodes(wsm->getClusterSize());
                gm->setGatewayId(wsm->getSenderAddress());
                gm->setChId(wsm->getGvClusterID());
                gateways.push_back(gm);
            } else {
                bool temp = false;
                for (unsigned int r = 0; r < gateways.size(); r++) {
                    if (gateways[r]->getChId() == wsm->getGvClusterID()) {
                        temp = true;
                        break;
                    }

                }
                if (temp == false) {
                    GatewayModel* gm = new GatewayModel();
                    gm->setNumNodes(wsm->getClusterSize());
                    gm->setGatewayId(wsm->getSenderAddress());
                    gm->setChId(wsm->getGvClusterID());
                    gateways.push_back(gm);

                }
                for (unsigned int s = 0; s < gateways.size(); s++) {

                    EV << "gateway :" << gateways[s]->getChId() << "\n";
                }
            }
        }
    }
    //11 means FIN message arrived
    else if (wsm->getMsgType() == 11) {
        //at CH
        if(isClusterHead==true){
            for(unsigned int y=0; y<clusterMembers.size();y++){
                if(clusterMembers[y]->getCarId()==wsm->getSenderAddress()){
                    clusterMembers.erase(clusterMembers.begin()+y);
                    break;
                }
            }
        }
        //at CM
        else{
            this->clusterHead=this->getParentModule()->getId();
            isClusterHead=true;
            findHost()->getDisplayString().updateWith("r=20,white");

            advertize(clusterHead);
        }

    }


    /*


     findHost()->getDisplayString().updateWith("r=16,green");


     double destX=mobility->getCurrentPosition().x - wsm->getPosX();
     double destY=mobility->getCurrentPosition().y - wsm->getPosY();

     double distance=  sqrt(destX*destX + destY*destY);

     //EV<<"distance between "<<wsm->getSenderModule()->getId() + 1<<" and " <<this->getParentModule()->getId()<<" is "<< distance;

     if(carsInRange.size() > 0){
     for(unsigned int j=0; j< carsInRange.size(); j++){
     if(carsInRange[j]==wsm->getGeneratorAddress()){
     carsInRangeDistances[j]=distance;
     tempUpdate=true;
     break;
     }

     }
     if(tempUpdate==false){
     carsInRange.push_back(wsm->getGeneratorAddress());
     carsInRangeDistances.push_back(distance);
     tempUpdate=false;
     }

     }
     else{
     carsInRange.push_back(wsm->getGeneratorAddress());
     carsInRangeDistances.push_back(distance);

     }



     if(carsInRange.size()>0){
     for(unsigned int i=0; i< carsInRange.size(); i++){
     EV<<"length: "<<carsInRange.size()<<" cluster Head: "<<this->getParentModule()->getId() <<" has cars "<<carsInRange[i]<<" as CM with distance "<<carsInRangeDistances[i]<<"\n";
     //EV<<"distance between "<<carsInRange[i]<<" and " <<this->getParentModule()->getId()<<" is "<< carsInRangeDistances[i] <<" -list size-"<<carsInRange.size()<<"\n";

     }
     }



     if(wsm->getMsgType()==1){
     if(wsm->getClusteHead()< clusterHead){
     clusterHead=wsm->getClusteHead();

     WaveShortMessage* wsm = new WaveShortMessage();
     populateWSM(wsm);
     wsm->setWsmData(mobility->getRoadId().c_str());
     wsm->setMsgType(2);//scanning
     wsm->setClusteHead(clusterHead);

     std::string s = std::to_string(clusterHead);


     this->getParentModule()->bubble(s.c_str());

     if (dataOnSch) {
     startService(Channels::SCH2, 42, "Traffic Information Service");
     //started service and server advertising, schedule message to self to send later
     scheduleAt(computeAsynchronousSendingTime(1,type_SCH),wsm);
     }
     else {
     //send right away on CCH, because channel switching is disabled
     sendDown(wsm);

     }
     }


     */

    //&& clusterMembers.size() > 0 removed from if 04-10-18
    if (isClusterHead == true  && wsm->getMsgType() == 9 && wsm->getCountFrwrd() < 2) {

        //apply here change route if same direction only
        if (mobility->getRoadId()[0] != ':')
            traciVehicle->changeRoute(wsm->getWsmData(), 9999);

        wsm->setSenderAddress(myId);
        //wsm->setRecipientAddress(0);
        wsm->setCountFrwrd(wsm->getCountFrwrd()+ 1); //1 means CH is forwarding it to All CMs
        wsm->setSerial(3);
        EV << "accident hpnd";
        wsm->setTimestamp(simTime());

        emit(emrgencyFrwrdSignal, simTime().dbl()-wsm->getCreationTime().dbl());
        scheduleAt(simTime() + 1 + uniform(0.01, 0.2), wsm->dup());

        /*

        if (!sentMessage) {
            sentMessage = true;
            //findHost()->getDisplayString().updateWith("r=16,blue");
            //repeat the received traffic update once in 2 seconds plus some random delay
            wsm->setSenderAddress(myId);
            //wsm->setRecipientAddress(0);

            wsm->setSerial(3);
            EV << "accident hpnd";
            emit(emrgencySentSignal, simTime().dbl());
            scheduleAt(simTime() + 1 + uniform(0.01, 0.2), wsm->dup());
        }

         */
    }

    //add it into lower else if
    //&& clusterHead==wsm->getSenderAddress()
    else if (isClusterHead == false && wsm->getMsgType() == 9 && this->clusterHead==wsm->getSenderAddress()) {
        emit(emrgencyRcvdSignal, simTime().dbl()-wsm->getTimestamp().dbl());

        if (mobility->getRoadId()[0] != ':')
            traciVehicle->changeRoute(wsm->getWsmData(), 9999);
        /*

        if (!sentMessage) {
            sentMessage = true;
            //findHost()->getDisplayString().updateWith("r=16,green");
            //repeat the received traffic update once in 2 seconds plus some random delay
            wsm->setSenderAddress(myId);

            emit(emrgencyRcvdSignal, simTime().dbl());

            wsm->setSerial(3);
            EV << "accident hpnd";
            //scheduleAt(simTime() + 1 + uniform(0.01,0.2), wsm->dup());
        }
        */

    }

}

void TraCIDemo11p::handleSelfMsg(cMessage* msg) {
    if (WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg)) {
        //send this message on the service channel until the counter is 3 or higher.
        //this code only runs when channel switching is enabled
        sendDown(wsm->dup());
        wsm->setSerial(wsm->getSerial());
        if (wsm->getSerial() >= 3) {
            //stop service advertisements
            stopService();
            delete (wsm);
        } else {
            scheduleAt(simTime() + 1, wsm);
        }
    } else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }
}

void TraCIDemo11p::handlePositionUpdate(cObject* obj) {
    BaseWaveApplLayer::handlePositionUpdate(obj);
    std::string s = std::to_string(clusterHead);
    this->getParentModule()->bubble(s.c_str());

    if(clusterHead==-1){
        this->clusterHead=this->getParentModule()->getId();
        isClusterHead=true;
        findHost()->getDisplayString().updateWith("r=20,white");
        if((lastAdvertize + 0.0005)< simTime()){
                        advertize(clusterHead);
            }
        //advertize(clusterHead);
    }

    if (isClusterHead == true) {
        if((lastAdvertize + 0.0005)< simTime()){
                        advertize(clusterHead);
                    }
        //advertize(clusterHead);

        if (clusterMembers.size() > 0) {
            for (unsigned int t = 0; t < clusterMembers.size(); t++) {
                if ((clusterMembers[t]->getLastUpdate().dbl() + 5)
                        < simTime().dbl()) {
                    clusterMembers.erase(clusterMembers.begin() + t);
                }
            }
        }

        if((lastBSMSent + 0.5)< simTime()){
            BasicSafetyMessage* bsm = new BasicSafetyMessage();
                    populateWSM(bsm);
                    bsm->setCarDirection(mobility->getAngleRad());
                    lastBSMSent=simTime();
                    sendDown(bsm);
        }

        //scheduleAt(simTime()+0.001, bsm);

        /*
         if(carsInRange.size()>0){
         for(unsigned int j=0; j<carsInRange.size();j++){
         BasicSafetyMessage* bsm=new BasicSafetyMessage();
         populateWSM(bsm);
         //bsm->setCarDirection(mobility->getAngleRad());

         //EV<<"mydirec: "<<mobility->getAngleRad() <<"\n";

         bsm->setRecipientAddress(carsInRange[j]);

         scheduleAt(simTime()+0.001, bsm);
         }
         }

         */

    } else {
        //send BSM to its cluster head
        if (clusterHead != -1) {
            if((lastBSMSent + 0.5)< simTime()){
                BasicSafetyMessage* bsm = new BasicSafetyMessage();
                            populateWSM(bsm);
                            bsm->setCarDirection(mobility->getAngleRad());
                            bsm->setRecipientAddress(clusterHead);
                            sendDown(bsm);
                    }

            //scheduleAt(simTime()+0.001, bsm);
            if ((lastUpdate.dbl() + 5) < simTime().dbl()) {
                clusterHead = this->getParentModule()->getId();
                isClusterHead = true;
                findHost()->getDisplayString().updateWith("r=20,white");
                EV << "herehere";
                advertize(clusterHead);
            }
        }

    }

    /* EV<<"CarDirection"<<mobility->getAngleRad()<<"\n";


     if(simTime()>10){
     WaveShortMessage* wsm = new WaveShortMessage();
     populateWSM(wsm);
     wsm->setWsmData(mobility->getRoadId().c_str());

     wsm->setPosX(mobility->getCurrentPosition().x);
     wsm->setPosY(mobility->getCurrentPosition().y);

     //wsm->setSenderAddress(this->getParentModule()->getId());
     wsm->setGeneratorAddress(this->getParentModule()->getId());
     wsm->setRecipientAddress(clusterHead);
     wsm->setMyVar(1000);

     wsm->setMsgType(1);//scanning
     wsm->setClusteHead(clusterHead);
     if (dataOnSch) {
     startService(Channels::SCH2, 42, "Traffic Information Service");
     //started service and server advertising, schedule message to self to send later
     scheduleAt(computeAsynchronousSendingTime(1,type_SCH),wsm);
     }
     else {
     //send right away on CCH, because channel switching is disabled
     sendDown(wsm);
     }
     EV_ERROR<<"Cluster Head: "<<this->getId() <<" :"<<clusterHead;


     if(hasScanned==false){
     WaveShortMessage* wsm = new WaveShortMessage();
     populateWSM(wsm);
     wsm->setWsmData(mobility->getRoadId().c_str());
     wsm->setMsgType(1);//scanning
     wsm->setClusteHead(clusterHead);
     if (dataOnSch) {
     startService(Channels::SCH2, 42, "Traffic Information Service");
     //started service and server advertising, schedule message to self to send later
     scheduleAt(computeAsynchronousSendingTime(1,type_SCH),wsm);
     }
     else {
     //send right away on CCH, because channel switching is disabled
     sendDown(wsm);
     }
     hasScanned=true;
     }


     }
     else{
     EV_ERROR<<"Cluster Head: "<<this->getId() <<" :"<<clusterHead;

     }*/

    // stopped for for at least 10s?
    if (mobility->getSpeed() < 1) {
        //now sending emrgncy msg more than once, && sentMessage == false 03-10-18
        if (simTime() - lastDroveAt >= 5 ) {
            findHost()->getDisplayString().updateWith("r=16,red");
            //sentMessage = true;

            WaveShortMessage* wsm = new WaveShortMessage();
            populateWSM(wsm);
            wsm->setMsgType(9);
            wsm->setCountFrwrd(0);

            //9 means emergency message
            //wsm->setRecipientAddress(this->clusterHead);
            wsm->setWsmData(mobility->getRoadId().c_str());
            emit(emrgencySentSignal,simTime().dbl());
            //host is standing still due to crash
            if (dataOnSch) {
                startService(Channels::SCH2, 42, "Traffic Information Service");
                //started service and server advertising, schedule message to self to send later
                scheduleAt(computeAsynchronousSendingTime(1, type_SCH), wsm);
                scheduleAt(simTime() + 0.001, wsm);
            } else {
                //send right away on CCH, because channel switching is disabled
                scheduleAt(simTime() + 0.001, wsm);
                //sendDown(wsm);
            }
        }
    } else {
        lastDroveAt = simTime();
    }
}
