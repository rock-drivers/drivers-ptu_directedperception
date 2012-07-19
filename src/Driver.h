/**
  * Definition of FLIR Pan-Tilt Unit Driver.
  * @file Driver.h
  * @author Remus Claudiu Dumitru <r.dumitru@jacobs-university.de>
  * @date Wed Apr 11 CEST 2012
  */

#ifndef _DRIVER_H
#define _DRIVER_H

//==============================================================================
// Includes
//==============================================================================
#include "iodrivers_base/Driver.hpp"
//#include "Driver.hpp"    // include I am using for eclipse

#include "Cmd.h"

//==============================================================================
// Declaration
//==============================================================================
namespace ptu {

class Driver : public iodrivers_base::Driver {
private:
    static const int DEFAULT_BAUDRATE;  //!< The default baudrate that the ptu starts with.
    static const int MAX_PACKET_SIZE;   //!< The maximum packet size.

    int _baudrate;  //!< The current baudrate.


protected:
    /**
     * Find a packet into the currently accumulated data.
     */
    int extractPacket(const uint8_t* buffer, size_t size) const;


public:
    
    // Constructors and destructors.
    Driver();
    ~Driver();

    /**
     * Sends a message to the device.
     * @param msg the message to be sent
     * @param timeout timeout in ms
     * @return true if successful
     */
    bool write(const std::string& msg, const int& timeout = -1);

    /**
     * Read the answer of a query.
     * @param ans the answer of the query
     * @param timeout timeout in ms
     * @return the size of the data contained in \c buffer
     */
    bool readAns(std::string& ans, int timeout = -1);

    /**
     * Tells if an answer is correct or not.
     * @param error the error in case the answer is invalid
     * @return false if the answer is invalid
     */
    bool validateAns(const std::string& ans, std::string& error);

    /**
     * Closes the device.
     */
    bool close();

    /**
     * Get current pan-tilt position.
     * @param axis the axis to be used
     * @param offset if true, the relative value is used
     * @return true if successful
     */
    bool getPos(const Axis& axis, const bool& offset, int& pos);

    /**
     * Set current pan-tilt position.
     * @param val the value to which to set the position
     * @param axis the axis to be used
     * @param offset if true, the relative value is used
     * @return true if successful
     */
    bool setPos(const Axis& axis, const bool& offset = false, const int& val = 0);
};

} // end of namespace ptu

#endif // _DRIVER_H
