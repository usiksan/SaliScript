#ifndef SVVMINTERFACE_H
#define SVVMINTERFACE_H


//Information
#define SVU_CMD_VERSION_GET        1 //Get controller version and type
                                     //data - none
#define SVU_CMD_VERSION            2 //Controller version and type
                                     //data [1-4]   version number
                                     //     [5-8]   programm memory size
                                     //     [9-10]  data memory size
                                     //     [11]    VPU max count
                                     //     [12-31] controller type


#define SVU_CMD_STATE_GET          3 //Get controller state
                                     //data - none
#define SVU_CMD_STATE              4 //Controller state
                                     //data [1]    - active VPU count
                                     //     [2]    - log byte count
                                     //     [3-63] - log contents


//Flash memory
#define SVU_CMD_FLASH_ERASE        5 //Erase flash memory
                                     //data - none

#define SVU_CMD_FLASH_ERASE_OK     6 //Flash memory erased successfully

#define SVU_CMD_FLASH_READ         7 //Read flash memory block
                                     //data [1]   block size
                                     //     [2-5] start address of readed block

#define SVU_CMD_FLASH_BLOCK        8 //Flash memory block
                                     //data [1]    block size
                                     //     [2-5]  start address of readed block
                                     //     [6-63] flash memory block data

#define SVU_CMD_FLASH_WRITE        9 //Write flash memory block
                                     //data [1]    block size
                                     //     [2-5]  start address of readed block
                                     //     [6-63] flash memory block data

#define SVU_CMD_FLASH_WRITE_OK    10 //Flash memory block writed successfully
                                     //data - none



//Variables
#define SVU_CMD_VARS_READ         11 //Read variables
                                     //data [1]   variables count
                                     //     [2-3] start address of readed variables

#define SVU_CMD_VARS_BLOCK        12 //Variables block
                                     //data [1]    variables count
                                     //     [2-3]  start address of readed variables block
                                     //     [4-63] variables

#define SVU_CMD_VARS_WRITE        13 //Write values to variables
                                     //data [1] - address-value pair count (max 10)
                                     //     [2-3] address
                                     //     [4-7] value
                                     //       ...

#define SVU_CMD_VARS_WRITE_OK     14 //Variables writed successfully


//Tasks
#define SVU_CMD_VPU_STATE_GET     15 //Get VPU state
                                     //data [1] - VPU index
#define SVU_CMD_VPU_STATE         16 //SvVpuState structure
                                     //data [1]     - VPU index
                                     //     [2-5]   - ip
                                     //     [6-9]   - sp
                                     //     [10-13] - bp
                                     //     [14-17] - tm
                                     //     [18-21] - baseSp
                                     //     [22-25] - throw
                                     //     [26]    - run


//Control
#define SVU_CMD_VPU_RESTART       17 //Restart current programm with debug flags
                                     //data [1] run without debug (1-without debug, 0-with debug)

#define SVU_CMD_VPU_RESTART_OK    18 //Restart successfully

#define SVU_CMD_VPU_RESET         19 //Reset current VPU without starting

#define SVU_CMD_VPU_RESET_OK      20 //Resetting successfully

#define SVU_CMD_DEBUG             21 //VPU debug command
                                     //data [1]    VPU number
                                     //     [2]    debug command
                                     //     [3-6]  param 0 (low bound)
                                     //     [7-10] param 1 (upper bound)

#define SVU_CMD_DEBUG_OK          22 //VPU debug command successfully


#endif // SVVMINTERFACE_H
