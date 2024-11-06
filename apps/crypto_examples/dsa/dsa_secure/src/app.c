/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define SESSION_ID    1
#define SYSTICK_MAX   0xFFFFFF

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void ECDSA_Sign_Test(ECDSA *ecdsa)

  Remarks:
    See prototype in app.h.
 */

typedef struct {
    float timerFreqHZ;
    float timerPeriodMS;
    
    uint32_t startTick;
    uint32_t endTick;
    uint32_t rolloverCount;
} SYSTICK_TIMER_MEASUREMENT;

SYSTICK_TIMER_MEASUREMENT measurement = {0};

// Callback to count rollovers
void SysTick_Callback(uintptr_t context) {
    measurement.rolloverCount++;
}

// Initialize SysTick timer with callback
void InitSysTickTimer(void) {
    //DEBUGGING
    printf("measurement.timerFreqHZ: %f\r\n", measurement.timerFreqHZ);
    printf("measurement.timerPeriodMS: %f\r\n", measurement.timerPeriodMS);
    
    SYSTICK_TimerCallbackSet(SysTick_Callback, 0);
    SYSTICK_TimerStart();
}

void StartMeasurement() {
    measurement.rolloverCount = 0;  // Reset rollover count
    measurement.startTick = SYSTICK_TimerCounterGet();
}

void EndMeasurement() {
    measurement.endTick = SYSTICK_TimerCounterGet();
}

double CalculateElapsedTime() {
    // Calculate tick difference for a down-counting timer
    uint32_t tickDifference;
    
    // DEBUGGING
    printf("startTick: %ld, endTick: %ld\r\n", measurement.startTick, measurement.endTick);
    printf("measurement.rolloverCount: %ld\r\n", measurement.rolloverCount);
    
    if (measurement.startTick >= measurement.endTick) {
        // Normal case: startTick is later, and endTick is closer to zero
        printf("normal\r\n");
        tickDifference = measurement.startTick - measurement.endTick;
    } else {
        // Handle wraparound: startTick was taken after a rollover
        printf("rollover\r\n");
        tickDifference = (SYSTICK_MAX - measurement.endTick) + measurement.startTick + 1;
                            // largest val - endTick + initial val + 1 to count zero
                            //TODO: Figure out a good way to get the largest value 
    }

    // Calculate elapsed time in milliseconds
    double timeFromTicks = (double)((tickDifference)/(measurement.timerFreqHZ/1000));
    printf("timeFromTicks: %f\r\n", timeFromTicks);
    double timeFromRollovers = measurement.rolloverCount * measurement.timerPeriodMS;
    return timeFromRollovers + timeFromTicks;
}

void ECDSA_Sign_Test(ECDSA *ecdsa)
{
    crypto_DigiSign_Status_E status;
    
    (void) memset(ecdsa->sig, 0, ecdsa->sigSize);
  
    StartMeasurement();
    
    status = Crypto_DigiSign_Ecdsa_Sign(
        ecdsa->handler,
        ecdsa->inputHash,
        ecdsa->inputHashSize,
        ecdsa->sig,
        ecdsa->sigSize,
        ecdsa->key,
        ecdsa->keySize,
        ecdsa->curveType,
        SESSION_ID
    );

    EndMeasurement();

    double elapsedTime = CalculateElapsedTime();
    printf("Elapsed time: %f ms\n", elapsedTime);

    if (status != CRYPTO_DIGISIGN_SUCCESS)
    {
        printf("Failed to create message signature\r\n");
        printf("Status: %d\r\n", status);
        appData.testsFailed++;
    }
    else
    {
        printf("Test successful\r\n");
        appData.testsPassed++;
    }
}

/*******************************************************************************
  Function:
    void ECDSA_Verify_Test(ECDSA *ecdsa)

  Remarks:
    See prototype in app.h.
 */

void ECDSA_Verify_Test(ECDSA *ecdsa)
{
    crypto_DigiSign_Status_E status;
    
    StartMeasurement();
    
    status = Crypto_DigiSign_Ecdsa_Verify(
        ecdsa->handler,
        ecdsa->inputHash,
        ecdsa->inputHashSize,
        ecdsa->sig,
        ecdsa->sigSize,
        ecdsa->key,
        ecdsa->keySize,
        &(ecdsa->hashVerifyStat),
        ecdsa->curveType,
        SESSION_ID
    );
    
    EndMeasurement();

    double elapsedTime = CalculateElapsedTime();
    printf("Elapsed time: %f ms\n", elapsedTime);

    if (status != CRYPTO_DIGISIGN_SUCCESS)
    {
        printf("Failed to verify signature\r\n");
        printf("Status: %d\r\n", status);
        appData.testsFailed++;
    }

    if (ecdsa->hashVerifyStat)
    {
        appData.testsPassed++;
        printf("Test successful\r\n");
    }
    else
    {
        appData.testsFailed++;
        printf("Test unsuccessful\r\n");
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    appData.isTestedECDSA = false;
    
    appData.testsFailed = 0;
    appData.testsPassed = 0;
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;

            measurement.timerFreqHZ = (float) SYSTICK_FREQ;
            measurement.timerPeriodMS  = (float) SYSTICK_INTERRUPT_PERIOD_IN_US/1000;
            InitSysTickTimer();

            if (appInitialized)
            {
                appData.state = APP_STATE_SERVICE_TASKS;
            }

            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {               
            if (!appData.isTestedECDSA)
            {                
                printf("\r\n-----------ECDSA wolfCrypt Wrapper-------------\r\n");
                ECDSA_Test(CRYPTO_HANDLER_SW_WOLFCRYPT);
                
                appData.isTestedECDSA = true;

                printf("\r\n-----------------------------------\r\n");
                printf("Tests attempted: %d\r\n", appData.testsPassed + appData.testsFailed);
                printf("Tests successful: %d\r\n", appData.testsPassed);
            }
           
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            break;
        }
    }

}

/*******************************************************************************
 End of File
 */
