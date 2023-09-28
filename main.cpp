//                            Возможные параметры:
//   Название        | Тип данных | Обязательный параметр | Значение по умолчанию
//-l или --lines     | Int        | Нет                   | Весь файл (-1)
//-t или --tail      | Int        | Нет                   | false
//-d или --delimiter | Int        | Нет                   | '\n'

#include <iostream>
#include <string>
#include <fstream>

//Параметры и файл
std::fstream file;
int line_options = -1;
bool tail_options = false;
char delimiter_options = '\n';

//Все испульзуемые функции
int ParseInfo (std::string argv_first, std::string argv_second);
int LinesCheck(std::string argv_first, std::string argv_second);
int TailCheck(std::string argv_first);
int DelimiterCheck(std::string argv_first, std::string argv_second);
auto SeparateData (std::string str);
int CheckParsedInfo(std::string argv_first, std::string argv_second);
std::string CorrectPath (std::string path);
bool IfOptionsAreCorrect (int argc, char* argv[]);
void ReadFile ();
int CountLines ();


int main(int argc, char* argv[]) {
    if (IfOptionsAreCorrect(argc, argv)) {
        ReadFile();
        return 0;
    }
    else {
        return 0;
    }
}


void ReadFile (){
    char c = '1';
    bool flag_for_tail = false;
    int all_lines = CountLines ();
    file.seekg(0);
    while (file.peek() != EOF){
        if ((!tail_options && line_options == -1) || (line_options >= all_lines)){
            file.get(c);
            std::cout << c;
        }
        else if (!tail_options && line_options > 0) {
            file.get(c);
            if (c == delimiter_options){
                line_options--;
            }
            if (line_options > 0) std::cout << c;;
        }
        else if (tail_options){
            file.get(c);
            if (c == delimiter_options && all_lines>line_options){
                all_lines--;
            }
            else if (all_lines == line_options){

                if (c == delimiter_options){
                    line_options--;
                }
                if (line_options > 0) std::cout << c;;
            }
        }
        else break;

    }

}//Считывает файл и выводит данные в соответствие с написанными параметрами
int CountLines (){
    char c = '1';
    int counter = 0;
    while (file.peek() != EOF){
        file.get(c);
        if (c == delimiter_options){
            counter += 1;
        }
    }
    return counter+1;
}//Считывает весь файл и считает количество строк по разделителю


bool IfOptionsAreCorrect (int argc, char* argv[]){
    if (argc < 2){
        std::cout << "Probably you forgot to enter path to the file";
        return 0;
    }
    std::string path = CorrectPath(argv[argc-1]);
    file.open(path);
    if (file.fail()) {
        std::cout << std::endl << "File not found\nPlease write options in following structure:\nPrintFile.exe [OPTION] filename" << std::endl;
        return 0;
    }
    if (argc > 2){
        for (int i = 1; i<=argc-2; i++){
            if (i+1 <= argc-1){
                int temp = ParseInfo(argv[i], argv[i+1]);
                if (temp == 2){
                    i++;
                    continue;
                }
                else if (temp == 0){
                    return 0;
                }
            }
        }
    }
    return 1;
}//Проверка всех написанных опций

std::string CorrectPath (std::string path){
    int i = 0;
    std::string result ="";
    while (path[i] != '\0'){
        if (path[i]=='\\'){
            result += '/';
        }
        else {
            result += path[i];
        }
        i++;
    }
    return result;
}//Проверка на корректное написание пути файла


auto SeparateData (std::string str){
    struct separated_data {std::string option = ""; std::string value = ""; bool flag = false;};
    separated_data result;
    bool forflag = false;
    for (int i = 0; i<str.length(); i++) {
        if (str[i] == '=') {
            if (!forflag) {
                result.flag=true;
                forflag = true;
                i++;
            } else {
                result.flag = false;
                std::cout<< "\nPls wrile options in one of this ways:\n-option value\n--option=value\n";
                return result;
            }
        }
        if (forflag) result.value += str[i];
        else result.option += str[i];
    }
    return result;
} //Разбиение данных типа --lines=50 на два значение (--lines и 50)

int ParseInfo (std::string argv_first, std::string argv_second){
    switch (CheckParsedInfo(argv_first, argv_second)) {
        case 0: return 0;
        case 1: return 1;
        case 2:return 2;
        case -1: {
            auto result = SeparateData(argv_first);
            int temp;
            if (result.flag) temp = CheckParsedInfo(result.option, result.value);
            else temp = -1;
            switch (temp) {
                case 0: return 0;
                case 1: return 1;
                case 2:return 2;
                case -1:{
                    std::cout << std::endl << "Choose one of the following available options\n-l num\n--lines=num\n-t\n--tail\n-d '\\char'\n--delimiter='\\char'" << std::endl;
                    return 0;
                }
            }
        }
    }
    return 0;
}//Парсинг инфы

int CheckParsedInfo(std::string argv_first, std::string argv_second){
    int line_check = LinesCheck(argv_first, argv_second);
    int tail_check = TailCheck(argv_first);
    int deli_check = DelimiterCheck(argv_first, argv_second);
    if (line_check == 0 || tail_check == 0 || deli_check == 0){
        return 0; //Если была ошибка в одной из проверок
    }
    else if (line_check == 1 || tail_check == 1 || deli_check == 1){
        return 1; //Если была введена опция -l -t или -d
    }
    else if (line_check == 2 || tail_check == 2 || deli_check == 2){
        return 2;
    }
    else if (line_check == -1 && tail_check == -1 && deli_check == -1){
        return -1;
    }
    else{
        return 0;
    }
}//Проверка распарсенных ключей


int LinesCheck(std::string argv_first, std::string argv_second){
    try {
        if (argv_first == "-l") {
            if (argv_second[0] == '-') {
                std::cout << std::endl << "expected num after lines (-l) option" << std::endl;
                return 0;
            }
            line_options = std::stoi(argv_second);
            return 2;
            }
        else if (argv_first == "--lines"){
            line_options = std::stoi(argv_second);
            return 1;
        }
    }
    catch (...) {
        std::cout << std::endl << "expected num after lines (-l) option" << std::endl;
        return 0;
    }
    return -1;

} //Проверка на корректность ввода данных для опции -l

int TailCheck(std::string argv_first){
//    try{
    if (argv_first == "-t" || argv_first == "--tail") {
        tail_options = true;
        return 1;
    }
    return -1;
}//Проверка на корректность ввода данных для опции -t

int DelimiterCheck(std::string argv_first, std::string argv_second){
    try {
        if (argv_first == "-d" || argv_first == "--delimiter")

            if (argv_second[0] == '-') {
                std::cout << std::endl << "Expected char in format: 'your char'" << std::endl;
                return 0;
            }
            else {
                if (argv_second[1]== '\\'){
                    char temp;
                    if (argv_second[2] == 'n') temp='\n';
                    else if (argv_second[2] == 't') temp = '\t';
                    else if (argv_second[2] == 'v') temp = '\v';
                    else if (argv_second[2] == 'b') temp = '\b';
                    else if (argv_second[2] == 'r') temp = '\r';
                    else if (argv_second[2] == 'a') temp = '\a';
                    else if (argv_second[2] == '\\') temp = '\\';
                    else if (argv_second[2] == '?') temp='\?';
                    else if (argv_second[2] == '\'') temp = '\'';
                    else if (argv_second[2] == '\"') temp = '\"';
                    else if (argv_second[4] == '\0') {
                        temp = argv_second[2];
                    }
                    else{
                        std::cout << std::endl << "Expected char after delimiter (-d) option in format: '\\any char' eg. \\n \\a etc << std::endl";
                        return 0;
                    }
                    if (argv_first == "-d"){
                        delimiter_options = temp;
                        return 2;
                    }
                    else if (argv_first == "--delimiter"){
                        delimiter_options = temp;
                        return 1;
                    }
                }
                else{
                    std::cout << std::endl << "Expected char after delimiter (-d) option in format: '\\any char' eg. \\n \\a etc << std::endl";
                    return 0;
                }
            }
        }
    catch(...) {
        std::cout << std::endl << "Expected only 1 char in this format: 'your char'" << std::endl;
        return 0;
    }
    return -1;
}//Проверка на корректность ввода данных для опции -d



