#include "ap_state_config.h"
#include "audio_encoder.h"
#include "ap_state_resource.h"

///INT32U CRC32_tbl[256];
SYSTEM_USER_OPTION Global_User_Optins;
///USER_ITEMS user_item;

//	prototypes
void ap_state_config_default_set(void);
extern INT32S ext_rtc_time_set(t_rtc *rtc_time);
void ap_state_config_initial(INT32S status)
{
	//default setting
	Global_User_Optins.item.ifdirty = 0;
	if (status == STATUS_FAIL) { /* check nvdb initial status */
		ap_state_config_default_set();
		}

}

void ap_state_config_default_set(void)
{
	// some default config is 0
	gp_memset((INT8S *)&Global_User_Optins, 0x00, sizeof(SYSTEM_USER_OPTION));
	///ap_state_resource_user_option_load(&Global_User_Optins);
	ap_state_config_voice_record_switch_set(1); //mic on
    ap_state_config_video_resolution_set(0);	//720
	ap_state_config_burst_set(0);	//连拍off
	ap_state_config_ev_set(6);
	ap_state_config_ev1_set(6);
	ap_state_config_beep_sound_set(1);
	ap_state_config_capture_date_stamp_set(2);
	ap_state_config_date_stamp_set(2);
	//ap_state_config_quality_set(2);

	ap_state_config_LDW_to_defalt();
#if TV_DET_ENABLE   // for tv out
       ap_state_config_tv_out_set(0);//TV OUT: NTSC
#endif   // end for tv out
	ap_state_config_pic_size_set(0);//8--1280*720
#if ENABLE_CHECK_RTC == 1
	ap_state_config_data_time_save_set_default();
#endif
	ap_state_config_record_time_set(3);//循环录像的时间，设置为0表示关闭循环录像功能
	//ap_state_config_pic_size_set(8);
	ap_state_resource_time_stamp_position_x_set(65);
	ap_state_resource_time_stamp_position_y_set(434);

	Global_User_Optins.item.ifdirty = 1;
}

void ap_state_config_store(void)
{
	return;
	if (Global_User_Optins.item.ifdirty == 1) {
		/* check SPIFC exist or not */
		volatile INT32U *pSYM = (volatile INT32U *)(0xB0000000);
		if (*pSYM==0x564E5047)	// "GPNV" 0x564E5047
		{
			/* calculate CRC */
			///CRC_cal((INT8U*)&Global_User_Optins.item, sizeof(USER_ITEMS),Global_User_Optins.crc);
			DBG_PRINT("store CRC = %02x %02x %02x %02x\r\n",Global_User_Optins.crc[0],Global_User_Optins.crc[1],Global_User_Optins.crc[2],Global_User_Optins.crc[3]);
	    	nvmemory_user_sector_store(0, (INT32U *)&Global_User_Optins, 1);
	    	///gp_memcpy((INT8S*)&user_item, (INT8S*)&Global_User_Optins.item, sizeof(USER_ITEMS));
	    	nvmemory_user_sector_load(0, (INT32U *)&Global_User_Optins, 1);
	    	/*if (gp_memcmp((INT8S*)&user_item, (INT8S*)&Global_User_Optins.item, sizeof(USER_ITEMS)) != 0) {
	    		DBG_PRINT("verify failed, store again\r\n");
				gp_memcpy((INT8S*)&Global_User_Optins.item,(INT8S*)&user_item, sizeof(USER_ITEMS));
	    		CRC_cal((INT8U*)&Global_User_Optins.item, sizeof(USER_ITEMS),Global_User_Optins.crc);
	    		nvmemory_user_sector_store(0, (INT32U *)&Global_User_Optins, 1);
			}*/
		}
		Global_User_Optins.item.ifdirty = 0;
    }
}
void ap_state_config_store_power_off(void)
{
	OS_CPU_SR cpu_sr;
	DBG_PRINT("config_store_power_off\r\n");
	OS_ENTER_CRITICAL();
	DBG_PRINT("Global_User_Optins.item.ifdirty = %d \r\n",Global_User_Optins.item.ifdirty);
	if (Global_User_Optins.item.ifdirty == 1) {
		/* check SPIFC exist or not */
//		volatile INT32U *pSYM = (volatile INT32U *)(0xB0000000);
//		DBG_PRINT("*pSYM =0x%x (GPNV 0x564e5047)\r\n",*pSYM);
//		if (*pSYM==0x564E5047)	// "GPNV" 0x564E5047
		{
			/* calculate CRC */
	/*		CRC_cal((INT8U*)&Global_User_Optins.item, sizeof(USER_ITEMS),Global_User_Optins.crc);
			DBG_PRINT("store CRC = %02x %02x %02x %02x\r\n",Global_User_Optins.crc[0],Global_User_Optins.crc[1],Global_User_Optins.crc[2],Global_User_Optins.crc[3]);
	    	nvmemory_user_sector_store(0, (INT32U *)&Global_User_Optins, 1);
	    	gp_memcpy((INT8S*)&user_item, (INT8S*)&Global_User_Optins.item, sizeof(USER_ITEMS));
	    	nvmemory_user_sector_load(0, (INT32U *)&Global_User_Optins, 1);
	    	if (gp_memcmp((INT8S*)&user_item, (INT8S*)&Global_User_Optins.item, sizeof(USER_ITEMS)) != 0) {
	    		DBG_PRINT("verify failed, store again\r\n");
				gp_memcpy((INT8S*)&Global_User_Optins.item,(INT8S*)&user_item, sizeof(USER_ITEMS));
	    		CRC_cal((INT8U*)&Global_User_Optins.item, sizeof(USER_ITEMS),Global_User_Optins.crc);
	    		nvmemory_user_sector_store(0, (INT32U *)&Global_User_Optins, 1);
			}*/
		}
		Global_User_Optins.item.ifdirty = 0;
    }
    OS_EXIT_CRITICAL();
}
INT32S ap_state_config_load(void)
{
	INT8U expect_crc[4];

	nvmemory_user_sector_load(0, (INT32U *)&Global_User_Optins, 1);		//load user setting
	DBG_PRINT("load CRC = %02x %02x %02x %02x\r\n",Global_User_Optins.crc[0],Global_User_Optins.crc[1],Global_User_Optins.crc[2],Global_User_Optins.crc[3]);
	/* calculate expect CRC */
	CRC_cal((INT8U*)&Global_User_Optins.item, sizeof(USER_ITEMS),expect_crc);
	DBG_PRINT("expect CRC = %02x %02x %02x %02x\r\n",expect_crc[0],expect_crc[1],expect_crc[2],expect_crc[3]);
	/* compare CRC and expect CRC */
	if (gp_memcmp((INT8S*)expect_crc, (INT8S*)Global_User_Optins.crc, 4) != 0) {
		DBG_PRINT("crc error\r\n");
		return STATUS_FAIL; /* CRC error */
	}
	return STATUS_OK;
}

// 0:+2, 1:+5/3, 2:+4/3, 3:+1.0, 4:+2/3, 5:+1/3, 6:+0.0, 7:-1/3, 8:-2/3, 9:-1.0, 10:-4/3, 11:-5/3, 12:-2.0 
void ap_state_config_ev_set(INT8U ev)
{
	if (ev != ap_state_config_ev_get()) {
		Global_User_Optins.item.midi_exist_mode = ev;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_ev_get(void)
{
	return Global_User_Optins.item.midi_exist_mode;
}

void ap_state_config_ev1_set(INT8U ev)
{
	if (ev != ap_state_config_ev1_get()) {
		Global_User_Optins.item.save_as_logo_flag = ev;
		Global_User_Optins.item.ifdirty = 1;
	}
}
INT8U ap_state_config_ev1_get(void)
{
	return Global_User_Optins.item.save_as_logo_flag;
}

void ap_state_config_anti_shaking_set(INT8U onoff)
{
	if (onoff != ap_state_config_anti_shaking_get()) {
		Global_User_Optins.item.powertime_modual = onoff;
		Global_User_Optins.item.ifdirty = 1;
	}
}
INT8U ap_state_config_anti_shaking_get(void)
{
	return Global_User_Optins.item.powertime_modual;
}

void ap_state_config_white_balance_set(INT8U white_balance)
{
	if (white_balance != ap_state_config_white_balance_get()) {
		Global_User_Optins.item.alarm_mute2 = white_balance;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_white_balance_get(void)
{
	return Global_User_Optins.item.alarm_mute2;
}

void ap_state_config_video_resolution_set(INT8U resolution)
{
	if (resolution != ap_state_config_video_resolution_get()) {
		Global_User_Optins.item.video_resolution = resolution;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_video_resolution_get(void)
{
	return Global_User_Optins.item.video_resolution;
}

void ap_state_config_md_set(INT8U md)
{
	if (md != ap_state_config_md_get()) {
		Global_User_Optins.item.alarm_volume2 = md;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_md_get(void)
{
	return Global_User_Optins.item.alarm_volume2;
}

void ap_state_config_voice_record_switch_set(INT8U mic_switch)
{
	if (mic_switch != ap_state_config_voice_record_switch_get()) {
		Global_User_Optins.item.alarm_volume1 = mic_switch;
		Global_User_Optins.item.ifdirty = 1;
	}

	// modify josephhsieh@140822    // voice record :OFF  => put silence to avi file
	/*
	if(mic_switch != 0) {
		audio_encode_audio_format_set(WAVE_FORMAT_PCM);
	} else {
		audio_encode_audio_format_set(0);
	}
	*/
}

INT8U ap_state_config_voice_record_switch_get(void)
{
	return Global_User_Optins.item.alarm_volume1;
}


void ap_state_config_G_sensor_set(INT8U Gsensor)
{
	if (Gsensor != ap_state_config_G_sensor_get()) {
		Global_User_Optins.item.slideshow_bg_music[1] = Gsensor;
		Global_User_Optins.item.ifdirty = 1;
		ap_gsensor_set_sensitive(Gsensor);
	}
}

INT8U ap_state_config_G_sensor_get(void)
{
	return Global_User_Optins.item.slideshow_bg_music[1];
}

void ap_state_config_park_mode_G_sensor_set(INT8U Gsensor)
{
	if (Gsensor != ap_state_config_park_mode_G_sensor_get()) {
		Global_User_Optins.item.slideshow_bg_music[2] = Gsensor;
		Global_User_Optins.item.ifdirty = 1;
		//ap_gsensor_set_sensitive(Gsensor);
	}
}

INT8U ap_state_config_park_mode_G_sensor_get(void)
{
	return Global_User_Optins.item.slideshow_bg_music[2];
}







INT8U ap_state_config_LDW_get(INT8U LDW_choice)
{
	INT8U value;
	value = 0xff;
	if(LDW_choice == LDW_ON_OFF){
		value =	Global_User_Optins.item.nv_alarm_struct1.b_alarm_flag;
	}else if(LDW_choice == LDW_CAR_TYPE){
		value =	Global_User_Optins.item.nv_alarm_struct1.B_alarm_hour;
	}else if(LDW_choice == LDW_SENSITIVITY){
		value =	Global_User_Optins.item.nv_alarm_struct1.B_alarm_minute;
	}else if(LDW_choice == LDW_AREA_CHOICE){
		value =	Global_User_Optins.item.nv_alarm_struct1.Alarm_Music_idx[0];
	}else if(LDW_choice == LDW_START_SPEED){
		value =	Global_User_Optins.item.nv_alarm_struct1.Alarm_Music_idx[1];
	}else if(LDW_choice == LDW_ONOFF_SOUND){
		value =	Global_User_Optins.item.nv_alarm_struct1.Alarm_Music_idx[2];
	}
	
	return value;
}

void ap_state_config_LDW_set(INT8U LDW_choice,INT8U value)
{
	if(LDW_choice == LDW_ON_OFF){
		if (value != ap_state_config_LDW_get(LDW_ON_OFF)) {
			Global_User_Optins.item.nv_alarm_struct1.b_alarm_flag = value;
			Global_User_Optins.item.ifdirty = 1;
		}
	}else if(LDW_choice == LDW_CAR_TYPE){
		if (value != ap_state_config_LDW_get(LDW_CAR_TYPE)) {
			Global_User_Optins.item.nv_alarm_struct1.B_alarm_hour = value;
			Global_User_Optins.item.ifdirty = 1;
		}
	}else if(LDW_choice == LDW_SENSITIVITY){
		if (value != ap_state_config_LDW_get(LDW_SENSITIVITY)) {
			Global_User_Optins.item.nv_alarm_struct1.B_alarm_minute = value;
			Global_User_Optins.item.ifdirty = 1;
		}
	}else if(LDW_choice == LDW_AREA_CHOICE){
		if (value != ap_state_config_LDW_get(LDW_AREA_CHOICE)) {
			Global_User_Optins.item.nv_alarm_struct1.Alarm_Music_idx[0] = value;
			Global_User_Optins.item.ifdirty = 1;
		}
	}else if(LDW_choice == LDW_START_SPEED){
		if (value != ap_state_config_LDW_get(LDW_START_SPEED)) {
			Global_User_Optins.item.nv_alarm_struct1.Alarm_Music_idx[1] = value;
			Global_User_Optins.item.ifdirty = 1;
		}
	}else if(LDW_choice == LDW_ONOFF_SOUND){
		if (value != ap_state_config_LDW_get(LDW_ONOFF_SOUND)) {
			Global_User_Optins.item.nv_alarm_struct1.Alarm_Music_idx[2] = value;
			Global_User_Optins.item.ifdirty = 1;
		}
	}
}

void ap_LDW_set_by_config(INT8U LDW_choice,INT8U value)
{
	if(LDW_choice == LDW_ON_OFF){
		ap_state_config_LDW_set(LDW_choice,value);
	}else if(LDW_choice == LDW_CAR_TYPE){
		ap_state_config_LDW_set(LDW_choice,value);
	}else if(LDW_choice == LDW_SENSITIVITY){
		ap_state_config_LDW_set(LDW_choice,value);
	}else if(LDW_choice == LDW_AREA_CHOICE){
		if(value == 0){
			value = 1;
			ap_state_config_LDW_set(LDW_choice,value);
		}else if(value == 1){
			value = 0;
			ap_state_config_LDW_set(LDW_choice,value);
		}
	}else if(LDW_choice == LDW_START_SPEED){
		ap_state_config_LDW_set(LDW_choice,value);
	}else if(LDW_choice == LDW_ONOFF_SOUND){
		ap_state_config_LDW_set(LDW_choice,value);
	}

}
INT8U ap_LDW_get_from_config(INT8U LDW_choice)
{
	INT8U value;
	if(LDW_choice == LDW_ON_OFF){
		value = ap_state_config_LDW_get(LDW_choice);
	}else if(LDW_choice == LDW_CAR_TYPE){
		value = ap_state_config_LDW_get(LDW_choice);
	}else if(LDW_choice == LDW_SENSITIVITY){
		value = ap_state_config_LDW_get(LDW_choice);
	}else if(LDW_choice == LDW_AREA_CHOICE){
		value = ap_state_config_LDW_get(LDW_choice);
		if(value == 0){
			value = 1;
		}else if(value == 1){
			value = 0;
		}
	}else if(LDW_choice == LDW_START_SPEED){
		value = ap_state_config_LDW_get(LDW_choice);
	}else if(LDW_choice == LDW_ONOFF_SOUND){
		value = ap_state_config_LDW_get(LDW_choice);
	}
	return value;
}


void ap_state_config_LDW_to_defalt(void)
{
	ap_LDW_set_by_config(LDW_ON_OFF,0); //0 = 關, 1 = 開
	ap_LDW_set_by_config(LDW_CAR_TYPE,0);// 0 = 轎車, 1 = 休旅車, 2 = 特高車    , defult = 0 
	ap_LDW_set_by_config(LDW_SENSITIVITY,1);// 敏感度, 0 = 低, 1 = 高           , defult = 1
	ap_LDW_set_by_config(LDW_AREA_CHOICE,1);// 0 = 台灣, 1 = 大陸               , defult = 1 
	ap_LDW_set_by_config(LDW_START_SPEED,1);// 啟動車速, 0 = high, 1 = low      , defult = 1
	ap_LDW_set_by_config(LDW_ONOFF_SOUND,0);// on\off提示音, 0 = off, 1 = on    , defult = 0
}


extern INT32U last_pic_size;//unit: KB
extern INT32U last_pic_size_temp;
void ap_state_config_pic_size_set(INT8U pic_size)
{
	if (pic_size != ap_state_config_pic_size_get()) {
		Global_User_Optins.item.slideshow_photo_date_on = pic_size;
		Global_User_Optins.item.ifdirty = 1;

		last_pic_size = 0;
		last_pic_size_temp = 0;
	}
}

INT8U ap_state_config_pic_size_get(void)
{
	return Global_User_Optins.item.slideshow_photo_date_on;
}

void ap_state_config_quality_set(INT8U quality)
{
	if (quality != ap_state_config_quality_get()) {
		Global_User_Optins.item.slideshow_bg_music[0] = quality;
		Global_User_Optins.item.ifdirty = 1;
		
		last_pic_size = 0;
		last_pic_size_temp = 0;
	}
}

INT8U ap_state_config_quality_get(void)
{
	return Global_User_Optins.item.slideshow_bg_music[0];
}

void ap_state_config_scene_mode_set(INT8U scene_mode)
{
	if (scene_mode != ap_state_config_scene_mode_get()) {
		Global_User_Optins.item.slideshow_duration = scene_mode;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_scene_mode_get(void)
{
	return Global_User_Optins.item.slideshow_duration;
}

void ap_state_config_iso_set(INT8U iso)
{
	if (iso != ap_state_config_iso_get()) {
		Global_User_Optins.item.lcd_backlight = iso;
		Global_User_Optins.item.ifdirty = 1;
	}

}

INT8U ap_state_config_iso_get(void)
{
	return Global_User_Optins.item.lcd_backlight;
}

void ap_state_config_color_set(INT8U color)
{
	if (color != ap_state_config_color_get()) {
		Global_User_Optins.item.ui_style = color;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_color_get(void)
{
	return Global_User_Optins.item.ui_style;
}

void ap_state_config_saturation_set(INT8U saturation)
{
	if (saturation != ap_state_config_saturation_get()) {
		Global_User_Optins.item.calendar_duration = saturation;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_saturation_get(void)
{
	return Global_User_Optins.item.calendar_duration;
}

void ap_state_config_sharpness_set(INT8U sharpness)
{
	if (sharpness != ap_state_config_sharpness_get()) {
		Global_User_Optins.item.calendar_displaymode = sharpness;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_sharpness_get(void)
{
	return Global_User_Optins.item.calendar_displaymode;
}

void ap_state_config_preview_set(INT8U preview)
{
	if (preview != ap_state_config_preview_get()) {
		Global_User_Optins.item.alarm_mute1 = preview;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_preview_get(void)
{
	return Global_User_Optins.item.alarm_mute1;
}

void ap_state_config_burst_set(INT8U burst)
{
	if (burst != ap_state_config_burst_get()) {
		Global_User_Optins.item.music_on_off = burst;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_burst_get(void)
{
	return Global_User_Optins.item.music_on_off;
}


void ap_state_config_data_time_mode_set(INT8U dis_mode)
{
	if (dis_mode != ap_state_config_data_time_mode_get()) {
		Global_User_Optins.item.reserved2 = dis_mode;
		Global_User_Optins.item.ifdirty = 1;
	}
}
INT8U ap_state_config_data_time_mode_get(void)
{
	return	Global_User_Optins.item.reserved2;
}

#if ENABLE_CHECK_RTC == 1
extern INT32S ext_rtc_time_get(t_rtc *rtc_time);
extern INT64U ext_rtc_time_t_rtc_to_sec(t_rtc *rtc_time);

void ap_state_config_data_time_save_set(void)
{

	INT8U num_tmp0;
	t_rtc time;
	INT64U sec1;
	ext_rtc_time_get(&time);
	DBG_PRINT("t_rtc.RTC_day = %d, ",time.rtc_day);
	DBG_PRINT("t_rtc.RTC_hour = %d, ",time.rtc_hour);
	DBG_PRINT("t_rtc.RTC_min = %d, ",time.rtc_min);
	DBG_PRINT("t_rtc.RTC_sec = %d\r\n",time.rtc_sec);

	sec1 = ext_rtc_time_t_rtc_to_sec(&time);
	DBG_PRINT("sec1 = 0x%x\r\n",sec1);
	
	num_tmp0 = (INT8U)((time.rtc_day>>24)&0xff);
 	Global_User_Optins.item.powertime_onoff_struct2.on_hour = num_tmp0;

	num_tmp0 = (INT8U)((time.rtc_day>>16)&0xff);
	Global_User_Optins.item.powertime_onoff_struct1.mode = num_tmp0;

	num_tmp0 = (INT8U)((time.rtc_day>>8)&0xff);	
	Global_User_Optins.item.powertime_onoff_struct1.on_hour = num_tmp0;

	// DAY
	num_tmp0 = (INT8U)((time.rtc_day)&0xff);
	Global_User_Optins.item.powertime_onoff_struct1.on_minute = num_tmp0;
	
	// HOUR
	num_tmp0 = (INT8U)(time.rtc_hour);
	Global_User_Optins.item.powertime_onoff_struct1.off_hour = num_tmp0;

	// MINUTE
	num_tmp0 = (INT8U)(time.rtc_min);
	Global_User_Optins.item.powertime_onoff_struct1.off_minute = num_tmp0;

	// SECOND
	num_tmp0 = (INT8U)(time.rtc_sec);
	Global_User_Optins.item.powertime_onoff_struct2.mode = num_tmp0;


	Global_User_Optins.item.ifdirty = 1;

}

#define three_month_sec		7776000	
#define ten_day_sec			864000	
extern INT32S ext_rtc_time_add_now_and_set(t_rtc *rtc_time); // rtc_time -> added 
extern INT64U ext_rtc_time_get_value(void);

INT8U RTC_TEST_SHOW = 0;
INT8U ap_state_config_data_time_check(void)
{
	t_rtc time;
	INT64U sec;
	INT64U sec1;
	INT32S num0;
	sec = ext_rtc_time_get_value();

	num0 = 0;
	num0 = Global_User_Optins.item.powertime_onoff_struct2.on_hour;
 	time.rtc_day = (num0<<24)&0xff000000;

	num0 = 0;
	num0 = Global_User_Optins.item.powertime_onoff_struct1.mode;
	time.rtc_day = time.rtc_day|((num0<<16)&0x00ff0000);
	num0 = 0;
	num0 = Global_User_Optins.item.powertime_onoff_struct1.on_hour;
	time.rtc_day = time.rtc_day|((num0<<8)&0x0000ff00);

	// DAY
	num0 = 0;
	num0 = Global_User_Optins.item.powertime_onoff_struct1.on_minute;
	time.rtc_day = time.rtc_day|(num0&0x000000ff);
	// HOUR
	num0 = 0;
	num0 |= Global_User_Optins.item.powertime_onoff_struct1.off_hour;
	time.rtc_hour = num0;
	// MINUTE
	num0 = 0;
	num0 |= Global_User_Optins.item.powertime_onoff_struct1.off_minute;
	time.rtc_min = num0;
	// SECOND
	num0 = 0;
	num0 |= Global_User_Optins.item.powertime_onoff_struct2.mode;
	time.rtc_sec = num0;
	
	DBG_PRINT("t_rtc.RTC_day = %d, ",time.rtc_day);
	DBG_PRINT("t_rtc.RTC_hour = %d, ",time.rtc_hour);
	DBG_PRINT("t_rtc.RTC_min = %d, ",time.rtc_min);
	DBG_PRINT("t_rtc.RTC_sec = %d\r\n",time.rtc_sec);
	sec1 = ext_rtc_time_t_rtc_to_sec(&time);

	DBG_PRINT("sec = 0x%x\r\n",sec);
	DBG_PRINT("sec1 = 0x%x\r\n",sec1);
	if(sec<sec1){	//fail
		if(sec<three_month_sec){
			ext_rtc_time_add_now_and_set(&time);
			DBG_PRINT("calender_restore......___________________aaaaaaaaaaaaaaaa\r\n");
			if(sec1 - sec >5){
				DBG_PRINT("power_OFF reset RTC\r\n");
				RTC_TEST_SHOW = 1;
			}else{
				DBG_PRINT("power_ON reset RTC\r\n");
				RTC_TEST_SHOW = 2;
			}
		}else{
			ext_rtc_time_set(&time);
			DBG_PRINT("calender_restore......_________ err sec__________aaaaaaaaaaaaaaaa\r\n");
			RTC_TEST_SHOW = 3;
			DBG_PRINT("power_ON RTC ERR sec<  more three month\r\n");
		}
	 	return 1;
	}else if(sec>(sec1+three_month_sec)){
		ext_rtc_time_set(&time);
		DBG_PRINT("calender_restore......_________ err sec__________aaaaaaaaaaaaaaaa\r\n");
		RTC_TEST_SHOW = 3;
		DBG_PRINT("power_ON RTC ERR more three month\r\n");
	 	return 1;
	}else{			//OK
		RTC_TEST_SHOW = 0;
		return 0;
	}

}


INT8U ap_state_config_data_time_power_off_check(void)
{
	t_rtc time;
	INT64U sec;
	INT64U sec1;
	INT32S num0;
	sec = ext_rtc_time_get_value();

	num0 = 0;
	num0 = Global_User_Optins.item.powertime_onoff_struct2.on_hour;
 	time.rtc_day = (num0<<24)&0xff000000;

	num0 = 0;
	num0 = Global_User_Optins.item.powertime_onoff_struct1.mode;
	time.rtc_day = time.rtc_day|((num0<<16)&0x00ff0000);
	num0 = 0;
	num0 = Global_User_Optins.item.powertime_onoff_struct1.on_hour;
	time.rtc_day = time.rtc_day|((num0<<8)&0x0000ff00);

	// DAY
	num0 = 0;
	num0 = Global_User_Optins.item.powertime_onoff_struct1.on_minute;
	time.rtc_day = time.rtc_day|(num0&0x000000ff);
	// HOUR
	num0 = 0;
	num0 |= Global_User_Optins.item.powertime_onoff_struct1.off_hour;
	time.rtc_hour = num0;
	// MINUTE
	num0 = 0;
	num0 |= Global_User_Optins.item.powertime_onoff_struct1.off_minute;
	time.rtc_min = num0;
	// SECOND
	num0 = 0;
	num0 |= Global_User_Optins.item.powertime_onoff_struct2.mode;
	time.rtc_sec = num0;
	
	DBG_PRINT("t_rtc.RTC_day = %d, ",time.rtc_day);
	DBG_PRINT("t_rtc.RTC_hour = %d, ",time.rtc_hour);
	DBG_PRINT("t_rtc.RTC_min = %d, ",time.rtc_min);
	DBG_PRINT("t_rtc.RTC_sec = %d\r\n",time.rtc_sec);
	sec1 = ext_rtc_time_t_rtc_to_sec(&time);

	DBG_PRINT("sec = 0x%x\r\n",sec);
	DBG_PRINT("sec1 = 0x%x\r\n",sec1);
	if(sec>(sec1+three_month_sec)){
		ext_rtc_time_set(&time);
		DBG_PRINT("calender_restore......_________ err sec__________aaaaaaaaaaaaaaaa\r\n");
	 	return 1;
	}else{			//OK
		RTC_TEST_SHOW = 0;
		return 0;
	}
}

void ap_state_config_data_time_save_set_default(void)
{

	INT8U num_tmp0;
	
	num_tmp0 = 0;
 	Global_User_Optins.item.powertime_onoff_struct2.on_hour = num_tmp0;

	Global_User_Optins.item.powertime_onoff_struct1.mode = num_tmp0;

	Global_User_Optins.item.powertime_onoff_struct1.on_hour = num_tmp0;
	// DAY
	Global_User_Optins.item.powertime_onoff_struct1.on_minute = num_tmp0;
	
	// HOUR
	Global_User_Optins.item.powertime_onoff_struct1.off_hour = num_tmp0;

	// MINUTE
	Global_User_Optins.item.powertime_onoff_struct1.off_minute = num_tmp0;

	// SECOND
	Global_User_Optins.item.powertime_onoff_struct2.mode = num_tmp0;


	Global_User_Optins.item.ifdirty = 1;

}

#endif

void ap_state_config_auto_off_set(INT8U auto_off)
{
	if (auto_off != ap_state_config_auto_off_get()) {
//		Global_User_Optins.item.save_as_logo_flag = auto_off;
		Global_User_Optins.item.date_format_display = auto_off;	//wwj modify
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_auto_off_get(void)
{
//	return Global_User_Optins.item.save_as_logo_flag;
	return	Global_User_Optins.item.date_format_display;	//wwj modify
}


void ap_state_config_auto_off_TFT_BL_set(INT8U auto_off_TFT_BL)
{
	if (auto_off_TFT_BL != ap_state_config_auto_off_TFT_BL_get()) {
		Global_User_Optins.item.reserved0 = auto_off_TFT_BL;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_auto_off_TFT_BL_get(void)
{
	return	Global_User_Optins.item.reserved0;
}

void ap_state_config_delay_power_on_set(INT8U delay_time)
{
	if (delay_time != ap_state_config_delay_power_on_get()) {
		Global_User_Optins.item.powertime_active_id = delay_time;
		Global_User_Optins.item.ifdirty = 1;
	}
}
INT8U ap_state_config_delay_power_on_get(void)
{
	return	Global_User_Optins.item.powertime_active_id;
}

void ap_state_config_beep_sound_set(INT8U off)
{
	if (off != ap_state_config_beep_sound_get()) {
		Global_User_Optins.item.factory_time[0] = off;
		Global_User_Optins.item.ifdirty = 1;
	}
}
INT8U ap_state_config_beep_sound_get(void)
{
	return	Global_User_Optins.item.factory_time[0];
}

void ap_state_config_flash_LED_set(INT8U off)
{
	if (off != ap_state_config_flash_LED_get()) {
		Global_User_Optins.item.factory_time[1] = off;
		Global_User_Optins.item.ifdirty = 1;
	}
}
INT8U ap_state_config_flash_LED_get(void)
{
	return	Global_User_Optins.item.factory_time[1];
}

void ap_state_config_light_freq_set(INT8U light_freq)
{
	if (light_freq != ap_state_config_light_freq_get()) {
		Global_User_Optins.item.alarm_modual = light_freq;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_light_freq_get(void)
{
	return Global_User_Optins.item.alarm_modual;
}

void ap_state_config_tv_out_set(INT8U tv_out)
{
	if (tv_out != ap_state_config_tv_out_get()) {
		Global_User_Optins.item.powertime_happened = tv_out;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_tv_out_get(void)
{
	return Global_User_Optins.item.powertime_happened;
}

void ap_state_config_usb_mode_set(INT8U usb_mode)
{
	if (usb_mode != ap_state_config_usb_mode_get()) {
		Global_User_Optins.item.week_format_display = usb_mode;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_usb_mode_get(void)
{
	return Global_User_Optins.item.week_format_display;
}

void ap_state_config_language_set(INT8U language)
{
	//if (language != ap_state_config_language_get()) {
	if (language != (ap_state_config_language_get() - 1)) {	//wwj modify
		//Global_User_Optins.item.language = language;
		Global_User_Optins.item.language = language + 1;	//wwj modify
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_language_get(void)
{
	return Global_User_Optins.item.language;
}

void ap_state_config_volume_set(INT8U sound_volume)
{
	if (sound_volume != ap_state_config_volume_get()) {
		Global_User_Optins.item.sound_volume = sound_volume;
		Global_User_Optins.item.ifdirty = 1;	
	}
}

INT8U ap_state_config_volume_get(void)
{
	return Global_User_Optins.item.sound_volume;
}

void ap_state_config_motion_detect_set(INT8U motion_detect)	//Motion Detect Sensitivity
{
	if (motion_detect != ap_state_config_motion_detect_get()) {
		Global_User_Optins.item.music_play_mode = motion_detect;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_motion_detect_get(void)
{
	return Global_User_Optins.item.music_play_mode;
}

void ap_state_config_record_time_set(INT8U record_time)
{
	if (record_time != ap_state_config_record_time_get()) {
		Global_User_Optins.item.full_screen = record_time;
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT8U ap_state_config_record_time_get(void)
{
	return Global_User_Optins.item.full_screen;
}

void ap_state_config_date_stamp_set(INT8U date_stamp)
{
	if (date_stamp != ap_state_config_date_stamp_get()) {
		Global_User_Optins.item.thumbnail_mode = date_stamp;
		Global_User_Optins.item.ifdirty = 1;
	}
}
INT8U ap_state_config_date_stamp_get(void)
{
	return Global_User_Optins.item.thumbnail_mode;
}

void ap_state_config_capture_date_stamp_set(INT8U date_stamp)
{
	if (date_stamp != ap_state_config_capture_date_stamp_get()) {
		Global_User_Optins.item.reserved1 = date_stamp;
		Global_User_Optins.item.ifdirty = 1;
	}
}
INT8U ap_state_config_capture_date_stamp_get(void)
{
	return Global_User_Optins.item.reserved1;
}

void ap_state_musiic_play_onoff_set(INT8U on_off)
{
	if (on_off != ap_state_music_play_onoff_get()) {
		Global_User_Optins.item.slideshow_transition = on_off;
		Global_User_Optins.item.ifdirty = 1;	
	}
}

INT8U ap_state_music_play_onoff_get(void)
{
	return Global_User_Optins.item.slideshow_transition;
}

void ap_state_music_set(INT32U music_index)
{
	if (music_index != ap_state_music_get()) {
		Global_User_Optins.item.current_photo_index[0] = (INT8U)(music_index);
		Global_User_Optins.item.current_photo_index[1] = (INT8U)(music_index >> 8);
		Global_User_Optins.item.current_photo_index[2] = (INT8U)(music_index >> 16);
		Global_User_Optins.item.ifdirty = 1;
	}
}

INT32U ap_state_music_get(void)
{
	INT32U index;

	index = Global_User_Optins.item.current_photo_index[2];
	index <<= 8;
	index += Global_User_Optins.item.current_photo_index[1];
	index <<= 8;
	index += Global_User_Optins.item.current_photo_index[0];
	return index;
}

void ap_state_music_play_mode_set(INT8U music_play_mode)
{
	if (music_play_mode != ap_state_music_play_mode_get()) {
		Global_User_Optins.item.sleep_time = music_play_mode;
		Global_User_Optins.item.ifdirty = 1;
	}

}

INT8U ap_state_music_play_mode_get(void)
{
	return Global_User_Optins.item.sleep_time;
}

void ap_state_config_factory_date_get(INT8U *date)
{
	*date++ = Global_User_Optins.item.factory_date[0];
	*date++ = Global_User_Optins.item.factory_date[1];
	*date = Global_User_Optins.item.factory_date[2];
}

void ap_state_config_factory_time_get(INT8U *time)
{
	*time++ = Global_User_Optins.item.factory_time[0];
	*time++ = Global_User_Optins.item.factory_time[1];
	*time = Global_User_Optins.item.factory_time[2];
}

void ap_state_config_base_day_set(INT32U day)
{
	if (day != ap_state_config_base_day_get()) {
		Global_User_Optins.item.base_day = day;
		Global_User_Optins.item.ifdirty = 1;
	}

}

INT32U ap_state_config_base_day_get(void)
{
	return Global_User_Optins.item.base_day;
}


void CRC32tbl_init(void)
{
    INT32U i,j;
    INT32U c;

    for (i=0 ; i<256 ; ++i) {
        c = i ;
        for (j=0 ; j<8 ; j++) { 
            c = (c & 1) ? (c >> 1) ^ CRC32_POLY : (c >> 1);
        }
        ///CRC32_tbl[i] = c;
    }
}

void CRC_cal(INT8U *pucBuf, INT32U len, INT8U *CRC)
{
    INT32U i_CRC, value;
    //INT32U i;

    /*if (!CRC32_tbl[1]) {
        CRC32tbl_init();
    }    
    i_CRC = 0xFFFFFFFF;
    for (i=0 ; i<len ; i++) {
        i_CRC = (i_CRC >> 8) ^ CRC32_tbl[(i_CRC ^ pucBuf[i]) & 0xFF];
    }*/
    value = ~i_CRC;
    CRC[0] = (value & 0x000000FF) >> 0;
    CRC[1] = (value & 0x0000FF00) >> 8;
    CRC[2] = (value & 0x00FF0000) >> 16;
    CRC[3] = (value & 0xFF000000) >> 24;
}
