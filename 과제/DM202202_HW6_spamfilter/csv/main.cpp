#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>
#include <chrono>
using namespace std;

void copy(string first, char *second)
{
    int i;
    for (i = 0; first[i] != '\0'; i++)
        second[i] = first[i];
    second[i] = first[i];
}
void tokenizing(string mail, queue<string> &Queue)
{
    char *token;
    char *line = new char[mail.size() + 1];
    copy(mail, line);
    char delimiter[] = {" ,.'`÷╥╧{[]}~|?!-=/+%^@&*$#_\"<>\\:;1234567890()"};
    token = strtok_r(line, delimiter, &line);
    while (token)
    {
        Queue.push(token);
        token = strtok_r(line, delimiter, &line);
    }
}
void makeFile(const string filename)
{
    ifstream input(filename);
    int flag = 0;
    int i;
    map<string, unsigned int> word_data;
    map<string, unsigned int> label;
    word_data.clear();
    string word;
    string hamm("ham");
    string spamm("spam");
    string compare("subject");
    string previous = compare;
    while (input >> word)
    {
        if (word.compare(hamm) == 0)
        {
            flag++;
            continue;
        }
        if (word.compare(spamm) == 0)
        {
            flag++;
            continue;
        }
        if ((flag + 1) % 2 == 0 && word.compare(compare) == 0)
        {
            flag++;
            label.clear();
            continue;
        }
        if (flag % 2 == 0)
        {
            if (word_data.find(word) != word_data.end())
            {
                word_data[word]++;
                label[word]++;
                if (label[word] > 1)
                    word_data[word]--;
            }
            else
                word_data[word] = 1;
        }
        else
            flag--;
    }
    input.close();
    ofstream output(filename);
    for (auto iter = word_data.begin(); iter != word_data.end(); ++iter)
    {
        output << iter->second << "\t" << iter->first << "\n";
    }
}
void readFile(const string filename, map<string, unsigned int> &spamWord)
{
    ifstream input(filename);
    string word;
    unsigned int num;
    while (input >> num >> word)
    {
        spamWord.insert(pair<string, int>(word, num));
    }
    input.close();
}
void readFile(const string filename, queue<string> &spamWord)
{
    ifstream input(filename);
    string word;
    while (input >> word)
    {
        spamWord.push(word);
    }
    input.close();
}
void preProcessing(queue<string> &Queue)
{
    ifstream ham_train("./train/dataset_ham_train100.csv");
    ifstream spam_train("./train/dataset_spam_train100.csv");
    ifstream ham_test("./test/dataset_ham_test20.csv");
    ifstream spam_test("./test/dataset_spam_test20.csv");
    string line;
    getline(ham_train, line, '\n');
    ofstream fout;
    while (!ham_train.eof())
    {
        getline(ham_train, line, '\n');
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        tokenizing(line, Queue);
        fout.open("ham_train.txt", ios::out | ios::app);
        while (!Queue.empty())
        {
            fout << Queue.front() << "\n";
            Queue.pop();
        }
        fout.close();
    }
    getline(spam_train, line, '\n');
    while (!spam_train.eof())
    {
        getline(spam_train, line, '\n');
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        tokenizing(line, Queue);
        fout.open("spam_train.txt", ios::out | ios::app);
        while (!Queue.empty())
        {
            fout << Queue.front() << "\n";
            Queue.pop();
        }
        fout.close();
    }
    getline(ham_test, line, '\n');
    while (!ham_test.eof())
    {
        getline(ham_test, line, '\n');
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        tokenizing(line, Queue);
        fout.open("ham_test.txt", ios::out | ios::app);
        while (!Queue.empty())
        {
            fout << Queue.front() << "\n";
            Queue.pop();
        }
        fout.close();
    }
    getline(spam_test, line, '\n');
    while (!spam_test.eof())
    {
        getline(spam_test, line, '\n');
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        tokenizing(line, Queue);
        fout.open("spam_test.txt", ios::out | ios::app);
        while (!Queue.empty())
        {
            fout << Queue.front() << "\n";
            Queue.pop();
        }
        fout.close();
    }
    makeFile("ham_train.txt");
    makeFile("spam_train.txt");
}

int main()
{
    queue<string> Queue;
    preProcessing(Queue);
    double spam_chance = 1.0;
    double ham_chance = 1.0;
    map<string, unsigned int> spam_train;
    map<string, unsigned int> ham_train;
    queue<string> spam_test;
    queue<string> ham_test;
    string before;
    int i = 0;
    readFile("ham_train.txt", ham_train);
    readFile("spam_train.txt", spam_train);
    readFile("ham_test.txt", ham_test);
    readFile("spam_test.txt", spam_test);
    double result_spam[20] = {0.0};
    double result_ham[20] = {0.0};
    int a = 0;
    while (!spam_test.empty())
    {
        if (a != 2)
        {
            if (spam_test.front() == "subject" || spam_test.front() == "spam")
            {
                a++;
                spam_test.pop();
                continue;
            }
        }
        if (spam_train.find(spam_test.front()) != spam_train.end() && ham_train.find(spam_test.front()) != ham_train.end() && spam_test.front() != "subject")
        {
            spam_chance *= spam_train[spam_test.front()] / 100.0;
            ham_chance *= ham_train[spam_test.front()] / 100.0;
        }

        if ((spam_test.front() == "subject" && before == "spam") || spam_test.size() < 2)
        {
            double go = spam_chance / (spam_chance + ham_chance);
            result_spam[i++] = go;
            spam_chance = 1.0;
            ham_chance = 1.0;
        }
        before = spam_test.front();
        spam_test.pop();
    }
    spam_chance = 1.0;
    ham_chance = 1.0;
    i = 0;
    a = 0;
    int cnt = 0;
    while (!ham_test.empty())
    {
        if (a != 2)
        {
            if (ham_test.front() == "subject" || ham_test.front() == "ham")
            {
                a++;
                ham_test.pop();
                continue;
            }
        }
        if (spam_train.find(ham_test.front()) != spam_train.end() && ham_train.find(ham_test.front()) != ham_train.end() && ham_test.front() != "subject")
        {
            spam_chance *= spam_train[ham_test.front()] / 100.0;
            ham_chance *= ham_train[ham_test.front()] / 100.0;
        }
        if ((ham_test.front() == "subject" && before == "ham") || ham_test.size() < 2)
        {
            double go = spam_chance / (spam_chance + ham_chance);
            result_ham[i++] = go;
            spam_chance = 1.0;
            ham_chance = 1.0;
        }
        before = ham_test.front();
        ham_test.pop();
    }
    for (int k = 0; k < 20; k++)
    {
        cout << "spam mail " << k + 1 << " is " << result_spam[k];
        if (result_spam[k] > 0.95)
        {
            cout << " and spam mail" << endl;
            cnt++;
        }
        else
            cout << " and ham mail" << endl;
    }
    for (int k = 0; k < 20; k++)
    {
        cout << "ham mail " << k + 1 << " is " << result_ham[k];
        if (result_ham[k] > 0.95)
            cout << " and spam mail" << endl;
        else
        {
            cout << " and ham mail" << endl;
            cnt++;
        }
    }
    float accuracy = cnt / 40.0 * 100.0;
    cout << "Accuracy : " << accuracy << endl;
    return 0;
}