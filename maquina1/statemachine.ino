void setup_State_Machine(){

    //add transitions
    stateMachine.addTransition(SECURITY_CHECK, PASSWORD_OK, EVENT_DOORS_WINDOWS);
    stateMachine.addTransition(SECURITY_CHECK, PASSWORD_ERROR, SECURITY_ALERT);
    stateMachine.addTransition(EVENT_DOORS_WINDOWS, DOORS_WINDOWS_EVENT, MONITOR_ENVIRONMENT);
    stateMachine.addTransition(EVENT_DOORS_WINDOWS, DOORS_WINDOWS_NO_EVENT, SECURITY_ALERT);
    stateMachine.addTransition(MONITOR_ENVIRONMENT, TEMPERATURE_HIGH, ALARM_ENVIRONMENT);
    stateMachine.addTransition(MONITOR_ENVIRONMENT, TEMPERATURE_LOW, ALARM_ENVIRONMENT);
    stateMachine.addTransition(ALARM_ENVIRONMENT, TEMPERATURE_HIGH_ALARM, SECURITY_ALERT);
    stateMachine.addTransition(ALARM_ENVIRONMENT, TEMPERATURE_HIGH_NO_ALARM, MONITOR_ENVIRONMENT);
    stateMachine.addTransition(ALARM_ENVIRONMENT, TEMPERATURE_LOW_ALARM, SECURITY_ALERT);
    stateMachine.addTransition(ALARM_ENVIRONMENT, TEMPERATURE_LOW_NO_ALARM, MONITOR_ENVIRONMENT);
    stateMachine.addTransition(SECURITY_ALERT, SECURITY_ALERT_TIMEOUT, SECURITY_CHECK);
    stateMachine.addTransition(SECURITY_ALERT, DOORS_WINDOWS_TIMEOUT, SECURITY_CHECK);
    stateMachine.addTransition(SECURITY_ALERT, ENVIRONMENTAL_TIMEOUT, SECURITY_CHECK);

    //add actions
    //stateMachine.SetOnEntering
    stateMachine.SetOnEntering(SECURITY_CHECK, []() { 
        return currentInput 

}