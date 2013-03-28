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
#include <base/logging.h>
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
    static const float DEGREEPERTICK; //Degrees per tick. (same for TILT AND PAN) //TODO maybe calculated??
    static const float DEGREEPERSECARC;

    float mPanResolutionDeg;
    float mTiltResolutionDeg;
    
    float mMinPanRad;
    float mMaxPanRad;
    float mMinTiltRad;
    float mMaxTiltRad;

    int mTimeout; //<! Timeout in milliseconds(?).

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

    /** The minimum pan postion in rad. */
    float getMinPanRad() { return mMinPanRad; }
    /** The maximum pan postion in rad. */
    float getMaxPanRad() { return mMaxPanRad; }
    /** The minimum tilt postion in rad. */
    float getMinTiltRad() { return mMinTiltRad; }
    /** The maximum tilt postion in rad. */
    float getMaxTiltRad() { return mMaxTiltRad; }

    /** Initial communication with the device to set proper modes and query limits. */
    bool initialize();

    /**
     * Sends a message to the device.
     * @param msg the message to be sent
     * @param timeout timeout in ms
     * @throws iodrivers_base write errors
     */
    void write(const std::string& msg, const int& timeout = -1);

    /**
     * Read the answer of a query.
     * @param ans the answer of the query
     * @param timeout timeout in ms
     * @return the size of the data contained in \c buffer
     */
    //TODO fix timeout!
    bool readAns(std::string& ans, int timeout = 10000);

    /**
     * Tells if an answer is correct or not.
     * @param error the error in case the answer is invalid
     * @return false if the answer is invalid
     */
    bool validateAns(const std::string& ans, std::string& error);

    /**
     * Get an answer. Combines readAns and validateAns.
     */
    bool getAns(std::string& ans);

    /**
     * Parse the query \p result form the string \p ans.
     * The query result will be like '* <result><CR>'.
     * @return false if something went wrong.
     */
    template<typename T>
    bool getQueryResult(const std::string& ans, T& result);

    /**
     * Get current pan-tilt position.
     * @param axis the axis to be used
     * @param offset if true, the relative value is used
     * @return true if successful
     */
    bool getPos(const Axis& axis, const bool& offset, int& pos);

    /**
     * Get the position as degree value instead of ticks as given by getPos.
     * @param axis Select the axis to be read out. (PAN or TILT)
     * @param offset if true the offset command will be used.
     * @return the position of selected axis as degree value. (0 is front center).
     *  
     */
    float getPosDeg(const Axis &axis, const bool &offset);
    
    /** Get the position in radian. @see getPos */
    float getPosRad(const Axis &axis, const bool &offset);


    /**
      * Set the Position for selected axis to given value in degree.
      * @param axis Specify the axis which should be set (PAN, TILT).
      * @param offset Select if the position should be set as an offset from current position
      *               or if false it will be set as absolute value (while 0 is front center).
      * @param awaitCompletion Set if movement should be completed before processing next command.
      * @return Returns bool value. true if success, false otherwise. 
      */
    bool setPosDeg(const Axis &axis, const bool &offset, const float &val, const bool &awaitCompletion = false);

    /** Set the position in radian. @see setPos */
    bool setPosRad(const Axis &axis, const bool &offset, const float &val, const bool &awaitCompletion = false);
    /**
     * Set current pan-tilt position.
     * @param val the value to which to set the position
     * @param axis the axis to be used
     * @param offset if true, the relative value is used
     * @param awaitCompletion force the command to be completed 
     *        before next command will be processed.
     * @return true if successful
     */
    bool setPos(const Axis& axis, const bool& offset = false, const int& val = 0, 
                const bool& awaitCompletion = false);

    /**
     * Set desired \p speed for an \p axis in positions/second.
     */
    bool setSpeed(const Axis& axis, const int& speed);
    
    /**
     * Set desired \p speed for an \p axis in degree/second.
     */
    bool setSpeedDeg(const Axis& axis, const float& speed);
    
    /**
     * Set desired \p speed for an \p axis in radian/second.
     */
    bool setSpeedRad(const Axis& axis, const float& speed);

    /**
     * Stops motion.
     */
    bool setHalt();
};
    
template<typename T>
bool Driver::getQueryResult(const std::string& ans, T& result) {

    std::string toFind("* ");
    size_t found = ans.find(toFind);

    if (found == std::string::npos) {
        LOG_ERROR_S << "getPos: invalid answer format";
        return false;
    }

    std::stringstream ss;
    ss << ans.substr(found + toFind.size());
    ss >> result;

    return true;
}

} // end of namespace ptu

#endif // _DRIVER_H
