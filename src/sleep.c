
#include <stdint.h>
#include <stdbool.h>


#include "em_emu.h"
#include "em_int.h"
#include "em_core.h"
#include "sleep.h"


uint32_t sleep_block_counter[NUM_SLEEP_MODES];

void sleep(void)
{
    if (sleep_block_counter[0] > 0) {
        /* Blocked everything below EM0, so just return */
        return;
    } else if (sleep_block_counter[1] > 0) {
        /* Blocked everything below EM1, enter EM1 */
        EMU_EnterEM1();
    } else if (sleep_block_counter[2] > 0) {
        /* Blocked everything below EM2, enter EM2 */
        EMU_EnterEM2(true);
    } else {
        /* Blocked everything below EM3, enter EM3 */
        EMU_EnterEM3(true);
    } /* Never enter EM4, as mbed has no way of configuring EM4 wakeup */
    return;
}


/** Block the microcontroller from sleeping below a certain mode
 *
 * This will block sleep() from entering an energy mode below the one given.
 * -- To be called by peripheral HAL's --
 *
 * After the peripheral is finished with the operation, it should call unblock with the same state
 *
 */
void blockSleepMode(sleepstate_enum minimumMode)
{
    CORE_ATOMIC_IRQ_DISABLE();
    sleep_block_counter[minimumMode]++;
    CORE_ATOMIC_IRQ_ENABLE();
}

/** Unblock the microcontroller from sleeping below a certain mode
 *
 * This will unblock sleep() from entering an energy mode below the one given.
 * -- To be called by peripheral HAL's --
 *
 * This should be called after all transactions on a peripheral are done.
 */
void unblockSleepMode(sleepstate_enum minimumMode)
{
    CORE_ATOMIC_IRQ_DISABLE();
    if(sleep_block_counter[minimumMode] > 0) {
        sleep_block_counter[minimumMode]--;
    }
    CORE_ATOMIC_IRQ_ENABLE();
}
