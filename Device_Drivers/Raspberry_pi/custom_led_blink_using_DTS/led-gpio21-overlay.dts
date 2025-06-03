/dts-v1/;
/plugin/;

/ {
    compatible = "brcm,bcm2711";

    fragment@0 {
        target-path = "/";
        __overlay__ {
            led_node {
                compatible = "techdhaba,led-blink";
                status = "okay";
                gpios = <&gpio 21 0>; // GPIO21, active high
                label = "led_gpio";
            };
        };
    };
};
