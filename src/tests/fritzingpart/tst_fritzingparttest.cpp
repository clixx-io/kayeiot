#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QObject>
#include <QFileInfo>

#include "clixxiotprojects.h"
#include "fritzinglibrary.h"

class FritzingpartTest : public QObject
{
    Q_OBJECT

public:
    FritzingpartTest();

private Q_SLOTS:
    void testCase1_data();
    void testCase1();
    void testCaseConstructor();
    void testCaseReadHeaderFields();
    void testCaseReadConnectors();
    void testCaseReadBoardSize();
    void testCaseWriteBoardfile();
    void testCaseWriteBoardPins();
    void testCaseReadBoardSizeIn();
    void testCaseConvertfpz1();
    void testCaseWriteWeMosBoard();

private:
    QString datadir;
};

/*
 * Files from this directory that we want

--- ~/kayeiot/fritzing-parts/svg/core/breadboard
--- ~/kayeiot/fritzing-parts/svg/core/icon

*/

FritzingpartTest::FritzingpartTest()
{
    ClixxIoTProjects environment;

    datadir = QObject::tr("%1/fritzing-vendor-libraries/fritzing/fritzing-parts").arg(environment.getPartsLibraryDir());

    QList<QString> filelist = {
        "1X05.svg",
        "1X02_LOCK.svg",
        "3_Axis_Accelerometer_Breakout___MMA8452Q_Icon.svg",
        "7segment.svg",
        "7-segment display.svg",
        "16-segment display.svg",
        "Adafruit_Atmega32u4_Breakout.svg",
        "ADP3300.svg",
        "ADTL082.svg",
        "ADXL327.svg",
        "antenna.svg",
        "Arduino_ADK_MEGA_2560-Rev3_icon.svg",
        "Arduino_DUE_V02b_breadboard.svg",
        "Arduino_Esplora_icon.svg",
        "Arduino_Ethernet_breadboard.svg",
        "Arduino_Motor_Shield_rev3_breadboard.svg",
        "Arduino_Pro_breadboard.svg",
        "AUDIO-JACK.svg",
        "Barometric_Pressure_Sensor__2eaec138e035.svg",
        "basic-diode.svg",
        "basic_ldr.svg",
        "basic_resistor.svg",
        "basic_servo.svg",
        "Battery_block_9V82.svg",
        "batterypack_2xAA.svg",
        "Blend_Micro_1.0_icon.svg",
        "bmp180_icon.svg",
        "BUTTONPAD-4X4.svg",
        "BUTTONPAD-2X2-SPECIAL.svg",
        "CAMERA_SFV24R-2ST_2.svg",
        "chipkit_BasicIOShield_icon.svg",
        "chipkit_MotorShield_icon.svg",
        "chipkit_MX4_icon.svg",
        "chipkit_WF32_icon.svg",
        "Color_Sensor_Breakout_ADJD_S311_icon.svg",
        "crystal.svg",
        "d2523t.svg",
        "DB9_MALE.svg",
        "DB25F.svg",
        "DB25M.svg",
        "DS18B20_icon.svg",
        "dc_motor.svg",
        "DENSO 174917 ECU socket 48pin_1.svg",
        "DRV8825_breakout_icon.svg",
        "Dual_VNH2SP30_Motor_Driver_icon.svg",
        "EDGY_Analog_In_icon.svg",
        "electrolytic_capacitor_small_icon.svg",
        "electron_board_icon.svg",
        "ESP8266_module_icon.svg",
        "ESP8266_7_icon.svg",
        "EspruinoPico_icon.svg",
        "generic_shrouded_header.svg",
        "hc-sr04_ultrafix_icon.svg",
        "HC-05-male_icon.svg",
        "Humidity_and_Temperature_Sensor_RHT03_6.svg",
        "Humidity_Sensor_SHT15_icon.svg",
        "inductor.svg",
        "infrared proximity sensor.svg",
        "JOYSTICK-PSP1000.svg",
        "LCD_CFAX.svg",
        "lcd-GDM1602K.svg",
        "LED_Matrix_Kingbright.svg",
        "LED-red-5mmicon.svg",
        "LED_RING.svg",
        "LITTLE_BITS_MALE.svg",
        "LITTLE_BITS_FEMALE.svg",
        "loudspeaker.svg",
        "MIC-9.7MM.svg",
        "microphone.svg",
        "microsdmodule_icon.svg",
        "microSD-socket.svg",
        "mosfet_p_channel_icon.svg",
        "mosfet_n_channel_icon.svg",
        "MPL3115A2_icon.svg",
        "NOKIA6100_LCD.svg",
        "NRF24L01+_breakout_icon.svg",
        "NRF24L01_fix_icon.svg",
        "OBDII.svg",
        "OS4000-T.svg",
        "peltier_element.svg",
        "PHOTON_824ff3ebabbafb3949f059a17a7025a9_2_icon.svg",
        "PocketBeagle_icon.svg",
        "pololu0002_a988_icon.svg",
        "potentiometer.svg",
        "POT_PTH_SWITCH.svg",
        "pot-slider.svg",
        "POWER_JACK_PTH.svg",
        "PowerBoost500C-Adafruit_icon.svg",
        "powerpushbutton_adafruit_icon.svg",
        "prefix0000_SFE-10467-v10_2.svg",
        "Pushbuttonc.svg",
        "raspberrypi_zero_1_icon.svg",
        "RCA.svg",
        "RCA_LOCK.svg",
        "Real_Time_Clock_Module___DS1307_RTC_Brea.svg",
        "red_led_matrix8x8.svg",
        "reed_switch.svg",
        "resistor_icon_220.svg",
        "RFduinoSMD_icon.svg",
        "rfid_reader_id12.svg",
        "RFM12b_HopeRF_Transceiver__9c603c735b502.svg",
        "RFM22-XXX-S2.svg",
        "RFM23BP_28f0aeb1c8ab1c048f7c7ebd3fa05abe_1_icon.svg",
        "RioRand_LM2596_94ce9810d0922bc731e2f315085d46ab_1_icon.svg",
        "RJ45_TRXCOM2_icon.svg",
        "RMCM01.svg",
        "RN171.svg",
        "Rotary_Encoder.svg",
        "Rotary_Switch_12x1_5.svg",
        "Round_Pushbuttonba.svg",
        "rtc_ds3231_breakout_icon.svg",
        "SCREENKEY-TFT128.svg",
        "screw_terminal.svg",
        "SCREWTERMINAL-3.5MM-2-NS.svg",
        "SCREWTERMINAL-3.5MM-4.svg",
        "SCW05B-05_4.svg",
        "seeed_arch_pro_v1_icon.svg",
        "seeed_arch_v1.1_icon.svg",
        "seeed_grove_3axis_digital_accelerometer_16g_icon.svg",
        "seeed_grove_4digit_display_icon.svg",
        "seeed_grove_button_icon.svg",
        "seeed_grove_buzz_icon.svg",
        "seeed_grove_infrared_emitter_icon.svg",
        "seeed_grove_infrared_receiver_icon.svg",
        "seeed_grove_light_sensor_icon.svg",
        "seeed_grove_oled_128x96_icon.svg",
        "seeed_grove_temperature_sensor_icon.svg",
        "seeed_seeeduino_v3_icon.svg",
        "sfv24r-2st_2.svg",
        "SIM800L_7.svg",
        "SMARTCARD.svg",
        "SN74LVC2G14DBVR_7.svg",
        "SOLARBIT-12X1.svg",
        "solenoid.svg",
        "Sparkfun_IMU_Breakout_MPU-9250_icon.svg",
        "sparkfun_storage_microsd_icon.svg",
        "Sparkfun_Tilt-a-Whirl_icon.svg",
        "SSM2212.svg",
        "SSM2220.svg",
        "starter-poti-small_icon.svg",
        "stereo-jack-3_5mm.svg",
        "STM32F103_icon.svg",
        "Stretch sensor.svg",
        "SUP500F.svg",
        "Stepper_Motor_-_Bipolar__icon.svg",
        "Stepper_Motor_-_Unipolar__icon.svg",
        "STEREOJACK2.5MM_SPECIAL_HOLES-ONLY.svg",
        "TACTILE-PTH-12MM.svg",
        "TACTILE_SWITCH_TALL.svg",
        "Taiway-100-SP-X-T1_SPDT_miniature_toggle_switch_icon.svg",
        "teensy_3.1_icon.svg",
        "Teensy_3.0_icon.svg",
        "USB-A-S.svg",
        "USB-B-PTH-VERTICAL.svg",
        "USB-B-SMT.svg",
        "USB-MINIB.svg",
        "USB-MINIB-NOSTOP.svg",
        "WeMos-D1-mini-male-headers-above_icon.svg",
        "WIZ811MJ.svg",
        "WLANMODULE.svg",
        "WIFLY_GSX.svg",
        "WeIO_icon.svg",
    };
}

void FritzingpartTest::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void FritzingpartTest::testCaseConstructor()
{
    FritzingLibrary *fzpl = new FritzingLibrary(datadir);

    QVERIFY(fzpl != 0);

    delete fzpl;
}

void FritzingpartTest::testCaseReadHeaderFields()
{

    FritzingLibrary *fzpl = new FritzingLibrary(datadir);

    QVERIFY(fzpl != 0);

    QMap <QString, QVariant> p = fzpl->readPartFile("25LC320.fzp");

    QVERIFY(p.size() != 0);
    QVERIFY(p["title"] == "25LC320");
    QVERIFY(p["label"] == "IC");
    QVERIFY(p["date"] == "2010-05-26");
    QVERIFY(p["tags"] == "EEPROM, SPI, DIP");
    QVERIFY(p["description"] == "25LC320 SPI EEPROM");
    QVERIFY(p["fullimagepath"].toString().endsWith("/25LC320_icon.svg"));

    qDebug() << "p[fullimagepath]=" << p["fullimagepath"];

    delete fzpl;
}

void FritzingpartTest::testCaseReadConnectors()
{

    FritzingLibrary *fzpl = new FritzingLibrary(datadir);

    QVERIFY(fzpl != 0);

    QMap <QString, QVariant> p = fzpl->readPartFile("Real_Time_Clock_Module___DS1307_RTC_Breakout_Board.fzp");

    QVERIFY(p.size() != 0);
    QVERIFY(p["title"] == "Real Time Clock Module - DS1307 RTC Breakout Board");
    QVERIFY(p["date"] == "2011-09-07");
    QVERIFY(p["tags"] == "Real, Time, Clock, Alarm");

    QVERIFY(p["connector-name-connector5"] == "5V");
    QVERIFY(p["connector-type-connector5"] == "male");

    QVERIFY(p["connector-name-connector6"] == "GND");
    QVERIFY(p["connector-type-connector6"] == "male");

    QVERIFY(p["connector-name-connector7"] == "SQW");
    QVERIFY(p["connector-type-connector7"] == "male");

    QVERIFY(p["connector-name-connector8"] == "SCL");
    QVERIFY(p["connector-type-connector8"] == "male");

    QVERIFY(p["connector-name-connector9"] == "SDA");
    QVERIFY(p["connector-type-connector9"] == "male");

    delete fzpl;
}

void FritzingpartTest::testCaseReadBoardSize()
{

    FritzingLibrary *fzpl = new FritzingLibrary(datadir);

    QVERIFY(fzpl != 0);

    QMap <QString, QVariant> p = fzpl->readImageFile("Real_Time_Clock_Module___DS1307_RTC_Brea.svg");

    QVERIFY(p.size() != 0);
    QVERIFY(p["board-width"] == 70.875);
    QVERIFY(p["board-height"] == 70.875);

    delete fzpl;
}

void FritzingpartTest::testCaseReadBoardSizeIn()
{

    FritzingLibrary *fzpl = new FritzingLibrary(datadir);

    QVERIFY(fzpl != 0);

    QMap <QString, QVariant> p = fzpl->readImageFile("Teensy_3.0_icon.svg");

    QVERIFY(p.size() != 0);
    QVERIFY(p["board-width"] == 0.701486);
    QVERIFY(p["board-height"] == 1.43756);
    QVERIFY(p["board-units"] == "In");

    delete fzpl;
}

void FritzingpartTest::testCaseWriteBoardfile()
{
    FritzingLibrary *fzpl = new FritzingLibrary(datadir);

    QVERIFY(fzpl != 0);

    QString basename("Real_Time_Clock_Module___DS1307_RTC_Breakout_Board"),
            boardfilename;

    boardfilename = QObject::tr("%1.board").arg(basename);

    QMap <QString, QVariant> datapairs = fzpl->readPartFile(QObject::tr("%1.fzp").arg(basename));

    fzpl->writeBoardFile(boardfilename,datapairs);

    QFileInfo check_file(boardfilename);
    QVERIFY(check_file.exists() && check_file.isFile());

    QFile file(boardfilename);
    file.remove();

    delete fzpl;
}

void FritzingpartTest::testCaseConvertfpz1()
{
    FritzingLibrary *fzpl = new FritzingLibrary(datadir);

    QVERIFY(fzpl != 0);

    QString basename("Real_Time_Clock_Module___DS1307_RTC_Breakout_Board"),
            boardfilename,
            outputfile;

    boardfilename = QObject::tr("%1.board").arg(basename);

    fzpl->convertfpzToBoard(QObject::tr("%1.fzp").arg(basename), outputfile);

    QFileInfo check_file(outputfile);
    QVERIFY(check_file.exists() && check_file.isFile());

    // QFile file(boardfilename);
    // file.remove();

    delete fzpl;
}

void FritzingpartTest::testCaseWriteBoardPins()
{

    FritzingLibrary *fzpl = new FritzingLibrary(datadir);

    QVERIFY(fzpl != 0);

    QMap <QString, QVariant> p = fzpl->readPartFile("Teensy 3.0.fzp");

    QVERIFY(p.size() != 0);
    QVERIFY(p["board-width"] == 0.701486);
    QVERIFY(p["board-height"] == 1.43756);
    QVERIFY(p["board-units"] == "In");

    fzpl->writeBoardFile("Teensy 3.0.board",p);

    delete fzpl;
}

void FritzingpartTest::testCaseWriteWeMosBoard()
{

    FritzingLibrary *fzpl = new FritzingLibrary(datadir);

    QVERIFY(fzpl != 0);

    QMap <QString, QVariant> p = fzpl->readPartFile("WeMos-D1-mini-WS2812B-RGB-Shield-no-headers.fzp");

    qDebug() << "image" << p["image"].toString();
    qDebug() << "image-file" << p["image-file"].toString();
    qDebug() << "board-width" << p["board-width"].toFloat();
    qDebug() << "board-height" << p["board-height"].toFloat();

    QVERIFY(p.size() != 0);
    QVERIFY(p["board-width"].toFloat() != 0.0);
    QVERIFY(p["board-height"].toFloat() != 0.0);
    QVERIFY(p["board-units"] == "cm");

    fzpl->writeBoardFile("WeMos-D1-mini-WS2812B-RGB-Shield-no-headers.board",p);

    delete fzpl;
}

void FritzingpartTest::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");

    FritzingLibrary *fzpl = new FritzingLibrary("hello");

    delete fzpl;

}

QTEST_MAIN(FritzingpartTest)

#include "tst_fritzingparttest.moc"
