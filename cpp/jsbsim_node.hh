#ifndef JSBSIM_NODE_HH
#define JSBSIM_NODE_HH

#define RTI_USES_STD_FSTREAM

#include <RTI.hh>
#include <NullFederateAmbassador.hh>
#include <fedtime.hh>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <FGFDMExec.h>
#pragma GCC diagnostic pop

#include <boost/property_tree/ptree.hpp>

class jsbsim_node : public NullFederateAmbassador
{
public:

    jsbsim_node(const boost::property_tree::ptree&, const std::string&);
    virtual ~jsbsim_node() throw (RTI::FederateInternalError);

    virtual void declare();
    void join(std::string, std::string);
    void pause();
    virtual void publishAndSubscribe();
    void resign();
    void setTimeRegulation(bool constrained, bool regulating);
    void setVerbose(bool flag) { verbose = flag ; }
    void step();
    void synchronize(int);
    void tick();
    void tick2();

    RTI::FederateHandle getHandle() const ;

    // Callbacks
    void announceSynchronizationPoint(const char *label, const char *tag)
        throw (RTI::FederateInternalError);

    void federationSynchronized(const char *label)
        throw (RTI::FederateInternalError);

    void timeAdvanceGrant(const RTI::FedTime& theTime)
        throw (RTI::FederateInternalError, RTI::TimeAdvanceWasNotInProgress, 
	       RTI::InvalidFederationTime);

    void reflectAttributeValues(RTI::ObjectHandle theObject,
                                const RTI::AttributeHandleValuePairSet& theAttributes,
                                const RTI::FedTime& theTime, const char *theTag,
                                RTI::EventRetractionHandle theHandle)
        throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown,
	       RTI::InvalidFederationTime, RTI::FederateInternalError);

    void reflectAttributeValues(RTI::ObjectHandle,
                                const RTI::AttributeHandleValuePairSet &,
                                const char *)
        throw (RTI::ObjectNotKnown, RTI::AttributeNotKnown,
	       RTI::FederateInternalError);

    void sendUpdate(const RTI::FedTime &, RTI::ObjectHandle);

    bool getCreator(){return creator;};

	void init_fdm();
private:

    virtual void getHandles();
 
    RTI::RTIambassador rtiamb ;
	boost::property_tree::ptree _ptree;

    std::string federateName ;
    std::string federationName ;
	const char* _sync_point;
	std::string _robot_name;

    RTI::FederateHandle handle ;
    bool creator ;
    long nbTicks ;

    bool regulating ;
    bool constrained ;
	double _fps;
    RTIfedTime localTime ;
    const RTIfedTime TIME_STEP ;

    bool paused ;
    bool granted ;
    bool verbose ;

    // Handles
    RTI::ObjectClassHandle aircraft_class_id;
    RTI::AttributeHandle position_id;
    RTI::AttributeHandle orientation_id;
    RTI::AttributeHandle magnetometer_id;
    RTI::AttributeHandle control_id;

	int aircraft_id;

	double _x, _y, _z;
	double _yaw, _pitch, _roll;
	bool _has_mag;
	double _mag_x, _mag_y, _mag_z;
	std::map<std::string, float> _controls;

	JSBSim::FGFDMExec _fdm_exec;
	void feed_jsbsim();
	void jsbsim_step() { _fdm_exec.Run(); }
	void copy_jsbsim_output();
	void set_attribute(const std::string& attribute, double value)
	{
		_fdm_exec.GetPropertyManager()->GetNode(attribute)->SetDouble("", value);
	}

	bool has_attribute(const std::string& attribute)
	{
		return _fdm_exec.GetPropertyManager()->GetNode(attribute) != 0;
	}
		
	double get_attribute(const std::string& attribute)
	{
		return _fdm_exec.GetPropertyManager()->GetNode(attribute)->GetDouble("");
	}
};

#endif // JSBSIM_NODE_HH
