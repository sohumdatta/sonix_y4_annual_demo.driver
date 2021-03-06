#include <stdio.h>
#include <string.h>
#include <time.h>
#include "emg_driver.h"
#include "dsp.h"

/** TODO: remove
 * The IIR filter output computation requires samples from previous instants too. 
 * Please review its implementation in <bluetooth_driver_root>/lib/disp folder.
 * In the current implementataion, 10 consecutive raw samples are taken and the IIR
 * output of the last instant is returned.
 * 
 * Set the SAMPLE_LEN to appropriate value if you want to change the window away from 10.

#define SAMPLE_LEN 10

 */
int main(int argc, char* argv[])
{
    int i, j;
    //!< the serial port assumed is rfcomm0, usually the default
    struct emg_driver* emg_config = emg_driver_init("/dev/rfcomm0");	
	    
	if (!emg_config) return -1;

    struct emg_data data;

    double data_array[4][SAMPLE_LEN];
    double filtered_data[4][SAMPLE_LEN];

    long timestamps_s[SAMPLE_LEN];
    long timestamps_ns[SAMPLE_LEN];

    long sec_elapsed[10000];
    int ms_elapsed[10000];
    int us_elapsed[10000];

    for (i = 0; i < SAMPLE_LEN; i++)
    {
        emg_driver_get_samples(emg_config, &data);

        timestamps_s[i] = data.timestamp_s;
        timestamps_ns[i] = data.timestamp_ns;

        sec_elapsed[i] = data.sec_elapsed;
        ms_elapsed[i] = data.ms_elapsed;
        us_elapsed[i] = data.us_elapsed;

        for (j = 0; j < 4; j++)
        {
            data_array[j][i] = data.channels[j];
            filtered_data[j][i] = iir_filter(data_array[j][i]);
        }
    }
    emg_driver_deinit(emg_config);

    for (i = 0; i < 4; i++)
    {
        iir_filter(data_array[i], filtered_data[i], SAMPLE_LEN);
    }
        
    printf("%ld %ld %f %f %f %f\n",
               timestamps_s[SAMPLE_LEN-1],
               timestamps_ns[SAMPLE_LEN-1],
               filtered_data[0][SAMPLE_LEN-1],
               filtered_data[1][SAMPLE_LEN-1],
               filtered_data[2][SAMPLE_LEN-1],
               filtered_data[3][SAMPLE_LEN-1]);
    
    return 0;
}
