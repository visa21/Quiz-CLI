#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <filesystem>


const std::string YELLOW = "\033[33m"; 
const std::string BLUE = "\033[34m";
const std::string GREEN = "\033[32m"; 
const std::string RESET = "\033[0m";
static bool review = false;


struct Question
{
    std::string question;
    std::string options[4];
    char correctAnswer;
};

std::string formatQuestion(const std::string& rawQuestion)
{
    std::string formatted = rawQuestion;
    size_t pos = 0;

    while ((pos = formatted.find("\\n", pos)) != std::string::npos) {
        formatted.replace(pos, 2, "\n");
        pos += 1; 
    }


    std::regex templateRegex(R"(template\s*<\s*typename\s+\w+\s*>)");
    std::regex functionRegex(R"(\b(void|char|double|float)\s+\w+\s*\([^)]*\)\s*)");
    std::regex keywordRegex(R"(\b(if|else|for|while|return|const|const&)\b)");
    std::regex typeRegex(R"(\b(int)\b)");

    formatted = std::regex_replace(formatted, templateRegex, YELLOW + "$&" + RESET);
    formatted = std::regex_replace(formatted, functionRegex, BLUE + "$&" + RESET);
    formatted = std::regex_replace(formatted, typeRegex, BLUE + "$&" + RESET);
    formatted = std::regex_replace(formatted, keywordRegex, GREEN + "$&" + RESET);

    return formatted;
}

char askQuestion(const Question& q, int questionNumber)
{
    restart:
    std::cout << "Question " <<questionNumber <<": " << q.question << "\n";
    std::cout << "a) " << q.options[0] << "\n";
    std::cout << "b) " << q.options[1] << "\n";
    std::cout << "c) " << q.options[2] << "\n";
    std::cout << "d) " << q.options[3] << "\n";
    std::cout << "Select your answer (a/b/c/d): ";
    
    char answer;
    std::cin >> answer;
    if (answer != 'a' && answer != 'b' && answer != 'c' && answer != 'd'){
    std::cout << "Invalid input. Please respond with a/b/c/d \n" <<std::endl;
    goto restart;
    }
    return answer;
}

std::vector<Question> loadQuestionsFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    std::vector<Question> questions;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            Question q;
            q.question = formatQuestion(line);

            for (int i = 0; i < 4; ++i) {
                std::getline(file, line);
                q.options[i] = line;
            }

            std::getline(file, line);
            q.correctAnswer = line[0];

            questions.push_back(q);

            std::getline(file, line);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << "\n";
    }

    return questions;
}

void runQuiz(const std::string& filename)
{
    char user_input;
    std::cout << "Do you want to repeat each question if a wrong answer is submitted?(y/n)" << std::endl;
    std::cin >> user_input;
    if (user_input == 'y')
    review = true;
    else
    review = false;

    std::vector<Question> questions = loadQuestionsFromFile(filename);
    int totalQuestions = questions.size();
    int correctAnswers = 0;

    for(int i = 0; i < totalQuestions; i++)
    {
        restart:
        char a = askQuestion(questions[i],i+1);
        if (a == questions[i].correctAnswer)
        {
            ++correctAnswers;
            std::cout << "Correct!!\n\n" << std::endl;;
        }
        else
        {

            std::cout <<"Wrong!\n\n" << std::endl;
            if(review == true)goto restart;
        }
    }
    if (!review)
    std::cout << "You answered " << correctAnswers << " out of " << totalQuestions << " questions correctly.\n";
}

std::string getcwd() {
    return std::filesystem::current_path().string();
}

std::string listTxtFiles() {
    std::string cwd = getcwd();
    std::vector<std::string> txtFiles;

    for (const auto& entry : std::filesystem::directory_iterator(cwd)) {
        if (entry.path().extension() == ".txt") {
            txtFiles.push_back(entry.path().string());
        }
    }

    size_t totalFiles = txtFiles.size();
    size_t pageSize = 10;
    size_t currentPage = 0;

    while (true) {
        size_t start = currentPage * pageSize;
        size_t end = std::min(start + pageSize, totalFiles);

        for (size_t i = start; i < end; ++i) {
            std::cout << i - start << ": " << txtFiles[i] << std::endl;
        }

        if (end < totalFiles) {
            std::cout << "n: Next page" << std::endl;
        }

        std::cout << "Select a file by number (0-9): ";
        std::string input;
        std::cin >> input;

        if (input == "n" && end < totalFiles) {
            currentPage++;
        } 
        else if (input == "b")
        {
            currentPage--;
        }else
         {
            int selection = std::stoi(input);
            if (selection >= 0 && selection < 10 && start + selection < totalFiles) {
                std::cout << "You selected: " << txtFiles[start + selection] << std::endl;
                return txtFiles[start + selection];
            } else {
                std::cout << "Invalid selection. Try again." << std::endl;
            }
        }
    }
    return " ";
}

int main()
{
    
    std::string filename = listTxtFiles();
    runQuiz(filename);
    return 0;
}