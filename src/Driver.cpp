/**
 * Implementation for the FLIR Pan-Tilt Unit Driver.
 * @file Driver.cpp
 * @author Remus Claudiu Dumitru <r.dumitru@jacobs-university.de>
 * @date Wed Apr 11 13:25:02 CEST 2012
 */

//==============================================================================
// Includes
//==============================================================================
#include "Driver.h"
using namespace ptu;

#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;

#include <boost/current_function.hpp>
using namespace boost;

//==============================================================================
// Static members initialization
//==============================================================================
const int ptu::Driver::DEFAULT_BAUDRATE    = 19200;
const int ptu::Driver::MAX_PACKET_SIZE     = 8192;

//==============================================================================
// Implementation
//==============================================================================
bool Driver::write(const string& msg, const int& timeout) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    try {
        if (timeout == -1) {
            writePacket(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size());
        } else {
            writePacket(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size(), timeout);
        }
    } catch (iodrivers_base::TimeoutError e) {
        cerr << currName + ": timeout error" << endl;
        return false;
    } catch (...) {
        cerr << currName + ": unknown error" << endl;
        return false;
    }

    return true;
}

bool Driver::readAns(std::string& ans, int timeout) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    uint8_t buffer[MAX_PACKET_SIZE];
    size_t bufferSize = MAX_PACKET_SIZE;
    size_t packetSize;

    try {
        if (timeout < 0) {
            packetSize = readPacket(buffer, bufferSize);
        } else {
            packetSize = readPacket(buffer, bufferSize, timeout);
        }
    } catch (iodrivers_base::TimeoutError e) {
        cerr << currName + ": timeout error" << endl;
        return false;
    } catch (...) {
        cerr << currName + ": unknown error" << endl;
        return false;
    }

    ans.assign(reinterpret_cast<const char*>(buffer), packetSize);
    return true;
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

int Driver::extractPacket(const uint8_t* buffer, size_t size) const {
    string currName = string(BOOST_CURRENT_FUNCTION);

    // possible beginnings of a packet
    string begs = Cmd::SUCC_BEG + Cmd::ERR_BEG;

    // the current packet
    string packet(reinterpret_cast<const char*>(buffer), size);

    // find the beginning and the end of a packet
    size_t beg = packet.find_first_of(begs);
    size_t end = packet.find_first_of(Cmd::DELIM_CR);

    //cout << beg << " " << end << endl;

    // see the documentation of extractPacket for further details
    if (beg == string::npos && end == string::npos) {
        return (-size);
    } else if (beg != 0 && beg != string::npos) {
        return (-beg);
    } else if (beg == 0 && end == string::npos) {
        return 0;
    } else if (beg == 0 && end != string::npos) {
        return end + 1;
    } else {
        throw runtime_error(currName + ": invalid branch taken");
    }

    return 0;
}

Driver::Driver() :
        iodrivers_base::Driver(MAX_PACKET_SIZE),
        _baudrate(DEFAULT_BAUDRATE)
{}

Driver::~Driver() {
    if (this->isValid()) {
        this->close();
    }
}

bool Driver::open(const std::string& uri) {
//    bool ret = Driver::openSerial(uri, DEFAULT_BAUDRATE);
    Driver::openTCP(uri, 8080);

//    if (ret == false) {
//        return false;
//    }

    return true;
}

bool Driver::close() {
    iodrivers_base::Driver::close();
    return true;
}

bool Driver::getPos(const Axis& axis, const bool& offset, int& pos) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    bool ret = write(Cmd::getPos(axis, offset));
    if (!ret) return false;

    string ans;
    // TODO fix the timeout
    ret = readAns(ans, 10000);
    if (!ret) return false;

    string error;
    ret = validateAns(ans, error);
    if (!ret) {
        cerr << currName + ": " << error << endl;
        return false;
    }

    string toFind("position is ");
    size_t found = ans.find(toFind);

    if (found == string::npos) {
        cerr << currName + ": invalid answer format" << endl;
        return false;
    }

    stringstream ss;
    ss << ans.substr(found + toFind.size());
    ss >> pos;

    return true;
}

bool Driver::setPos(const int& val, const Axis& axis, const bool& offset) {
    string currName = string(BOOST_CURRENT_FUNCTION);

    bool ret = write(Cmd::setPos(val, axis, offset));
    if (!ret) return false;

    string ans;
    // TODO fix the timeout
    ret = readAns(ans, 10000);
    if (!ret) return false;

    string error;
    ret = validateAns(ans, error);
    if (!ret) {
        cerr << currName + ": " << error << endl;
        return false;
    }

    if (ans == Cmd::SUCC_CMD) {
        return true;
    } else {
        cerr << currName + ": failed to set position" << endl;
        return false;
    }

    return true;
}
