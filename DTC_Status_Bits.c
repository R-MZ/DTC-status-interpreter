#include <stdio.h>
#include <math.h>

#define FALSE                               0x00
#define TRUE                                0x01
#define DTC_STATUS_SIZE                     0x02
#define NUMBER_OF_SHIFTS                    0x08

#define IS_IN_ASCII_NUMERICAL_RANGE(n)      ( ( (n >= 0x30) && (n <= 0x39) ) ? TRUE : FALSE )
#define IS_IN_ASCII_UPPERCASE_HEX_RANGE(n)  ( ( (n >= 0x41) && (n <= 0x46) ) ? TRUE : FALSE )
#define IS_IN_ASCII_LOWERCASE_HEX_RANGE(n)  ( ( (n >= 0x61) && (n <= 0x66) ) ? TRUE : FALSE )

#define ASCII_NUMERICAL_TO_DECIMAL_CONSTANT 0x30
#define ASCII_UPPERCASE_TO_DECIMAL_CONSTANT 0x37
#define ASCII_LOWERCASE_TO_DECIMAL_CONSTANT 0x57

char UserInputPreconditionsCheck(char * raw_user_input);
char InputLenghtCheck(char * raw_user_input);
char InputRangeCheck(char * raw_user_input);
unsigned char ASCIIToDecimal(char * input_value);
void DTCStatusInterpreter(unsigned char dtc_status);
void OutputToUserManager(char bit_position, char current_bit_status);

/* Returns TRUE if user input is 2 bytes long and in a valid hexadecimal range (0-9 or A-Z or a-z) */
char UserInputPreconditionsCheck(char * raw_user_input)
{
    char ret_val = FALSE;
    char input_range_status = FALSE;
    char input_length_status = InputLenghtCheck(raw_user_input);
    
    if(FALSE != input_length_status)
    {
        input_range_status = InputRangeCheck(raw_user_input);
    }

    ret_val = input_length_status & input_range_status;
    return ret_val;
}

/* Returns TRUE only if user input is 2 bytes long*/
char InputLenghtCheck(char * raw_user_input)
{
    char ret_val = FALSE;
    unsigned char iterator = 0;
    
    /* Checks how many bytes are in user input */
    while( (iterator <= DTC_STATUS_SIZE) && (raw_user_input[iterator] != '\0') )
    {
        ++iterator;
    }

    /* User input is 2 bytes long */
    if(DTC_STATUS_SIZE == iterator)
    {
        ret_val = TRUE;
    }
    
    else
    {
        printf("NRC 13: Incorrect message lenght or invalid format :v\n");
    }

    return ret_val;
}

/* Returns TRUE only if user input is in a valid hexadecimal range (0-9 or A-Z or a-z)  */ 
char InputRangeCheck(char * raw_user_input)
{
    char ret_val = FALSE;

    ret_val = ( IS_IN_ASCII_NUMERICAL_RANGE(raw_user_input[0]) || IS_IN_ASCII_UPPERCASE_HEX_RANGE(raw_user_input[0]) || IS_IN_ASCII_LOWERCASE_HEX_RANGE(raw_user_input[0]) );
    ret_val &= ( IS_IN_ASCII_NUMERICAL_RANGE(raw_user_input[1]) || IS_IN_ASCII_UPPERCASE_HEX_RANGE(raw_user_input[1]) || IS_IN_ASCII_LOWERCASE_HEX_RANGE(raw_user_input[1]) );

    if(FALSE == ret_val)
    {
        printf("NRC 31: Request Out Of Range xd\n");
    }

    return ret_val;
}

/* Converts the previously verified user input from ASCII to decimal */
unsigned char ASCIIToDecimal(char * dtc_status)
{
    unsigned char ret_val = 0;
    char power = 1;

    /* Byte 0 from dtc_status is the most significant nibble, thus, in order to convert it to decimal we have to multiply it by 16^1 */
    for(char i = 0; i < DTC_STATUS_SIZE; ++i)
    {
        if( IS_IN_ASCII_NUMERICAL_RANGE(dtc_status[i]) )
        {
            ret_val += (dtc_status[i] - ASCII_NUMERICAL_TO_DECIMAL_CONSTANT) * pow(16, power);
        }

        else if( IS_IN_ASCII_UPPERCASE_HEX_RANGE(dtc_status[i]) )
        {
            ret_val += (dtc_status[i] - ASCII_UPPERCASE_TO_DECIMAL_CONSTANT) * pow(16, power);
        }

        else
        {
            ret_val += (dtc_status[i] - ASCII_LOWERCASE_TO_DECIMAL_CONSTANT) * pow(16, power);
        }
        power--;
    }
    
    return ret_val;
}

/* Logic for the interpretation of the DTC status given by the user */
void DTCStatusInterpreter(unsigned char dtc_status)
{
    char dtc_lsb_status = FALSE;
    for(char current_shift = 0; current_shift < NUMBER_OF_SHIFTS; ++current_shift)
    {
        dtc_lsb_status = ( (dtc_status >> current_shift) & TRUE);
        OutputToUserManager(current_shift, dtc_lsb_status);
    }
}

void OutputToUserManager(char bit_position, char current_bit_status)
{
    static char first_run_flag = FALSE;

    /* Print the header only in the first run */   
    if(FALSE == first_run_flag)
    {
        printf("\n Response bit | Bit position |                 Status\n");
        printf(" -------------|--------------|----------------------------------------\n");
        first_run_flag = TRUE;
    }

    switch(bit_position)
    {
        case 0:
            printf("       %d      |      %d       | Test failed\n", current_bit_status, bit_position);
            break;
        case 1:
            printf("       %d      |      %d       | Test failed this operation cycle\n", current_bit_status, bit_position);
            break;
        case 2:
            printf("       %d      |      %d       | Pending DTC\n", current_bit_status, bit_position);
            break;
        case 3:
            printf("       %d      |      %d       | Confirmed DTC\n", current_bit_status, bit_position);
            break;
        case 4:
            printf("       %d      |      %d       | Test not completed since last clear\n", current_bit_status, bit_position);
            break;
        case 5:
            printf("       %d      |      %d       | Test failed since last clear\n", current_bit_status, bit_position);
            break;
        case 6:
            printf("       %d      |      %d       | Test not completed this operation cycle\n", current_bit_status, bit_position);
            break;
        case 7:
            printf("       %d      |      %d       | Warning indicator requested\n\n", current_bit_status, bit_position);
            break;
        default:
            printf("Impossible my fren");
    }
}

int main()
{
    /* Each character that the user inputs equals one byte because it comes in ASCII code. Ex: "1003" is 4 bytes long (+ 1 because of the implicit use of eol character '\0') */
    char raw_user_input[10] = {0};
    char user_input_status = FALSE;
    
    printf("\nPlease enter the DTC status value in hexadecimal: ");
    scanf("%s", &raw_user_input);

    user_input_status = UserInputPreconditionsCheck(raw_user_input);
    if(FALSE != user_input_status)
    {
        unsigned char decimal_dtc_status_value = ASCIIToDecimal(raw_user_input);
        DTCStatusInterpreter(decimal_dtc_status_value);
    }

    return 0;
}