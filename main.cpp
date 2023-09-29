//                            Возможные параметры:
//   Название        | Тип данных | Обязательный параметр | Значение по умолчанию
//-l или --lines     | Int        | Нет                   | Весь файл (-1)
//-t или --tail      | Int        | Нет                   | false
//-d или --delimiter | Int        | Нет                   | '\n'

#include <iostream>
#include <string>
#include <fstream>

enum class ParseOutput {
    Error,
    LongOption,
    ShortOption

};

enum class OptionStatus {
    WrongOption,
    Error,
    LongOption,
    ShortOption
};

struct Options {
    std::fstream file;
    int line_options = -1;
    bool tail_options = false;
    char delimiter_options = '\n';
};

struct SeparatedData {
    std::string option;
    std::string value;
    bool flag = false;
};



OptionStatus LinesOption(const std::string& argv_first, const std::string& argv_second, Options* arguments) {
    try {
        if (argv_first == "-l") {
            if (argv_second[0] == '-') {
                std::cerr << std::endl << "expected num after lines (-l) option" << std::endl;
                return OptionStatus::Error;
            }
            arguments->line_options = std::stoi(argv_second);
            return OptionStatus::ShortOption;
        }
        else if (argv_first == "--lines") {
            arguments->line_options = std::stoi(argv_second);
            return OptionStatus::LongOption;
        }
    } catch (...) {
        std::cerr << std::endl << "expected num after lines (-l) option" << std::endl;
        return OptionStatus::Error;
    }
    return OptionStatus::WrongOption;

}

OptionStatus TailOption(const std::string& argv_first, Options* arguments) {
    if (argv_first == "-t" || argv_first == "--tail") {
        arguments->tail_options = true;
        return OptionStatus::LongOption;
    }
    return OptionStatus::WrongOption;
}

OptionStatus DelimiterOption(const std::string& argv_first, const std::string& argv_second, Options* arguments) {
    if (argv_first == "-d" || argv_first == "--delimiter")
        if (argv_second[0] == '-') {
            std::cerr << std::endl << "Expected char in format: 'your char'" << std::endl;
            return OptionStatus::Error;
        }
        else if (argv_second[1]== '\\' && argv_second.length() == 4) {
            try {
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

                if (argv_first == "-d") {
                    arguments->delimiter_options = temp;
                    return OptionStatus::ShortOption;
                }
                else if (argv_first == "--delimiter") {
                    arguments->delimiter_options = temp;
                    return OptionStatus::LongOption;
                }
            } catch(...) {
                std::cerr << std::endl << "Expected char after delimiter (-d) option in format: '\\any char' eg. \\n \\a etc << std::endl";
                return OptionStatus::Error;
            }
        } else {
            std::cerr << std::endl << "Expected char after delimiter (-d) option in format: '\\any char' eg. \\n \\a etc << std::endl";
            return OptionStatus::Error;
        }
    return OptionStatus::WrongOption;
}

int CountLinesByDelimiter(Options* arguments) {
    char c;
    int counter = 0;
    while (arguments->file.peek() != EOF) {
        arguments->file.get(c);
        if (c == arguments->delimiter_options) {
            ++counter;
        }
    }
    return counter + 1;
}


SeparatedData SplitLongArgument(const std::string& str) {
    SeparatedData result;
    bool flag = false;
    for (int i = 0; i<str.length(); ++i) {
        if (str[i] == '=') {
            if (!flag) {
                result.flag=true;
                flag = true;
                ++i;
            } else {
                result.flag = false;
                std::cerr<< "\nPls wrile options in one of this ways:\n-option value\n--option=value\n";
                return result;
            }
        }
        if (flag) {
            result.value += str[i];
        } else {
            result.option += str[i];
        }
    }
    return result;
}

OptionStatus CheckParsedInfo(const std::string& argv_first, const std::string& argv_second, Options* arguments) {
    OptionStatus line_check = LinesOption(argv_first, argv_second, arguments);
    OptionStatus tail_check = TailOption(argv_first, arguments);
    OptionStatus deli_check = DelimiterOption(argv_first, argv_second, arguments);
    if (line_check == OptionStatus::Error || deli_check == OptionStatus::Error) {
        return OptionStatus::Error;
    }
    else if (line_check == OptionStatus::LongOption || tail_check == OptionStatus::LongOption || deli_check == OptionStatus::LongOption) {
        return OptionStatus::LongOption;
    }
    else if (line_check == OptionStatus::ShortOption || deli_check == OptionStatus::ShortOption) {
        return OptionStatus::ShortOption;
    }
    else if (line_check == OptionStatus::WrongOption && tail_check == OptionStatus::WrongOption && deli_check == OptionStatus::WrongOption) {
        return OptionStatus::WrongOption;
    }
    else {
        return OptionStatus::Error;
    }
}

ParseOutput ParseInfo(const std::string& argv_first, const std::string& argv_second, Options* arguments) {
    switch (CheckParsedInfo(argv_first, argv_second, arguments)) {
        case OptionStatus::Error: return ParseOutput::Error;
        case OptionStatus::LongOption: return ParseOutput::LongOption;
        case OptionStatus::ShortOption: return ParseOutput::ShortOption;
        case OptionStatus::WrongOption: {
            SeparatedData result = SplitLongArgument(argv_first);
            OptionStatus temp;
            if (result.flag) {
                temp = CheckParsedInfo(result.option, result.value, arguments);
            }
            switch (temp) {
                case OptionStatus::Error: return ParseOutput::Error;
                case OptionStatus::LongOption: return ParseOutput::LongOption;
                case OptionStatus::ShortOption: return ParseOutput::ShortOption;
                default:{
                    std::cerr << std::endl << "Choose one of the following available options\n-l num\n--lines=num\n-t\n--tail\n-d '\\char'\n--delimiter='\\char'" << std::endl;
                    return ParseOutput::Error;
                }
            }
        }
    }
    return ParseOutput::Error;
}

bool GetOptions(int argc, char* argv[], Options* arguments) {
    if (argc < 2) {
        std::cerr << "Probably you forgot to enter path to the file";
        return false;
    }
    arguments->file.open(argv[argc - 1]);
    if (arguments->file.fail()) {
        std::cerr << std::endl
                  << "File not found" << std::endl
                  << "Please write options in following structure:" << std::endl
                  << "PrintFile.exe [OPTION] filename" << std::endl
                  << std::endl;
        return false;
    }

    for (int i = 1; i<=argc-2; ++i) {
        if (i+1 <= argc-1) {
            ParseOutput temp = ParseInfo(argv[i], argv[i+1], arguments);
            if (temp == ParseOutput::ShortOption) {
                ++i;
                continue;
            }
            else if (temp == ParseOutput::Error) {
                return false;
            }
        }
    }

    return true;
}

void ReadFile(Options* arguments) {
    char c;
    int all_lines = CountLinesByDelimiter (arguments);
    arguments->file.seekg(0);
    while (arguments->file.peek() != EOF) {
        if ((!arguments->tail_options && arguments->line_options == -1) || (arguments->line_options >= all_lines)) {
            arguments->file.get(c);
            std::cout << c;
        }
        else if (!arguments->tail_options && arguments->line_options > 0) {
            arguments->file.get(c);
            if (c == arguments->delimiter_options){
                --arguments->line_options;
            }
            if (arguments->line_options > 0) {
                std::cout << c;;
            }
        }
        else if (arguments->tail_options) {
            arguments->file.get(c);
            if (c == arguments->delimiter_options && all_lines>arguments->line_options){
                --all_lines;
            }
            else if (all_lines == arguments->line_options) {

                if (c == arguments->delimiter_options){
                    --arguments->line_options;
                }
                if (arguments->line_options > 0) {
                    std::cout << c;;
                }
            }
        }
        else break;

    }

}



int main(int argc, char* argv[]) {
    Options arguments;
    if (GetOptions(argc, argv, &arguments)) {
        ReadFile(&arguments);
    }
    return EXIT_SUCCESS;
}



