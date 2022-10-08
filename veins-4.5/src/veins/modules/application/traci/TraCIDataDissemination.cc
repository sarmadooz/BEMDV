/*
 * TraCIDataDissemination.cc
 *
 *  Created on: Mar 15, 2022
 *      Author: Sarmad
 */


#include "veins/modules/application/traci/TraCIDataDissemination.h"
#include<math.h>

Define_Module(TraCIDataDissemination);

simsignal_t TraCIDataDissemination::delaySignal = registerSignal("delay");

simsignal_t TraCIDataDissemination::emrgencySentSignal = registerSignal("emrgencySent");
simsignal_t TraCIDataDissemination::emrgencyRcvdSignal = registerSignal("emrgencyRcvd");
simsignal_t TraCIDataDissemination::emrgencyFrwrdSignal = registerSignal("emrgencyFrwrd");

void TraCIDataDissemination::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {

        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;

        //lastBSMReceived=simTime();
        lastBSMReceived=1000.0;

        isConnectionInitiated=false;

        lastBSMSent=simTime();
        lastAssociated=simTime();

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
            findHost()->getDisplayString().updateWith("r=16,green");
        }

    }
}

void TraCIDataDissemination::advertize(int CHid) {

    WaveShortMessage *wsmAdvertize = new WaveShortMessage();
    populateWSM(wsmAdvertize);
    wsmAdvertize->setWsmData(mobility->getRoadId().c_str());
    wsmAdvertize->setMyRoadID(mobility->getRoadId().c_str());
    wsmAdvertize->setMsgType(2);  //advertizing
    wsmAdvertize->setClusteHead(CHid);
    wsmAdvertize->setClusterSize(clusterMembers.size());
    wsmAdvertize->setByteLength(4000);
    lastAdvertize=simTime();
    sendDown(wsmAdvertize);


}


void TraCIDataDissemination::onBSM(BasicSafetyMessage* bsm) {
    if(isClusterHead==true){
        if(strcmp(mobility->getRoadId().c_str(), bsm->getRoadID())==0){
            for(unsigned int i=0; i<clusterMembers.size(); i++){
                if(clusterMembers[i]->getCarId()==bsm->getSenderAddress()){
                    clusterMembers[i]->setLastUpdate(simTime());
                    break;
                }
            }
        }
        else{
            for(unsigned int i=0; i<clusterMembers.size(); i++){
                if(clusterMembers[i]->getCarId()==bsm->getSenderAddress()){
                    WaveShortMessage *wsmFIN = new WaveShortMessage();
                    populateWSM(wsmFIN);
                    wsmFIN->setMsgType(5);  //FIN
                    wsmFIN->setRecipientAddress(clusterMembers[i]->getCarId());

                    clusterMembers.erase(clusterMembers.begin() + i);
                    wsmFIN->setByteLength(4000);

                    sendDown(wsmFIN);
                    //delete bsm;

                    break;
                }
            }
        }
    }
    else if(clusterHead != -1 && clusterHead==bsm->getSenderAddress() && isClusterHead==false){
        if(bsm->getRoadID()==this->mobility->getRoadId().c_str()){
            lastBSMReceived=simTime();
            lastAssociated=simTime();
            //delete bsm;
        }
        else{

            WaveShortMessage *wsmFIN = new WaveShortMessage();
            populateWSM(wsmFIN);

            wsmFIN->setMsgType(5);  //FIN
            wsmFIN->setRecipientAddress(clusterHead);
            clusterHead= -1;
            lastAssociated=simTime();
            wsmFIN->setByteLength(4000);
            sendDown(wsmFIN);
            //delete bsm;
        }
    }


}

void TraCIDataDissemination::onWSA(WaveServiceAdvertisment* wsa) {

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

void TraCIDataDissemination::onWSM(WaveShortMessage* wsm) {


    switch (wsm->getMsgType()) {
    case 2: //advertize
        emit(delaySignal, (simTime().dbl()-wsm->getCreationTime().dbl()));
        if(isClusterHead==true){
           if(clusterMembers.size() < 1 && isConnectionInitiated==false && strcmp(mobility->getRoadId().c_str(), wsm->getMyRoadID())==0 && wsm->getClusterSize()!=0){
                WaveShortMessage *wsmCon = new WaveShortMessage();
                populateWSM(wsmCon);
                //wsmCon->setWsmData(mobility->getRoadId().c_str());
                wsmCon->setMsgType(3);        //rquest to CH
                wsmCon->setRecipientAddress(wsm->getClusteHead());
                isConnectionInitiated=true;
                wsmCon->setByteLength(4000);
                sendDown(wsmCon);
                //wsm->setClusteHead(CHid);
                //clusterHead = wsm->getClusteHead();
                //findHost()->getDisplayString().updateWith("r=16,black");
                //ssend it once instead of scheduleAt
                //delete(wsm);
            }
           //this->getParentModule()->getId() > wsm->getSenderAddress()
           else if(clusterMembers.size() < 1 && isConnectionInitiated==false && strcmp(mobility->getRoadId().c_str(), wsm->getMyRoadID())==0 && this->getParentModule()->getId() < wsm->getSenderAddress() && wsm->getClusterSize()==0){
               WaveShortMessage *wsmCon = new WaveShortMessage();
                               populateWSM(wsmCon);
                               //wsmCon->setWsmData(mobility->getRoadId().c_str());
                               wsmCon->setMsgType(3);        //rquest to CH
                               wsmCon->setRecipientAddress(wsm->getClusteHead());
                               isConnectionInitiated=true;
                               wsmCon->setByteLength(4000);
                               sendDown(wsmCon);
           }
        }
        else{
            //&& mobility->getRoadId().c_str()==wsm->getWsmData()
            if(clusterHead == -1 && isConnectionInitiated==false && strcmp(mobility->getRoadId().c_str(), wsm->getMyRoadID())==0){
            //if(clusterHead != -1  && isConnectionInitiated==false){

                WaveShortMessage *wsmCon = new WaveShortMessage();
                populateWSM(wsmCon);
                //wsmCon->setWsmData(mobility->getRoadId().c_str());
                wsmCon->setMsgType(3);        //rquest to CH
                wsmCon->setRecipientAddress(wsm->getClusteHead());
                isConnectionInitiated=true;
                //scheduleAt(simTime()+ 0.01, wsmCon);
                wsmCon->setByteLength(4000);
                sendDown(wsmCon);

            }
        }


        break;
    case 3: //conIniti
        if(isClusterHead==true && clusterHead==wsm->getRecipientAddress()){
            CarInformation* car = new CarInformation();
            car->setCarId(wsm->getSenderAddress());
            car->setLastUpdate(simTime());
            clusterMembers.push_back(car);

            for(unsigned int j=0; j<clusterMembers.size(); j++){
                EV<<"case3-"<<clusterMembers[j]->getCarId()<<" size-"<<clusterMembers.size()<<"\n";
            }

            WaveShortMessage *wsmConAck = new WaveShortMessage();
            populateWSM(wsmConAck);
            //wsmConAck->setWsmData(mobility->getRoadId().c_str());
            wsmConAck->setMsgType(4); //response back
            wsmConAck->setClusteHead(clusterHead);
            wsmConAck->setRecipientAddress(wsm->getSenderAddress());
            //clusterHead=wsm->getClusteHead();
            wsmConAck->setByteLength(4000);
            sendDown(wsmConAck);


        }
        break;
    case 4: //conAck
        if(this->getParentModule()->getId()==wsm->getRecipientAddress()){
            clusterHead=wsm->getClusteHead();
            lastAssociated=simTime();
            isClusterHead=false;
            isConnectionInitiated=true;
            findHost()->getDisplayString().updateWith("r=16,red");

        }
        //delete(wsm);
        break;
    case 5: //FIN
        if(isClusterHead==true && this->getParentModule()->getId()==wsm->getRecipientAddress()){
            for(unsigned int y=0; y<clusterMembers.size();y++){
                if(clusterMembers[y]->getCarId()==wsm->getSenderAddress()){
                    clusterMembers.erase(clusterMembers.begin()+y);
                    break;
                }
            }
        }
        else if(clusterHead != -1){
            clusterHead=-1;
        }
        //delete(wsm);
        break;

    default:
        EV<<"nothing";

    }//end main switch

}

void TraCIDataDissemination::handleSelfMsg(cMessage* msg) {
    if (WaveShortMessage *wsm = dynamic_cast<WaveShortMessage*>(msg)) {
        //send this message on the service channel until the counter is 3 or higher.
        //this code only runs when channel switching is enabled
        sendDown(wsm->dup());
        wsm->setSerial(wsm->getSerial());
        if (wsm->getSerial() >= 3) {
            //stop service advertisements
            stopService();
            //delete (wsm);
        } else {
            scheduleAt(simTime() + 1, wsm);
        }
    } else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }
}

void TraCIDataDissemination::handlePositionUpdate(cObject* obj) {

    BaseWaveApplLayer::handlePositionUpdate(obj);
    std::string s = std::to_string(clusterHead);
    this->getParentModule()->bubble(s.c_str());

    if(isClusterHead==true){
        advertize(clusterHead);
        findHost()->getDisplayString().updateWith("r=16,white");
    }
    else if(clusterHead != -1){
        findHost()->getDisplayString().updateWith("r=16,red");
    }
    else if(clusterHead == -1){
        findHost()->getDisplayString().updateWith("r=16,green");
    }



    if(clusterHead==-1){
        if(lastAssociated.dbl() + 10.0 < simTime().dbl() ){
            isClusterHead=true;
            clusterHead=this->getParentModule()->getId();
            advertize(clusterHead);
            isConnectionInitiated=false;
        }
    }
    if(lastBSMSent.dbl() + 3.0 < simTime().dbl()){
        if(isClusterHead==true){
            BasicSafetyMessage *bsm = new BasicSafetyMessage();
            populateWSM(bsm);
            bsm->setRoadID(this->mobility->getRoadId().c_str());

            lastBSMSent=simTime();
            bsm->setByteLength(4000);
            sendDown(bsm);
        }
        else if(clusterHead != -1 && isClusterHead==false){
            BasicSafetyMessage *bsm = new BasicSafetyMessage();
            populateWSM(bsm);
            bsm->setRoadID(this->mobility->getRoadId().c_str());
            bsm->setRecipientAddress(clusterHead);
            lastBSMSent=simTime();
            bsm->setByteLength(4000);
            sendDown(bsm);
        }

    }
    if(clusterHead != -1){
        if(lastBSMReceived.dbl() + 20.0 < simTime().dbl()){
            clusterHead= -1;
        }
        else if(isClusterHead==true){
            for(unsigned int y=0; y<clusterMembers.size();y++){
                if(clusterMembers[y]->getLastUpdate().dbl() + 20.0 < simTime().dbl()){
                    clusterMembers.erase(clusterMembers.begin()+y);

                }
            }
        }
    }

        if (mobility->getSpeed() < 1) {
            //now sending emrgncy msg more than once, && sentMessage == false 03-10-18
            if (simTime() - lastDroveAt >= 5 ) {
                findHost()->getDisplayString().updateWith("r=16,yellow");
                //sentMessage = true;

                WaveShortMessage *wsm = new WaveShortMessage();
                populateWSM(wsm);
                wsm->setMsgType(9);
                wsm->setCountFrwrd(0);
                //9 means emergency message
                //wsm->setRecipientAddress(this->clusterHead);
                wsm->setWsmData(mobility->getRoadId().c_str());
                wsm->setMyRoadID(mobility->getRoadId().c_str());
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



