#include <zephyr/ztest.h>
#include <string.h>
#include "scpi_commands.h"

static char output_buf[256];
static size_t output_pos;

static size_t test_write(scpi_t *context, const char *data, size_t len)
{
    if (output_pos + len >= sizeof(output_buf)) {
        len = sizeof(output_buf) - output_pos - 1;
    }
    memcpy(output_buf + output_pos, data, len);
    output_pos += len;
    output_buf[output_pos] = '\0';
    return len;
}

static void *scpi_setup(void)
{
    scpi_interface.write = test_write;
    output_pos = 0;
    output_buf[0] = '\0';
    scpi_init();
    scpi_context.user_context = NULL;
    return NULL;
}

static void scpi_test_cmd(const char *cmd, const char *expected)
{
    output_pos = 0;
    output_buf[0] = '\0';
    zassert_true(SCPI_Input(&scpi_context, cmd, strlen(cmd)), "SCPI_Input failed");
    zassert_true(strcmp(output_buf, expected) == 0, "Unexpected output: %s", output_buf);
}

ZTEST(scpi_suite, test_idn_query)
{
    scpi_test_cmd("*IDN?\r\n", "MANUFACTURE,INSTR2013,0,01-02\r\n");
}

ZTEST(scpi_suite, test_opc_query)
{
    scpi_test_cmd("*OPC?\r\n", "1\r\n");
}

ZTEST(scpi_suite, test_cls)
{
    scpi_test_cmd("*CLS\r\n", "");
}

ZTEST(scpi_suite, test_ese_set_and_query)
{
    scpi_test_cmd("*ESE 1\r\n", "");
    scpi_test_cmd("*ESE?\r\n", "1\r\n");
}

ZTEST(scpi_suite, test_esr_query)
{
    scpi_test_cmd("*ESR?\r\n", "0\r\n");
}

ZTEST(scpi_suite, test_sre_set_and_query)
{
    scpi_test_cmd("*SRE 255\r\n", "");
    scpi_test_cmd("*SRE?\r\n", "255\r\n");
}

ZTEST(scpi_suite, test_stb_query)
{
    scpi_test_cmd("*STB?\r\n", "0\r\n");
}

ZTEST(scpi_suite, test_tst_query)
{
    scpi_test_cmd("*TST?\r\n", "0\r\n");
}

ZTEST(scpi_suite, test_wai)
{
    scpi_test_cmd("*WAI\r\n", "");
}

ZTEST(scpi_suite, test_system_error)
{
    scpi_test_cmd("SYSTem:ERRor:NEXT?\r\n", "0,\"No error\"\r\n");
}

ZTEST(scpi_suite, test_system_error_count)
{
    scpi_test_cmd("SYSTem:ERRor:COUNt?\r\n", "0\r\n");
}

ZTEST(scpi_suite, test_system_version)
{
    scpi_test_cmd("SYSTem:VERSion?\r\n", "1999.0\r\n");
}

ZTEST(scpi_suite, test_status_questionable_enable)
{
    scpi_test_cmd("STATus:QUEStionable:ENABle 2\r\n", "");
    scpi_test_cmd("STATus:QUEStionable:ENABle?\r\n", "2\r\n");
}

ZTEST(scpi_suite, test_status_questionable_event)
{
    scpi_test_cmd("STATus:QUEStionable:EVENt?\r\n", "0\r\n");
}

ZTEST(scpi_suite, test_status_preset)
{
    scpi_test_cmd("STATus:PRESet\r\n", "");
}

ZTEST(scpi_suite, test_digital_output)
{
    scpi_test_cmd("DIGital:OUTPut 1\r\n", "");
}

ZTEST_SUITE(scpi_suite, NULL, scpi_setup, NULL, NULL, NULL);

