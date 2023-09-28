//                            Возможные параметры:
//   Название        | Тип данных | Обязательный параметр | Значение по умолчанию
//-l или --lines     | Int        | Нет                   | Весь файл (-1)
//-t или --tail      | Int        | Нет                   | false
//-d или --delimiter | Int        | Нет                   | '\n'

#include <iostream>
#include <string>
#include <fstream>

enum ExitArgs{
    EXIT_ERROR = 0,
    EXIT_SHORT_OPTION= 2,
    EXIT_LONG_OPTION = 1,
};
struct Argument {
    std::fstream file;
    int line_options = -1;
    bool tail_options = false;
    char delimiter_options = '\n';
};

int LinesCheck(std::string argv_first, std::string argv_second);
int TailCheck(std::string argv_first);
int DelimiterCheck(std::string argv_first, std::string argv_second);
auto SeparateData(std::string str);
int CheckParsedInfo(std::string argv_first, std::string argv_second);
std::string CorrectPath(std::string path);
bool IfOptionsAreCorrect(int argc, char* argv[]);
void ReadFile();
int CountLines();
ExitArgs ParseInfo(std::string argv_first, std::string argv_second);


Argument arguments;
int main(int argc, char* argv[]) {
    if (IfOptionsAreCorrect(argc, argv)) ReadFile();
    return 0;
}


void ReadFile(){
    char c = '1';
    int all_lines = CountLines ();
    arguments.file.seekg(0);
    while (arguments.file.peek() != EOF){
        if ((!arguments.tail_options && arguments.line_options == -1) || (arguments.line_options >= all_lines)){
            arguments.file.get(c);
            std::cout << c;
        }
        else if (!arguments.tail_options && arguments.line_options > 0) {
            arguments.file.get(c);
            if (c == arguments.delimiter_options){
                arguments.line_options--;
            }
            if (arguments.line_options > 0) std::cout << c;;
        }
        else if (arguments.tail_options){
            arguments.file.get(c);
            if (c == arguments.delimiter_options && all_lines>arguments.line_options){
                all_lines--;
            }
            else if (all_lines == arguments.line_options){

                if (c == arguments.delimiter_options){
                    arguments.line_options--;
                }
                if (arguments.line_options > 0) std::cout << c;;
            }
        }
        else break;

    }

}//Считывает файл и выводит данные в соответствие с написанными параметрами

int CountLines(){
    char c = '1';
    int counter = 0;
    while (arguments.file.peek() != EOF){
        arguments.file.get(c);
        if (c == arguments.delimiter_options){
            counter += 1;
        }
    }
    return counter+1;
}//Считывает весь файл и считает количество строк по разделителю


bool IfOptionsAreCorrect(int argc, char* argv[]){
    if (argc < 2){
        std::cout << "Probably you forgot to enter path to the file";
        return false;
    }
    std::string path = CorrectPath(argv[argc-1]);
    arguments.file.open(path);
    if (arguments.file.fail()) {
        std::cout << std::endl << "File not found\nPlease write options in following structure:\nPrintFile.exe [OPTION] filename" << std::endl;
        return false;
    }
    if (argc > 2){
        for (int i = 1; i<=argc-2; ++i){
            if (i+1 <= argc-1){
                int temp = ParseInfo(argv[i], argv[i+1]);
                if (temp == 2){
                    i++;
                    continue;
                }
                else if (temp == 0){
                    return false;
                }
            }
        }
    }
    return true;
}//Проверка всех написанных опций

std::string CorrectPath(std::string path){
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


auto SeparateData(std::string str){
    struct SeparatedData {std::string option; std::string value; bool flag = false;};
    SeparatedData result;
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

ExitArgs ParseInfo(std::string argv_first, std::string argv_second){
    switch (CheckParsedInfo(argv_first, argv_second)) {
        case 0: return EXIT_ERROR;
        case 1: return EXIT_LONG_OPTION;
        case 2:return EXIT_SHORT_OPTION;
        case -1: {
            auto result = SeparateData(argv_first);
            int temp;
            if (result.flag) temp = CheckParsedInfo(result.option, result.value);
            else temp = -1;
            switch (temp) {
                case 0: return EXIT_ERROR;
                case 1: return EXIT_LONG_OPTION;
                case 2:return EXIT_SHORT_OPTION;
                case -1:{
                    std::cout << std::endl << "Choose one of the following available options\n-l num\n--lines=num\n-t\n--tail\n-d '\\char'\n--delimiter='\\char'" << std::endl;
                    return EXIT_ERROR;
                }
            }
        }
    }
    return EXIT_ERROR;
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
            arguments.line_options = std::stoi(argv_second);
            return 2;
            }
        else if (argv_first == "--lines"){
            arguments.line_options = std::stoi(argv_second);
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
    if (argv_first == "-t" || argv_first == "--tail") {
        arguments.tail_options = true;
        return 1;
    }
    return -1;
}//Проверка на корректность ввода данных для опции -t

int DelimiterCheck(std::string argv_first, std::string argv_second){
    if (argv_first == "-d" || argv_first == "--delimiter")

        if (argv_second[0] == '-') {
            std::cout << std::endl << "Expected char in format: 'your char'" << std::endl;
            return 0;
        }
        else {
            if (argv_second[1]== '\\' && argv_second.length() == 4){
                try{
                    char temp = argv_second[2];
                    switch (argv_second[2]){
                        case 'n': temp = '\n'; break;
                        case 't': temp = '\t'; break;
                        case 'v': temp = '\v'; break;
                        case 'b': temp = '\b'; break;
                        case 'r': temp = '\r'; break;
                        case 'a': temp = '\a'; break;
                        case '\\': temp = '\\'; break;
                    }

                    if (argv_first == "-d"){
                        arguments.delimiter_options = temp;
                        return 2;
                    }
                    else if (argv_first == "--delimiter"){
                        arguments.delimiter_options = temp;
                        return 1;
                    }
                }
                catch(...) {
                    std::cout << std::endl << "Expected char after delimiter (-d) option in format: '\\any char' eg. \\n \\a etc << std::endl";
                    return 0;
                }
            }
            else{
                std::cout << std::endl << "Expected char after delimiter (-d) option in format: '\\any char' eg. \\n \\a etc << std::endl";
                return 0;
            }
        }
    return -1;
}

//Проверка на корректность ввода данных для опции -d



