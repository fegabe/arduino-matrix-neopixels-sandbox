// https://wokwi.com/projects/332348441429541458

class Button
{                                             // a simple class for buttons based on the "Debounce" example
    const byte buttonPin;                     // the GPIO / pin for the button
    static constexpr byte debounceDelay = 30; // the debounce time; increase if the output flickers. Static because we only need one value for all buttons
    const bool active;                        // is the pin active HIGH or active LOW (will also activate the pullups!)
    bool lastButtonState = HIGH;              // the previous reading from the input pin
    byte lastDebounceTime = 0;                // the last time the output pin was toggled - we check only ONE byte, so I didn't mess around with unsigned long

public:
    /**
       \brief constructor for a button

       The constructor takes the GPIO as parameter.
       If you omit the second parameter, the library will activate the internal pullup resistor
       and the button should connect to GND.
       If you set the second parameter to HIGH, the button is active HIGH.
       The button should connect to VCC.
       The internal pullups will not be used but you will need an external pulldown resistor.

       \param attachTo the GPIO for the button
       \param active LOW (default) - if button connects to GND, HIGH if button connects to VCC
    */
    Button(byte attachTo, bool active = LOW) : buttonPin(attachTo), active(active) {}

    /**
       \brief set the pin to the proper state

       Call this function in your setup().
       The pinMode will be set according to your constructor.
    */
    void begin()
    {
        if (active == LOW)
            pinMode(buttonPin, INPUT_PULLUP);
        else
            pinMode(buttonPin, INPUT);
    }

    /**
        \brief indicate if button was pressed since last call

        @return HIGH if button was pressed since last call - debounce
    */
    bool wasPressed()
    {
        bool buttonState = LOW; // the current reading from the input pin
        byte reading = LOW;     // "translated" state of button LOW = released, HIGH = pressed, despite the electrical state of the input pint
        if (digitalRead(buttonPin) == active)
            reading = HIGH;                                         // if we are using INPUT_PULLUP we are checking invers to LOW Pin
        if (((millis() & 0xFF) - lastDebounceTime) > debounceDelay) // If the switch changed, AFTER any pressing or noise
        {
            if (reading != lastButtonState && lastButtonState == LOW) // If there was a change and and last state was LOW (= released)
            {
                buttonState = HIGH;
            }
            lastDebounceTime = millis() & 0xFF;
            lastButtonState = reading;
        }
        return buttonState;
    }
};

// AZ-Delivery Gamepad Shield code:
// Digital inputs, connected to push buttons
// const byte PIN_BUTTON_X = 2;
// const byte PIN_BUTTON_X_BYTE = 0b00000100;
// const byte PIN_BUTTON_A = 3;
// const byte PIN_BUTTON_A_BYTE = 0b00001000;
// const byte PIN_BUTTON_B = 4;
// const byte PIN_BUTTON_B_BYTE = 0b00010000;
// const byte PIN_BUTTON_Y = 5;
// const byte PIN_BUTTON_Y_BYTE = 0b00100000;
// const byte PIN_BUTTON_E = 6;
// const byte PIN_BUTTON_E_BYTE = 0b01000000;
// const byte PIN_BUTTON_F = 7;
// const byte PIN_BUTTON_K_BYTE = 0b10000000;
// const byte PIN_BUTTON_K = 8;

// bool isButtonPressed(byte button)
// {
//     return (ButtonStatus & button) == 0;
// }

// void readGamepadOld()
// {
//     ButtonStatus = PIND & 0b11111100;
//     // Pin 2 is configured to input with pullup
//     // Pin 3 is configured to input with pullup
//     // Pin 4 is configured to input with pullup
//     // Pin 5 is configured to input with pullup
//     // Pin 6 is configured to input with pullup
//     // Pin 7 is configured to input with pullup
//     // Pin 8 is configured to input with pullup
//     // Reading of inputs 2-7 directly via PortD
//     // A bitwise AND mask prevents the reading of
//     // pins 0 and 1 (serial interface)
//     JoystickButtonStatus = digitalRead(PIN_BUTTON_K);
//     XCoord = analogRead(PIN_ANALOG_X);
//     YCoord = analogRead(PIN_ANALOG_Y);
//     if ((ButtonStatus != ButtonOLDStatus) || (XCoord != XOLDCoord) ||
//         (YCoord != YOLDCoord) || (JoystickButtonStatus != JoystickButtonOLDStatus))
//     {
//         // delay(100); // Button debounce
//         ButtonOLDStatus = ButtonStatus;
//         JoystickButtonOLDStatus = JoystickButtonStatus;
//         XOLDCoord = XCoord;
//         YOLDCoord = YCoord;
//         // Serial.print("Taster: ");
//         // Serial.print(ButtonStatus, BIN);
//         // Serial.print(" JoystickTaster: ");
//         // Serial.print(JoystickButtonStatus, BIN);
//         // Serial.print(" Position X:");
//         // Serial.print(XCoord);
//         // Serial.print("Y: ");
//         // Serial.println(YCoord);

//         Serial.println("XCoord,YCoord: " + String(XCoord) + "," + String(YCoord));

//         // convert XY to a value between - 1 and 1, rounding to closest values, not using map float y = ((YCoord - 512.0) / 512.0); // roundf
//         float x = ((XCoord - 512.0) / 512.0);                                                                                    // roundf

//         Serial.println("X,Y: " + String(x) + "," + String(y));

//         if (isButtonPressed(PIN_BUTTON_X_BYTE))
//         {
//             Serial.println("Button X pressed");
//         }
//         if (isButtonPressed(PIN_BUTTON_A_BYTE))
//         {
//             Serial.println("Button A pressed");
//         }
//         if (isButtonPressed(PIN_BUTTON_B_BYTE))
//         {
//             Serial.println("Button B pressed");
//         }
//         if (isButtonPressed(PIN_BUTTON_Y_BYTE))
//         {
//             Serial.println("Button Y pressed");
//         }
//     }
// }
