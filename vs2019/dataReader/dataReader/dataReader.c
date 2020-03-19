/*
 * File        : dataReader.c
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This program is to generate an S19 download file format or an
 *               assembly file which are both readable by human.
 *               This utility takes any binary input file and fransforms it
 *               into S-Record output file, or an assembly file for use in an
 *               embedded software development environment.
 */

#include "dataReader.h"

int main(int argc, char* argv[])
{
    FILE* fpInput = NULL;                               // a file pointer for an input binary file
    FILE* fpOutput = NULL;                              // a file pointer for an output text file
    int sizeFile = 0;								    // size of a file to be transformed
    Mode eMode = Assembly;                              // for identifying the mode
    Delimeter eDelimeter = Help;                        // for identifying the delimeter

    char inputFileName[MAX_LEN_FILE_NAME] = "";
    char outputFileName[MAX_LEN_FILE_NAME] = "";
    char delimiters[NUM_OPTIONAL_SWITCHES][MAX_LEN_OPTIONAL_SWITCHES + 1] =
    { "-i", "-o", "-srec", "-h" };                      // 4 optional switches

    // processing the arguments of the command line
    if (argc <= MAX_ARGC)
    {
        char buffString[LEN_STRING_ARGV + 1] = "";

        for (int i = 1; i < argc; i++)
        {
            // check if there is the switch of "-srec"
            strcpy(buffString, argv[i]);
            buffString[MAX_LEN_OPTIONAL_SWITCHES] = '\0';
            eDelimeter = Srec;                                          // "-srec"
            if (strcmp(buffString, delimiters[eDelimeter]) == 0)
            {
                if (strlen(argv[i]) == MAX_LEN_OPTIONAL_SWITCHES)       // "-srec"
                {
                    eMode = Srecord;
                }
                else                                                    // abnormal cases
                {
                    eMode = DisplayHelp;
                    break;
                }
            }

            // check if there is the switch of "-i", and identify the input file name
            buffString[MIN_LEN_OPTIONAL_SWITCHES] = '\0';
            eDelimeter = Input;                                         // "-i"
            if (strcmp(buffString, delimiters[eDelimeter]) == 0)
            {
                if (strlen(argv[i]) > MIN_LEN_OPTIONAL_SWITCHES)        // "-i"
                {
                    strcpy(inputFileName, (argv[i] + MIN_LEN_OPTIONAL_SWITCHES));
                }
                else                                                    // abnormal cases
                {
                    eMode = DisplayHelp;
                    break;
                }
            }

            // check if there is the switch of "-o", and identify the output file name
            eDelimeter = Output;                                        // "-o"
            if (strcmp(buffString, delimiters[eDelimeter]) == 0)
            {
                if (strlen(argv[i]) > MIN_LEN_OPTIONAL_SWITCHES)        // "-o"
                {
                    if (strstr(argv[i], buffString) != NULL)
                    {
                        strcpy(outputFileName, (argv[i] + MIN_LEN_OPTIONAL_SWITCHES));
                    }
                }
                else                                                    // abnormal cases
                {
                    eMode = DisplayHelp;
                    break;
                }
            }

            // check if the switch of "-h"
            eDelimeter = Help;                                          // "-h"
            if (strcmp(buffString, delimiters[eDelimeter]) == 0)        // "-h" or abnormal cases     
            {
                eMode = DisplayHelp;
                break;
            }

        }
    }
    else                                                                // abnormal cases: over AX_ARGC
    {
        eMode = DisplayHelp;
    }

    if (eMode == DisplayHelp)
    {

        return -1;
    }

    // processing file I/O for opening
    if (strcmp(inputFileName, "") != 0)                                 // input file name given
    {
        if ((fpInput = fopen(inputFileName, "rb")) == NULL)             // for an input binary file
        {
            printf("\nError: opening an input file [%s]\n", inputFileName);
            return (-2);
        }
        fseek(fpInput, 0, SEEK_END);
        sizeFile = ftell(fpInput);
        fseek(fpInput, 0, SEEK_SET);
    }

    if (strcmp(outputFileName, "") != 0)                                // output file name given
    {
        if ((fpOutput = fopen(outputFileName, "w")) == NULL)            // for an output text file
        {
            printf("\nError: opening an output file [%s]\n", outputFileName);
            return (-2);
        }
    }
    else
    {
        if (strcmp(inputFileName, "") != 0)
        {
            strcpy(outputFileName, inputFileName);
            if (eMode == Srecord)
            {
                strcat(outputFileName, ".srec");
            }
            else                                                        // eMode == Assembly
            {
                strcat(outputFileName, ".asm");
            }
            if ((fpOutput = fopen(outputFileName, "w")) == NULL)        // for an output text file
            {
                printf("\nError: opening an output file [%s]\n", outputFileName);
                return (-2);
            }
        }
    }

    // processing transforming

    // initialization
    int numS1 = 0;
    int readBytes = 0;
    char buffLineInput[LEN_FIELD_DATA + 1] = { 0 };

    if (eMode == Srecord)
    {
        while (LOOP_FOREVER)                                            // an infinite loop
        {
            if (fpInput != NULL)
            {// in the case of using an output file
                readBytes = (int)fread(buffLineInput, sizeof(char), LEN_FIELD_DATA, fpInput);
            }
            else
            {// in the case of using stdin
                unsigned char buffASCII[2] = { 0, 0 };
                int counter = 0;
                printf("\nWhen total 16 characters are entered or EOF, the encoding process runs.\n");
                while (LOOP_FOREVER)
                {
                    buffASCII[0] = getch();
                    if ((buffASCII[0] == 0x0D))
                    {
                        putchar('\n');
                    }
                    else
                    {
                        putchar(buffASCII[0]);
                    }
                    //if ((buffASCII[0] != 0x0A) && (buffASCII[0] != 0x04)) // <CR> in Linux
                    if ((buffASCII[0] != 0x0D) && (buffASCII[0] != 0x04))   // <CR> in Windows
                    {
                        buffLineInput[counter++] = buffASCII[0];
                    }

                    if (counter == LEN_FIELD_DATA)                          // full of line input
                    {
                        readBytes = counter;
                        putchar('\n');
                        break;
                    }
                    //if ((buffASCII[0] == 0x04) && (buffASCII[1] == 0x0A))    //EOF: CTRL-D in Linux
                    if ((buffASCII[0] == 0x04) && ((buffASCII[1] == 0x0D) || (counter == 0)))      //EOF: CTRL-D in Windows
                    {
                        readBytes = counter;
                        putchar('\n');
                        break;
                    }
                    buffASCII[1] = buffASCII[0];
                }
            }
            if (readBytes > 0)
            {
                numS1++;
                sizeFile -= readBytes;
                processSREC(buffLineInput, fpOutput, readBytes, numS1);
            }
            else
            {
                if (sizeFile > 0)
                {
                    printf("Error: reading an input file [%s]\n", inputFileName);
                    return (-2);
                }
                else                                                // in case of EOF
                {
                    processS5S9(fpOutput, numS1);
                    break;
                }
            }

        }
    }
    else if (eMode == Assembly)
    {
        while (LOOP_FOREVER)                                            // an infinite loop
        {
            if (fpInput != NULL)                                        
            {// in the case of using an output file
                readBytes = (int)fread(buffLineInput, sizeof(char), LEN_FIELD_DATA, fpInput);
            }
            else                                              
            {// in the case of using stdin
                unsigned char buffASCII[2] = { 0, 0};
                int counter = 0;
                printf("\nWhen total 16 characters are entered or EOF, the encoding process runs.\n");
                while (LOOP_FOREVER)
                {
                    buffASCII[0] = getch();
                    if ((buffASCII[0] == 0x0D))
                    {
                        putchar('\n');
                    }
                    else
                    {
                        putchar(buffASCII[0]);
                    }
                    //if ((buffASCII[0] != 0x0A) && (buffASCII[0] != 0x04)) // <CR> in Linux
                    if ((buffASCII[0] != 0x0D) && (buffASCII[0] != 0x04))   // <CR> in Windows
                    {
                        buffLineInput[counter++] = buffASCII[0];
                    }
                    
                    if (counter == LEN_FIELD_DATA)                          // full of line input
                    {
                        readBytes = counter;
                        putchar('\n');
                        break;
                    }
                    //if ((buffASCII[0] == 0x04) && (buffASCII[1] == 0x0A))    //EOF: CTRL-D in Linux
                    if ((buffASCII[0] == 0x04) && ((buffASCII[1] == 0x0D) || (counter == 0)))      //EOF: CTRL-D in Windows
                    {
                        readBytes = counter;
                        putchar('\n');
                        break;
                    }
                    buffASCII[1] = buffASCII[0];
                }
            }
            if (readBytes > 0)
            {
                numS1++;
                sizeFile -= readBytes;
                processASM(buffLineInput, fpOutput, readBytes);
            }
            else
            {
                if (sizeFile > 0)
                {
                    printf("Error: reading an input file [%s]\n", inputFileName);
                    return (-2);
                }
                else                                                // in case of EOF
                {
                    break;
                }
            }
        }
    }

    // processing file I/O for closing
    if (fpInput != NULL)
    {
        if (fclose(fpInput) != 0)                                   // closing the input binary file
        {
            printf("\nError: closing the input file\n");
            return (-2);
        }
    }

    if (fpOutput != NULL)
    {
        if (fclose(fpOutput) != 0)                                  // closing the output text file
        {
            printf("\nError: closing the output file\n");
            return (-2);
        }
    }

    return (0);
}