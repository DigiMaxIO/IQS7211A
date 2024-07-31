#ifndef IQS7211A_INIT_TOUCHPAD4_H
#define IQS7211A_INIT_TOUCHPAD4_H

#define REG_INFO_FLAGS                            0x10
#define REG_GESTURES                              0x11
#define REG_RELATIVE_X                            0x12
#define REG_RELATIVE_Y                            0x13
#define REG_FINGER1_X                             0x14
#define REG_FINGER1_Y                             0x15
#define REG_FINGER1_TOUCH_STR                     0x16
#define REG_FINGER1_AREA                          0x17
#define REG_FINGER2_X                             0x18
#define REG_FINGER2_Y                             0x19
#define REG_FINGER2_TOUCH_STR                     0x1A
#define REG_FINGER2_AREA                          0x1B
#define REG_TOUCH_STATUS0                         0x20
#define REG_TOUCH_STATUS1                         0x21
#define REG_TOUCH_STATUS2                         0x22
#define REG_ALP_CHANNEL_COUNT                     0x23
#define REG_ALP_CHANNEL_LTA                       0x24
#define REG_ALP_COUNT_A                           0x25
#define REG_ALP_COUNT_B                           0x26
#define REG_TRACKPAD_ATI_MULT_DIV                 0x30
#define REG_TRACKPAD_ATI_COMP_DIV                 0x31
#define REG_TRACKPAD_ATI_TARGET                   0x32
#define REG_TRACKPAD_REF_DRIFT_LIMIT              0x33
#define REG_TRACKPAD_MIN_CNT_REATI_VAL            0x34
#define REG_REATI_RETRY_TIME                      0x35
#define REG_ALP_ATI_MULT_DIV                      0x36
#define REG_ALP_ATI_COMP_DIV                      0x37
#define REG_ALP_ATI_TARGET                        0x38
#define REG_ALP_ATI_DRIFT_LIMIT                   0x39
#define REG_ALP_ATI_COMP_A                        0x3A
#define REG_ALP_ATI_COMP_B                        0x3B
#define REG_ACTIVE_MODE_REPORT_RATE               0x40
#define REG_IDLE_TOUCH_MODE_REPORT_RATE           0x41
#define REG_IDLE_MODE_REPORT_RATE                 0x42
#define REG_LP1_MODE_REPORT_RATE                  0x43
#define REG_LP2_MODE_REPORT_RATE                  0x44
#define REG_ACTIVE_MODE_TIMEOUT                   0x45
#define REG_IDLE_TOUCH_MODE_TIMEOUT               0x46
#define REG_IDLE_MODE_TIMEOUT                     0x47
#define REG_LP1_MODE_TIMEOUT                      0x48
#define REG_REFERENCE_UPDATE_TIME                 0x49
#define REG_I2C_TIMEOUT                           0x4A
#define REG_SYSTEM_CONTROL                        0x50
#define REG_CONFIG_SETTINGS                       0x51
#define REG_OTHER_SETTINGS                        0x52
#define REG_TOUCH_CLEAR_SET_THRESH                0x53
#define REG_ALP_THRESH                            0x54
#define REG_OPEN_0                                0x55
#define REG_ALP_CLEAR_SET_DEBOUNCE                0x56
#define REG_OPEN_1                                0x57
#define REG_TRACKPAD_CONVER_FREQ                  0x58
#define REG_ALP_CONVER_FREQ                       0x59
#define REG_TRACKPAD_HW_SETTINGS                  0x5A
#define REG_ALP_HW_SETTINGS                       0x5B
#define REG_TOTAL_RXS_TRACKPAD_SETTINGS           0x60
#define REG_MAX_MULTITOUCHES_TOTAL_TXS            0x61
#define REG_X_RES                                 0x62
#define REG_Y_RES                                 0x63
#define REG_XY_DYNAMIC_FILTER_BOT_SPEED           0x64
#define REG_XY_DYNAMIC_FILTER_TOP_SPEED           0x65
#define REG_STATIC_FILTER_VAL_DYNAMIC_FILTER_BOT  0x66
#define REG_FINGER_SPLIT_STAT_TOUCH_MOVE_THRESH   0x67
#define REG_X_TRIM_VALUE                          0x68
#define REG_Y_TRIM_VALUE                          0x69
#define REG_ALP_COUNT_FILTER_BETA                 0x70
#define REG_LP2_LP1_BETA                          0x71
#define REG_ALP_SETUP                             0x72
#define REG_ALP_TX_ENABLE                         0x73
#define REG_SETTINGS_MAJOR_MINOR_VER              0x74
#define REG_GESTURE_ENABLE                        0x80
#define REG_TAP_TIME                              0x81
#define REG_TAP_DIST                              0x82
#define REG_HOLD_TIME                             0x83
#define REG_SWIPE_TIME                            0x84
#define REG_SWIPE_X_DIST                          0x85
#define REG_SWIPE_Y_DIST                          0x86
#define REG_SWIPE_ANGLE                           0x87
#define REG_RXTX_MAP_1_0                          0x90
#define REG_RXTX_MAP_3_2                          0x91
#define REG_RXTX_MAP_5_4                          0x92
#define REG_RXTX_MAP_7_6                          0x93
#define REG_RXTX_MAP_9_8                          0x94
#define REG_RXTX_MAP_11_10                        0x95
#define REG_RXTX_MAP_13_12                        0x96
#define REG_1st_CHANNEL_FOR_CYCLE0                0xA0
#define REG_2nd_CHANNEL_FOR_CYCLE0                0xA1
#define REG_2nd1st_CHANNEL_FOR_CYCLE1             0xA2
#define REG_1st_CHANNEL_FOR_CYCLE2                0xA3
#define REG_2nd_CHANNEL_FOR_CYCLE2                0xA4
#define REG_2nd1st_CHANNEL_FOR_CYCLE3             0xA5
#define REG_1st_CHANNEL_FOR_CYCLE4                0xA6
#define REG_2nd_CHANNEL_FOR_CYCLE4                0xA7
#define REG_2nd1st_CHANNEL_FOR_CYCLE5             0xA8
#define REG_1st_CHANNEL_FOR_CYCLE6                0xA9
#define REG_2nd_CHANNEL_FOR_CYCLE6                0xAA
#define REG_2nd1st_CHANNEL_FOR_CYCLE7             0xAB
#define REG_1st_CHANNEL_FOR_CYCLE8                0xAC
#define REG_2nd_CHANNEL_FOR_CYCLE8                0xAD
#define REG_2nd1st_CHANNEL_FOR_CYCLE9             0xAE
#define REG_1st_CHANNEL_FOR_CYCLE10               0xB0
#define REG_2nd_CHANNEL_FOR_CYCLE10               0xB1
#define REG_2nd1st_CHANNEL_FOR_CYCLE11            0xB2
#define REG_1st_CHANNEL_FOR_CYCLE12               0xB3
#define REG_2nd_CHANNEL_FOR_CYCLE12               0xB4
#define REG_2nd1st_CHANNEL_FOR_CYCLE13            0xB5
#define REG_1st_CHANNEL_FOR_CYCLE14               0xB6
#define REG_2nd_CHANNEL_FOR_CYCLE14               0xB7
#define REG_2nd1st_CHANNEL_FOR_CYCLE15            0xB8
#define REG_1st_CHANNEL_FOR_CYCLE16               0xB9
#define REG_2nd_CHANNEL_FOR_CYCLE16               0xBA
#define REG_2nd1st_CHANNEL_FOR_CYCLE17            0xBB
#define REG_TRACKPAD_COUNT_VAL                    0xE0
#define REG_TRACKPAD_REF_VAL                      0xE1
#define REG_TRACKPAD_DELTA_VAL                    0xE2
#define REG_TRACKPAD_ATI_COMPENSATION_VAL         0xE3

/*! @} */ // reg

/**
 * @defgroup set Touchpad 4 Registers Settings
 * @brief Settings for registers of Touchpad 4 Click driver.
 */

/**
 * @addtogroup set
 * @{
 */

/**
 * @brief Change the ATI Settings.
 * @details Memory Map Position 0x30 - 0x39.
 */
#define TP_ATI_MULTIPLIERS_DIVIDERS_0             0xE1
#define TP_ATI_MULTIPLIERS_DIVIDERS_1             0x3B
#define TP_COMPENSATION_DIV_0                     0x0F
#define TP_COMPENSATION_DIV_1                     0x00
#define TP_ATI_TARGET_0                           0xFA
#define TP_ATI_TARGET_1                           0x00
#define TP_REF_DRIFT_LIMIT_0                      0x32
#define TP_REF_DRIFT_LIMIT_1                      0x00
#define TP_MIN_COUNT_REATI_0                      0x32
#define TP_MIN_COUNT_REATI_1                      0x00
#define REATI_RETRY_TIME_0                        0x05
#define REATI_RETRY_TIME_1                        0x00
#define ALP_ATI_MULTIPLIERS_DIVIDERS_0            0x82
#define ALP_ATI_MULTIPLIERS_DIVIDERS_1            0x28
#define ALP_COMPENSATION_DIV_0                    0x05
#define ALP_COMPENSATION_DIV_1                    0x00
#define ALP_ATI_TARGET_0                          0xFA
#define ALP_ATI_TARGET_1                          0x00
#define ALP_LTA_DRIFT_LIMIT_0                     0x14
#define ALP_LTA_DRIFT_LIMIT_1                     0x00

/**
 * @brief Change the ALP ATI Compensation.
 * @details Memory Map Position 0x3A - 0x3B.
 */
#define ALP_COMPENSATION_A_0                      0xC4
#define ALP_COMPENSATION_A_1                      0x01
#define ALP_COMPENSATION_B_0                      0xE8
#define ALP_COMPENSATION_B_1                      0x01

/**
 * @brief Change the Report Rates and Timing.
 * @details Memory Map Position 0x40 - 0x4A.
 */
#define ACTIVE_MODE_REPORT_RATE_0                 0x0A
#define ACTIVE_MODE_REPORT_RATE_1                 0x00
#define IDLE_TOUCH_MODE_REPORT_RATE_0             0x32
#define IDLE_TOUCH_MODE_REPORT_RATE_1             0x00
#define IDLE_MODE_REPORT_RATE_0                   0x32
#define IDLE_MODE_REPORT_RATE_1                   0x00
#define LP1_MODE_REPORT_RATE_0                    0x32
#define LP1_MODE_REPORT_RATE_1                    0x00
#define LP2_MODE_REPORT_RATE_0                    0x64
#define LP2_MODE_REPORT_RATE_1                    0x00
#define ACTIVE_MODE_TIMEOUT_0                     0x0A
#define ACTIVE_MODE_TIMEOUT_1                     0x00
#define IDLE_TOUCH_MODE_TIMEOUT_0                 0x3C
#define IDLE_TOUCH_MODE_TIMEOUT_1                 0x00
#define IDLE_MODE_TIMEOUT_0                       0x14
#define IDLE_MODE_TIMEOUT_1                       0x00
#define LP1_MODE_TIMEOUT_0                        0x0A
#define LP1_MODE_TIMEOUT_1                        0x00
#define REF_UPDATE_TIME_0                         0x08
#define REF_UPDATE_TIME_1                         0x00
#define I2C_TIMEOUT_0                             0x64
#define I2C_TIMEOUT_1                             0x00

/**
 * @brief Change the System Settings.
 * @details Memory Map Position 0x50 - 0x5B.
 */
#define SYSTEM_CONTROL_0                          0x00
#define SYSTEM_CONTROL_1                          0x00
#define CONFIG_SETTINGS0                          0x3C
#define CONFIG_SETTINGS1                          0x06
#define OTHER_SETTINGS_0                          0x20
#define OTHER_SETTINGS_1                          0xFF
#define TRACKPAD_TOUCH_SET_THRESHOLD              0x1A
#define TRACKPAD_TOUCH_CLEAR_THRESHOLD            0x14
#define ALP_THRESHOLD_0                           0x08
#define ALP_THRESHOLD_1                           0x00
#define OPEN_0_0                                  0xFF
#define OPEN_0_1                                  0xFF
#define ALP_SET_DEBOUNCE                          0x02
#define ALP_CLEAR_DEBOUNCE                        0x04
#define OPEN_1_0                                  0xFF
#define OPEN_1_1                                  0xFF
#define TP_CONVERSION_FREQUENCY_UP_PASS_LENGTH    0x05
#define TP_CONVERSION_FREQUENCY_FRACTION_VALUE    0x7F
#define ALP_CONVERSION_FREQUENCY_UP_PASS_LENGTH   0x05
#define ALP_CONVERSION_FREQUENCY_FRACTION_VALUE   0x7F
#define TRACKPAD_HARDWARE_SETTINGS_0              0x01
#define TRACKPAD_HARDWARE_SETTINGS_1              0x0D
#define ALP_HARDWARE_SETTINGS_0                   0x65
#define ALP_HARDWARE_SETTINGS_1                   0x1D

/**
 * @brief Change the Trackpad Settings.
 * @details Memory Map Position 0x60 - 0x69.
 */
#define TRACKPAD_SETTINGS_0_0                     0x28
#define TRACKPAD_SETTINGS_0_1                     0x04
#define TRACKPAD_SETTINGS_1_0                     0x05
#define TRACKPAD_SETTINGS_1_1                     0x01
#define X_RESOLUTION_0                            0x00
#define X_RESOLUTION_1                            0x03
#define Y_RESOLUTION_0                            0x00
#define Y_RESOLUTION_1                            0x04
#define XY_DYNAMIC_FILTER_BOTTOM_SPEED_0          0x06
#define XY_DYNAMIC_FILTER_BOTTOM_SPEED_1          0x00
#define XY_DYNAMIC_FILTER_TOP_SPEED_0             0x7C
#define XY_DYNAMIC_FILTER_TOP_SPEED_1             0x00
#define XY_DYNAMIC_FILTER_BOTTOM_BETA             0x07
#define XY_DYNAMIC_FILTER_STATIC_FILTER_BETA      0x80
#define STATIONARY_TOUCH_MOV_THRESHOLD            0x14
#define FINGER_SPLIT_FACTOR                       0x00
#define X_TRIM_VALUE_0                            0x14
#define X_TRIM_VALUE_1                            0x00
#define Y_TRIM_VALUE_0                            0x14
#define Y_TRIM_VALUE_1                            0x00

/**
 * @brief Change the ALP Settings.
 * @details Memory Map Position 0x70 - 0x73.
 */
#define ALP_COUNT_FILTER_BETA_0                   0xB4
#define OPEN_0                                    0x00
#define ALP_LTA_BETA_LP1                          0x06
#define ALP_LTA_BETA_LP2                          0x04
#define ALP_SETUP_0                               0x33
#define ALP_SETUP_1                               0x03
#define ALP_TX_ENABLE_0                           0x40
#define ALP_TX_ENABLE_1                           0x05

/**
 * @brief Change the Settings Version Numbers.
 * @details Memory Map Position 0x74 - 0x74.
 */
#define MINOR_VERSION                             0x00
#define MAJOR_VERSION                             0x01

/**
 * @brief Change the Gesture Settings.
 * @details Memory Map Position 0x80 - 0x87.
 */
#define GESTURE_ENABLE_0                          0x3F
#define GESTURE_ENABLE_1                          0x0F
#define TAP_TIME_0                                0x96
#define TAP_TIME_1                                0x00
#define TAP_DISTANCE_0                            0x32
#define TAP_DISTANCE_1                            0x00
#define HOLD_TIME_0                               0x2C
#define HOLD_TIME_1                               0x01
#define SWIPE_TIME_0                              0x96
#define SWIPE_TIME_1                              0x00
#define SWIPE_X_DISTANCE_0                        0xC8
#define SWIPE_X_DISTANCE_1                        0x00
#define SWIPE_Y_DISTANCE_0                        0xC8
#define SWIPE_Y_DISTANCE_1                        0x00
#define SWIPE_ANGLE_0                             0x17
#define GESTURE_OPEN_0                            0x00

/**
 * @brief Change the RxTx Mapping.
 * @details Memory Map Position 0x90 - 0x96.
 */
#define RX_TX_MAP_0                               0x05
#define RX_TX_MAP_1                               0x04
#define RX_TX_MAP_2                               0x01
#define RX_TX_MAP_3                               0x00
#define RX_TX_MAP_4                               0x0A
#define RX_TX_MAP_5                               0x09
#define RX_TX_MAP_6                               0x08
#define RX_TX_MAP_7                               0x07
#define RX_TX_MAP_8                               0x06
#define RX_TX_MAP_9                               0x0A
#define RX_TX_MAP_10                              0x09
#define RX_TX_MAP_11                              0x08
#define RX_TX_MAP_12                              0x00
#define RX_TX_MAP_13                              0x00

/**
 * @brief Change the Allocation of channels into cycles 0-9.
 * @details Memory Map Position 0xA0 - 0xAE.
 */
#define PLACEHOLDER_0                             0x05
#define CH_1_CYCLE_0                              0x00
#define CH_2_CYCLE_0                              0x02
#define PLACEHOLDER_1                             0x05
#define CH_1_CYCLE_1                              0x01
#define CH_2_CYCLE_1                              0x03
#define PLACEHOLDER_2                             0x05
#define CH_1_CYCLE_2                              0x04
#define CH_2_CYCLE_2                              0x06
#define PLACEHOLDER_3                             0x05
#define CH_1_CYCLE_3                              0x05
#define CH_2_CYCLE_3                              0x07
#define PLACEHOLDER_4                             0x05
#define CH_1_CYCLE_4                              0x08
#define CH_2_CYCLE_4                              0x0A
#define PLACEHOLDER_5                             0x05
#define CH_1_CYCLE_5                              0x09
#define CH_2_CYCLE_5                              0x0B
#define PLACEHOLDER_6                             0x05
#define CH_1_CYCLE_6                              0x0C
#define CH_2_CYCLE_6                              0x0E
#define PLACEHOLDER_7                             0x05
#define CH_1_CYCLE_7                              0x0D
#define CH_2_CYCLE_7                              0x0F
#define PLACEHOLDER_8                             0x05
#define CH_1_CYCLE_8                              0x10
#define CH_2_CYCLE_8                              0x12
#define PLACEHOLDER_9                             0x05
#define CH_1_CYCLE_9                              0x11
#define CH_2_CYCLE_9                              0x13

/**
 * @brief Change the Allocation of channels into cycles 10-17.
 * @details Memory Map Position 0xB0 - 0xBB.
 */
#define PLACEHOLDER_10                            0x05
#define CH_1_CYCLE_10                             0xFF
#define CH_2_CYCLE_10                             0xFF
#define PLACEHOLDER_11                            0x05
#define CH_1_CYCLE_11                             0xFF
#define CH_2_CYCLE_11                             0xFF
#define PLACEHOLDER_12                            0x05
#define CH_1_CYCLE_12                             0xFF
#define CH_2_CYCLE_12                             0xFF
#define PLACEHOLDER_13                            0x05
#define CH_1_CYCLE_13                             0xFF
#define CH_2_CYCLE_13                             0xFF
#define PLACEHOLDER_14                            0x05
#define CH_1_CYCLE_14                             0xFF
#define CH_2_CYCLE_14                             0xFF
#define PLACEHOLDER_15                            0x05
#define CH_1_CYCLE_15                             0xFF
#define CH_2_CYCLE_15                             0xFF
#define PLACEHOLDER_16                            0x05
#define CH_1_CYCLE_16                             0xFF
#define CH_2_CYCLE_16                             0xFF
#define PLACEHOLDER_17                            0x05
#define CH_1_CYCLE_17                             0xFF
#define CH_2_CYCLE_17                             0xFF

#endif	/* IQS7211A_INIT_H */