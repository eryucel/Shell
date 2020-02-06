#define MAX 4096
void init();
void commandControl(char[MAX],int*);
void executeCommand(char*,int*);
void process(char**,int,int);
void processBackground(char**,int,int);
void backgroundSignal(int);