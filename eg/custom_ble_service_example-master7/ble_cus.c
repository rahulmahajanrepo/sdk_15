#include "sdk_common.h"
#include "ble_cus.h"
#include <string.h>
#include "ble_srv_common.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_log.h"

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
    p_cus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    ble_cus_evt_t evt;

    evt.evt_type = BLE_CUS_EVT_CONNECTED;

    p_cus->evt_handler(p_cus, &evt);
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    ble_cus_evt_t evt;

    evt.evt_type = BLE_CUS_EVT_DISCONNECTED;

    p_cus->evt_handler(p_cus, &evt);
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    
    // Custom Value Characteristic Written to.
    if (p_evt_write->handle == p_cus->custom_value_handles.value_handle)
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
    if ((p_evt_write->handle == p_cus->custom_value_handles.cccd_handle)
        && (p_evt_write->len == 2)
       )
    {
        // CCCD written, call application event handler
        if (p_cus->evt_handler != NULL)
        {
            ble_cus_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_CUS_EVT_NOTIFICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_CUS_EVT_NOTIFICATION_DISABLED;
            }
            // Call the application event handler.
            p_cus->evt_handler(p_cus, &evt);
        }
    }

}

void ble_cus_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_cus_t * p_cus = (ble_cus_t *) p_context;
    
 //   NRF_LOG_INFO("BLE event received. Event type = %d\r\n", p_ble_evt->header.evt_id); 
    if (p_cus == NULL || p_ble_evt == NULL)
    {
        return;
    }
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_cus, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_cus, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_cus, p_ble_evt);
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
 * @param[in]   p_cus        Battery Service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t custom_value_char_add(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init)
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
    
    cccd_md.write_perm = p_cus_init->custom_value_char_attr_md.cccd_write_perm;
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md; 
    char_md.p_sccd_md         = NULL;
		
    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = CUSTOM_VALUE_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_cus_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->custom_value_char_attr_md.write_perm;
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

    err_code = sd_ble_gatts_characteristic_add(p_cus->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_cus->custom_value_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

uint32_t ble_cus_init(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init)
{
    if (p_cus == NULL || p_cus_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_cus->evt_handler               = p_cus_init->evt_handler;
    p_cus->conn_handle               = BLE_CONN_HANDLE_INVALID;

    // Add Custom Service UUID
    ble_uuid128_t base_uuid = {CUSTOM_SERVICE_UUID_BASE};
    err_code =  sd_ble_uuid_vs_add(&base_uuid, &p_cus->uuid_type);
    VERIFY_SUCCESS(err_code);
    
    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = CUSTOM_SERVICE_UUID;

    // Add the Custom Service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cus->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Custom Value characteristic
    return custom_value_char_add(p_cus, p_cus_init);
}

uint32_t ble_cus_custom_value_update(ble_cus_t * p_cus, uint8_t custom_value)
{
//    NRF_LOG_INFO("In ble_cus_custom_value_update. \r\n"); 
    if (p_cus == NULL)
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
    err_code = sd_ble_gatts_value_set(p_cus->conn_handle,
                                      p_cus->custom_value_handles.value_handle,
                                      &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Send value if connected and notifying.
    if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID)) 
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_cus->custom_value_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len  = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
//        NRF_LOG_INFO("sd_ble_gatts_hvx result: %x. \r\n", err_code); 
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
//        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE. \r\n"); 
    }


    return err_code;
}

/**@brief Update Read characteristic data 
 */
uint32_t long_string_value_update(nrf_ble_qwrs_t *p_qwrs, uint8_t *ls_update_data, uint16_t ls_update_data_len)
{
    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;
	
    if(p_qwrs == NULL)
    {
        return NRF_ERROR_NULL;
    }
	
	NRF_LOG_INFO("long string data read value update: "); 

    memset(&gatts_value, 0, sizeof(gatts_value));
    gatts_value.len     = ls_update_data_len;
    gatts_value.offset  = 0;
    gatts_value.p_value = ls_update_data;

    err_code = sd_ble_gatts_value_set(p_qwrs->conn_handle, 
									  p_qwrs->long_charact_handles.value_handle,
                                      &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return err_code;
}

/**@brief Queued Write Events 
 */
uint16_t nrf_ble_qwrs_on_qwr_evt(nrf_ble_qwrs_t *p_qwrs,
                                 nrf_ble_qwr_t * p_qwr,
                                 nrf_ble_qwr_evt_t * p_evt)
{
    if (p_qwrs->evt_handler != NULL)
    {
        nrf_ble_qwrs_evt_t cur_evt;
        cur_evt.rcv_length = NRF_BLE_QWRS_MAX_RCV_SIZE;
        ret_code_t err_code = nrf_ble_qwr_value_get(p_qwr, p_evt->attr_handle, cur_evt.rcvd_data, &cur_evt.rcv_length);
        if (err_code != NRF_SUCCESS)
        {
            NRF_LOG_ERROR("nrf_ble_qwr_value_get failed.");
            return BLE_GATT_STATUS_ATTERR_INSUF_AUTHORIZATION;
        }
//		else
//		{
//			NRF_LOG_ERROR("Event Type = %d, Error = %d \n", p_evt->evt_type, err_code);
//		}
		
        if (p_evt->evt_type == NRF_BLE_QWR_EVT_AUTH_REQUEST)
        {
            cur_evt.evt_type = BLE_QWRS_CHECK_RCVD_DATA;
            return p_qwrs->evt_handler(p_qwrs, &(cur_evt));
        }
        else if (p_evt->evt_type == NRF_BLE_QWR_EVT_EXECUTE_WRITE)
        {
            if (p_qwrs->evt_handler != NULL)
            {
                cur_evt.evt_type = BLE_QWRS_NEW_DATA_RCVD;
                return p_qwrs->evt_handler(p_qwrs, &(cur_evt));
            }
        }
//		else
//		{
//			NRF_LOG_ERROR("Event Type = %d \n", p_evt->evt_type);
//		}
    }
    return BLE_GATT_STATUS_SUCCESS;
}

/**@brief Queued Write Init 
 */
uint32_t nrf_ble_qwrs_init(nrf_ble_qwrs_init_t *p_qwrs_init, nrf_ble_qwrs_t *p_qwrs)
{
    ret_code_t    err_code;
    ble_uuid_t    ble_uuid;

    // Initialize service structure.
    p_qwrs->evt_handler   = p_qwrs_init->evt_handler;
    p_qwrs->error_handler = p_qwrs_init->error_handler;
    p_qwrs->conn_handle   = BLE_CONN_HANDLE_INVALID;

    // Add Custom Service UUID
    ble_uuid128_t base_uuid = {NEW_CUSTOM_SERVICE_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_qwrs->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_qwrs->uuid_type;
    ble_uuid.uuid = NEW_CUSTOM_SERVICE_UUID;

    // Add the Custom Service
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_qwrs->service_handle);
    VERIFY_SUCCESS(err_code);

//	NRF_LOG_INFO("Primary Service Handle Value = %d\r\n", p_qwrs->service_handle); 

    //Add Long characteristic
    ble_add_char_params_t add_char_params;

    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = LONG_STRING_CHAR;
    add_char_params.uuid_type        = p_qwrs->uuid_type;
    add_char_params.max_len          = BLE_QWRS_MAX_LONG_CHAR_LEN;
    add_char_params.init_len         = 0;
    add_char_params.char_props.write = true;
	add_char_params.char_props.read  = true;
//	add_char_params.char_props.notify= true;
    add_char_params.write_access     = SEC_OPEN;
	add_char_params.read_access      = SEC_OPEN;
    add_char_params.is_defered_write = true;

    err_code = characteristic_add(p_qwrs->service_handle,
                              &add_char_params,
                              &p_qwrs->long_charact_handles);
    VERIFY_SUCCESS(err_code);

//    NRF_LOG_INFO("QLong String handle = %d\r\n", p_qwrs->long_charact_handles.value_handle);
		
	if (p_qwrs_init->p_qwr_ctx != NULL)
    {
        err_code = nrf_ble_qwr_attr_register(p_qwrs_init->p_qwr_ctx,
                                             p_qwrs->long_charact_handles.value_handle);
        VERIFY_SUCCESS(err_code);
    }
    return err_code;
}


