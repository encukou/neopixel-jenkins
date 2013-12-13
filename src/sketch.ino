#include <Adafruit_NeoPixel.h>

#define LED_PIN 7
#define NUM_LEDS 60

#define IRED 0
#define IGREEN 1
#define IBLUE 2

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB | NEO_KHZ800);

byte led_info[NUM_LEDS][2];
int current_led;
byte current_color;

void setup() {
    Serial.begin(9600);
    strip.begin();
    //strip.setBrightness(10);
}

void loop() {
    if (Serial.available()) {
        char c = Serial.read();
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            current_color = c;
        }
        if (c >= '0' && c <= '9') {
            led_info[current_led][0] = current_color;
            led_info[current_led][1] = c - '0';
            current_led++;
            if (current_led >= NUM_LEDS) current_led = 0;
        }
        if (c == '\n' || c == '\r') {
            for (; current_led < NUM_LEDS; current_led++) {
                led_info[current_led][0] = 0;
                led_info[current_led][1] = 1;
            }
            current_led = 0;
        }
    }
    for (int led = 0; led < NUM_LEDS; led++) {
        byte lo = led_info[led][1];
        byte hi = 255 - led_info[led][1] * 24;
        byte rgb[3] = {lo, lo, lo};
        bool ok = true;
        switch (led_info[led][0] | 0x20) {
            case 'r': rgb[0]=hi; break;
            case 'y': rgb[0]=hi; rgb[1]=hi; break;
            case 'g': rgb[1]=hi; break;
            case 'x': rgb[2]=hi; break;
            default: ok = false; break;
        }
        if (!ok) {
            strip.setPixelColor(led, 0);
            continue;
        }
        if (!(led_info[led][0] & 0x20)) {
            unsigned value;
            unsigned ms = millis() / 2;
            if (ms % 512 < 256) {
                value = ms % 256;
            } else {
                value = 255 - ms % 256;
            }
            for(int i=0; i <3; i++) {
                rgb[i] = (rgb[i] * value) / 256;
            }
        }
        for(int i=0; i <3; i++) {
            rgb[i] = rgb[i] / 10;
        }
        uint32_t color = strip.Color(rgb[0], rgb[1], rgb[2]);
        strip.setPixelColor(led, color);
    }
    strip.show();
}
