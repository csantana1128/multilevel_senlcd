#define MAX6675_SPI_ID SPI_0_ID
#define MAX6675_CS_INDEX 0


typedef enum
{
    MAX6675_OK = 0,
    MAX6675_ERROR = 1,
    MAX6675_OPEN_CIRCUIT = 2
} max6675_status_t;


max6675_status_t max6675_init(void);
max6675_status_t max6675_read_temperature(int *temp_x100);
void max6675_example_usage(void);
void init_timer_30s(void);
