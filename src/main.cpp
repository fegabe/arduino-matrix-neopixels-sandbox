#include <Arduino.h>
#include <FastLED.h>

// #define SERIAL_BAUD 115200
#define SERIAL_BAUD 9600

#pragma region GAMEPAD

//   (X)
// (Y)  (A)
//    (B)

// Digital inputs, connected to push buttons
const byte PIN_BUTTON_X = 2;
const byte PIN_BUTTON_X_BYTE = 0b00000100;
const byte PIN_BUTTON_A = 3;
const byte PIN_BUTTON_A_BYTE = 0b00001000;
const byte PIN_BUTTON_B = 4;
const byte PIN_BUTTON_B_BYTE = 0b00010000;
const byte PIN_BUTTON_Y = 5;
const byte PIN_BUTTON_Y_BYTE = 0b00100000;
const byte PIN_BUTTON_E = 6;
const byte PIN_BUTTON_E_BYTE = 0b01000000;
const byte PIN_BUTTON_F = 7;
const byte PIN_BUTTON_K_BYTE = 0b10000000;
const byte PIN_BUTTON_K = 8;

// X Coordinate is queried via analog port Min:0 / Max: 1023
// A nalog Inputs, connected to the joystiyck coordinate is queried // via analog port 0 Min:0 / Max: 102 3 const byte
const byte PIN_ANALOG_X = 0;
const byte PIN_ANALOG_Y = 1;
byte ButtonStatus = 0;
byte ButtonOLDStatus = 0;
byte JoystickButtonStatus = 1;
byte JoystickButtonOLDStatus = 1;
int XCoord = 0;
int XOLDCoord = 0;
int YCoord = 0;
int YOLDCoord = 0;

#pragma endregion

const uint8_t blinkLedPin = 11;
const uint8_t blinkButtonPin = 12;

#pragma region MATRIX

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define MATRIX_DATA_PIN 10
#define CLOCK_PIN 13

#define NUM_LEDS (8 * 8)
#define GLOBAL_BRIGHTNESS_SCALING 64
const uint8_t brightness = 255 / 4;
// Define the array of leds
CRGB leds[NUM_LEDS];

uint8_t matrixColorIndex;

uint8_t currentPaletteIndex;
CRGBPalette16 currentPalette;
TBlendType currentBlending;

uint8_t mappedFallingPixel = 0;
uint8_t fallingPixelIndex = 0;
int fallingDirection = -1;

#pragma endregion

// variables will change:
int buttonState = 0; // variable for reading the pushbutton status
int currentMillis = 0;
int millisDelay = 4000;
int blinkOn = 0;

bool isButtonPressed(byte button);
void readGamepad();

void changePalettePeriodically();
void fillLEDsFromPaletteColors(bool isPressed, uint8_t startColorIndex);
void ledWrapper(bool on);
bool readButton();

void setup()
{
    Serial.begin(SERIAL_BAUD);

    pinMode(PIN_BUTTON_X, INPUT_PULLUP);
    pinMode(PIN_BUTTON_A, INPUT_PULLUP);
    pinMode(PIN_BUTTON_B, INPUT_PULLUP);
    pinMode(PIN_BUTTON_Y, INPUT_PULLUP);
    pinMode(PIN_BUTTON_E, INPUT_PULLUP);
    pinMode(PIN_BUTTON_F, INPUT_PULLUP);
    pinMode(PIN_BUTTON_K, INPUT_PULLUP);

    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(blinkLedPin, OUTPUT);
    pinMode(blinkButtonPin, INPUT_PULLUP);

    FastLED.addLeds<NEOPIXEL, MATRIX_DATA_PIN>(leds, NUM_LEDS); // GRB ordering is assumed
    FastLED.setBrightness(GLOBAL_BRIGHTNESS_SCALING);

    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    // delay(500);
    Serial.println("Inicializando...\n\n");
}

void loop()
{
    readGamepad();

    if (currentMillis >= millisDelay)
    {
        currentMillis = 0;
        blinkOn = !blinkOn;
        // Serial.println("change blink!");
    }

    ledWrapper(blinkOn);

    buttonState = digitalRead(blinkButtonPin);
    // bool isPressed = buttonState == LOW;
    // bool isPressed = buttonState == HIGH;
    bool isPressed = isButtonPressed(PIN_BUTTON_B_BYTE);
    digitalWrite(blinkLedPin, isPressed ? HIGH : LOW);
    // digitalWrite(LED_BUILTIN, isPressed ? HIGH : LOW);

    // change color every X ms
    if (currentMillis % 2 == 0)
    {
        matrixColorIndex = matrixColorIndex + 1; /* motion speed */
    }

    changePalettePeriodically();
    fillLEDsFromPaletteColors(isPressed, matrixColorIndex);

    if (currentMillis % 10 == 0)
    {
        leds[mappedFallingPixel] = CRGB::Black;
        if (fallingDirection == -1)
        {
            mappedFallingPixel = map(fallingPixelIndex, 0, 8, NUM_LEDS - 8, NUM_LEDS);
        }
        else
        {
            mappedFallingPixel = map(fallingPixelIndex, 0, 8, NUM_LEDS - 1, NUM_LEDS - 9);
        }
        leds[mappedFallingPixel] = CRGB::Red;
        // Serial.println("fallingIdx: " + String(fallingPixelIndex) + " VS mappedIdx: " + String(mappedFallingPixel));
        fallingPixelIndex = ++fallingPixelIndex % 8;
        if (fallingPixelIndex == 0)
        {
            fallingDirection = -fallingDirection;
            // Serial.println("change direction! " + String(fallingDirection));
        }
    }

    FastLED.show();

    currentMillis++;

    // delay(200);
    // delay(1);
}

bool isButtonPressed(byte button)
{
    return (ButtonStatus & button) == 0;
}

void readGamepad()
{
    ButtonStatus = PIND & 0b11111100;
    // Pin 2 is configured to input with pullup
    // Pin 3 is configured to input with pullup
    // Pin 4 is configured to input with pullup
    // Pin 5 is configured to input with pullup
    // Pin 6 is configured to input with pullup
    // Pin 7 is configured to input with pullup
    // Pin 8 is configured to input with pullup
    // Reading of inputs 2-7 directly via PortD
    // A bitwise AND mask prevents the reading of
    // pins 0 and 1 (serial interface)
    JoystickButtonStatus = digitalRead(PIN_BUTTON_K);
    XCoord = analogRead(PIN_ANALOG_X);
    YCoord = analogRead(PIN_ANALOG_Y);
    if ((ButtonStatus != ButtonOLDStatus) || (XCoord != XOLDCoord) ||
        (YCoord != YOLDCoord) || (JoystickButtonStatus != JoystickButtonOLDStatus))
    {
        // delay(100); // Button debounce
        ButtonOLDStatus = ButtonStatus;
        JoystickButtonOLDStatus = JoystickButtonStatus;
        XOLDCoord = XCoord;
        YOLDCoord = YCoord;
        // Serial.print("Taster: ");
        // Serial.print(ButtonStatus, BIN);
        // Serial.print(" JoystickTaster: ");
        // Serial.print(JoystickButtonStatus, BIN);
        // Serial.print(" Position X:");
        // Serial.print(XCoord);
        // Serial.print("Y: ");
        // Serial.println(YCoord);

        Serial.println("XCoord,YCoord: " + String(XCoord) + "," + String(YCoord));

        // convert XY to a value between -1 and 1, rounding to closest values, not using map
        float y = ((YCoord - 512.0) / 512.0); // roundf
        float x = ((XCoord - 512.0) / 512.0); // roundf

        Serial.println("X,Y: " + String(x) + "," + String(y));

        if (isButtonPressed(PIN_BUTTON_X_BYTE))
        {
            Serial.println("Button X pressed");
        }
        if (isButtonPressed(PIN_BUTTON_A_BYTE))
        {
            Serial.println("Button A pressed");
        }
        if (isButtonPressed(PIN_BUTTON_B_BYTE))
        {
            Serial.println("Button B pressed");
        }
        if (isButtonPressed(PIN_BUTTON_Y_BYTE))
        {
            Serial.println("Button Y pressed");
        }
    }
}
void ledWrapper(bool on)
{
    digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
    // digitalWrite(blinkLedPin, on ? HIGH : LOW);
    // digitalWrite(blinkLedPin, !on ? HIGH : LOW);
}

void fillLEDsFromPaletteColors(bool isPressed, uint8_t startColorIndex)
{
    for (int i = 0; i < NUM_LEDS - 8 * 2; ++i)
    {
        leds[i] = !isPressed ? CRGB::Black : ColorFromPalette(currentPalette, startColorIndex + i + 3, brightness, LINEARBLEND);
    }
}

bool readButton()
{
    delay(1);
    return digitalRead(blinkButtonPin) == LOW;
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for (int i = 0; i < 16; ++i)
    {
        currentPalette[i] = CHSV(random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid(currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV(HUE_PURPLE, 255, 255);
    CRGB green = CHSV(HUE_GREEN, 255, 255);
    CRGB black = CRGB::Black;

    currentPalette = CRGBPalette16(
        green, green, black, black,
        purple, purple, black, black,
        green, green, black, black,
        purple, purple, black, black);
}

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
    {
        CRGB::Red,
        CRGB::Gray, // 'white' is too bright compared to red and blue
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Red,
        CRGB::Gray,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Blue,
        CRGB::Black,
        CRGB::Black};

void changePalettePeriodically()
{
    bool change = currentMillis % 500 == 0;
    if (!change)
    {
        return;
    }

    currentPaletteIndex += 5;
    currentPaletteIndex = currentPaletteIndex % 60;

    if (currentPaletteIndex < 10)
    {
        currentPalette = RainbowColors_p;
        currentBlending = LINEARBLEND;
    }
    if (currentPaletteIndex == 10)
    {
        currentPalette = RainbowStripeColors_p;
        currentBlending = NOBLEND;
    }
    if (currentPaletteIndex == 15)
    {
        currentPalette = RainbowStripeColors_p;
        currentBlending = LINEARBLEND;
    }
    if (currentPaletteIndex == 20)
    {
        SetupPurpleAndGreenPalette();
        currentBlending = LINEARBLEND;
    }
    if (currentPaletteIndex == 25)
    {
        SetupTotallyRandomPalette();
        currentBlending = LINEARBLEND;
    }
    if (currentPaletteIndex == 30)
    {
        SetupBlackAndWhiteStripedPalette();
        currentBlending = NOBLEND;
    }
    if (currentPaletteIndex == 35)
    {
        SetupBlackAndWhiteStripedPalette();
        currentBlending = LINEARBLEND;
    }
    if (currentPaletteIndex == 40)
    {
        currentPalette = CloudColors_p;
        currentBlending = LINEARBLEND;
    }
    if (currentPaletteIndex == 45)
    {
        currentPalette = PartyColors_p;
        currentBlending = LINEARBLEND;
    }
    if (currentPaletteIndex == 50)
    {
        currentPalette = myRedWhiteBluePalette_p;
        currentBlending = NOBLEND;
    }
    if (currentPaletteIndex == 55)
    {
        currentPalette = myRedWhiteBluePalette_p;
        currentBlending = LINEARBLEND;
    }
}