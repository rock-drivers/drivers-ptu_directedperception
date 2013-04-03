/**
  * Definition of FLIR Pan-Tilt Unit Driver.
  * @file Driver.h
  * @author Remus Claudiu Dumitru <r.dumitru@jacobs-university.de>
  */

#ifndef _DRIVER_H
#define _DRIVER_H

//==============================================================================
// Includes
//==============================================================================
#include <boost/lexical_cast.hpp>
#include <base/logging.h>
#include "iodrivers_base/Driver.hpp"

#include "Cmd.h"

//==============================================================================
// Declaration
//==============================================================================
namespace ptu {

class Driver : public iodrivers_base::Driver {
private:
    static const int DEFAULT_BAUDRATE;  //!< The default baudrate that the ptu starts with.
    static const int MAX_PACKET_SIZE;   //!< The maximum packet size.
    static const float DEGREEPERTICK; //!< Degrees per tick. (same for TILT AND PAN) //TODO maybe calculated??
    static const float DEGREEPERSECARC; //!<  Used for computing the resolution.

    float mPanResolutionDeg;
    float mTiltResolutionDeg;
    
    float mMinPanRad;
    float mMaxPanRad;
    float mMinTiltRad;
    float mMaxTiltRad;

    int mTimeout; //<! Timeout in milliseconds(?).


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
    void initialize();

    /**
     * Sends a message to the device.
     * @param msg the message to be sent
     * @param timeout timeout in ms
     * @throws iodrivers_base write errors
     */
    void write(const std::string& msg, int timeout = -1);

    /**
     * Read the answer of a query.
     * @param timeout timeout in ms
     * @return answer string
     */
    //TODO fix timeout! Could be remove could be set via iodrivers_base?
    std::string readAns(int timeout = 10000);

    /**
     * Converts an \p answer to a value of type T.
     * The answer string is like '* <result><CR>'.
     */
    template<typename T>
    T getQuery(const std::string& answer) {
        return boost::lexical_cast<T>( answer.substr(2) );
    }

    /**
     * Get current pan-tilt position.
     * @param axis the axis to be used
     * @param offset if true, the relative value is used
     * @return position
     */
    int getPos(Axis axis, bool offset);

    /**
     * Get the position as degree value instead of ticks as given by getPos.
     * @param axis Select the axis to be read out. (PAN or TILT)
     * @param offset if true the offset command will be used.
     * @return the position of selected axis as degree value. (0 is front center). 
     */
    float getPosDeg(Axis axis, bool offset);
    
    /** Get the position in radian. @see getPos */
    float getPosRad(Axis axis, bool offset);

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

    /** Set desired \p speed for an \p axis in positions/second. */
    bool setSpeed(const Axis& axis, const int& speed);
    
    /** Set desired \p speed for an \p axis in degree/second. */
    bool setSpeedDeg(const Axis& axis, const float& speed);
    
    /** Set desired \p speed for an \p axis in radian/second. */
    bool setSpeedRad(const Axis& axis, const float& speed);

    /** Stops motion. */
    void setHalt();
};
    
} // end of namespace ptu

#endif // _DRIVER_H
