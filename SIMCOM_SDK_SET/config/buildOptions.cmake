#########################
#       general config
#########################
if (EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_common.h)
    option(HAS_URC "build urc task" ON)
endif()

if (EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_usb_vcom.h)
    option(HAS_USB "build usb task" ON)
endif()

if(CONFIG_SIMCOM_UI_DEMO_TO_UART1_PORT)
    option(SIMCOM_UI_DEMO_TO_UART1_PORT "SIMCOM_UI_DEMO_TO_UART1_PORT" ON)
endif()

if(CONFIG_SIMCOM_UI_DEMO_TO_USB_AT_PORT)
    option(SIMCOM_UI_DEMO_TO_USB_AT_PORT "SIMCOM_UI_DEMO_TO_USB_AT_PORT" ON)
endif()

#########################
# driver
#########################
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_wtd.h)
    AND CONFIG_HAS_DEMO_WTD)
    option(HAS_DEMO_WTD "build WTD demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/sc_power.h)
    AND CONFIG_HAS_DEMO_PMU)
    option(HAS_DEMO_PMU "build PMU demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_gpio.h)
    AND CONFIG_HAS_DEMO_GPIO)
    option(HAS_DEMO_GPIO "build GPIO demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_pwm.h)
    AND CONFIG_HAS_DEMO_PWM)
    option(HAS_DEMO_PWM "build PWM demo" ON)
endif()
if (EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_uart.h)
    option(HAS_UART "build UART task" ON)

    if (CONFIG_HAS_DEMO_UART)
        option(HAS_DEMO_UART "build UART demo" ON)
    endif()
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_usb_vcom.h)
    AND CONFIG_HAS_DEMO_USB)
    option(HAS_DEMO_USB "build USB demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/sc_i2c.h)
    AND CONFIG_HAS_DEMO_I2C)
    option(HAS_DEMO_I2C "build I2C demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/sc_spi.h)
    AND CONFIG_HAS_DEMO_SPI)
    option(HAS_DEMO_SPI "build SPI demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/sc_flash.h)
    AND CONFIG_HAS_DEMO_FLASH)
    option(HAS_DEMO_FLASH "build FLASH demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_fota_download.h)
    AND CONFIG_HAS_DEMO_OTA)
    option(HAS_DEMO_OTA "build OTA demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_gps.h)
    AND CONFIG_HAS_DEMO_GNSS)
    option(HAS_DEMO_GNSS "build GNSS demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_lcd.h)
    AND CONFIG_HAS_DEMO_LCD)
    option(HAS_DEMO_LCD "build LCD demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_cam.h)
    AND CONFIG_HAS_DEMO_CAM)
    option(HAS_DEMO_CAM "build CAM demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_os.h)
    AND CONFIG_HAS_DEMO_SYS)
    option(HAS_DEMO_SYS "build sys demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_exfsmount.h)
    AND CONFIG_HAS_DEMO_EXFLASHMOUNTFS)
    option(HAS_DEMO_EXFLASHMOUNTFS "build exflashmountfs demo" ON)
endif()

if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_onewire.h)
    AND CONFIG_HAS_DEMO_ONEWIRE)
    option(HAS_DEMO_ONEWIRE "build one_wire demo" ON)
endif()

#########################
# modem
#########################
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_network.h)
    AND CONFIG_HAS_DEMO_NETWORK)
    option(HAS_DEMO_NETWORK "build NETWORK demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_simcard.h)
    AND CONFIG_HAS_DEMO_SIMCARD)
    option(HAS_DEMO_SIMCARD "build SIMCARD demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_call.h)
    AND CONFIG_HAS_DEMO_CALL)
    option(HAS_DEMO_CALL "build CALL demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_sms.h)
    AND CONFIG_HAS_DEMO_SMS)
    option(HAS_DEMO_SMS "build SMS demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_loc.h)
    AND CONFIG_HAS_DEMO_LBS)
    option(HAS_DEMO_LBS "build LBS demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_sjdr.h)
    AND CONFIG_HAS_DEMO_SJDR)
    option(HAS_DEMO_SJDR "build SJDR demo" ON)
endif()

if (CONFIG_HAS_DEMO_INTERNET_SERVICE)
    option(HAS_DEMO_INTERNET_SERVICE "build INTERNET_SERVICE demo" ON)
endif()

#########################
# app
#########################
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_ssl.h)
    AND CONFIG_HAS_DEMO_SSL)
    option(HAS_DEMO_SSL "build SSL demo" ON)
endif()
if (EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/scfw_socket.h)
    option(HAS_TCPIP "build TCPIP demo" ON)
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_tcpip.h)
    AND CONFIG_HAS_DEMO_TCPIP)
    option(HAS_DEMO_TCPIP "build TCPIP demo" ON)
endif()
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_https.h)
    AND CONFIG_HAS_DEMO_HTTPS)
    option(HAS_DEMO_HTTPS "build HTTPS demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_ftps.h)
    AND CONFIG_HAS_DEMO_FTPS)
    option(HAS_DEMO_FTPS "build FTPS demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_mqtts_client.h)
    AND CONFIG_HAS_DEMO_MQTTS)
    option(HAS_DEMO_MQTTS "build MQTTS demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_ntp_client.h)
    AND CONFIG_HAS_DEMO_NTP)
    option(HAS_DEMO_NTP "build NTP demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_htp_client.h)
    AND CONFIG_HAS_DEMO_HTP)
    option(HAS_DEMO_HTP "build HTP demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_audio.h)
    AND CONFIG_HAS_DEMO_AUDIO)
    option(HAS_DEMO_AUDIO "build AUDIO demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_tts.h)
    AND CONFIG_HAS_DEMO_TTS)
    option(HAS_DEMO_TTS "build TTS demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_poc.h)
    AND CONFIG_HAS_DEMO_POC)
    option(HAS_DEMO_POC "build POC demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_wifi.h)
    AND CONFIG_HAS_DEMO_WIFI)
    option(HAS_DEMO_WIFI "build WIFI demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_rtc.h)
    AND CONFIG_HAS_DEMO_RTC)
    option(HAS_DEMO_RTC "build RTC demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_ble.h)
    AND CONFIG_HAS_DEMO_BLE)
    option(HAS_DEMO_BLE "build BLE demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_bt.h)
    AND CONFIG_HAS_DEMO_BT)
    option(HAS_DEMO_BT "build BT demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_bt_stack.h)
    AND CONFIG_HAS_DEMO_BT_STACK)
    option(HAS_DEMO_BT_STACK "build BT_STACK demo" ON)
endif()
if (EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_file.h)
    option(HAS_FS "build FILE_SYSTEM demo" ON)

    if (CONFIG_HAS_DEMO_FS)
        option(HAS_DEMO_FS "build FILE_SYSTEM demo" ON)
        option(HAS_DEMO_FS_OLD "build FILE_SYSTEM_OLD demo" ON)
    endif()
endif()
if (EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_auth_api.h)
option(HAS_DEMO_FS_AUTH ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/sc_app_download.h)
    AND CONFIG_HAS_DEMO_APP_DOWNLOAD)
    option(HAS_DEMO_APP_DOWNLOAD "build APP_DOWNLOAD demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/sc_app_update.h)
    AND CONFIG_HAS_DEMO_APP_UPDATE)
    option(HAS_DEMO_APP_UPDATE "build APP_UPDATE demo" ON)
endif()
if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/simcom_ping.h)
    AND CONFIG_HAS_DEMO_PING)
    option(HAS_DEMO_PING "build PING demo" ON)
endif()

# ########################
# other libs
# ########################
if(HAS_FS AND (CONFIG_HAS_ZLIB OR DEFINED WALNUT OR DEFINED AKA OR REPLICANT) AND NOT DEFINED TBDTU)
    option(HAS_ZLIB "ZLIB support" ON)
endif()

if (CONFIG_HAS_SM2)
option(HAS_SM2 "SM2 support" ON)  # is based on miracl
endif()

if (HAS_SM2 OR CONFIG_HAS_MIRACL)
    option(HAS_MIRACL "MIRACL LIB" ON)
endif()

if(DEFINED TBDTU OR CONFIG_HAS_CJSON)
option(HAS_CJSON "CJSON support" ON)
endif()

if(DEFINED WALNUT)
option(HAS_MBEDTLS "build mbedtls demo" ON)
endif(DEFINED WALNUT)

if ((EXISTS ${CMAKE_SOURCE_DIR}/sc_lib/${SCMODULE}/inc/pl_crypto.h))
    option(HAS_CRYPTO "build crypto demo" ON)
endif()

#########################
# custom switch
#########################
if(DEFINED YSZNBG)
    option(PROJECT_YSZNBG "custom project yingshi zhinengbinggui" ON)
elseif(DEFINED TBDTU)
    option(PROJECT_TBDTU "custom project topband DTU" ON)
elseif(DEFINED LYTBOX)
    option(PROJECT_LYTBOX "custom project lianyou TBox" ON)
elseif(DEFINED TOPFLY)
    option(PROJECT_TOPFLY "custom project dingfei API" ON)

    if(CONFIG_HAS_DEMO AND ((CONFIG_SIMCOM_PROTOCOL_V1 AND EXISTS ${CMAKE_SOURCE_DIR}/sc_demo/V1) OR (CONFIG_SIMCOM_PROTOCOL_V2 AND EXISTS ${CMAKE_SOURCE_DIR}/sc_demo/V2)))
        option(HAS_DEMO "build simcom demo" ON)
    endif()
else()
    if(CONFIG_HAS_DEMO AND((CONFIG_SIMCOM_PROTOCOL_V1 AND EXISTS ${CMAKE_SOURCE_DIR}/sc_demo/V1) OR(CONFIG_SIMCOM_PROTOCOL_V2 AND EXISTS ${CMAKE_SOURCE_DIR}/sc_demo/V2)))
        option(HAS_DEMO "build simcom demo" ON)
    endif()
endif()


if((DEFINED YSZNBG) AND (DEFINED BSKL))
    option(PROJECT_YSZNBG_BSKL "custom project yingshi bashikele zhinengbinggui" ON)
endif()



if(CONFIG_SIMCOM_DEBUG)
    option(SIMCOM_DEBUG "build debug version" ON)
endif()

if(CONFIG_SIMCOM_PROTOCOL_V1)
    option(SIMCOM_PROTOCOL_V1 "simcom protocol v1" ON)
    option(SIMCOM_PROTOCOL_V2 "simcom protocol v2" OFF)
elseif(CONFIG_SIMCOM_PROTOCOL_V2)
    option(SIMCOM_PROTOCOL_V1 "simcom protocol v1" OFF)
    option(SIMCOM_PROTOCOL_V2 "simcom protocol v2" ON)
endif()
