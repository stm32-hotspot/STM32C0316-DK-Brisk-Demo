/**
  ******************************************************************************
  * @file    SGP30TOP.c (Sensirion CO2 sensor Application to Sensirion's SW maintained driver, adaptation layer)
  * @author  S.Ma
  * @brief   SW Plumbing with minimal maintenance effort in mind. 
  * 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018-2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"
#include "brisk.h"

#include "sgp30.h"
#include "sgp30top.h"

// slave address is 0xB0/B1


#include <stdio.h> // printf
//not available #include <unistd.h> // sleep
#include <inttypes.h> // PRIu64

/* TO USE CONSOLE OUTPUT (printf) AND WAIT (sleep) YOU MAY NEED TO ADAPT THE
 * INCLUDES ABOVE OR DEFINE THEM ACCORDING TO YOUR PLATFORM.
 * #define printf(...)
 #define sleep(...)
 */
#define sleep HAL_Delay

    u16 tvoc_ppb, co2_eq_ppm;
    u32 iaq_baseline;
    u16 ethanol_raw_signal, h2_raw_signal;
    u32 SGP30_ElapsedSeconds;


int32_t SGP30_Report(u8fifo_t* pL) { // where to send out result with base char key
  
  BTEL_Printf(pL,"*O%d*P%d", (uint32_t)tvoc_ppb , co2_eq_ppm); // O and P
  return 0;
}
    

int32_t SGP30_IsPlugged(void) {
    u16 i = 0;
    s16 err;

    const char *driver_version = sgp30_get_driver_version();
    if (driver_version) {
        NOPs(1);//printf("SGP30 driver version %s\n", driver_version);
    } else {
        NOPs(1);//printf("fatal: Getting driver version failed\n");
        return 0;
    }

    /* Busy loop for initialization. The main loop does not work without
     * a sensor. */
    while (sgp30_probe() != STATUS_OK) {
        NOPs(1);//printf("SGP sensor probing failed\n");
        sleep(1000);
    }
    NOPs(1);//printf("SGP sensor probing successful\n");

    u16 feature_set_version;
    u8 product_type;
    err = sgp30_get_feature_set_version(&feature_set_version, &product_type);
    if (err == STATUS_OK) {
        NOPs(1);//printf("Feature set version: %u\n", feature_set_version);
        NOPs(1);//printf("Product type: %u\n", product_type);
    } else {
        NOPs(1);//printf("sgp30_get_feature_set_version failed!\n");
    }
    u64 serial_id;
    err = sgp30_get_serial_id(&serial_id);
    if (err == STATUS_OK) {
        NOPs(1);//printf("SerialID: %" PRIu64 "\n", serial_id);
    } else {
        NOPs(1);//printf("sgp30_get_serial_id failed!\n");
        return 0;
    }
    
    SGP30_1sec(); // kick in the first measurement here
    
    return 1; // stop before measurement start

    /* Read gas raw signals */
    err = sgp30_measure_raw_blocking_read(&ethanol_raw_signal,
                                          &h2_raw_signal);
    if (err == STATUS_OK) {
        /* Print ethanol raw signal and h2 raw signal */
        NOPs(1);//printf("Ethanol raw signal: %u\n", ethanol_raw_signal);
        NOPs(1);//printf("H2 raw signal: %u\n", h2_raw_signal);
    } else {
        NOPs(1);//printf("error reading raw signals\n");
    }


    /* Consider the two cases (A) and (B):
     * (A) If no baseline is available or the most recent baseline is more than
     *     one week old, it must discarded. A new baseline is found with
     *     sgp30_iaq_init() */
    err = sgp30_iaq_init();
    if (err == STATUS_OK) {
        NOPs(1);//printf("sgp30_iaq_init done\n");
    } else {
        NOPs(1);//printf("sgp30_iaq_init failed!\n");
    }
    /* (B) If a recent baseline is available, set it after sgp30_iaq_init() for
     * faster start-up */
    /* IMPLEMENT: retrieve iaq_baseline from presistent storage;
     * err = sgp30_set_iaq_baseline(iaq_baseline);
     */

    /* Run periodic IAQ measurements at defined intervals */
    while (1) {
        /*
        * IMPLEMENT: get absolute humidity to enable humidity compensation
        * u32 ah = get_absolute_humidity(); // absolute humidity in mg/m^3
        * sgp30_set_absolute_humidity(ah);
        */

        err = sgp30_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
        if (err == STATUS_OK) {
            NOPs(1);//printf("tVOC  Concentration: %dppb\n", tvoc_ppb);
            NOPs(1);//printf("CO2eq Concentration: %dppm\n", co2_eq_ppm);
        } else {
            NOPs(1);//printf("error reading IAQ values\n");
        }

        /* Persist the current baseline every hour */
        if (++i % 3600 == 3599) {
            err = sgp30_get_iaq_baseline(&iaq_baseline);
            if (err == STATUS_OK) {
                /* IMPLEMENT: store baseline to presistent storage */
            }
        }

        /* The IAQ measurement must be triggered exactly once per second (SGP30)
         * to get accurate values.
         */
        sleep(1000); // SGP30
    }
    //return 0;
}

uint32_t stampstart, stampstop, stampduration;

int32_t SGP30_1sec(void) {
  
  s16 err;  // launch a measurement
  
  SGP30_ElapsedSeconds++;
  
#if 0
  /* Persist the current baseline every hour */
  if (SGP30_ElapsedSeconds % 3600 == 3599) {
      err = sgp30_get_iaq_baseline(&iaq_baseline);
      if (err == STATUS_OK) {
          /* IMPLEMENT: store baseline to presistent storage */
      }
  }
#endif
  
  err = sgp30_measure_iaq();
  if(err == STATUS_OK) {
    stampstart = HAL_GetTick();
    return 0;
  };
  
  return -1; // error
}


int32_t SGP30_IsPolling(void) {
  
  s16 err;  
  uint16_t tvoc, co2;

  // is measurement ready?
    err = sgp30_read_iaq(&tvoc, &co2);
    if(err != STATUS_OK) {
      NOPs(1);
      return -1; // not valid yet
    };
    
    stampstop = HAL_GetTick();
    stampduration = stampstop - stampstart; // <70 msec, by blocking, 650 msec+1000 sec delay with next measurement=1650msec per measurement
    tvoc_ppb = tvoc;
    co2_eq_ppm = co2;
  
  return 0; // error
}
