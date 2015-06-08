#include <jsbsim_node.hh>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <unistd.h>
#include <MessageBuffer.hh>

#include <memory>

#include <FGJSBBase.h>
#include <initialization/FGInitialCondition.h>

// ----------------------------------------------------------------------------
/** Constructor
 */
jsbsim_node::jsbsim_node(std::string federate_name)
    : rtiamb(),
      federateName(federate_name),
      handle(0),
      creator(false),
      nbTicks(0),
      regulating(false),
      constrained(false),
      localTime(0.0),
      TIME_STEP(1.0 / 60),
	  _control({1.0, 1.0, 1.0, 1.0})
{
}

// ----------------------------------------------------------------------------
/** Destructor
 */
jsbsim_node::~jsbsim_node()
    throw (RTI::FederateInternalError)
{
}

// ----------------------------------------------------------------------------
/** Get the federate handle
 */
RTI::FederateHandle
jsbsim_node::getHandle() const
{
    return handle ;
}

// ----------------------------------------------------------------------------
/** Join the federation
    \param federation_name Federation name
    \param fdd_name Federation designator (.fed file)
 */
void
jsbsim_node::join(std::string federation_name, std::string fdd_name)
{
    federationName = federation_name ;

    // create federation
    try {
        rtiamb.createFederationExecution(federation_name.c_str(),
                                         fdd_name.c_str());
		std::cerr <<  "Federation execution created." << std::endl;
        creator = true ;
    }
    catch (RTI::FederationExecutionAlreadyExists& e) {
		std::cerr << "Federation execution already created." << std::endl;
    }
    catch (RTI::CouldNotOpenFED& e) {
		std::cerr << "Error catched " << e._name << " " << e._reason << std::endl;
        exit(0);
    }

	handle = rtiamb.joinFederationExecution(federateName.c_str(),
											federation_name.c_str(),
											this);
}

// ----------------------------------------------------------------------------
/** Creator put federation in pause.
 */
void
jsbsim_node::pause()
{
    if (creator) {
		std::cerr << "Pause requested\n" << std::endl;
		rtiamb.registerFederationSynchronizationPoint("Init", "Waiting all players.");
    }
}

// ----------------------------------------------------------------------------
/** tick the RTI
 */
void
jsbsim_node::tick()
{
    usleep( 0 ) ;
    rtiamb.tick();
    nbTicks++ ;
}
void
jsbsim_node::tick2()
{
    rtiamb.tick2();
    nbTicks++ ;
}

// ----------------------------------------------------------------------------
/** Set time regulation (time regulating and time constrained)
    @param start_constrained boolean, if true federate is constrained
    @param start_regulating boolean, if true federate is regulating
 */
void
jsbsim_node::setTimeRegulation(bool start_constrained, bool start_regulating)
{
	std::cerr << "Time Regulation setup" << std::endl;

    if (start_constrained) {
        if (!constrained) {
            // change from no constrained to constrained
            rtiamb.enableTimeConstrained();
            constrained = true ;
			std::cerr << "Time Constrained enabled." << std::endl;
        }
        //rtiamb.modifyLookahead(TIME_STEP);
    }
    else {
        if (constrained) {
            // change from constrained to no constrained
            rtiamb.disableTimeConstrained();
            constrained = false ;
			std::cerr << "Time Constrained disabled." << std::endl;
        }
    }

    if (start_regulating) {
        if (!regulating) {
            // change from no regulating to regulating
            for (;;) {
                rtiamb.queryFederateTime(localTime);

                try {
                    rtiamb.enableTimeRegulation(localTime, TIME_STEP);
                    regulating = true ;
                    break ;
                }
                catch (RTI::FederationTimeAlreadyPassed) {
                    // Si Je ne suis pas le premier, je vais les rattraper.
                    rtiamb.queryFederateTime(localTime);

                    RTIfedTime requestTime(((RTIfedTime&)localTime).getTime());
                    requestTime += TIME_STEP ;

		    granted = false ;
                    rtiamb.timeAdvanceRequest(requestTime);
                    while (!granted) {
                        try {
                            tick();
                        }
                        catch (RTI::RTIinternalError) {
							std::cerr << "RTIinternalError Raised in tick." << std::endl;
                            exit(-1);
                        }
                    }
                }
                catch (RTI::RTIinternalError) {
					std::cerr << "RTIinternalError Raised in setTimeRegulating" << std::endl;
                    exit(-1);
                }
            }
        }
    }
    else {
        if (regulating) {
            // change from regulating to no regulating
            rtiamb.disableTimeRegulation();
            regulating = false ;
        }
    }
}

// ----------------------------------------------------------------------------
/** Synchronize with other federates
 */
void
jsbsim_node::synchronize(int autostart)
{
	std::cerr << "Synchronize" << std::endl;

    if (creator) {
        // Wait a signal from user and stop the pause synchronization.

		std::cout << "Press ENTER to start execution..." << std::endl ;
		getchar();

		std::cerr << "Creator can resume execution..." << std::endl;
        while (!paused)
            try {
				std::cerr << "Not paused!" << std::endl;
                tick();
            }
            catch (RTI::Exception& e) {
                throw ;
            }

        try {
            rtiamb.synchronizationPointAchieved("Init");
        }
        catch (RTI::Exception& e) {
        }

        while (paused)
            try {
                tick();
            }
            catch (RTI::Exception& e) {
                throw ;
            }
    }
    else {
		std::cerr << "Synchronization..." << std::endl;

        if (!paused) {
            while (!paused) {
                try {
                    tick();
                }
                catch (RTI::Exception& e) {
                    throw ;
                }
            }
        }
		std::cerr << "Federate paused" << std::endl;

        try {
            // Federate ends its synchronization.
            rtiamb.synchronizationPointAchieved("Init");
			std::cerr << "Pause achieved." << std::endl;
        }
        catch (RTI::Exception& e) {
        }

        while (paused) {
            try {
                tick();
            }
            catch (RTI::Exception& e) {
                throw ;
            }
        }
		std::cerr << "End of pause" << std::endl;
    }

	std::cerr << "Federation is synchronized." << std::endl;
}

// ----------------------------------------------------------------------------
/** create objects, regions, etc.
 */
void
jsbsim_node::declare()
{
    aircraft_id = rtiamb.registerObjectInstance(aircraft_class_id, "robot");
}

// ----------------------------------------------------------------------------
/** one simulation step advance)
 */
void
jsbsim_node::step()
{
    granted = false ;

    try {
        rtiamb.queryFederateTime(localTime);
    }
    catch (RTI::Exception& e) {
    }

    try {
        RTIfedTime time_aux(localTime.getTime()+TIME_STEP.getTime());

        granted = false ;
		std::cerr << "timeAdvanceRequest " << localTime.getTime() + TIME_STEP.getTime() << std::endl;
        rtiamb.timeAdvanceRequest(time_aux);
    }
    catch (RTI::Exception& e) {
    }


    while (!granted) {
        try {
			std::cerr << "Waiting for grant " << std::endl;
            tick2();
        }
        catch (RTI::Exception& e) {
            throw ;
        }
    }

    RTIfedTime next_step(localTime + TIME_STEP);

	// step
	feed_jsbsim();
	jsbsim_step();

	// update
	copy_jsbsim_output();
	sendUpdate(next_step, aircraft_id);

}

// ----------------------------------------------------------------------------
/** resign the federation
 */
void
jsbsim_node::resign()
{
    try {
        rtiamb.deleteObjectInstance(aircraft_id, localTime, "DO");
    }
    catch (RTI::Exception &e) {
    }


    setTimeRegulation(false, false);

    try {
        rtiamb.resignFederationExecution(
            RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
    }
    catch (RTI::Exception &e) {
    }
    // Detruire la federation

    if (creator) {
        for (;;) {
            tick();
            try {
                break ;
            }
            catch (RTI::FederatesCurrentlyJoined) {
                sleep(5);
            }
        }
    }
}

// ----------------------------------------------------------------------------
/** Carry out publications and subscriptions
 */
void
jsbsim_node::publishAndSubscribe()
{
    // Get all class and attributes handles
    getHandles();

	std::auto_ptr<RTI::AttributeHandleSet> attributes_pub(RTI::AttributeHandleSetFactory::create(2));
    attributes_pub->add(position_id);
    attributes_pub->add(orientation_id);

	std::auto_ptr<RTI::AttributeHandleSet> attributes_sub(RTI::AttributeHandleSetFactory::create(1));
	attributes_sub->add(control_id);

    rtiamb.subscribeObjectClassAttributes(aircraft_class_id, *attributes_sub, RTI::RTI_TRUE);
    rtiamb.publishObjectClass(aircraft_class_id, *attributes_pub);
}

// ----------------------------------------------------------------------------
/** get handles of objet/interaction classes
 */
void
jsbsim_node::getHandles()
{
    aircraft_class_id = rtiamb.getObjectClassHandle("Aircraft");

    // Attributs des classes d'Objets
    position_id = rtiamb.getAttributeHandle("position", aircraft_class_id);
    orientation_id = rtiamb.getAttributeHandle("orientation", aircraft_class_id);
    control_id = rtiamb.getAttributeHandle("control", aircraft_class_id);
}


// ----------------------------------------------------------------------------
/** Updates a ball by sending entity position and color.
    \param x X position
    \param y Y position
    \param color Color
    \param UpdateTime Event time
    \param id Object handle (ball)
 */
void
jsbsim_node::sendUpdate(
			const RTI::FedTime& UpdateTime,
		    RTI::ObjectHandle id)
{
    libhla::MessageBuffer buffer;
    RTI::AttributeHandleValuePairSet *attributeSet ;

    attributeSet = RTI::AttributeSetFactory::create(2);

    buffer.reset();
    buffer.write_double(_x);
	buffer.write_double(_y);
	buffer.write_double(_z);
    buffer.updateReservedBytes();
    attributeSet->add(position_id, static_cast<char*>(buffer(0)),buffer.size());

    buffer.reset();
    buffer.write_double(_yaw);
    buffer.write_double(_pitch);
    buffer.write_double(_roll);
    buffer.updateReservedBytes();
    attributeSet->add(orientation_id, static_cast<char*>(buffer(0)),buffer.size());


    try {
		rtiamb.updateAttributeValues(id, *attributeSet, UpdateTime, "coucou");
    }
    catch (RTI::Exception& e) {
        std::cout<<"Exception "<<e._name<<" ("<<e._reason<<")"<<std::endl;
    }

    delete attributeSet ;
}


// ============================================================================
// FEDERATE AMBASSADOR CALLBACKS
// ============================================================================


// ----------------------------------------------------------------------------
/** Callback announce synchronization point
 */
void
jsbsim_node::announceSynchronizationPoint(const char *label, const char */*tag*/)
    throw (RTI::FederateInternalError)
{
    if (strcmp(label, "Init") == 0) {
        paused = true ;
    }
    else {
        std::cout << "Unexpected synchronization label" << std::endl ;
        exit(1);
    }
}

// ----------------------------------------------------------------------------
/** Callback : federation synchronized
 */
void
jsbsim_node::federationSynchronized(const char *label)
    throw (RTI::FederateInternalError)
{
    if (strcmp(label, "Init") == 0) {
        paused = false ;
    }
}


// ----------------------------------------------------------------------------
/** Callback : reflect attribute values with time
 */
void
jsbsim_node::reflectAttributeValues(
    RTI::ObjectHandle theObject,
    const RTI::AttributeHandleValuePairSet& theAttributes,
    const RTI::FedTime& /*theTime*/,
    const char */*theTag*/,
    RTI::EventRetractionHandle /*theHandle*/)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotKnown,
           RTI::InvalidFederationTime,
           RTI::FederateInternalError)
{
    libhla::MessageBuffer buffer;

    RTI::ULong valueLength ;

    for (unsigned int j=0 ; j<theAttributes.size(); j++) {

        RTI::AttributeHandle parmHandle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        assert(valueLength>0);
        buffer.resize(valueLength);
        buffer.reset();
        theAttributes.getValue(j, static_cast<char*>(buffer(0)), valueLength);
        buffer.assumeSizeFromReservedBytes();

        if (parmHandle == control_id)
			buffer.read_floats(_control, sizeof(_control)/sizeof(_control[0]));
    }
}

// ----------------------------------------------------------------------------
/** Callback : reflect attribute values without time
 */
void
jsbsim_node::reflectAttributeValues(
    RTI::ObjectHandle theObject,
    const RTI::AttributeHandleValuePairSet& theAttributes,
    const char */*theTag*/)
    throw (RTI::ObjectNotKnown,
           RTI::AttributeNotKnown,
           RTI::FederateInternalError)
{
    libhla::MessageBuffer buffer;

    RTI::ULong valueLength ;

    for (unsigned int j=0 ; j<theAttributes.size(); j++) {

        RTI::AttributeHandle parmHandle = theAttributes.getHandle(j);
        valueLength = theAttributes.getValueLength(j);
        assert(valueLength>0);
        buffer.resize(valueLength);
        buffer.reset();
        theAttributes.getValue(j, static_cast<char*>(buffer(0)), valueLength);
        buffer.assumeSizeFromReservedBytes();

        if (parmHandle == control_id)
			buffer.read_floats(_control, sizeof(_control)/sizeof(_control[0]));
    }
}

// ----------------------------------------------------------------------------
/** Callback : time advance granted
 */
void
jsbsim_node::timeAdvanceGrant(const RTI::FedTime& theTime)
    throw (RTI::InvalidFederationTime, RTI::TimeAdvanceWasNotInProgress,
	   RTI::FederateInternalError)
{
    granted = true ;
    localTime = theTime ;
}

void jsbsim_node::feed_jsbsim()
{
	set_attribute("fcs/front_motor", _control[0]);
	set_attribute("fcs/back_motor", _control[1]);
	set_attribute("fcs/left_motor", _control[2]);
	set_attribute("fcs/right_motor", _control[3]);
}

#define GROUND_ALT 130.0
/// Macro to convert from feet to metres
#define MetersOfFeet(_f) ((_f)/3.2808399)
#define FeetOfMeters(_m) ((_m)*3.2808399)

/**
 * getenv with default value if the variable is not set
 */
std::string getenv(const std::string& name, const std::string& def) {
    const char* value = std::getenv( name.c_str() );
    return value ? value : def;
}


void
jsbsim_node::init_fdm(const std::string& model)
{
	std::string rootdir = getenv("JSBSIM_MORSE_HOME", "jsbsim/");
	_fdm_exec.LoadModel(rootdir + "aircraft",
					   rootdir + "engine",
					   rootdir + "systems",
					   model, false);


	_fdm_exec.Setdt(1.0 / 60.0);
	JSBSim::FGInitialCondition *IC = _fdm_exec.GetIC();
    IC->SetVgroundFpsIC(0.);

    // Use flight plan initial conditions
    // convert geodetic lat from flight plan to geocentric
    IC->SetLatitudeDegIC(43.36);
    IC->SetLongitudeDegIC(1.26);

    IC->SetWindNEDFpsIC(0.0, 0.0, 0.0);
    IC->SetAltitudeASLFtIC(FeetOfMeters(GROUND_ALT + 2.0));
    IC->SetTerrainElevationFtIC(FeetOfMeters(GROUND_ALT));
    IC->SetPsiDegIC(0.0);
    IC->SetVgroundFpsIC(0.);

	feed_jsbsim();
	_fdm_exec.RunIC(); // loop JSBSim once w/o integrating
}

void
jsbsim_node::copy_jsbsim_output()
{
	JSBSim::FGPropagate* propagate = _fdm_exec.GetPropagate();
	_x = propagate->GetLocation().Entry(1);
	_y = propagate->GetLocation().Entry(2);
	_z = propagate->GetLocation().Entry(3);

	_yaw = propagate->GetEuler(3);
	_pitch = propagate->GetEuler(2);
	_roll = propagate->GetEuler(1);
}
