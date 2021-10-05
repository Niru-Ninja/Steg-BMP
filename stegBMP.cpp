#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
using namespace std;

void printHelp(int errcode){
switch(errcode){
    case 0:
        printf("\nEnter '-W' for write mode or '-R' for read mode (note that the letters are capitalized). \nFor more information use stegBMP -h\n");
    break;
    case 1:
        printf("\nParameters are missing. \n");
    break;
    case 2:
        printf("\nThere was a problem reading the -v parameter, check your syntax and try again. \n");
    break;
    case 3:
        printf("\nThere was a problem reading the -r parameter, check your syntax and try again. \n");
    break;
    case 4:
        printf("\nThere was a problem reading the -i parameter, check your syntax and try again. \n");
    break;
    case 5:
        printf("\nThere was a problem reading the -d parameter, check your syntax and try again. \n");
    break;
    case 6:
        printf("\nVessel file does not exist (-v)\n");
    break;
    case 7:
        printf("\nInfo file does not exist (-i)\n");
    break;
    default:
        printf("\nWRITE MODE : -W\n");
        printf("\nCopies the info (-i) file into the vessel (-v) file, storing the result in the result (-r) file. \n");
        printf("\n\t-v : Indicates the path to the vessel. (MUST BE A 24 bit deep .bmp IMAGE)\n");
        printf("\t-i : Indicates the path to the info file.\n");
        printf("\t-r : Indicates the path where the result file will be stored.\n");
        //printf("\t-d : (optional) Indicates the density: how many Bytes of the vessel file are needed to store a Byte of the info file. (minimum 1, maximum 8. Default value = 8). \n");
        //printf("\nThe lower the density, it is more noticeable that the vessel file was modified. \n\n");
        printf("\nREADING MODE: -R\n");
        printf("\nRead the file vessel (-v) selecting the indicated bits to retrieve a previously hidden file, store the result in the file result (-r). \n");
        printf("\n\t-v : Indicates the path to the vessel. \n");
        printf("\t-r : Indicates the path where the result file will be stored.\n");
        printf("\n\n");
        //printf("\t-d : (optional) Indicates how many bits will be used to rebuild the file for each Byte of the vessel file. (minimum 1, maximum 8. Default value = 1). \n");
        //printf("\nWhen rebuilding a file it is crucial to use the correct density value.\n\n");
    break;
}
return;
}

int checkFile(string checkMe){
    return (access(checkMe.c_str(), F_OK));
}

string hexToBin(char hexChar){
    string retMe;
    switch(hexChar){
    case '0':
        retMe = "0000";
    break;
    case '1':
        retMe = "0001";
    break;
    case '2':
        retMe = "0010";
    break;
    case '3':
        retMe = "0011";
    break;
    case '4':
        retMe = "0100";
    break;
    case '5':
        retMe = "0101";
    break;
    case '6':
        retMe = "0110";
    break;
    case '7':
        retMe = "0111";
    break;
    case '8':
        retMe = "1000";
    break;
    case '9':
        retMe = "1001";
    break;
    case 'A':
        retMe = "1010";
    break;
    case 'B':
        retMe = "1011";
    break;
    case 'C':
        retMe = "1100";
    break;
    case 'D':
        retMe = "1101";
    break;
    case 'E':
        retMe = "1110";
    break;
    case 'F':
        retMe = "1111";
    break;
    }
    return retMe;
}

string binToHex(string binChain){
    string retMe;
    if(binChain == "0000") retMe = "0";
    if(binChain == "0001") retMe = "1";
    if(binChain == "0010") retMe = "2";
    if(binChain == "0011") retMe = "3";
    if(binChain == "0100") retMe = "4";
    if(binChain == "0101") retMe = "5";
    if(binChain == "0110") retMe = "6";
    if(binChain == "0111") retMe = "7";
    if(binChain == "1000") retMe = "8";
    if(binChain == "1001") retMe = "9";
    if(binChain == "1010") retMe = "a";
    if(binChain == "1011") retMe = "b";
    if(binChain == "1100") retMe = "c";
    if(binChain == "1101") retMe = "d";
    if(binChain == "1110") retMe = "e";
    if(binChain == "1111") retMe = "f";
    return retMe;
}

char binToChar(string bitChain){
    unsigned char retMe;
    string chain01;
    string chain02;
    string fullHex;
    stringstream sDecena;
    stringstream sUnidad;
    int decena = 0;
    int unidad = 0;
    int aux = 0;
    while(aux < 4){
    chain01 += bitChain[aux];
    aux++;
    }
    while(aux < 8){
    chain02 += bitChain[aux];
    aux++;
    }
    sDecena << hex << binToHex(chain01);
    sUnidad << hex << binToHex(chain02);
    sDecena >> decena;
    sUnidad >> unidad;
    retMe = (16*decena) + unidad;
    return retMe;
}

string charToBin(unsigned char car){
    char hexStr[2];
    string retMe;
    string aux01, aux02;
    stringstream hexString;
    //We convert the character entered to hexadecimal. 
    hexString << uppercase << hex << setw(2) << setfill('0') << (int)(unsigned char)car;
    //We store the hexadecimal digits in a 3 character string. 
    hexString >> hexStr;
    aux01 = hexToBin(hexStr[0]);
    aux02 = hexToBin(hexStr[1]);
    retMe = aux01 + aux02;
    return retMe;
}

void writeBMP(FILE* vessel, FILE* info, FILE* result, string density){
char byte;
int byteCount = 0;
char charVessel, charInfo;
//WE MAKE AN EXACT COPY OF THE HEADER OF THE VESSEL IN THE RESULT:
fseek(vessel,0,SEEK_SET);
fread(&byte,1,1,vessel);
    while(byteCount<53){
        fprintf(result,"%c", byte);
        fread(&byte,1,1,vessel);
        byteCount++;
    }
    byteCount = 0;
    fprintf(result,"%c", byte);
    printf("\nHeader listo.\n");
//WE ASSUME THAT THE INFO FILE WILL HAVE ENOUGH SPACE IN THE RESULT FILE:
    //SIMPLE VERSION WITH DENSITY FIXED AT 8. 
byteCount = 0;
string InfoByte;
string VesselByte;
printf("Insertando archivo info...\n");
    while(!feof(info)){
        fread(&charInfo,1,1,info);
        //WE CONVERT THE CHARACTER READ TO A BINARY CHAIN: 
        InfoByte = charToBin(charInfo);
        while(byteCount < 8){
            //WE READ CHARACTER OF THE VESSEL: 
            fread(&charVessel,1,1,vessel);
            //WE CONVERT IT TO BINARY CHAIN: 
            VesselByte = charToBin(charVessel);
            //WE INSERT THE INFO BIT IN THE LEAST SIGNIFICANT BIT OF THE VESSEL: 
            VesselByte[7] = InfoByte[byteCount];
            byte = binToChar(VesselByte);
            //WE WRITE IT IN RESULT: 
            fprintf(result,"%c",byte);
            byteCount++;
        }
        byteCount = 0;
    }
    //WE ALREADY SAVED THE INFO INSIDE THE VESSEL, NOW WE HAVE TO COPY THE REST OF THE VESSEL IN THE RESULT. 
    //fprintf(result,"%c",charVessel);
    while(!feof(vessel)){
        fread(&charVessel,1,1,vessel);
        fprintf(result,"%c",charVessel);
    }
printf("Terminado.\n");
return;
}

void readBMP(FILE* vessel, FILE* result, string density){
char byte;
int byteCount = 0;
char saveMe;
//Variable to accumulate the least significant bits of each byte that we read. 
string bitAcum = "";
//Variable to store all the bits of the byte that we read. 
string bitString;
//We skip the header. 
while(byteCount < 54){
    fread(&byte,1,1,vessel);
    byteCount++;
}
byteCount = 0;
printf("\nLeyendo Vessel...\n");
while(!feof(vessel)){
    while(byteCount < 8){
        fread(&byte,1,1,vessel);
        bitString = charToBin(byte);
        bitAcum += bitString[7];
        byteCount++;
    }
    //EVERY 8 BYTES READ FROM THE VESSEL, WE HAVE A BYTE OF THE RESULT. 
    saveMe = binToChar(bitAcum);
    fprintf(result,"%c",saveMe);
    byteCount = 0;
    bitAcum = "";
}
printf("Terminado\n");
return;
}

int main(int argc, char* argv[]){

//VARIABLES TO SEE WHERE TO GO TO SEARCH THE REQUESTED FILES
    string vesselDIR;
    string resultDIR;
    string infoDIR;
    int n = 3, i = 2;
//DENSITY: HOW MANY BYTES OF THE VESSEL FILE I NEED TO STORE A BYTE OF THE INFO FILE. (Not in use)
    string density = "11111111";
//VARIABLES TO DETECT WHICH PARAMETERS WERE SPECIFIED BY THE USER. 
    bool ve = false, inf = false, re = false, den = false;

    if (argc < 2){
        printHelp(0);
        return 1;
    }

//WE CHECK THE INPUT AND SEE IF THE USER CHOSE WRITING MODE: 
    if(string(argv[1]) == "-W"){
        if(argc < 7){
            printHelp(1);
            return 1;
        }
        while(i < argc){
            n = i+1;
            if(string(argv[i]) == "-v"){
                if((n < argc) and (n != argc) and (string(argv[n]) != "-v") and (string(argv[n]) != "-r") and (string(argv[n]) != "-i") and (string(argv[n]) != "-d") and (string(argv[n]) != "-W") and (string(argv[n]) != "-R")){
                   ve = true;
                   vesselDIR = argv[n];
                }
                else{
                    printHelp(2);
                    return 1;
                }
            }
            if(string(argv[i]) == "-r"){
                if((n < argc) and (n != argc) and (string(argv[n]) != "-v") and (string(argv[n]) != "-r") and (string(argv[n]) != "-i") and (string(argv[n]) != "-d") and (string(argv[n]) != "-W") and (string(argv[n]) != "-R")){
                   re = true;
                   resultDIR = argv[n];
                }
                else{
                    printHelp(3);
                    return 1;
                }
            }
            if(string(argv[i]) == "-i"){
                if((n < argc) and (n != argc) and (string(argv[n]) != "-v") and (string(argv[n]) != "-r") and (string(argv[n]) != "-i") and (string(argv[n]) != "-d") and (string(argv[n]) != "-W") and (string(argv[n]) != "-R")){
                   inf = true;
                   infoDIR = argv[n];
                }
                else{
                    printHelp(4);
                    return 1;
                }
            }
            if(string(argv[i]) == "-d"){
                if((n < argc) and (n != argc) and (string(argv[n]) != "-v") and (string(argv[n]) != "-r") and (string(argv[n]) != "-i") and (string(argv[n]) != "-d") and (string(argv[n]) != "-W") and (string(argv[n]) != "-R")){
                   den = true;
                   density = atoi(argv[n]);
                }
                else{
                    printHelp(5);
                    return 1;
                }
            }
            i++;
        }
        if((ve == true) and (re == true) and (inf == true)){
            if(checkFile(vesselDIR) == -1){
                printHelp(6);
                return 1;
            }
            if(checkFile(infoDIR) == -1){
                printHelp(7);
                return 1;
            }
            if((checkFile(vesselDIR) == 0) and (checkFile(infoDIR) == 0)){

                FILE* vessel = fopen(vesselDIR.c_str(), "rb");
                FILE* result = fopen(resultDIR.c_str(), "wb");
                FILE* info = fopen(infoDIR.c_str(), "rb");
                writeBMP(vessel,info,result,density);
                printf("Cerrando archivos...\n");
                fclose(vessel);
                fclose(result);
                fclose(info);
                printf("Terminado.\n");
            }
        }
    }
    else if(string(argv[1]) == "-R"){
        if(argc < 5){
            printHelp(1);
            return 1;
        }
        while(i < argc){
            n = i+1;
            if(string(argv[i]) == "-v"){
                if((n < argc) and (n != argc) and (string(argv[n]) != "-v") and (string(argv[n]) != "-r") and (string(argv[n]) != "-i") and (string(argv[n]) != "-d") and (string(argv[n]) != "-W") and (string(argv[n]) != "-R")){
                   ve = true;
                   vesselDIR = argv[n];
                }
                else{
                    printHelp(2);
                    return 1;
                }
            }
            if(string(argv[i]) == "-r"){
                if((n < argc) and (n != argc) and (string(argv[n]) != "-v") and (string(argv[n]) != "-r") and (string(argv[n]) != "-i") and (string(argv[n]) != "-d") and (string(argv[n]) != "-W") and (string(argv[n]) != "-R")){
                   re = true;
                   resultDIR = argv[n];
                }
                else{
                    printHelp(3);
                    return 1;
                }
            }
            i++;
        }
        if((ve == true) and (re == true)){
            if(checkFile(vesselDIR)== 0){
                FILE* vessel = fopen(vesselDIR.c_str(),"rb");
                FILE* result = fopen (resultDIR.c_str(),"wb");
                readBMP(vessel,result,density);
                printf("Cerrando archivos...\n");
                fclose(vessel);
                fclose(result);
                printf("Terminado.");
            }
            else{
                printHelp(6);
                return 1;
            }
        }
        else{
            printHelp(6);
            return 1;
        }
    }
    else if(string(argv[1]) == "-h"){
        printHelp(-1);
        return 1;
    }
    else{
        printHelp(1);
        return 1;
    }
return 0;
}
