#include "sdk_common.h"
#include "ble_cus.h"
#include <string.h>
#include "ble_srv_common.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_log.h"



///**@brief Function for handling the Connect event.
// *
// * @param[in]   p_cus       Custom Service structure.
// * @param[in]   p_ble_evt   Event received from the BLE stack.
// */
//static void on_connect(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
//{
//    p_cus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

////    ble_cus_evt_t evt;

////    evt.evt_type = BLE_CUS_EVT_CONNECTED;

////    p_cus->evt_handler(p_cus, &evt);
//}

///**@brief Function for handling the Disconnect event.
// *
// * @param[in]   p_cus       Custom Service structure.
// * @param[in]   p_ble_evt   Event received from the BLE stack.
// */
//static void on_disconnect(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
//{
//    UNUSED_PARAMETER(p_ble_evt);
//    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID;
//    
////    ble_cus_evt_t evt;

////    evt.evt_type = BLE_CUS_EVT_DISCONNECTED;

////    p_cus->evt_handler(p_cus, &evt);
//}

///**@brief data notification
// */
//uint32_t ble_cs_notify_data(ble_cus_t *p_cus, uint8_t *u8_buffer, uint16_t len, uint8_t notify_type)
//{
//	uint32_t               	err_code;
//	ble_gatts_hvx_params_t 	hvx_params;
//	
//	if(p_cus->conn_handle == BLE_CONN_HANDLE_INVALID) return NRF_ERROR_INVALID_STATE;

//	NRF_LOG_INFO("\nDevice to Mobile -->");
//	
//	memset(&hvx_params, 0, sizeof(hvx_params));
//	
//	if(notify_type == TYPE_ID_NOTIFY)
//	{
//		NRF_LOG_INFO("\n[N]Type ID Data:");
//		len = TYPE_ID_BUFFER_SIZE;
//		hvx_params.handle   = p_cus->type_id_value_handles.value_handle;
//	}
//	else if(notify_type == LONG_STRING_NOTIFY)
//	{
//		NRF_LOG_INFO("\n[N]Long String Data:"); 
//		len = LONG_STRING_BUFFER_SIZE;
//		hvx_params.handle   = p_cus->long_string_value_handles.value_handle;
//	}
//	else if(notify_type == IMAGE_NOTIFY)
//	{
//		NRF_LOG_INFO("\n[N]Image Data:"); 
//		len = IMAGE_BUFFER_SIZE;
//		hvx_params.handle   = p_cus->image_value_handles.value_handle;
//	}
//	hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
//	hvx_params.offset   = 0;
//	hvx_params.p_len    = &len;
//	hvx_params.p_data   = u8_buffer;
//	
//	NRF_LOG_RAW_HEXDUMP_INFO(hvx_params.p_data, len);
//	
//	err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
//	VERIFY_SUCCESS(err_code);	
//	
//	return NRF_SUCCESS;
//}

/**@brief Add characteristic as a string 
 * @param[in]   p_custom       custom Service structure.
 * @param[in]   uuid           UUID of characteristic to be added.
 * @param[in]   p_char_value   Initial value of characteristic to be added.
 * @param[in]   char_len       Length of initial value. This will also be the maximum value.
 * @param[out]  p_handles      Handles of new characteristic.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t custom_char_add_string(nrf_ble_qwrs_t *				p_qwrs,
										uint16_t        			uuid,
										uint8_t *      	 			p_char_value,
										uint16_t        			char_len,
										ble_gatts_char_handles_t *  p_handles)
{
    ble_uuid_t          ble_uuid;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;

    APP_ERROR_CHECK_BOOL(p_char_value != NULL);
    APP_ERROR_CHECK_BOOL(char_len > 0);
    
    // The ble_gatts_char_md_t structure uses bit fields. So we reset the memory to zero.
    memset(&char_md, 0, sizeof(char_md));
	char_md.char_props.write  = 1;	
	char_md.char_props.read   = 1;	
	char_md.char_props.notify = 1;			
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;

	ble_uuid.type = p_qwrs->uuid_type;
	ble_uuid.uuid = uuid;		
		
    memset(&attr_md, 0, sizeof(attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);	
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);	
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;		
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = char_len;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = char_len;
    attr_char_value.p_value      = p_char_value;

    return sd_ble_gatts_characteristic_add(p_qwrs->service_handle, &char_md, &attr_char_value, p_handles);
}

//uint32_t ble_cus_custom_value_update(ble_cus_t * p_cus, uint8_t custom_value)
//{
//    NRF_LOG_INFO("In ble_cus_custom_value_update. \r\n"); 
//    if (p_cus == NULL)
//    {
//        return NRF_ERROR_NULL;
//    }

//    uint32_t err_code = NRF_SUCCESS;
//    ble_gatts_value_t gatts_value;

//    // Initialize value struct.
//    memset(&gatts_value, 0, sizeof(gatts_value));

//    gatts_value.len     = sizeof(uint8_t);
//    gatts_value.offset  = 0;
//    gatts_value.p_value = &custom_value;

//    // Update database.
//    err_code = sd_ble_gatts_value_set(p_cus->conn_handle,
//                                      p_cus->custom_value_handles.value_handle,
//                                      &gatts_value);
//    if (err_code != NRF_SUCCESS)
//    {
//        return err_code;
//    }

//    // Send value if connected and notifying.
//    if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID)) 
//    {
//        ble_gatts_hvx_params_t hvx_params;

//        memset(&hvx_params, 0, sizeof(hvx_params));

//        hvx_params.handle = p_cus->custom_value_handles.value_handle;
//        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
//        hvx_params.offset = gatts_value.offset;
//        hvx_params.p_len  = &gatts_value.len;
//        hvx_params.p_data = gatts_value.p_value;

//        err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
//        NRF_LOG_INFO("sd_ble_gatts_hvx result: %x. \r\n", err_code); 
//    }
//    else
//    {
//        err_code = NRF_ERROR_INVALID_STATE;
//        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE. \r\n"); 
//    }


//    return err_code;
//}

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

uint32_t nrf_ble_qwrs_init(nrf_ble_qwrs_init_t *p_qwrs_init, nrf_ble_qwrs_t *p_qwrs)
{
    ret_code_t    err_code;
    ble_uuid_t    ble_uuid;

    // Initialize service structure.
    p_qwrs->evt_handler   = p_qwrs_init->evt_handler;
    p_qwrs->error_handler = p_qwrs_init->error_handler;
    p_qwrs->conn_handle   = BLE_CONN_HANDLE_INVALID;

    // Add Custom Service UUID
    ble_uuid128_t base_uuid = {CUSTOM_SERVICE_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_qwrs->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_qwrs->uuid_type;
    ble_uuid.uuid = CUSTOM_SERVICE_UUID;

    // Add the Custom Service
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_qwrs->service_handle);
    VERIFY_SUCCESS(err_code);

//	NRF_LOG_INFO("Primary Service Handle Value = %d\r\n", p_qwrs->service_handle); 

	// Add Custom Value characteristic	
	if(p_qwrs_init->type_id_data.length > 0)
	{		
		err_code = custom_char_add_string(p_qwrs, TYPE_ID_CHAR,
											p_qwrs_init->type_id_data.p_str, 
											p_qwrs_init->type_id_data.length, 					
											&p_qwrs->type_id_value_handles);					
		VERIFY_SUCCESS(err_code);
//		NRF_LOG_INFO("Type ID handle = %d\r\n", p_qwrs->type_id_value_handles.value_handle); 
	}		

	if(p_qwrs_init->image_data.length > 0)
	{		
		err_code = custom_char_add_string(p_qwrs, IMAGE_CHAR,
											p_qwrs_init->image_data.p_str, 
											p_qwrs_init->image_data.length, 					
											&p_qwrs->image_value_handles);					
		VERIFY_SUCCESS(err_code);
//		NRF_LOG_INFO("Image handle = %d\r\n", p_qwrs->image_value_handles.value_handle); 
	}
	
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


