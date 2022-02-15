// Roll Number: 2021201041


// Header Files

#include <bits/stdc++.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <ctime>

#define MAX 5
using namespace std;
string rootPath;
int top, bottom, cursor = 1;
int col, horizontalCentre;
int commandLine;
int alertLine;
int statusLine;
int cursorLoc = 2;
vector<string> commandTokens;
stack<string> backStack, forwardStack;
size_t cwdLength = 1024;
char cwd[1024];
struct winsize winDim;
struct termios originalTermios;
vector<dirent *> fileNames;

void moveCursor(int x, int y = 0)
{
    cout << "\033[" << x << ";" << y << "H";
    fflush(stdout);
}
void printAlertLine(string s)
{
    moveCursor(alertLine);
    cout << "ALERT : " << s;
    moveCursor(cursor, 0);
}
void printStatusLine(string s)
{
    moveCursor(statusLine, 0);
    cout << "STATUS : " << s;
    moveCursor(commandLine, 2);
}
void enableNormalMode()
{
    tcgetattr(STDIN_FILENO, &originalTermios);
    struct termios raw;
    raw = originalTermios;
    raw.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0)
    {
        printAlertLine("Unable to switch to Normal Mode                        ");
    }
}

void init(char cwd[])
{
    // enable normal mode
    enableNormalMode();
    rootPath = string(cwd);
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &winDim);
    commandLine = winDim.ws_row - 8;
    col = winDim.ws_col;  
    top = 0;
    cursor = 0;
    bottom = top + MAX;
    alertLine = MAX + 4;
    statusLine = commandLine + 2;
}

void resetPointers()
{
    top = 0;
    cursor = 0;
    bottom = min(top + MAX, int(fileNames.size()));
}

void printNormalMode()
{
    moveCursor(commandLine - 2, 0);
    cout << "Mode : Normal Mode";
    moveCursor(commandLine, 0);
    for (int i = commandLine; i < statusLine; i++)
        cout << "\033[K";
    moveCursor(cursor, 0);
}

void printCommandMode()
{
   
    moveCursor(commandLine - 2, 0);
    cout << "Mode : Command Mode";
    
    moveCursor(commandLine, 0);
    cout << ":";
    int temp = 0;
    moveCursor(commandLine, 2);
    temp = 1;
    for (int i = commandLine; i < statusLine; i++)
        printf("\033[K");
    moveCursor(commandLine, 2);
}
void printCWD()
{
    moveCursor(alertLine + 2, 0);
    cout << "Root : " << rootPath << endl;
    cout << "Current Working Directory : " << cwd << endl;
    moveCursor(cursor, 0);
}


void displayFiles()
{
    printf("\033[H\033[J");
    struct stat fileInfo;
    register uid_t uid;
    register gid_t gid;
    register struct group *gf;
    register struct passwd *tf;
    int itr=top;
    while ( itr < min(bottom, int(fileNames.size())))
    {
        // lstat(fileNames[itr]->d_name, &fileInfo);
        stat(fileNames[itr]->d_name, &fileInfo);

        cout << "#" << itr + 1 << " \t ";
        string permissions = "";
        permissions = (S_ISDIR(fileInfo.st_mode)) ? "d" : S_ISSOCK(fileInfo.st_mode) ? "s"
                                                                                     : "-";
        permissions += (S_IRUSR & fileInfo.st_mode) ? "r" : "-";
        permissions += (S_IWUSR & fileInfo.st_mode) ? "w" : "-";
        permissions += (S_IXUSR & fileInfo.st_mode) ? "x" : "-";

        permissions += (S_IRGRP & fileInfo.st_mode) ? "r" : "-";
        permissions += (S_IWGRP & fileInfo.st_mode) ? "w" : "-";
        permissions += (S_IXGRP & fileInfo.st_mode) ? "x" : "-";

        permissions += (S_IROTH & fileInfo.st_mode) ? "r" : "-";
        permissions += (S_IWOTH & fileInfo.st_mode) ? "w" : "-";
        permissions += (S_IXOTH & fileInfo.st_mode) ? "x" : "-";
        cout << permissions << '\t';
        long long size = fileInfo.st_size;
        if (size >= (1 << 30))
            printf("%4lldG ", size / (1 << 30));
        
        else if (size >= (1 << 20))
            printf("%4lldM ", size / (1 << 20));
        
        else if (size >= (1 << 10))
            printf("%4lldK ", size / (1 << 10));
        // cout << size / (1 << 20) << "K";
        else
            printf("%4lldB ", size);
        
        // tf = getpwuid(fileInfo.st_uid);
        //  printf("%6s ", tf->pw_name);
         
        //  cout<<string(1,'\t');
        //  gf = getgrgid(fileInfo.st_gid);
        // printf("%6s ", gf->gr_name);
        // char *tt = (ctime(&fileInfo.st_mtime));
        // tt[strlen(tt) - 1] = '\0';
        // printf("%-30s", tt);
        cout << "\t" << fileNames[itr]->d_name;
        cout << endl;
        itr+=1;
    }
    printCWD();
    printNormalMode();
    return;
}

void processCurrentDir(const char *dir)
{
    struct stat fileStat;
    DIR *di;
    struct dirent *direntStructure;

    if (!(di = opendir(dir)))
    {
        printAlertLine("Directory is empty");
        return;
    }
    else
    {
        ;
    }
    chdir(dir);
    int temp=1;
    getcwd(cwd, cwdLength);
    fileNames.clear();

    for(;(direntStructure = readdir(di));)
    {
        fileNames.push_back(direntStructure);
       
    }
    closedir(di);
    resetPointers();
    displayFiles();
    return;
}

void scrollDown()
{
    
    if (cursor < MAX)
    {
        cursor += 1;
        moveCursor(cursor, 0);
        return;
    }
    else if (bottom == int(fileNames.size()))
    {
        printAlertLine("You hit the bottom");
        return;
    }

    top++;
    bottom++;
    displayFiles();
    moveCursor(cursor, 0);
    return;
}
void scrollUp()
{

    if (cursor > 1)
    {
        cursor -= 1;
        moveCursor(cursor, 0);
        return;
    }
    if (top == 0)
    {
        printAlertLine("You hit the top");
        return;
    }
    bottom--;
    top--;
    displayFiles();
    moveCursor(cursor, 0);
    return;
}
void scrollUpK()
{
    
    top -= MAX;
    if (top < 0)
        top = 0;
    bottom = top + MAX;
    displayFiles();
    moveCursor(cursor, 0);
}
void scrollDownL()
{
    bottom = bottom + MAX;
    if (bottom > int(fileNames.size()))
        bottom = int(fileNames.size());
    top = bottom - MAX;
    displayFiles();
    moveCursor(cursor, 0);
}
void disableNormalMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios);
}
void levelUp()
{
    if (cwd == rootPath)
    {
        printAlertLine("You're already present in the home directory");
        return;
    }
    backStack.push(string(cwd));
    processCurrentDir("../");
    return;
}
// working with some errors
void enter()
{
    struct stat fileInfo;
    dirent *fileDirent = fileNames[cursor + top - 1];
    lstat(fileDirent->d_name, &fileInfo);
    char *name = fileDirent->d_name;

    if (!S_ISDIR(fileInfo.st_mode))
    {
        pid_t processID = fork();
        if (!processID)
        {
            printAlertLine("file opened in default ");
            execl("/usr/bin/xdg-open", "xdg-open", fileDirent->d_name, NULL);
            exit(1);
        }
    }
    else
    {
        if (fileDirent->d_name == "..")
        {
            levelUp();
            return;
        }
        else if (fileDirent->d_name == ".")
            return;
        backStack.push(string(cwd));

        string s = string(cwd) + "/" + string(fileDirent->d_name);
        char *newDir = new char[s.length() + 1];
        strcpy(newDir, s.c_str());
        processCurrentDir(newDir);
    }
    return;
}
void moveBack(){
	if(backStack.size()==0)return;
	
	string prevDirectory = backStack.top();
	backStack.pop();
	forwardStack.push(string(cwd));
	processCurrentDir(prevDirectory.c_str());
	return;
}

void moveForward(){
	if(forwardStack.size()==0)return;
	
	string nextDirectory = forwardStack.top();
	forwardStack.pop();
	backStack.push(string(cwd));
	processCurrentDir(nextDirectory.c_str());
	return;
}

void home()
{
    if (cwd != rootPath)
    {
        backStack.push(string(cwd));
        processCurrentDir(rootPath.c_str());
    }
    else if (cwd == rootPath)
    {
        printAlertLine("You're already present in the home directory");
        return;
    }
    return;
}

string processInput()
{
    char ch;
    string command;
    cursorLoc = 2;
    int temp=10;
    moveCursor(commandLine, 2);
    while (1)
    {
        ch = cin.get();
        // on pressing enter key
        if (ch == 10)
        {
            cursorLoc = 2;
             if(temp==0)
             {
                 cout<<"error\n";
             }
            moveCursor(commandLine, cursorLoc);
            return command;
        }
        // Escape pressed
        if (ch == 27)
        {
            printAlertLine("Welocme back to normal mode");
             if(temp==0)
             {
                 cout<<"error\n";
             }
            return "esc";
        }
        else
        {
            // using backspace to erase a mistake
            if (ch == 127)
            {
                if (cursorLoc == 2)
                    continue;
                command.pop_back();
                cursorLoc = -1;
                printCommandMode();
                cout << command;
                moveCursor(commandLine, 2 + command.size());
            }
            else
            {
                command.push_back(ch);
                cout << ch;
                // cursorLoc += 1;
                cursorLoc++;
                moveCursor(commandLine, cursorLoc);
            }
        }
    }
    return "cont";
}
bool modifyPath()
{
    int len;
    len = commandTokens.size();
    
    string path;
    path = commandTokens[len - 1];
    string temp="";
    // making actual path

    if (path[0] == '~'){
        path.replace(0, 1, rootPath);
    }

    else if (path[0] == '/'){
        path = cwd + path;
    }
    else if (path[0] == '.'){
        path = cwd;
    }
    else
        return false;
    commandTokens[len - 1] = path;
    return true;
}
void copy_helper(string fname, string path)
{
    char b[1024];
    int fin;
    int fout;
    int nread;
    const char *temp = fname.c_str();
    fin = open(temp, O_RDONLY);
    fout = open((path).c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    for (; (nread = read(fin, b, sizeof(b))) > 0;)
    {
        write(fout, b, nread);
    }
}
void copy_dir_helper(string dirName, string destination)
{
    struct dirent *diren;
    struct stat fileInfo;
    DIR *di;

    if (!(di = opendir(dirName.c_str())))
    {
        printStatusLine("Can't open the directory                            ");
        return;
    }
    else
    {
        ;
        // do nothing
    }

    chdir(dirName.c_str());
    for (; (diren = readdir(di));)
    {
        lstat(diren->d_name, &fileInfo);
        string dname;
        dname = string(diren->d_name);
        if (S_ISDIR(fileInfo.st_mode))
        {

            if ((dname == ".") || (dname == ".."))
            {
                continue;
            }
            else
            {
                ;
            }

            mkdir((destination + '/' + dname).c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
            int chk=1;
            copy_dir_helper(dname, destination + '/' + dname);
        }
        else
        {
            copy_helper(dname, destination + '/' + dname);
        }
    }
    chdir("..");
    string str="";
    closedir(di);
    return;
}
void copy_dir(int i)
{
    int len = commandTokens.size();
    string destination;
    destination = commandTokens[len - 1];
    string dname;
    dname = commandTokens[i];
    mkdir((destination + '/' + dname).c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    copy_dir_helper(cwd + '/' + dname, destination + "/" + dname);
}
void copy(int i)
{
    int len;
    len = commandTokens.size();
    string destination;
    destination = commandTokens[len - 1];

    string fname = commandTokens[i];
    string path = destination + '/' + fname;
    copy_helper(fname, path);
}
void copyWrapper()
{
    int len;
    len = commandTokens.size();
    string destination;
    destination = commandTokens[len - 1];
    struct stat fileInfo;
    // copy files in the list
    int i=1;
    while( i < len - 1)
    {
        string loc = cwd + '/' + commandTokens[i];
        // copies the
        string temp="";
        lstat(loc.c_str(), &fileInfo);
        if (!S_ISDIR(fileInfo.st_mode))
        {
            copy(i);
        }
        else
        {
            copy_dir(i);
        }
        i+=1;
    }
}

void move(int i)
{
    copyWrapper();
    int len = commandTokens.size();
    string fname = commandTokens[i];
    string deleting_path = string(cwd) + "/" + fname;
    unlink(deleting_path.c_str());
}
void delete_dir_helper(string destination)
{
    struct stat fileInfo;
    DIR *di;
    struct dirent *diren;
    
    const char *temp= destination.c_str();
    if (!(di = opendir(temp)))
    {
        printStatusLine("Can't open the directory                             ");
        return;
    }
    else{
        ;
    }
    chdir(temp);
    for (; (diren = readdir(di));)
    {
        lstat(diren->d_name, &fileInfo);
        string name=diren->d_name;
        if (S_ISDIR(fileInfo.st_mode))
        {
            if (name=="."|| name=="..")
            {
                continue;
            }
            delete_dir_helper(diren->d_name);
            rmdir(diren->d_name);
        }
        else
        {
            unlink(diren->d_name);
        }
    }
    chdir("..");
    closedir(di);
}
void move_dir(int i)
{
    int len = commandTokens.size();
    string destination = commandTokens[len - 1];

    string dname = commandTokens[i];
    const char *newDir = (destination + '/' + dname).c_str();

    mkdir(newDir, S_IXUSR | S_IRUSR | S_IWUSR);
    copy_dir_helper(cwd + '/' + dname, destination + '/' + dname);
    int temp;
    delete_dir_helper(dname);
    rmdir(dname.c_str());
}

void moveWrapper()
{
    int len = commandTokens.size();
    string destination = commandTokens[len - 1];
    struct stat fileInfo;

    for (int i = 1; i < len - 1; i++)
    {
        const char *loc = (cwd + '/' + commandTokens[i]).c_str();
        lstat(loc, &fileInfo);
        if (!S_ISDIR(fileInfo.st_mode))
        {
            move(i);
        }
        else
        {
            move_dir(i);
        }
    }
}

void renameFun()
{
    const char *oldName = commandTokens[1].c_str();
    const char *newName = commandTokens[2].c_str();
    rename(oldName, newName);
}
void goTo()
{
    const char *des = commandTokens[1].c_str();
    if (des != "/")
        processCurrentDir(des);
    else
        home();
}

bool search_helper(string dirName, string tobeSearch)
{
    DIR *di;
    struct stat fileInfo;
    struct dirent *diren;
    if (!(di = opendir(dirName.c_str())))
    {
        printStatusLine("can't open the directory");
        return false;
    }
    else
    {
        ;
        // do nothing
    }
    chdir(dirName.c_str());
    for (; (diren = readdir(di));)
    {
        lstat(diren->d_name, &fileInfo);
        string dname = string(diren->d_name);
        if (tobeSearch == dname)
        {
            processCurrentDir(dirName.c_str());
            return true;
        }
        if (S_ISDIR(fileInfo.st_mode))
        {
            if ((dname == ".") || (dname == ".."))
            {
                continue;
            }
            bool val = search_helper(dirName + '/' + dname, tobeSearch);
            if (val)
                return true;
        }
    }
    chdir("..");
    closedir(di);
    return false;
}

bool search()
{
    string tbs = commandTokens[1];
    return search_helper(cwd, tbs);
}

void create_dir()
{
    int len = commandTokens.size();
    if (len <= 2)
    {
        printStatusLine("too few arguments ");
        return;
    }
    string destination = commandTokens[len - 1];

    for (int i = 1; i < len - 1; i++)
    {
        string fname = commandTokens[i];
        int status = mkdir((destination + '/' + fname).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (status = -1)
        {
            printStatusLine("Error: In creating Directory ");
        }
        else
        {
            printStatusLine("Directory created successfully");
        }
    }
}

void create_file()
{
    int len = commandTokens.size();
    string des = commandTokens[len - 1];
    for (int i = len - 2; i > 0; i--)
    {
        string fname = commandTokens[i];
        int status = open((des + '/' + fname).c_str(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (status == -1)
        {
            printStatusLine("Error in creating file");
        }
        else
        {
            printStatusLine("Files created successfully");
        }
    }
}
int delete_dir()
{
    string destination;
    destination = commandTokens[1];
    if (destination == cwd)
    {
        printStatusLine("Current directory can't be deleted");
        return 0;
    }
    delete_dir_helper(destination);
    rmdir(destination.c_str());
    return 1;
}
int delete_file()
{
    string des;
    des = commandTokens[1];
    int status = unlink(des.c_str());
    return status;
}
void performActions(string query)
{
    if (query == "move")
    {
        moveWrapper();
        processCurrentDir(cwd);
        printStatusLine("files moved successfully");
        
    }
    else if (query == "copy")
    {
        copyWrapper();
        printStatusLine("Files copied ");
    }
    else if (query == "delete_dir")
    {
        int sts;
        sts = delete_dir();
        if (sts != 0)
        {
            processCurrentDir(cwd);
            printStatusLine("Directory deleted successfully");
        }
    }
    else if (query == "delete_file")
    {
        int status = delete_file();
        if (!status)
        {
            processCurrentDir(cwd);
            printStatusLine("File deletd successfully");
        }
        else
            printStatusLine("Deletion Failed");
    }
    else if (query == "rename")
    {
        renameFun();
        processCurrentDir(cwd);
        printStatusLine("File renamed successfully");
    }
    else if (query == "create_dir")
    {
        create_dir();
        processCurrentDir(cwd);
    }
    else if (query == "create_file")
    {
        create_file();
        processCurrentDir(cwd);
    }
    else if (query == "goto")
    {
        string temp = string(cwd);
        backStack.push(temp);
        goTo();
    }
    else if (query == "search")
    {
        bool f = search();
        if (!f)
        {
            printStatusLine("file/folder not found");
        }
        else
        {
            printStatusLine("file/folder found");
        }
    }
    else
    {
        printStatusLine("invalid Command!");
    }
    return;
}

void switchToCommandMode()
{

    while (true)
    {
        printCommandMode();
        commandTokens.clear();
        string command = processInput();
        if (command == "esc")
        {
            printNormalMode();
            break;
        }
        else
        {
            // clearLine
            printf("\033[K");
        }

        stringstream sscommand(command);
        string token;
        while (getline(sscommand, token, ' '))
            commandTokens.push_back(token);

        string query = commandTokens[0];
        int temp;
        bool isValidPath;
        isValidPath = modifyPath();
        int chk=0;
        if ((!isValidPath && query != "search" && query != "rename" && query != "delete_file" && query != "delete_dir" && query != "move" && query != "copy" && query != "create_dir" && query != "create_file" && query != "goto"))
        {
            printAlertLine("Invalid Command !                                                 ");
            moveCursor(statusLine, 0);
            // clearLine
            printf("\033[K");
            printCommandMode();
            continue;
        }

        performActions(query);
    }
    return;
}

int main()
{
    printf("\033[H\033[J");
    getcwd(cwd, cwdLength);
    init(cwd);
    printCWD();
    processCurrentDir(cwd);
    char ch = ' ';
    while (ch != 'q')
    {
        ch = cin.get();
        // cout<<ch;
        if (ch == ':')
            switchToCommandMode();
        //  UpArrow
        else if (ch == 65)
            scrollUp();
        else if (ch == 66) // downArrow
            scrollDown();
        // K
        else if (ch == 107)
            scrollUpK();
        // L
        else if (ch == 108)
            scrollDownL();
        // Backspace
        else if (ch == 127)
            levelUp();
        // Enter
        else if (ch == 10)
        {
            
            enter();
        }
        // right arrow
        else if (ch == 67)
        {
            moveForward();
        }
        // left arrow
        else if (ch == 68)
        {
            moveBack();
        }
        // h
        else if (ch == 104)
        {
            home();
        }
    }
    // clearScreen
    cout << "\033[H\033[J";
    atexit(disableNormalMode);
    return 0;
}