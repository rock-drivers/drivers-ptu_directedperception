/**
 * Implementation for the FLIR Pan-Tilt Unit Driver.
 * @file Driver.cpp
 * @author Remus Claudiu Dumitru <r.dumitru@jacobs-university.de>
 */

//==============================================================================
// Includes
//==============================================================================
#include "Driver.h"
using namespace ptu;


#include <iostream>
#include <stdexcept>
#include <cmath>

using boost::lexical_cast;

//==============================================================================
// Static members initialization
//==============================================================================
const int ptu::Driver::DEFAULT_BAUDRATE    = 9600;
const int ptu::Driver::MAX_PACKET_SIZE     = 8192;
const float ptu::Driver::DEGREEPERTICK     = 0.051432698;
const float ptu::Driver::DEGREEPERSECARC = 0.0002778;

//==============================================================================
// Implementation
//==============================================================================


bool Driver::initialize() {   

	//set response mode of the device to short (easier parsing) mode.
	write("FT ");

	//check if set was done.
	std::string ans, err="";
	ans = readAns(mTimeout);
	if(!getAns()){
		LOG_ERROR_S << "error while changing echo mode: " << err;
		return false;
	}

        // get the pan resolution
        write(Cmd::getResolution(PAN));
        if ( !getAns(ans) ) {
            LOG_ERROR_S << "error while reading the pan resolution";
            return false;
        }
        mPanResolutionDeg = getQuery<float>(ans) * DEGREEPERSECARC;
        LOG_INFO_S << "Pan resolution is " << mPanResolutionDeg << " deg/position";

        // get the tilt resolution
        write(Cmd::getResolution(TILT));
        if( !getAns(ans) ) {
            LOG_ERROR_S << "error while reading the tilt resolution";
            return false;
        }
        mTiltResolutionDeg = getQuery<float>(ans) * DEGREEPERSECARC;
        LOG_INFO_S << "Tilt resolution is " << mTiltResolutionDeg << " deg/position";

        // get min/max pan in rad
        int min_pan = 0, max_pan = 0;

        write(Cmd::getMinPos(PAN));
        if ( ! getAns(ans) ) 
            LOG_WARN_S << "could not acquire min pan positon";
        else
            min_pan = getQuery<int>(ans);

        write(Cmd::getMaxPos(PAN));
        if ( !getAns(ans) ) 
            LOG_WARN_S << "could not acquire max pan positon";
        else
            max_pan = getQuery<int>(ans);

        mMinPanRad = float(min_pan) * mPanResolutionDeg * M_PI / 180.0;
        mMaxPanRad = float(max_pan) * mPanResolutionDeg * M_PI / 180.0;

        LOG_INFO_S << "Pan limits (rad): " <<  mMinPanRad << " to " << mMaxPanRad;
	     
        // get min/max tilt in rad
        int min_tilt = 0, max_tilt = 0;

        write(Cmd::getMinPos(TILT));
        if ( !getAns(ans) ) 
            LOG_WARN_S << "could not acquire min tilt positon"; 
        else
            min_tilt = getQuery<int>(ans);

        write(Cmd::getMaxPos(TILT));
        if ( !getAns(ans) ) 
            LOG_WARN_S << "could not acquire max tilt positon";
        else
            max_tilt = getQuery<int>(ans);

        mMinTiltRad = float(min_tilt) * mTiltResolutionDeg * M_PI / 180.0;
        mMaxTiltRad = float(max_tilt) * mTiltResolutionDeg * M_PI / 180.0;

        LOG_INFO_S << "Tilt limits (rad): " <<  mMinTiltRad << " to " << mMaxTiltRad;

	return true;
}


void Driver::write(const std::string& msg, int timeout) {
    
    if (timeout == -1) {

        writePacket(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size());

    } else {

        writePacket(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size(), timeout);

    }
}

std::string Driver::readAns(int timeout) {

    uint8_t buffer[MAX_PACKET_SIZE];
    size_t bufferSize = MAX_PACKET_SIZE;
    size_t packetSize;

    if (-1 == timeout) {
        packetSize = readPacket(buffer, bufferSize);
    } else {
        packetSize = readPacket(buffer, bufferSize, timeout);
    }
	
    return std::string(reinterpret_cast<const char*>(buffer), packetSize);
}

bool Driver::validateAns(const std::string& ans, std::string& error) {

    if (ans.size() < 2) {
        error = "answer must be at least of size 2";
        return false;
    } else if (ans[0] != Cmd::SUCC_BEG[0] && ans[0] != Cmd::ERR_BEG[0]) {
        error = "answer must always start with " + Cmd::SUCC_BEG + " or " + Cmd::ERR_BEG;
        return false;
    } else if (ans[0] == Cmd::ERR_BEG[0]) {
        error = ans.substr(1, ans.size() - 2);
        return false;
    }

    return true;
}

bool Driver::getAns(std::string& ans) {

    std::string error;
    
    ans = readAns(mTimeout);

    if (!validateAns(ans, error) ) {
	LOG_ERROR_S << "validation error /" << error << "/ while reading " << ans;
        return false;
    }

    return true;
}
    

int Driver::extractPacket(const uint8_t* buffer, size_t size) const {
    

    // possible beginnings of a packet
    std::string begs = Cmd::SUCC_BEG + Cmd::ERR_BEG;

    // the current packet
    std::string packet(reinterpret_cast<const char*>(buffer), size);

    // find the beginning and the end of a packet
    size_t beg = packet.find_first_of(begs);
    size_t end = packet.find_first_of(Cmd::DELIM_CR);

    // see the documentation of extractPacket for further details
    if (beg == std::string::npos && end == std::string::npos) {
        return (-size);
    } else if (beg != 0 && beg != std::string::npos) {
        return (-beg);
    } else if (beg == 0 && end == std::string::npos) {
        return 0;
    } else if (beg == 0 && end != std::string::npos) {
        return end + 1;
    } else {
        throw std::runtime_error("extractPacket: invalid branch taken");
    }

    return 0;
}

Driver::Driver() :
        iodrivers_base::Driver(MAX_PACKET_SIZE),
        mPanResolutionDeg(1.0),
        mTiltResolutionDeg(1.0),
        mTimeout(10000)
{}

Driver::~Driver() {
    if (this->isValid()) {
        this->close();
    }
}

//TODO check if offset parameter is really usefull here, since PO and PP seem to 
//     have always the same answer.
bool Driver::getPos(const Axis& axis, const bool& offset, int& pos) {

    bool ret = write(Cmd::getPos(axis, offset));
    if (!ret) return false;

    std::string ans = readAns(mTimeout);

    std::string error;
    if ( !validateAns(ans, error) ) {
	LOG_ERROR_S << "getPos: " << error;
        return false;
    }

    std::string toFind("* ");
    size_t found = ans.find(toFind);

    if (found == std::string::npos) {
	LOG_ERROR_S << "getPos: invalid answer format";
        return false;
    }

    pos = lexical_cast<int>(ans.substr(found + toFind.size()));

    return true;
}


float Driver::getPosDeg(const Axis& axis, const bool& offset){

    int tickPos = 0;
    getPos(axis,offset,tickPos);
    return (float)tickPos * DEGREEPERTICK;
}

float Driver::getPosRad(const Axis& axis, const bool& offset){

    return getPosDeg(axis, offset) / 180.0 * M_PI;
}


//set position as degree value.
bool Driver::setPosDeg(const Axis &axis, const bool &offset, const float &val, 
                       const bool &awaitCompletion){
    
    int ticks = round(val / DEGREEPERTICK);
    setPos(axis, offset, ticks, awaitCompletion);
    
    return true;
}
bool Driver::setPosRad(const Axis &axis, const bool &offset, const float &val, 
                       const bool &awaitCompletion){

    return setPosDeg(axis, offset, val / M_PI * 180.0, awaitCompletion);
}


bool Driver::setPos(const Axis& axis, const bool& offset, const int& val, 
                    const bool& awaitCompletion) {

    bool ret = write(Cmd::setPos(val, axis, offset));
    if (!ret) return false;

    std::string ans = readAns(mTimeout);

    std::string error;
    if ( !validateAns(ans, error) ) {
	LOG_ERROR_S << "setPos: " << error;
        return false;
    }

    if (awaitCompletion){
	
	//set awaitCompletion mode      
	LOG_DEBUG_S << "setPos: with await completion.";
	write(Cmd::awaitPosCmdCompletion());
	
	//check if command was set successfully.
	ans = readAns(mTimeout);
	std::string err = "";
	if(!validateAns(ans, err)){
		LOG_ERROR_S << "failed to set await completion mode: " << err;
		//log error but do not answer with fail code.
	}
	
    }

    if (ans == Cmd::SUCC_CMD) {
        return true;
    } else {
	LOG_ERROR_S << "setPos: failed to set position";
        return false;
    }

    return true;
}


bool Driver::setSpeed(const Axis& axis, const int& speed) {
    
    if (!write(Cmd::setDesiredSpeed(speed,axis)))
        return false;
    
    std::string ans;
    return getAns(ans);
}

bool Driver::setSpeedDeg(const Axis& axis, const float& speed) {
    
    int speed_pos_s;
    if ( axis == TILT )
        speed_pos_s = speed / mTiltResolutionDeg;
    else if (axis == PAN )
        speed_pos_s = speed / mPanResolutionDeg;

    return setSpeed(axis,speed_pos_s);
}

bool Driver::setSpeedRad(const Axis& axis, const float& speed) {

    return setSpeedDeg(axis, speed * 180.0 / M_PI);
}

bool Driver::setHalt() {

    return write(Cmd::haltPosCmd(true,true));
}
