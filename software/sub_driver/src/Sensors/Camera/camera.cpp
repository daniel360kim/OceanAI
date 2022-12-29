#include "camera.h"
#include "../../core/debug.h"

    /**
     * @brief Construct a new  OV2640_Mini::OV2640_Mini object
     *
     * @param cs_pin Chip select pin
     * @param resolution image resolution
     * @param use_eeprom get settings from eeprom
     * @param frame_number how many images to buffer before sending to SD
     * @param saturation saturation level
     * @param brightness brightness level
     * @param contrast contrast level
     * @param color color level
     * @param white_balance white balance level
     * @param format BMP, JPEG, or RAW
     */
    OV2640_Mini::OV2640_Mini(const uint8_t cs_pin, Resolution resolution, Frame_Number frame_number, bool use_eeprom,
                             Saturation saturation,
                             Brightness brightness,
                             Contrast contrast,
                             Color color,
                             White_Balance white_balance,
                             Format format)
    {
        m_cs_pin = cs_pin;

        if (!use_eeprom)
        {
            m_resolution = resolution;
            m_frame_number = frame_number;
            m_saturation = saturation;
            m_brightness = brightness;
            m_contrast = contrast;
            m_color = color;
            m_white_balance = white_balance;
            m_format = format;
        }
        else
        {
            retrieveSettings();
        }

        m_camera = new ArduCAM(OV2640, m_cs_pin); // Create ArduCAM object
    }

    void OV2640_Mini::setResolution(Resolution resolution)
    {
        m_resolution = resolution;

        m_camera->OV2640_set_JPEG_size(static_cast<uint8_t>(m_resolution));
    }

    void OV2640_Mini::setFrameNumber(Frame_Number frame_number)
    {
        m_frame_number = frame_number;
        m_camera->write_reg(ARDUCHIP_FRAMES, static_cast<uint8_t>(m_frame_number));
    }

    void OV2640_Mini::setSaturation(Saturation saturation)
    {
        m_saturation = saturation;

        m_camera->OV2640_set_Color_Saturation(static_cast<uint8_t>(m_saturation));
    }

    void OV2640_Mini::setBrightness(Brightness brightness)
    {
        m_brightness = brightness;

        m_camera->OV2640_set_Brightness(static_cast<uint8_t>(m_brightness));
    }

    void OV2640_Mini::setContrast(Contrast contrast)
    {
        m_contrast = contrast;

        m_camera->OV2640_set_Contrast(static_cast<uint8_t>(m_contrast));
    }

    void OV2640_Mini::setColor(Color color)
    {
        m_color = color;
        if (m_color == Color::NONE)
            return;
        else
        {
            m_camera->OV2640_set_Color_Saturation(static_cast<uint8_t>(m_color));
        }
    }

    void OV2640_Mini::setWhiteBalance(White_Balance white_balance)
    {
        m_white_balance = white_balance;

        m_camera->OV2640_set_Special_effects(static_cast<uint8_t>(m_white_balance));
    }

    void OV2640_Mini::setFormat(Format format)
    {
        m_format = format;

        m_camera->set_format(static_cast<uint8_t>(m_format));
    }

    /**
     * @brief Saves current settings to EEPROM on TEENSY for next boot
     *
     */
    void OV2640_Mini::saveSettings()
    {
        EEPROM.update(0, static_cast<uint8_t>(m_resolution));
        EEPROM.update(1, static_cast<uint8_t>(m_frame_number));
        EEPROM.update(2, static_cast<uint8_t>(m_saturation));
        EEPROM.update(3, static_cast<uint8_t>(m_brightness));
        EEPROM.update(4, static_cast<uint8_t>(m_contrast));
        EEPROM.update(5, static_cast<uint8_t>(m_color));
        EEPROM.update(6, static_cast<uint8_t>(m_white_balance));
        EEPROM.update(7, static_cast<uint8_t>(m_format));

        //SUCCESS_LOG("Settings saved to EEPROM");
    }

    void OV2640_Mini::saveSettings(Resolution resolution, Frame_Number frame_number,
                                   Saturation saturation, Brightness brightness, Contrast contrast, Color color, White_Balance white_balance, Format format)
    {
        m_resolution = resolution;
        m_frame_number = frame_number;
        m_saturation = saturation;
        m_brightness = brightness;
        m_contrast = contrast;
        m_color = color;
        m_white_balance = white_balance;
        m_format = format;
        saveSettings();
    }

    /**
     * @brief Retrieves settings from EEPROM on TEENSY
     *
     */
    void OV2640_Mini::retrieveSettings()
    {
        m_resolution = static_cast<Resolution>(EEPROM.read(0));
        m_frame_number = static_cast<Frame_Number>(EEPROM.read(1));
        m_saturation = static_cast<Saturation>(EEPROM.read(2));
        m_brightness = static_cast<Brightness>(EEPROM.read(3));
        m_contrast = static_cast<Contrast>(EEPROM.read(4));
        m_color = static_cast<Color>(EEPROM.read(5));
        m_white_balance = static_cast<White_Balance>(EEPROM.read(6));
        m_format = static_cast<Format>(EEPROM.read(7));

        //SUCCESS_LOG("Retrieved settings from EEPROM");
    }

    void OV2640_Mini::retrieveSettings(Resolution &resolution, Frame_Number &frame_number,
                                       Saturation &saturation, Brightness &brightness, Contrast &contrast, Color &color, White_Balance &white_balance, Format &format)
    {
        resolution = static_cast<Resolution>(EEPROM.read(0));
        frame_number = static_cast<Frame_Number>(EEPROM.read(1));
        saturation = static_cast<Saturation>(EEPROM.read(2));
        brightness = static_cast<Brightness>(EEPROM.read(3));
        contrast = static_cast<Contrast>(EEPROM.read(4));
        color = static_cast<Color>(EEPROM.read(5));
        white_balance = static_cast<White_Balance>(EEPROM.read(6));
        format = static_cast<Format>(EEPROM.read(7));

        //SUCCESS_LOG("Settings retrieved from EEPROM");

        m_resolution = resolution;
        m_frame_number = frame_number;
        m_saturation = saturation;
        m_brightness = brightness;
        m_contrast = contrast;
        m_color = color;
        m_white_balance = white_balance;
        m_format = format;
    }

    bool OV2640_Mini::initialize()
    {
        Wire.begin();

        pinMode(m_cs_pin, OUTPUT);
        digitalWrite(m_cs_pin, HIGH);

        SPI.begin();

        m_camera->write_reg(0x07, 0x80); // Reset the CPLD
        delay(100);
        m_camera->write_reg(0x07, 0x00); // Release CPLD reset
        delay(100);

        uint8_t temp = 0;

        while (1)
        {
            m_camera->write_reg(ARDUCHIP_TEST1, 0x55);
            temp = m_camera->read_reg(ARDUCHIP_TEST1);
            if (temp != 0x55)
            {
                //ERROR_LOG(Debug::Fatal, "OV2640 initialization failed");
                return false;
            }
            else
            {
                //SUCCESS_LOG("OV2640 initialization successful");
                break;
            }
        }

        uint8_t vid = 0, pid = 0;
        while (1)
        {
            // Check if the camera module type is OV2640
            m_camera->wrSensorReg8_8(0xff, 0x01);
            m_camera->rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
            m_camera->rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
            if ((vid != 0x26) && ((pid != 0x41) || (pid != 0x42)))
            {
                //ERROR_LOG(Debug::Fatal, "Can't find OV2640 module!");
                delay(1000);
                continue;
            }
            else
            {
                //SUCCESS_LOG("OV2640 module detected");
                break;
            }
        }

        m_camera->set_format(static_cast<uint8_t>(m_format));
        m_camera->InitCAM();

        //m_camera->OV2640_set_JPEG_size(static_cast<uint8_t>(m_resolution));
        delay(1000);

        //setFrameNumber(m_frame_number);
        //setSaturation(m_saturation);
        //setBrightness(m_brightness);
        //setContrast(m_contrast);
        //setColor(m_color);
        //setWhiteBalance(m_white_balance);

        m_camera->clear_fifo_flag();
        m_camera->write_reg(ARDUCHIP_FRAMES, 0x00);

        //SUCCESS_LOG("OV2640 initialization complete");
        return true;
    }

    void OV2640_Mini::capture()
    {
        m_camera->flush_fifo();
        m_camera->clear_fifo_flag();
        m_camera->OV2640_set_JPEG_size(static_cast<uint8_t>(m_resolution));
        //m_camera->OV2640_set_JPEG_size(static_cast<uint8_t>(m_resolution));
        m_camera->start_capture();

        //INFO_LOG("Capturing image...");
        while (!m_camera->get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
        //INFO_LOG("Image captured");
    }
