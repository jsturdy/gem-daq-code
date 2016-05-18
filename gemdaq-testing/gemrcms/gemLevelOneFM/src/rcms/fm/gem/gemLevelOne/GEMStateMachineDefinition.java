package rcms.fm.gem.gemLevelOne;

import rcms.fm.fw.parameter.CommandParameter;
import rcms.fm.fw.parameter.ParameterException;
import rcms.fm.fw.parameter.ParameterSet;
import rcms.fm.fw.parameter.type.IntegerT;
import rcms.fm.fw.parameter.type.LongT;
import rcms.fm.fw.parameter.type.StringT;
import rcms.fm.fw.user.UserStateMachineDefinition;
import rcms.statemachine.definition.State;
import rcms.statemachine.definition.StateMachineDefinitionException;

/**
 * This class defines the Finite State Machine for the GEM level 1 Function Manager.
 * 
 * The actual definition of the State Machine must be put in the "init" method.
 * 
 * @originalauthor Andrea Petrucci, Alexander Oh, Michele Gulmini
 * @author Jared Sturdy
 */
public class GEMStateMachineDefinition extends UserStateMachineDefinition {
    
    public GEMStateMachineDefinition() throws StateMachineDefinitionException {
	//
	// Defines the States for this Finite State Machine.
	//
	
	// steady states
	addState(GEMStates.INITIAL                 );
	addState(GEMStates.HALTED                  );
	addState(GEMStates.CONFIGURED              );
	addState(GEMStates.ENABLED                 );
	addState(GEMStates.RUNNING                 );
	addState(GEMStates.RUNNINGDEGRADED         );
	addState(GEMStates.RUNNINGSOFTERRORDETECTED);
	addState(GEMStates.PAUSED                  );
	addState(GEMStates.TTSTEST_MODE            );
	addState(GEMStates.ERROR                   );
		
	// transitional states
	addState(GEMStates.INITIALIZING);
	addState(GEMStates.ENABLING    );
	addState(GEMStates.CONFIGURING );
	addState(GEMStates.HALTING     );
	addState(GEMStates.PAUSING     );
	addState(GEMStates.RESUMING    );
	addState(GEMStates.STARTING    );
	addState(GEMStates.STOPPING    );
	addState(GEMStates.RECOVERING  );
	addState(GEMStates.RESETTING   );

	addState(GEMStates.PREPARING_TTSTEST_MODE);
	addState(GEMStates.TESTING_TTS           );

	//
	// Defines the Initial state.
	//
	setInitialState(GEMStates.INITIAL);

	//
	// Defines the Inputs (Commands) for this Finite State Machine.
	//
	addInput(GEMInputs.INITIALIZE);
	addInput(GEMInputs.ENABLE    );
	addInput(GEMInputs.CONFIGURE );
	addInput(GEMInputs.START     );
	addInput(GEMInputs.STOP      );
	addInput(GEMInputs.PAUSE     );
	addInput(GEMInputs.RESUME    );
	addInput(GEMInputs.HALT      );
	addInput(GEMInputs.RECOVER   );
	addInput(GEMInputs.RESET     );
	addInput(GEMInputs.SETERROR    );
	addInput(GEMInputs.PREPARE_TTSTEST_MODE);
	addInput(GEMInputs.TEST_TTS            );
	addInput(GEMInputs.COLDRESET   );
        addInput(GEMInputs.FIXSOFTERROR);
		
	// The SETERROR Input moves the FSM in the ERROR State.
	// This command is not allowed from the GUI.
	// It is instead used inside the FSM callbacks.
	GEMInputs.SETERROR.setVisualizable(false);

	// invisible commands needed for fully asynchronous behaviour
	addInput(GEMInputs.SETENABLED   );
	addInput(GEMInputs.SETCONFIGURED);
	addInput(GEMInputs.SETRUNNING   );
        addInput(GEMInputs.SETRUNNINGDEGRADED);
        addInput(GEMInputs.SETRUNNINGSOFTERRORDETECTED);
	addInput(GEMInputs.SETPAUSED    );
	addInput(GEMInputs.SETRESUMED   );
        addInput(GEMInputs.SETRESUMEDDEGRADED);
        addInput(GEMInputs.SETRESUMEDSOFTERRORDETECTED);
	addInput(GEMInputs.SETHALTED    );
	addInput(GEMInputs.SETINITIAL   );
	addInput(GEMInputs.SETRESET     );
	addInput(GEMInputs.SETTTSTEST_MODE);
	// addInput(GEMInputs.SETTESTING_TTS);
		
		
	// make these invisible
	GEMInputs.SETCONFIGURED.setVisualizable(false);
	GEMInputs.SETRUNNING.setVisualizable(false);
        GEMInputs.SETRUNNINGDEGRADED.setVisualizable(false);
        GEMInputs.SETRUNNINGSOFTERRORDETECTED.setVisualizable(false);
	GEMInputs.SETPAUSED.setVisualizable(false);
	GEMInputs.SETRESUMED.setVisualizable(false);
        GEMInputs.SETRESUMEDDEGRADED.setVisualizable(false);
        GEMInputs.SETRESUMEDSOFTERRORDETECTED.setVisualizable(false);
	GEMInputs.SETHALTED.setVisualizable(false);
	GEMInputs.SETINITIAL.setVisualizable(false);
	GEMInputs.SETRESET.setVisualizable(false);
	GEMInputs.SETTTSTEST_MODE.setVisualizable(false);
        
	//
	// Define command parameters.
	// These are then visible in the default GUI.
	//
		
	// define parameters for tts testing command
	//
	CommandParameter<IntegerT> ttsTestFedid          = new CommandParameter<IntegerT>(GEMParameters.TTS_TEST_FED_ID,          new IntegerT(-1));
	CommandParameter<StringT>  ttsTestMode           = new CommandParameter<StringT>( GEMParameters.TTS_TEST_MODE,            new StringT("") );
	CommandParameter<StringT>  ttsTestPattern        = new CommandParameter<StringT>( GEMParameters.TTS_TEST_PATTERN,         new StringT("") );
	CommandParameter<IntegerT> ttsTestSequenceRepeat = new CommandParameter<IntegerT>(GEMParameters.TTS_TEST_SEQUENCE_REPEAT, new IntegerT(-1));
		
	// define parameter set
	ParameterSet<CommandParameter> ttsTestParameters = new ParameterSet<CommandParameter>();
	try {
	    ttsTestParameters.add(ttsTestFedid);
	    ttsTestParameters.add(ttsTestMode);
	    ttsTestParameters.add(ttsTestPattern);
	    ttsTestParameters.add(ttsTestSequenceRepeat);
	} catch (ParameterException nothing) {
	    // Throws an exception if a parameter is duplicate
	    throw new StateMachineDefinitionException( "Could not add to ttsTestParameters. Duplicate Parameter?", nothing );
	}
		
	// set the test parameters
	GEMInputs.TEST_TTS.setParameters(ttsTestParameters);
	
	//
	// define parameters for Initialize command
	//
	CommandParameter<IntegerT> initializeSid                    = new CommandParameter<IntegerT>(GEMParameters.SID, new IntegerT(0));
	CommandParameter<StringT>  initializeGlobalConfigurationKey = new CommandParameter<StringT>(GEMParameters.GLOBAL_CONF_KEY, new StringT(""));

	// define parameter set
	ParameterSet<CommandParameter> initializeParameters = new ParameterSet<CommandParameter>();
	try {
	    initializeParameters.add(initializeSid);
	    initializeParameters.add(initializeGlobalConfigurationKey);
	} catch (ParameterException nothing) {
	    // Throws an exception if a parameter is duplicate
	    throw new StateMachineDefinitionException( "Could not add to initializeParameters. Duplicate Parameter?", nothing );
	}
		
	GEMInputs.INITIALIZE.setParameters(initializeParameters);
	
	//
	// define parameters for Configure command
	//
        CommandParameter<StringT>  configureFedEnableMask = new CommandParameter<StringT>(GEMParameters.FED_ENABLE_MASK, new StringT(""));
        CommandParameter<IntegerT> configureRunNumber     = new CommandParameter<IntegerT>(GEMParameters.RUN_NUMBER, new IntegerT(-1));
        CommandParameter<StringT>  configureRunKey        = new CommandParameter<StringT>(GEMParameters.RUN_KEY, new StringT(""));
        CommandParameter<StringT>  configureRunType       = new CommandParameter<StringT>(GEMParameters.RUN_TYPE, new StringT(""));

	// define parameter set
	ParameterSet<CommandParameter> configureParameters = new ParameterSet<CommandParameter>();
	try {
	    configureParameters.add(configureFedEnableMask);
	    configureParameters.add(configureRunNumber);
	    configureParameters.add(configureRunKey);
	    configureParameters.add(configureRunType);
	} catch (ParameterException nothing) {
	    // Throws an exception if a parameter is duplicate
	    throw new StateMachineDefinitionException( "Could not add to configureParameters. Duplicate Parameter?", nothing );
	}
		
	GEMInputs.CONFIGURE.setParameters(configureParameters);
	
	//
	// define parameters for Start command
	//
	CommandParameter<IntegerT> startRunNumber = new CommandParameter<IntegerT>(GEMParameters.RUN_NUMBER, new IntegerT(-1));

	// define parameter set
	ParameterSet<CommandParameter> startParameters = new ParameterSet<CommandParameter>();
	try {
	    startParameters.add(startRunNumber);
	} catch (ParameterException nothing) {
	    // Throws an exception if a parameter is duplicate
	    throw new StateMachineDefinitionException( "Could not add to startParameters. Duplicate Parameter?", nothing );
	}
		
	GEMInputs.START.setParameters(startParameters);
	
		
        //
        // define parameters for FixSoftError command
        //
        CommandParameter<LongT> triggerNumberAtPause = new CommandParameter<LongT>(GEMParameters.TRIGGER_NUMBER_AT_PAUSE, new LongT(-1));

        // define parameter set
        ParameterSet<CommandParameter> fixSoftErrorParameters = new ParameterSet<CommandParameter>();
        try {
            fixSoftErrorParameters.add(triggerNumberAtPause);
        } catch (ParameterException nothing) {
            // Throws an exception if a parameter is duplicate
            throw new StateMachineDefinitionException( "Could not add to fixSoftErrorParameters. Duplicate Parameter?", nothing );
        }

        GEMInputs.FIXSOFTERROR.setParameters(fixSoftErrorParameters);

        //
	// Define the State Transitions
	//

	// INIT Command:
	// The INIT input is allowed only in the INITIAL state, and moves the
	// FSM to the INITIALIZING state.
	//
	addTransition(GEMInputs.INITIALIZE, GEMStates.INITIAL, GEMStates.INITIALIZING);
	
	// TEST_MODE Command:
	// The TEST_MODE input is allowed in the HALTED state and moves 
	// the FSM to the PREPARING_TEST_MODE state.
	//
	addTransition(GEMInputs.PREPARE_TTSTEST_MODE, GEMStates.HALTED, GEMStates.PREPARING_TTSTEST_MODE);

	// Reach the TEST_MODE State
	addTransition(GEMInputs.SETTTSTEST_MODE, GEMStates.PREPARING_TTSTEST_MODE, GEMStates.TTSTEST_MODE);
	addTransition(GEMInputs.SETTTSTEST_MODE, GEMStates.TESTING_TTS,            GEMStates.TTSTEST_MODE);

	// TEST_TTS Command:
	// The TEST_TTS input is allowed in the TEST_MODE state and moves
	// the FSM to the TESTING_TTS state.
	addTransition(GEMInputs.TEST_TTS, GEMStates.TTSTEST_MODE, GEMStates.TESTING_TTS);

        // ColdReset  Command:
        addTransition(GEMInputs.COLDRESET, GEMStates.HALTED, GEMStates.COLDRESETTING);

        // CONFIGURE Command:
	// The CONFIGURE input is allowed only in the HALTED state, and moves
	// the FSM to the CONFIGURING state.
	//
	addTransition(GEMInputs.CONFIGURE, GEMStates.HALTED, GEMStates.CONFIGURING);

	// START Command:
	// The START input is allowed only in the CONFIGURED state, and moves
	// the FSM to the STARTING state.
	//
	addTransition(GEMInputs.START, GEMStates.CONFIGURED, GEMStates.STARTING);

	// PAUSE Command:
	// The PAUSE input is allowed only in the RUNNING (and derivatives) state, and moves
	// the FSM to the PAUSING state.
	//
	addTransition(GEMInputs.PAUSE, GEMStates.RUNNING,                  GEMStates.PAUSING);
        addTransition(GEMInputs.PAUSE, GEMStates.RUNNINGDEGRADED,          GEMStates.PAUSING);
        addTransition(GEMInputs.PAUSE, GEMStates.RUNNINGSOFTERRORDETECTED, GEMStates.PAUSING);
        
	// RESUME Command:
	// The RESUME input is allowed only in the PAUSED state, and moves
	// the FSM to the RESUMING state.
	//
	addTransition(GEMInputs.RESUME, GEMStates.PAUSED, GEMStates.RESUMING);
		
	// STOP Command:
	// The STOP input is allowed only in the RUNNING (and derivatives) and PAUSED state, and moves
	// the FSM to the CONFIGURED state.
	//
	addTransition(GEMInputs.STOP, GEMStates.RUNNING,                  GEMStates.STOPPING);
        addTransition(GEMInputs.STOP, GEMStates.RUNNINGDEGRADED,          GEMStates.STOPPING);
        addTransition(GEMInputs.STOP, GEMStates.RUNNINGSOFTERRORDETECTED, GEMStates.STOPPING);
        addTransition(GEMInputs.STOP, GEMStates.PAUSED,                   GEMStates.STOPPING);
		
	// HALT Command:
	// The HALT input is allowed in the RUNNING (and derivatives), CONFIGURED, PAUSED and TTSTEST_MODE
	// state, and moves the FSM to the HALTING state.
	//
	addTransition(GEMInputs.HALT, GEMStates.RUNNING,                  GEMStates.HALTING);
        addTransition(GEMInputs.HALT, GEMStates.RUNNINGDEGRADED,          GEMStates.HALTING);
        addTransition(GEMInputs.HALT, GEMStates.RUNNINGSOFTERRORDETECTED, GEMStates.HALTING);
	addTransition(GEMInputs.HALT, GEMStates.CONFIGURED,               GEMStates.HALTING);
	addTransition(GEMInputs.HALT, GEMStates.PAUSED,                   GEMStates.HALTING);
	addTransition(GEMInputs.HALT, GEMStates.TTSTEST_MODE,             GEMStates.HALTING);

	// RECOVER Command:
	// The RECOVER input is allowed from ERROR and moves the FSM in to
	// RECOVERING state.
	//
	addTransition(GEMInputs.RECOVER, GEMStates.ERROR, GEMStates.RECOVERING);

	// RESET Command:
	// The RESET input is allowed from any steady state and moves the FSM to the
	// RESETTING state.
	//
	addTransition(GEMInputs.RESET, GEMStates.HALTED,                   GEMStates.RESETTING);
	addTransition(GEMInputs.RESET, GEMStates.CONFIGURED,               GEMStates.RESETTING);
	addTransition(GEMInputs.RESET, GEMStates.RUNNING,                  GEMStates.RESETTING);
        addTransition(GEMInputs.RESET, GEMStates.RUNNINGDEGRADED,          GEMStates.RESETTING);
        addTransition(GEMInputs.RESET, GEMStates.RUNNINGSOFTERRORDETECTED, GEMStates.RESETTING);
	addTransition(GEMInputs.RESET, GEMStates.PAUSED,                   GEMStates.RESETTING);
	addTransition(GEMInputs.RESET, GEMStates.TTSTEST_MODE,             GEMStates.RESETTING);
	addTransition(GEMInputs.RESET, GEMStates.ERROR,                    GEMStates.RESETTING);		

	//
	// The following transitions are not triggered from the GUI.
	//

	// Transition for going to ERROR
	addTransition(GEMInputs.SETERROR, State.ANYSTATE, GEMStates.ERROR);

	//
	// add transitions for transitional States
	//

	// Reach the INITIAL State?
	addTransition(GEMInputs.SETINITIAL, GEMStates.RECOVERING, GEMStates.INITIAL);

	// Reach the HALTED State
	addTransition(GEMInputs.SETHALTED, GEMStates.INITIALIZING,  GEMStates.HALTED);
	addTransition(GEMInputs.SETHALTED, GEMStates.HALTING,       GEMStates.HALTED);
	addTransition(GEMInputs.SETHALTED, GEMStates.RECOVERING,    GEMStates.HALTED);
	addTransition(GEMInputs.SETHALTED, GEMStates.RESETTING,     GEMStates.HALTED);
	addTransition(GEMInputs.SETHALTED, GEMStates.COLDRESETTING, GEMStates.HALTED);

	// Reach the CONFIGURED State
	addTransition(GEMInputs.SETCONFIGURED, GEMStates.INITIALIZING, GEMStates.CONFIGURED);  //?
	addTransition(GEMInputs.SETCONFIGURED, GEMStates.RECOVERING,   GEMStates.CONFIGURED);
	addTransition(GEMInputs.SETCONFIGURED, GEMStates.CONFIGURING,  GEMStates.CONFIGURED);
	addTransition(GEMInputs.SETCONFIGURED, GEMStates.STOPPING,     GEMStates.CONFIGURED);
		
	// Reach the RUNNING State
	// addTransition(GEMInputs.SETRUNNING, GEMStates.INITIALIZING, GEMStates.RUNNING);
	// addTransition(GEMInputs.SETRUNNING, GEMStates.RECOVERING,   GEMStates.RUNNING);
	addTransition(GEMInputs.SETRUNNING, GEMStates.STARTING,     GEMStates.RUNNING);

        // Reach the RUNNINGDEGRADED State
        addTransition(GEMInputs.SETRUNNINGDEGRADED, GEMStates.STARTING, GEMStates.RUNNINGDEGRADED);

        // Reach the RUNNINGSOFTERRORDETECTED state
        addTransition(GEMInputs.SETRUNNINGSOFTERRORDETECTED, GEMStates.STARTING, GEMStates.RUNNINGSOFTERRORDETECTED);
        
	// Reach the PAUSED State
	addTransition(GEMInputs.SETPAUSED, GEMStates.PAUSING,    GEMStates.PAUSED);
	// addTransition(GEMInputs.SETPAUSED, GEMStates.RECOVERING, GEMStates.PAUSED);

        // Reach the RUNNING and RUNNINGDEGRADED states from RESUMING State
        addTransition(GEMInputs.SETRESUMED,                  GEMStates.RESUMING, GEMStates.RUNNING);
        addTransition(GEMInputs.SETRESUMEDDEGRADED,          GEMStates.RESUMING, GEMStates.RUNNINGDEGRADED);
        addTransition(GEMInputs.SETRESUMEDSOFTERRORDETECTED, GEMStates.RESUMING, GEMStates.RUNNINGSOFTERRORDETECTED);

        // Switch between RUNNING and RUNNINGDEGRADED states
        addTransition(GEMInputs.SETRUNNING,         GEMStates.RUNNINGDEGRADED, GEMStates.RUNNING);
        addTransition(GEMInputs.SETRUNNINGDEGRADED, GEMStates.RUNNING,         GEMStates.RUNNINGDEGRADED);

        // Switch to RUNNINGSOFTERRORDETECTED state
        addTransition(GEMInputs.SETRUNNINGSOFTERRORDETECTED, GEMStates.RUNNINGDEGRADED, GEMStates.RUNNINGSOFTERRORDETECTED);
        addTransition(GEMInputs.SETRUNNINGSOFTERRORDETECTED, GEMStates.RUNNING,         GEMStates.RUNNINGSOFTERRORDETECTED);

        // Reach the FIXINGSOFTERROR state
        addTransition(GEMInputs.FIXSOFTERROR, GEMStates.RUNNING,                  GEMStates.FIXINGSOFTERROR);
        addTransition(GEMInputs.FIXSOFTERROR, GEMStates.RUNNINGDEGRADED,          GEMStates.FIXINGSOFTERROR);
        addTransition(GEMInputs.FIXSOFTERROR, GEMStates.RUNNINGSOFTERRORDETECTED, GEMStates.FIXINGSOFTERROR);

        // Reach RUNNING and RUNNINGDEGRADED states from FIXINGSOFTERROR state
        addTransition(GEMInputs.SETRUNNING,         GEMStates.FIXINGSOFTERROR, GEMStates.RUNNING);
        addTransition(GEMInputs.SETRUNNINGDEGRADED, GEMStates.FIXINGSOFTERROR, GEMStates.RUNNINGDEGRADED);
    }

}