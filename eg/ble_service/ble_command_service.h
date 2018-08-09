#ifndef BLE_COMMAND_SERVICE_H__
#define BLE_COMMAND_SERVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

/**@brief   Macro for defining a ble_hrs instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_COMMAND_SERVICE_DEF(_name)                                                                          \
static ble_command_service_struct _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_HRS_BLE_OBSERVER_PRIO,                                                     \
                     ble_command_on_ble_evt, &_name)

// BLE_COMMAND_SERVICE_UUID_BASE f364adc9-b000-4042-ba50-05ca45bf8abc

#define BLE_COMMAND_SERVICE_UUID_BASE         {0xBC, 0x8A, 0xBF, 0x45, 0xCA, 0x05, 0x50, 0xBA, \
                                          0x40, 0x42, 0xB0, 0x00, 0xC9, 0xAD, 0x64, 0xF3}

#define BLE_COMMAND_SERVICE_UUID               0x1400
#define BLE_COMMAND_VALUE_CHAR_UUID            0x1401


/**@brief Custom Service event type. */
typedef enum
{
    BLE_COMMAND_EVT_NOTIFICATION_ENABLED,                             /**< Custom value notification enabled event. */
    BLE_COMMAND_EVT_NOTIFICATION_DISABLED,                             /**< Custom value notification disabled event. */
    BLE_COMMAND_EVT_DISCONNECTED,
    BLE_COMMAND_EVT_CONNECTED
} ble_command_evt_type_t;

/**@brief Custom Command Service event. */
typedef struct
{
    ble_command_evt_type_t evt_type;                                  /**< Type of event. */
} ble_command_evt_t;			

// Forward declaration of the ble_command_service_struct type.
typedef struct ble_command_service_struct_def ble_command_service_struct;

/**@brief Custom Service event handler type. */
typedef void (*ble_command_service_evt_handler_t) (ble_command_service_struct * ble_command_service, ble_command_evt_t * ble_command_evt);

/**@brief Custom Command Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_command_service_evt_handler_t     evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint8_t                       				initial_custom_value;           /**< Initial custom value */
    ble_srv_cccd_security_mode_t  				custom_value_char_attr_md;     /**< Initial security level for Custom characteristics attribute */
} ble_command_service_init_t;

/**@brief Custom Command Service structure. This contains various status information for the service. */
struct ble_command_service_struct_def
{
    ble_command_service_evt_handler_t     evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t                      				service_handle;                 /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t      				custom_value_handles;           /**< Handles related to the Custom Value characteristic. */
    uint16_t                      				conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                       				uuid_type; 
};

/**@brief Function for initializing the Custom Command Service.
 *
 * @param[out]  ble_command_service   Custom Service structure. This structure will have to be supplied by
 *                          					the application. It will be initialized by this function, and will later
 *                          					be used to identify this particular service instance.
 * @param[in]   command_service_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_command_service_init(ble_command_service_struct * ble_command_service, const ble_command_service_init_t * command_service_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Battery Service.
 *
 * @note 
 *
 * @param[in]   p_cus      Custom Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_command_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for updating the custom value.
 *
 * @details The application calls this function when the cutom value should be updated. If
 *          notification has been enabled, the custom value characteristic is sent to the client.
 *
 * @note 
 *       
 * @param[in]   ble_command_service          Custom Service structure.
 * @param[in]   Custom value 
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_command_service_value_update(ble_command_service_struct * ble_command_service, uint8_t custom_value);

#endif
