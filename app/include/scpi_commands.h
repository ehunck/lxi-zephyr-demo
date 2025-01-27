#ifndef SCPI_COMMANDS_H
#define SCPI_COMMANDS_H

#include "scpi/scpi.h"  // Include the SCPI header file

// Declare the SCPI context
extern scpi_t scpi_context;

// Function prototypes for SCPI command handlers
scpi_result_t SCPI_DigitalOutputQ(scpi_t *context);
scpi_result_t SCPI_AnalogInputQ(scpi_t *context);

// Function prototypes for SCPI command initialization
void scpi_init(void);

#endif // SCPI_COMMANDS_H
