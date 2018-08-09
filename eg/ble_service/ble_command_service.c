#include "sdk_common.h"
#include "ble_command_service.h"
#include <string.h>
#include "ble_srv_common.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_log.h"

/**@brief Function for handling the Connect event.
 *
 * @param[in]   ble_command_service       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_command_service_struct * ble_command_service, ble_evt_t const * p_ble_evt)
{
    ble_command_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    ble_command_evt_t evt;

    evt.evt_type = BLE_COMMAND_EVT_CONNECTED;

    ble_command_service->evt_handler(ble_command_service, &evt);
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   ble_command_service       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_command_service_struct * ble_command_service, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    ble_command_service->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    ble_command_evt_t evt;

    evt.evt_type = BLE_COMMAND_EVT_DISCONNECTED;

    ble_command_service->evt_handler(ble_command_service, &evt);
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   ble_command_service       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_command_service_struct * ble_command_service, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
    // Custom Value Characteristic Written to.
    if (p_evt_write->handle == ble_command_service->custom_value_handles.value_handle)
    {
        nrf_gpio_pin_toggle(LED_4);
        /*
        if(*p_evt_write->data == 0x01)
        {
            nrf_gpio_pin_clear(20); 
        }
        else if(*p_evt_write->data == 0x02)
        {
            nrf_gpio_pin_set(20); 
        }
        else
        {
          //Do nothing
        }
        */
    }

    // Check if the Custom value CCCD is written to and that the value is the appropriate length, i.e 2 bytes.
		// Writing to CCCD enables the notification
    if ((p_evt_write->handle == ble_command_service->custom_value_handles.cccd_handle)
        && (p_evt_write->len == 2)
       )
    {
        // CCCD written, call application event handler
        if (ble_command_service->evt_handler != NULL)
        {
            ble_command_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_COMMAND_EVT_NOTIFICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_COMMAND_EVT_NOTIFICATION_DISABLED;
            }
            // Call the application event handler.
            ble_command_service->evt_handler(ble_command_service, &evt);
        }
    }

}

void ble_command_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_command_service_struct * ble_command_service = (ble_command_service_struct *) p_context;
    
    NRF_LOG_INFO("BLE event received. Event type = %d\r\n", p_ble_evt->header.evt_id); 
    if (ble_command_service == NULL || p_ble_evt == NULL)
    {
        return;
    }
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
						NRF_LOG_INFO("BLE event on_connect.");
            on_connect(ble_command_service, p_ble_evt);
						NRF_LOG_INFO("BLE event on_connect connected.");
            break;

        case BLE_GAP_EVT_DISCONNECTED:
						NRF_LOG_INFO("BLE event on_disconnect.");
            on_disconnect(ble_command_service, p_ble_evt);
						NRF_LOG_INFO("BLE event on_disconnect disconnected.");
            break;

        case BLE_GATTS_EVT_WRITE:
						NRF_LOG_INFO("BLE event on_write.");
            on_write(ble_command_service, p_ble_evt);
						NRF_LOG_INFO("BLE event on_write value written.");
            break;
/* Handling this event is not necessary
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            NRF_LOG_INFO("EXCHANGE_MTU_REQUEST event received.\r\n");
            break;
*/
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for adding the Custom Value characteristic.
 *
 * @param[in]   ble_command_service        Battery Service structure.
 * @param[in]   command_service_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t ble_command_service_value_char_add(ble_command_service_struct * ble_command_service, const ble_command_service_init_t * command_service_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    
    cccd_md.write_perm = command_service_init->custom_value_char_attr_md.cccd_write_perm;
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

	  //char_md, which sets the properties that will be displayed to the central during service discovery.
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md; 
    char_md.p_sccd_md         = NULL;
		
    ble_uuid.type = ble_command_service->uuid_type;
    ble_uuid.uuid = BLE_COMMAND_VALUE_CHAR_UUID;

		//attr_md, which actually sets the properties( i.e. accessability of the attribute).
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = command_service_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = command_service_init->custom_value_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t);

    err_code = sd_ble_gatts_characteristic_add(ble_command_service->service_handle, &char_md,
                                               &attr_char_value,
                                               &ble_command_service->custom_value_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

uint32_t ble_command_service_init(ble_command_service_struct * ble_command_service, const ble_command_service_init_t * command_service_init)
{
    if (ble_command_service == NULL || command_service_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    ble_command_service->evt_handler = command_service_init->evt_handler;
    ble_command_service->conn_handle = BLE_CONN_HANDLE_INVALID; //Should only be valid when we are in connection

    // Add Custom Service UUID
    ble_uuid128_t base_uuid = {BLE_COMMAND_SERVICE_UUID_BASE}; //Add Vendor specific UUID to BLE stack's table
    err_code =  sd_ble_uuid_vs_add(&base_uuid, &ble_command_service->uuid_type);
    VERIFY_SUCCESS(err_code);
    
    ble_uuid.type = ble_command_service->uuid_type;
    ble_uuid.uuid = BLE_COMMAND_SERVICE_UUID;

    // Add the Custom Service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &ble_command_service->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Custom Value characteristic
    return ble_command_service_value_char_add(ble_command_service, command_service_init);
}

uint32_t ble_command_service_value_update(ble_command_service_struct * ble_command_service, uint8_t custom_value)
{
    NRF_LOG_INFO("In ble_cus_custom_value_update. \r\n"); 
    if (ble_command_service == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = sizeof(uint8_t);
    gatts_value.offset  = 0;
    gatts_value.p_value = &custom_value;

    // Update database.
    err_code = sd_ble_gatts_value_set(ble_command_service->conn_handle,
                                      ble_command_service->custom_value_handles.value_handle,
                                      &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Send value if connected and notifying.
    if ((ble_command_service->conn_handle != BLE_CONN_HANDLE_INVALID)) 
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = ble_command_service->custom_value_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len  = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(ble_command_service->conn_handle, &hvx_params);
        NRF_LOG_INFO("sd_ble_gatts_hvx result: %x. \r\n", err_code); 
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE. \r\n"); 
    }


    return err_code;
}
