#ifndef BLE_CUS_H__
#define BLE_CUS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_ble_qwr.h"


#define TYPE_IDEAL							0
#define TYPE_ID_NOTIFY						1
#define LONG_STRING_NOTIFY					2
#define IMAGE_NOTIFY						3

//#define TYPE_ID_BUFFER_SIZE					16
//#define LONG_STRING_BUFFER_SIZE				20
//#define IMAGE_BUFFER_SIZE					20
//#define NOTIFY_BUFFER_SIZE 					20

#define NRF_BLE_QWRS_MAX_RCV_SIZE   		510//128
#define BLE_QWRS_MAX_LONG_CHAR_LEN        	NRF_BLE_QWRS_MAX_RCV_SIZE   /**< Maximum length of the QWRS Long Characteristic (in bytes). */
#define BLE_QWRS_MAX_CHAR_LEN             	20                          /**< Maximum length of the QWRS Characteristic (in bytes). */


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



// CUSTOM_SERVICE_UUID_BASE f364adc9-b000-4042-ba50-05ca45bf8abc

#define CUSTOM_SERVICE_UUID_BASE         {0xBC, 0x8A, 0xBF, 0x45, 0xCA, 0x05, 0x50, 0xBA, \
                                          0x40, 0x42, 0xB0, 0x00, 0xC9, 0xAD, 0x64, 0xF3}

#define CUSTOM_SERVICE_UUID               0x1400
#define CUSTOM_VALUE_CHAR_UUID            0x1401
																					
// 18048d3e-1b26-467f-b02a-ff1121629548
#define NEW_CUSTOM_SERVICE_UUID_BASE        {0x48,0x95,0x62,0x21,0x11,0xFF,0x2A,0xB0,0x7F,0x46,0x26,0x1B,0x3E,0x8D,0x04,0x18}
#define NEW_CUSTOM_SERVICE_UUID             0x8D3E
#define LONG_STRING_CHAR                0x95E5


/**@brief Custom Service event type. */
typedef enum
{
    BLE_CUS_EVT_NOTIFICATION_ENABLED,                             /**< Custom value notification enabled event. */
    BLE_CUS_EVT_NOTIFICATION_DISABLED,                             /**< Custom value notification disabled event. */
    BLE_CUS_EVT_DISCONNECTED,
    BLE_CUS_EVT_CONNECTED
} ble_cus_evt_type_t;

/**@brief Custom Service event. */
typedef struct
{
    ble_cus_evt_type_t evt_type;                                  /**< Type of event. */
} ble_cus_evt_t;

// Forward declaration of the ble_cus_t type.
typedef struct ble_cus_s ble_cus_t;


/**@brief Custom Service event handler type. */
typedef void (*ble_cus_evt_handler_t) (ble_cus_t * p_bas, ble_cus_evt_t * p_evt);

/**@brief Battery Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_cus_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint8_t                       initial_custom_value;           /**< Initial custom value */
    ble_srv_cccd_security_mode_t  custom_value_char_attr_md;     /**< Initial security level for Custom characteristics attribute */
} ble_cus_init_t;

/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_cus_s
{
    ble_cus_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t                      service_handle;                 /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t      custom_value_handles;           /**< Handles related to the Custom Value characteristic. */
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                       uuid_type; 
};

/**@brief Queued Write Example Service event types. */
typedef enum
{
    BLE_QWRS_CHECK_RCVD_DATA,                        /* On this event, the application shall only verify if the data are correct.*/
    BLE_QWRS_NEW_DATA_RCVD,                          /* On this event, the application can act upon the received data*/
} nrf_ble_qwrs_evt_type;


/**@brief Queued Write Example Service event. */
typedef struct
{
    nrf_ble_qwrs_evt_type evt_type;                        //!< Type of event.
    uint16_t              rcv_length;
    uint8_t               rcvd_data[NRF_BLE_QWRS_MAX_RCV_SIZE];
} nrf_ble_qwrs_evt_t;

// Forward declaration of the nrf_ble_qwrs_t type.
struct nrf_ble_qwrs_t;

/**@brief Queued Write Example Service event handler type. returns a BLE_GATT_STATUS_CODES */
typedef uint16_t (*nrf_ble_qwrs_evt_handler_t) (struct nrf_ble_qwrs_t * p_qwrs,
                                                nrf_ble_qwrs_evt_t    * p_evt);

typedef struct
{
    nrf_ble_qwrs_evt_handler_t   	evt_handler;       	//!< Event handler to be called for handling events in the Queued Write Example  Service.
    ble_srv_error_handler_t      	error_handler;     	//!< Function to be called in case of an error.
    nrf_ble_qwr_t              		*p_qwr_ctx;         //!< pointer to the initialized queued write context
} nrf_ble_qwrs_init_t;


typedef struct nrf_ble_qwrs_t
{
    uint8_t                    uuid_type;               //!< UUID type.
    uint16_t                   service_handle;          //!< Handle of Queued Write Example  Service (as provided by the BLE stack).
    uint16_t                   conn_handle;             //!< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).
    nrf_ble_qwrs_evt_handler_t evt_handler;             //!< Event handler to be called for handling events in the Queued Write Example  Service.
    ble_srv_error_handler_t    error_handler;           //!< Function to be called in case of an error.
    ble_gatts_char_handles_t   long_charact_handles;    //!< Handles related to the Queued Write Example long characteristic.
    ble_gatts_char_handles_t   charact_handles;         //!< Handles related to the Queued Write Example characteristic.
    ble_gatts_char_handles_t   type_id_value_handles;   /**< Handles related to the type id characteristic. */
    ble_gatts_char_handles_t   image_value_handles;     /**< Handles related to the image characteristic. */
} nrf_ble_qwrs_t;


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

/**@brief Function for updating the custom value.
 *
 * @details The application calls this function when the cutom value should be updated. If
 *          notification has been enabled, the custom value characteristic is sent to the client.
 *
 * @note 
 *       
 * @param[in]   p_bas          Custom Service structure.
 * @param[in]   Custom value 
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_cus_custom_value_update(ble_cus_t * p_cus, uint8_t custom_value);

uint32_t long_string_value_update(nrf_ble_qwrs_t *p_qwrs, uint8_t *ls_update_data, uint16_t ls_update_data_len);

/**@brief Initialize Custom Service.
 */
void cs_init(void);

/**@brief data notification
 */
uint32_t ble_cs_notify_data(ble_cus_t *p_cus, uint8_t *u8_buffer, uint16_t len, uint8_t notify_type);

/**@brief Function for initializing the Queued Write Example Service.
 *
 * @details This call allows the application to initialize the Queued Write Example Service.
 *
 * @param[in]   p_qwrs_init  Information needed to initialize the service.
 * @param[out]  p_qwrs       Queued Write Example Service structure.

 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t nrf_ble_qwrs_init(nrf_ble_qwrs_init_t *p_qwrs_init, nrf_ble_qwrs_t *p_qwrs);


/**@brief event handler function for handling event from the queued write module.
 *
 * @param[in]  p_qwrs     Queued Write Example Service structure.
 * @param[in]  p_qwr      Queued Write structure.
 * @param[in]  p_evt      event received from the QWR module.
 *
 * @return      BLE_GATT_STATUS_SUCCESS if the received data are accepted, error code otherwise.
 */
uint16_t nrf_ble_qwrs_on_qwr_evt(nrf_ble_qwrs_t *p_qwrs,
                                 nrf_ble_qwr_t * p_qwr,
                                 nrf_ble_qwr_evt_t * p_evt);


#endif // BLE_CUS_H__
