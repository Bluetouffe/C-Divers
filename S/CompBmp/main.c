#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

enum outputFlag {   filesIdentical = 0,
                    filesDifferent,
                    errorOpeningFile};

int main(int argc, char *argv[])
{
    int  i, maskEnabled = 1;
    char testTime[10]={'\0'};
    uint32_t refScreenSize = 0, testScreenSize = 0, maskSize = 0, cnt = 0, start = 0x3E;

    FILE * refScreen = NULL;
    FILE * testScreen = NULL;
    FILE * mask = NULL;
    FILE * output = NULL;

    char bitToTest = 0, toTestBuffer = 0;
    char fileSizeBuffer[5];
    char testFullName[512]={0};

    char * testName = argv[1];
    char * testNbr = argv[2];

    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(testTime,"_%2d_%2d_%2d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    strncpy(testFullName, argv[3], strrchr(argv[3], '\\') - argv[3] + 1);
    strncat(testFullName, strrchr(argv[4],'\\')+1, strlen(strrchr(argv[4],'\\'))-5);
    strcat(testFullName, testTime);
    strcat(testFullName, ".bmp");

    refScreen = fopen(argv[4],"r");
    testScreen = fopen(argv[3],"r");
    output = fopen("C:\\IMU Tools\\MCU_EHMI_PRE_VERIF\\TestResult\\TestResult.csv","a+");

    if (argc == 7)
        maskEnabled = strcmp(argv[5], "True");
    if (maskEnabled == 0)
        mask = fopen(argv[6],"r");

    if ((refScreen != NULL) && ((testScreen != NULL) && ((mask != NULL) || maskEnabled)))
    {
        if (!maskEnabled)
        {
            fseek(mask, 2, SEEK_SET);
            if (fgets(fileSizeBuffer, 5, mask) != NULL)
            {
                for(i=0; i<4; i++)
                {
                    maskSize |= (fileSizeBuffer[i] & 0x000000FF) << (8*i);
                }
            }
            fseek(mask, start, SEEK_SET);
        }
        fseek(refScreen, 2, SEEK_SET);
        if (fgets(fileSizeBuffer, 5, refScreen) != NULL)
        {
            for(i=0; i<4; i++)
            {
                refScreenSize |= (fileSizeBuffer[i] & 0x000000FF) << (8*i);
            }
        }

        fseek(testScreen, 2, SEEK_SET);
        if (fgets(fileSizeBuffer, 5, testScreen) != NULL)
        {
            for(i=0; i<4; i++)
                testScreenSize |= (fileSizeBuffer[i] & 0x000000FF) << (8*i);
        }

        if (((refScreenSize != maskSize) && !maskEnabled)
             || ((testScreenSize != refScreenSize)
                 || ((testScreenSize != maskSize) && !maskEnabled)))
                 {
                    fprintf(output, "%s;%s;ERRSIZE;%s\n", testName, testNbr, testTime+1);
                    fclose(refScreen);
                    fclose(testScreen);
                    fclose(output);
                    if (maskEnabled == 0)
                        fclose(mask);
                    if (rename(argv[3], testFullName))
                        return errorOpeningFile;
                    return filesDifferent;
                 }

        do
        {
            fseek(testScreen, start+cnt, SEEK_SET);
            fseek(refScreen, start+cnt, SEEK_SET);

            bitToTest = fgetc(testScreen);
            toTestBuffer = fgetc(refScreen);
            bitToTest = (bitToTest == toTestBuffer);

            if (maskEnabled == 0)
                bitToTest |= fgetc(mask);

            if (!bitToTest)
            {
                fprintf(output, "%s;%s;NOK;%s\n", testName, testNbr, testTime+1);

                fclose(refScreen);
                fclose(testScreen);
                fclose(output);
                if (maskEnabled == 0)
                    fclose(mask);
                if (rename(argv[3], testFullName))
                {
                    return errorOpeningFile;
                }
                return filesDifferent;
            }
            cnt++;
        }while (cnt < (refScreenSize - start));
    }
    else
    {
        fprintf(output, "%s;%s;ERROP;%s\n", testName, testNbr, testTime+1);
        if (rename(argv[3], testFullName))
        {
            return errorOpeningFile;
        }
        fclose(refScreen);
        fclose(testScreen);
        fclose(output);
        if (maskEnabled == 0)
            fclose(mask);

        return errorOpeningFile;
    }

    fprintf(output, "%s;%s;OK;%s\n", testName, testNbr, testTime+1);

    fclose(refScreen);
    fclose(testScreen);
    fclose(output);
    if (maskEnabled == 0)
        fclose(mask);
    if (remove(argv[3]))
        return errorOpeningFile;

    return filesIdentical;
}
