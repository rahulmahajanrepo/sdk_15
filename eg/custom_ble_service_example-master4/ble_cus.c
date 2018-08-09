#include "sdk_common.h"
#include "ble_cus.h"
#include <string.h>
#include "ble_srv_common.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_log.h"


unsigned char 													type_id_data[TYPE_ID_BUFFER_SIZE];
unsigned char 													long_string_data[LONG_STRING_BUFFER_SIZE];
unsigned char 													image_data[IMAGE_BUFFER_SIZE];
uint8_t 														notify_ack[NOTIFY_BUFFER_SIZE];
uint8_t 														data_rcved_on_ble_type = TYPE_IDEAL;

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
    p_cus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

//    ble_cus_evt_t evt;

//    evt.evt_type = BLE_CUS_EVT_CONNECTED;

//    p_cus->evt_handler(p_cus, &evt);
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
    
//    ble_cus_evt_t evt;

//    evt.evt_type = BLE_CUS_EVT_DISCONNECTED;

//    p_cus->evt_handler(p_cus, &evt);
}

/**@brief data notification. ble device -> phone
 */
uint32_t ble_cs_notify_data(ble_cus_t *p_cus, uint8_t *u8_buffer, uint16_t len, uint8_t notify_type)
{
	uint32_t               	err_code;
	ble_gatts_hvx_params_t 	hvx_params;
	
	if(p_cus->conn_handle == BLE_CONN_HANDLE_INVALID) return NRF_ERROR_INVALID_STATE;

	NRF_LOG_INFO("\nDevice to Mobile -->");
	
	memset(&hvx_params, 0, sizeof(hvx_params));
	
	if(notify_type == TYPE_ID_NOTIFY)
	{
		NRF_LOG_INFO("\n[N]Type ID Data:");
		len = TYPE_ID_BUFFER_SIZE;
		hvx_params.handle   = p_cus->type_id_value_handles.value_handle;
	}
	else if(notify_type == LONG_STRING_NOTIFY)
	{
		NRF_LOG_INFO("\n[N]Long String Data:"); 
		len = LONG_STRING_BUFFER_SIZE;
		hvx_params.handle   = p_cus->long_string_value_handles.value_handle;
	}
	else if(notify_type == IMAGE_NOTIFY)
	{
		NRF_LOG_INFO("\n[N]Image Data:"); 
		len = IMAGE_BUFFER_SIZE;
		hvx_params.handle   = p_cus->image_value_handles.value_handle;
	}
	hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
	hvx_params.offset   = 0;
	hvx_params.p_len    = &len;
	hvx_params.p_data   = u8_buffer;
	
	NRF_LOG_RAW_HEXDUMP_INFO(hvx_params.p_data, len);
	
	err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
	VERIFY_SUCCESS(err_code);	
	
	return NRF_SUCCESS;
}

/*
* Update parameter as per operation request
*/
static void cs_write_data(uint16_t char_uuid, uint8_t const *data, uint8_t len)
{
	NRF_LOG_INFO("\n --> Mobile to Device:");
	memset(notify_ack, 0, NOTIFY_BUFFER_SIZE);
	memcpy(notify_ack, data, len);
	switch(char_uuid)
	{
		case TYPE_ID_CHAR:
			NRF_LOG_INFO("\nType ID Data:");
//		memset(type_id_data, 0, sizeof(ble_write_data));
			memcpy(type_id_data, data, len);
			NRF_LOG_RAW_HEXDUMP_INFO(type_id_data, len);
			data_rcved_on_ble_type = TYPE_ID_NOTIFY;
		break;
		
		case LONG_STRING_CHAR:
			NRF_LOG_INFO("\nLong String Data:"); 
//		memset(long_string_data, 0, sizeof(ble_write_data));
			memcpy(long_string_data, data, len);
			NRF_LOG_RAW_HEXDUMP_INFO(long_string_data, len);
			data_rcved_on_ble_type = LONG_STRING_NOTIFY;
		break;
		
		case IMAGE_CHAR:
			NRF_LOG_INFO("\nImage Data:"); 
//		memset(image_data, 0, sizeof(ble_write_data));
			memcpy(image_data, data, len);
			NRF_LOG_RAW_HEXDUMP_INFO(image_data, len);
			data_rcved_on_ble_type = IMAGE_NOTIFY;
		break;

		default:
			NRF_LOG_RAW_HEXDUMP_INFO(data, len);
		break;
	}
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	
//	NRF_LOG_RAW_HEXDUMP_INFO((void *)&p_ble_evt->evt.gatts_evt, sizeof(p_ble_evt->evt.gatts_evt)); 
//	
//	NRF_LOG_INFO("handle = %d\r\n", p_evt_write->handle); 
//	NRF_LOG_INFO("uuid = %d\r\n", p_evt_write->uuid.uuid); 
//	NRF_LOG_INFO("uuid type = %d\r\n", p_evt_write->uuid.type); 
//	NRF_LOG_INFO("op = %d\r\n", p_evt_write->op); 
//	NRF_LOG_INFO("auth_required = %d\r\n", p_evt_write->auth_required); 
//	NRF_LOG_INFO("offset = %d\r\n", p_evt_write->offset); 
//	NRF_LOG_INFO("len = %d\r\n", p_evt_write->len); 
//	NRF_LOG_INFO("data = %d\r\n", p_evt_write->data);
//	
//	NRF_LOG_RAW_HEXDUMP_INFO(p_evt_write->data, p_evt_write->len);
	
	    
	if((p_evt_write->handle == p_cus->type_id_value_handles.value_handle) ||
			(p_evt_write->handle == p_cus->long_string_value_handles.value_handle) ||
			(p_evt_write->handle == p_cus->image_value_handles.value_handle))
	{
//		if(p_evt_write->handle == p_cus->type_id_value_handles.value_handle)
//		{
//			NRF_LOG_INFO("Type ID handle = %d\r\n", p_evt_write->handle); 
//		}
//		else if(p_evt_write->handle == p_cus->long_string_value_handles.value_handle)
//		{
//			NRF_LOG_INFO("Long String handle = %d\r\n", p_evt_write->handle); 
//		}
//		else if(p_evt_write->handle == p_cus->image_value_handles.value_handle)
//		{
//			NRF_LOG_INFO("Image handle = %d\r\n", p_evt_write->handle); 
//		}
		
			cs_write_data(p_evt_write->uuid.uuid, p_evt_write->data, p_evt_write->len);
	}
//	else 
//	{
//		NRF_LOG_INFO("unknown write handle = %d\r\n", p_evt_write->handle); 
//	}
		
//	NRF_LOG_RAW_HEXDUMP_INFO(p_evt_write->data, p_evt_write->len);
	
//    // Custom Value Characteristic Written to.
//    if (p_evt_write->handle == p_cus->custom_value_handles.value_handle)
//    {
//        nrf_gpio_pin_toggle(LED_4);
//        /*
//        if(*p_evt_write->data == 0x01)
//        {
//            nrf_gpio_pin_clear(20); 
//        }
//        else if(*p_evt_write->data == 0x02)
//        {
//            nrf_gpio_pin_set(20); 
//        }
//        else
//        {
//          //Do nothing
//        }
//        */
//    }
}

void ble_cus_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_cus_t * p_cus = (ble_cus_t *) p_context;
    
//    NRF_LOG_INFO("BLE Event type = %d\n", p_ble_evt->header.evt_id); 
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


/**@brief Add characteristic as a string 
 * @param[in]   p_custom       custom Service structure.
 * @param[in]   uuid           UUID of characteristic to be added.
 * @param[in]   p_char_value   Initial value of characteristic to be added.
 * @param[in]   char_len       Length of initial value. This will also be the maximum value.
 * @param[out]  p_handles      Handles of new characteristic.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t custom_char_add_string(ble_cus_t *								p_cus,
																			uint16_t        						uuid,
																			uint8_t *      	 						p_char_value,
																			uint16_t        						char_len,
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

		ble_uuid.type = p_cus->uuid_type;
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

    return sd_ble_gatts_characteristic_add(p_cus->service_handle, &char_md, &attr_char_value, p_handles);
}

//Chars initilise foor custome service
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
	
//	NRF_LOG_INFO("Service Handle Value = %d\r\n", p_cus->service_handle); 

	// Add Custom Value characteristic	
	if(p_cus_init->type_id_data.length > 0)
	{		
		err_code = custom_char_add_string(p_cus, TYPE_ID_CHAR,
									p_cus_init->type_id_data.p_str, 
									p_cus_init->type_id_data.length, 					
									&p_cus->type_id_value_handles);					
		VERIFY_SUCCESS(err_code);
//		NRF_LOG_INFO("Type ID handle = %d\r\n", p_cus->type_id_value_handles.value_handle); 
	}		

	if(p_cus_init->long_string_data.length > 0)
	{		
		err_code = custom_char_add_string(p_cus, LONG_STRING_CHAR,
									p_cus_init->long_string_data.p_str, 
									p_cus_init->long_string_data.length, 					
									&p_cus->long_string_value_handles);					
		VERIFY_SUCCESS(err_code);
//		NRF_LOG_INFO("Long String handle = %d\r\n", p_cus->long_string_value_handles.value_handle); 
	}		

	if(p_cus_init->image_data.length > 0)
	{		
		err_code = custom_char_add_string(p_cus, IMAGE_CHAR,
									p_cus_init->image_data.p_str, 
									p_cus_init->image_data.length, 					
									&p_cus->image_value_handles);					
		VERIFY_SUCCESS(err_code);
//		NRF_LOG_INFO("Image handle = %d\r\n", p_cus->image_value_handles.value_handle); 
	}

	return NRF_SUCCESS;
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
