#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <typeinfo>
using namespace std;

struct Sline //store the info of each line
{
    string wholeString;

    string label;
    string mnemonic;
    string operand;

    int location = 0; //十進位
    string myOPCODE;

};

struct symbolNode //store the info of the symbol and its address
{
    string symbol;
    int address = 0;
};



map<string, string> MnemonicOPcode;
map<string, int> symTable;

symbolNode symbolTable[1000]; //(this one is for the output)

int symbolCount = 0;
int lineCount = 0;
Sline lineTable[1000];//preserve each Sline struct

void InitializeMnemonicOPcode()
{
    MnemonicOPcode["ADD"] = "18";
    MnemonicOPcode["ADDF"] = "58";
    MnemonicOPcode["ADDR"] = "90";
    MnemonicOPcode["AND"] = "40";
    MnemonicOPcode["CLEAR"] = "B4";
    MnemonicOPcode["COMP"] = "28";
    MnemonicOPcode["COMPF"] = "88";
    MnemonicOPcode["COMPR"] = "A0";
    MnemonicOPcode["DIV"] = "24";
    MnemonicOPcode["DIVF"] = "64";
    MnemonicOPcode["DIVR"] = "9C";
    MnemonicOPcode["FIX"] = "C4";
    MnemonicOPcode["FLOAT"] = "C0";
    MnemonicOPcode["HIO"] = "F4";
    MnemonicOPcode["J"] = "3C";
    MnemonicOPcode["JEQ"] = "30";
    MnemonicOPcode["JGT"] = "34";
    MnemonicOPcode["JLT"] = "38";
    MnemonicOPcode["JSUB"] = "48";
    MnemonicOPcode["LDA"] = "00";
    MnemonicOPcode["LDB"] = "68";
    MnemonicOPcode["LDCH"] = "50";
    MnemonicOPcode["LDF"] = "70";
    MnemonicOPcode["LDL"] = "08";
    MnemonicOPcode["LDS"] = "6C";
    MnemonicOPcode["LDT"] = "74";
    MnemonicOPcode["LDX"] = "04";
    MnemonicOPcode["LPS"] = "D0";
    MnemonicOPcode["MUL"] = "20";
    MnemonicOPcode["MULF"] = "60";
    MnemonicOPcode["MULR"] = "98";
    MnemonicOPcode["NORM"] = "C8";
    MnemonicOPcode["OR"] = "44";
    MnemonicOPcode["RD"] = "D8";
    MnemonicOPcode["RMO"] = "AC";
    MnemonicOPcode["RSUB"] = "4C";
    MnemonicOPcode["SHIFTL"] = "A4";
    MnemonicOPcode["SHIFTR"] = "A8";
    MnemonicOPcode["SIO"] = "F0";
    MnemonicOPcode["SSK"] = "EC";
    MnemonicOPcode["STA"] = "0C";
    MnemonicOPcode["STB"] = "78";
    MnemonicOPcode["STCH"] = "54";
    MnemonicOPcode["STF"] = "80";
    MnemonicOPcode["STI"] = "D4";
    MnemonicOPcode["STL"] = "14";
    MnemonicOPcode["STS"] = "7C";
    MnemonicOPcode["STSW"] = "E8";
    MnemonicOPcode["STT"] = "84";
    MnemonicOPcode["STX"] = "10";
    MnemonicOPcode["SUB"] = "1C";
    MnemonicOPcode["SUBF"] = "5C";
    MnemonicOPcode["SUBR"] = "94";
    MnemonicOPcode["SVC"] = "B0";
    MnemonicOPcode["TD"] = "E0";
    MnemonicOPcode["TIO"] = "F8";
    MnemonicOPcode["TIX"] = "2C";
    MnemonicOPcode["TIXR"] = "B8";
    MnemonicOPcode["WD"] = "DC";
}

// 將字串以任何字符分割成字詞
vector<string> split(string str) {
    
    // 分割字串
    vector<string> words{" "," "," "};

    int cur = 0;
    for (int i = 0; i < str.length(); i++) {
        // 跳過空格
        if (isspace(str[i]) || str[i] == '\t')
        {
            cur++;
            continue;
        }

        // 加入字詞
        string word;
        while (i < str.length() && str[i] != ' ' && str[i] != '\t') {
            word += str[i];
            i++;
        }
        words[cur] = word;
        cur++;
        //cout << cur << " " << word << endl;

        if (cur >= 3) break; //avoid mistake
    }

    // 輸出結果
    /*for (string word : words) {
        cout << word << endl;
    }*/

    //cout << "------------------------" << endl;
    return words;
}

int hexToDec(string s)//十六進位字串轉十進位數字 string to int 
{
    int total = 0;
    int mul = 1;
    int len = strlen(s.c_str());
    for (int i = len - 1; i >= 0; i--)//reverse order to get each digit(from least digit)
    {
        int t;
        if (s[i] >= '0' && s[i] <= '9')//number
        {
            t = s[i] - '0';
        }
        else//alphabet
        {
            t = (s[i] - 'A' + 10);
        }
        total += t * mul;
        mul *= 16;
    }
    return total;
}

int dec_atoi(string s)//十進位字串轉數字 string to int
{
    int total = 0;
    int mul = 1;
    int len = strlen(s.c_str());
    for (int i = len - 1; i >= 0; i--)//reverse order to get each digit(from least digit)
    {
        total += (s[i] - '0') * mul;
        mul *= 10;
    }
    return total;
}

string decToHex_four(int num)//turn the decimal to hexadecimal(int to string)(four digit,purpose of creating 4 digit location)
{
    string s = "";
    while (num > 0)
    {
        int t = num % 16;
        if (t <= 9)//0~9 turn to number
        {
            s += (char)('0' + t);
        }
        else//10~15turn to alphabet
        {
            s += (char)('A' + (t - 10));
        }
        num /= 16;
    }
    string rev_s = "";
    int len = strlen(s.c_str());
    if (len < 4)
    {
        for (int i = 0; i < (4 - len); i++)//the reverse order is the correct answer
        {
            rev_s += "0";
        }
    }
    for (int i = len - 1; i >= 0; i--)//the reverse order is the correct answer
    {
        rev_s += s[i];
    }
    return rev_s;
}

string decToHex(int num)//turn the decimal to hexadecimal(int to string)
{
    string s = "";
    while (num > 0)
    {
        int t = num % 16;
        if (t <= 9)//0~9 turn to number
        {
            s += (char)('0' + t);
        }
        else//10~15turn to alphabet
        {
            s += (char)('A' + (t - 10));
        }
        num /= 16;
    }
    string rev_s = "";
    int len = strlen(s.c_str());
    for (int i = len - 1; i >= 0; i--)//the reverse order is the correct answer
    {
        rev_s += s[i];
    }
    return rev_s;
}

int main()
{
    InitializeMnemonicOPcode(); //讓所有助記碼跟對應的OPCode儲存到MnemonicOPcode內

    // 開啟檔案
    fstream in;
    in.open("Input.txt");

    // 逐行讀取檔案
    string line;

    //------------------------PASS ONE-----------------------------------------------------
    
    //先讀取第一段指令
    getline(in, line);
    // 將行分割成字詞
    vector<string> words = split(line);

    Sline firstLine;
    firstLine.wholeString = line;
    firstLine.myOPCODE = "";
    firstLine.label = words[0];
    firstLine.mnemonic = words[1];
    firstLine.operand = words[2];
    firstLine.location = hexToDec(firstLine.operand);

    int pc = firstLine.location; //set the program counter
    
    //讀取剩餘指令
    while (getline(in, line)) 
    {
        vector<string> words = split(line);

        Sline temp;
        if (words[0] == ".") // 如果行首為 "."，則不記錄
        {
            continue;
        }

        temp.wholeString = line;
        temp.location = pc;//set the location to pc
        temp.myOPCODE = "";
        temp.mnemonic = words[1];
        temp.operand = words[2];

        if(words[0] != "\t" && words[0] != " ") //如果有label
        {
            temp.label = words[0];

            symbolNode newSymbol; //create new symbol object
            newSymbol.symbol = temp.label;
            newSymbol.address = pc;


            symbolTable[symbolCount] = newSymbol; //insert the new symbol to symbol table(this one is for the output)
            symTable[newSymbol.symbol] = newSymbol.address; //insert the new symbol to symbol table(the one i really use,using hashmap)
            symbolCount++;
        }
        else {
            firstLine.label = "";
        }

        //add the program counter
        if (temp.mnemonic == "WORD")
        {
            pc += 3;
        }
        else if (temp.mnemonic == "RESW")
        {
            pc += 3 * (dec_atoi(temp.operand));
        }
        else if (temp.mnemonic == "RESB")
        {
            pc += dec_atoi(temp.operand);
        }
        else if (temp.mnemonic == "BYTE")
        {
            if ((temp.operand)[0] == 'C')
            {
                pc += strlen((temp.operand).c_str()) - 3;//calculate how many character in the operand
            }
            else
            {
                pc += 1;
            }
        }
        else//normal instruction
        {
            pc += 3;
        }

        lineTable[lineCount] = temp;
        lineCount++;
    }

    //write the locationAndSource.txt
    fstream locationAndSource;
    locationAndSource.open("pass1.txt", ios::out);

    locationAndSource << "Loc\t" << "Source statement" << endl;
    
    locationAndSource << decToHex_four(firstLine.location) << "\t" << firstLine.wholeString << endl;

    for (int i = 0; i < lineCount - 1; i++)
    {
        locationAndSource << decToHex_four(lineTable[i].location) << "\t" << lineTable[i].wholeString << endl;
    }
    locationAndSource << "\t" << lineTable[lineCount - 1].wholeString << endl;
    locationAndSource.close();

    cout << "Pass 1 : " << endl << endl;
    cout << "write the location of each source code to pass1.txt!" << endl << endl;


    //------------------------PASS TWO-----------------------------------------------------

    for (int i = 0; i < lineCount - 1; i++)
    {
        if (lineTable[i].mnemonic == "RESW" || lineTable[i].mnemonic == "RESB") //不用OPcode
        {
            lineTable[i].myOPCODE = "";
            continue;
        }
        else
        {
            if (lineTable[i].mnemonic == "WORD") //運算子十進位轉十六進位
            {
                string opcode = "";
                string s1 = decToHex(dec_atoi(lineTable[i].operand));
                for (int j = 0; j < (6 - strlen(s1.c_str())); j++)//fill 0 to length 6
                {
                    opcode += "0";
                }
                opcode += s1;
                lineTable[i].myOPCODE = opcode;
            }
            else if (lineTable[i].mnemonic == "BYTE")
            {
                if ((lineTable[i].operand)[0] == 'C')//C''
                {
                    string opcode = "";
                    for (int j = 2; j < (strlen((lineTable[i].operand).c_str()) - 1); j++)//concatenate each character
                    {
                        string t = decToHex((int)((lineTable[i].operand)[j])); //轉成
                        if (strlen(t.c_str()) == 1)
                        {
                            opcode += "0";
                        }
                        opcode += t;
                    }
                    lineTable[i].myOPCODE = opcode;
                }
                else//X''
                {
                    int len = strlen((lineTable[i].operand).c_str());
                    string opcode = "";
                    for (int j = 2; j < len - 1; j++)
                    {
                        opcode += (lineTable[i].operand)[j];
                    }
                    lineTable[i].myOPCODE = opcode;
                }
            }
            else if (lineTable[i].operand == " ")//no operand
            {
                string opcode = MnemonicOPcode[lineTable[i].mnemonic];//從map找對應的OPcode
                opcode += "0000";
                lineTable[i].myOPCODE = opcode;
            }
            else//normal instruction
            {
                string opcode = MnemonicOPcode[lineTable[i].mnemonic];//use optable to get opcode

                int x = -1;
                for (int j = 0; j < strlen((lineTable[i].operand).c_str()); j++)
                {
                    if ((lineTable[i].operand)[j] == ',')//indexed addressing ex: BUFFER, X
                    {
                        x = j;
                        break;
                    }
                }
                if (x == -1)//normal addressing 直接找symbol
                {
                    opcode += decToHex_four(symTable[lineTable[i].operand]);
                }
                else//indexed addressing
                {
                    string realOperand = "";//to get the real operand without ',X'
                    for (int j = 0; j < x; j++)
                    {
                        realOperand += (lineTable[i].operand)[j];
                    }
                    int targetAddress = symTable[realOperand];
                    targetAddress += 32768;//to add the X, 十六進位的8000
                    opcode += decToHex_four(targetAddress);
                }

                lineTable[i].myOPCODE = opcode;
            }

        }
    }

    cout << "Pass_2 : " << endl << endl;
    //write the source with location and objectcode to pass2_source_LocObj.txt
    fstream source_LocObj;
    source_LocObj.open("pass2.txt", ios::out);
    source_LocObj << decToHex_four(firstLine.location) << "\t" << firstLine.wholeString << endl;

    for (int i = 0; i < lineCount - 1; i++)
    {
        if (strlen(lineTable[i].operand.c_str()) >= 8)
        {
            source_LocObj << decToHex_four(lineTable[i].location) << "\t" << lineTable[i].wholeString << "\t" << lineTable[i].myOPCODE << endl;
        }
        else if (lineTable[i].operand == " ")
        {
            source_LocObj << decToHex_four(lineTable[i].location) << "\t" << lineTable[i].wholeString << "\t\t\t" << lineTable[i].myOPCODE << endl;
        }
        else
        {
            source_LocObj << decToHex_four(lineTable[i].location) << "\t" << lineTable[i].wholeString << "\t\t" << lineTable[i].myOPCODE << endl;
        }
    }

    source_LocObj << "\t" << lineTable[lineCount - 1].wholeString << endl;
    source_LocObj.close();
    cout << "write the source with location and objectcode to pass2.txt!" << endl << endl;

    return 0;
}

/*for (std::string str; ifs >> str;) {
        std::cout << str << "\n";
    }*/