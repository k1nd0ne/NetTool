# NetTool
This is a set of functions that i've been implementing to manage socket creation and management more easily. 

# Usage 
When you want to create a socket server or a client in C, you can include this file and code faster.
This include a set of colored message functions.

Here is an example of usage :
```c++
#include"NetTool/NetTool.c"

int main(void){

int socketDescriptor = CreateSoccket(1);
.
.
. 
BindSocket(dS,"2222");
promptM("Binding done !");
}
```

#Man page
A man page is currently in work in progress. 
