#ifndef BLE_CUS_H__
#define BLE_CUS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#define TYPE_IDEAL											0
#define TYPE_ID_NOTIFY									1
#define LONG_STRING_NOTIFY							2
#define IMAGE_NOTIFY										3

#define TYPE_ID_BUFFER_SIZE							16
#define LONG_STRING_BUFFER_SIZE					20
#define IMAGE_BUFFER_SIZE								20
#define NOTIFY_BUFFER_SIZE 							20

/**@brief   Macro for defining a ble_hrs instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_CUS_DEF(_name)                                                                          \
static ble_cus_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_HRS_BLE_OBSERVER_PRIO,                                                     \
                     ble_cus_on_ble_evt, &_name)


// 18048d3e-1b26-467f-b02a-ff1121629548
#define CUSTOM_SERVICE_UUID_BASE         {0x48,0x95,0x62,0x21,0x11,0xFF,0x2A,0xB0,0x7F,0x46,0x26,0x1B,0x3E,0x8D,0x04,0x18}
#define CUSTOM_SERVICE_UUID             0x8D3E
#define TYPE_ID_CHAR            	  	  0x95E6
#define LONG_STRING_CHAR                0x95E5
#define IMAGE_CHAR            		      0x95E4

														
extern unsigned char 										type_id_data[TYPE_ID_BUFFER_SIZE];
extern unsigned char 										long_string_data[LONG_STRING_BUFFER_SIZE];
extern unsigned char 										image_data[IMAGE_BUFFER_SIZE];
extern uint8_t 													notify_ack[NOTIFY_BUFFER_SIZE];
extern uint8_t 													data_rcved_on_ble_type;


// Forward declaration of the ble_cus_t type.
typedef struct ble_cus_s ble_cus_t;


/**@brief Custom Service event handler type. */
typedef void (*ble_cus_evt_handler_t) (ble_cus_t * p_bas);

/**@brief Battery Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_cus_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint8_t                       initial_custom_value;           /**< Initial custom value */
    ble_srv_cccd_security_mode_t  custom_value_char_attr_md;      /**< Initial security level for Custom characteristics attribute */
		ble_srv_utf8_str_t           	type_id_data;           	  		/**< Type ID Data String. */
		ble_srv_utf8_str_t           	long_string_data;           		/**< Long Data String. */	
		ble_srv_utf8_str_t           	image_data;           					/**< Image Data String. */		
} 	ble_cus_init_t;

/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_cus_s
{
    ble_cus_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t                      service_handle;                 /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t      type_id_value_handles;          /**< Handles related to the type id characteristic. */
    ble_gatts_char_handles_t      long_string_value_handles;      /**< Handles related to the long string characteristic. */
    ble_gatts_char_handles_t      image_value_handles;            /**< Handles related to the image characteristic. */
		uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                       uuid_type; 
};

/**@brief Function for initializing the Custom Service.
 *
 * @param[out]  p_cus       Custom Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_cus_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_cus_init(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Battery Service.
 *
 * @note 
 *
 * @param[in]   p_cus      Custom Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_cus_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);

///**@brief Function for updating the custom value.
// *
// * @details The application calls this function when the cutom value should be updated. If
// *          notification has been enabled, the custom value characteristic is sent to the client.
// *
// * @note 
// *       
// * @param[in]   p_bas          Custom Service structure.
// * @param[in]   Custom value 
// *
// * @return      NRF_SUCCESS on success, otherwise an error code.
// */

//uint32_t ble_cus_custom_value_update(ble_cus_t * p_cus, uint8_t custom_value);

/**@brief Initialize Custom Service.
 */
void cs_init(void);

/**@brief data notification
 */
uint32_t ble_cs_notify_data(ble_cus_t *p_cus, uint8_t *u8_buffer, uint16_t len, uint8_t notify_type);

#endif // BLE_CUS_H__
