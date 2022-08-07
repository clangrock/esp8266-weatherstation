/* global datatypes
 * PCE Instruments PCE-WS P 
 * Author:  Christian Langrock
 * Date:    2022-July-31
 */


// store the device adresses
struct DeviceAddress_array{ byte Address[64][8]; };

// store the device adresses
struct Rain_array{ float Moisture;
                    char* Result {"false"}; };
