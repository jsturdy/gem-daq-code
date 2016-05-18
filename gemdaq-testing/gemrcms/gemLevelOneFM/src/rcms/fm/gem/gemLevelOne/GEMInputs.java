package rcms.fm.gem.gemLevelOne;

import rcms.statemachine.definition.Input;

/**
 * Definition of GEM Level 1 Function Manager Commands
 * 
 * @iriginalauthor Andrea Petrucci, Alexander Oh, Michele Gulmini
 * @author Jared Sturdy
 */
public class GEMInputs {
    
    // Defined commands for the level 1 Function Manager
    public static final Input INITIALIZE    = new Input("Initialize"   );
    public static final Input SETINITIAL    = new Input("SetInitial"   );
    public static final Input ENABLE        = new Input("Enable"       );
    public static final Input SETENABLED    = new Input("SetEnabled"   );
    public static final Input CONFIGURE     = new Input("Configure"    );
    public static final Input SETCONFIGURED = new Input("SetConfigured");
    public static final Input START         = new Input("Start"        );
    public static final Input SETRUNNING    = new Input("SetRunning"   );
    public static final Input STOP          = new Input("Stop"         );
    public static final Input HALT          = new Input("Halt"         );
    public static final Input SETHALTED     = new Input("SetHalted"    );
    public static final Input PAUSE         = new Input("Pause"        );
    public static final Input SETPAUSED     = new Input("SetPaused"    );
    public static final Input RESUME        = new Input("Resume"       );
    public static final Input SETRESUMED    = new Input("SetResumed"   );
    public static final Input RECOVER       = new Input("Recover"      );
    public static final Input RESET         = new Input("Reset"        );
    public static final Input SETRESET      = new Input("SetReset"     );
    public static final Input COLDRESET     = new Input("ColdReset"    );

    // special degraded commands
    public static final Input SETRUNNINGDEGRADED = new Input("SetRunningDegraded");
    public static final Input SETRESUMEDDEGRADED = new Input("SetResumedDegraded");
    
    public static final Input SETRUNNINGSOFTERRORDETECTED = new Input("SetRunningSoftErrorDetected");
    public static final Input SETRESUMEDSOFTERRORDETECTED = new Input("SetResumedSoftErrorDetected");
    public static final Input FIXSOFTERROR                = new Input("FixSoftError"               );
    
    // TTS states
    public static final Input PREPARE_TTSTEST_MODE = new Input("PrepareTTSTestMode");
    public static final Input SETTTSTEST_MODE      = new Input("SetTTSTestMode");
    public static final Input TEST_TTS             = new Input("TestTTS");

    // public static final Input TTSTEST_MODE         = new Input("TTSTestMode");
    // public static final Input SETTESTING_TTS       = new Input("SetTestingTTS");
    
    // Go to error
    public static final Input SETERROR = new Input("SetError");

}