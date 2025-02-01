Development Environment
-IDE : Microsoft Visual Studio Community 2022 
-IDE version : 17.11.4
-Compiler : Microsoft Visual C++ 20224
-Compiler version : 00482-90000-00000-AA341
-Programming language : C
-C language standard : Default (Legacy MSVC)
-C++ language standard : Default (ISO C++14 Standard)

Required Library 
-Winsock Library: The Winsock library is properly linked to the project for socket programming functionalities.
-You should read the compilation instruction below to link the library.

Location of users.txt
-The location of users.txt which includes the existing users in the server should be ../users.txt from the Debug file.
-The initial user accounts Tom, David, Beth should exist before executing the source code.

Compliation Instruction
-Open the client and server source code separately in Visual Studio each.
-Go to ‘Build’ > Click ‘Build Solution’ to compile the code
-Go to ‘Project’ > ‘Properties’ > Linker > Input > Additional Dependencies, you should change it to ‘odbc32.lib;odbccp32.lib;WS2_32.lib;%(AdditionalDependencies)’
-Go to ‘Project’ > ‘Properties’ > Linker > General > Output File, you should change it to ‘.\Debug\client.exe’ or ‘.\Debug\server.exe
-The executable file will be created in the each client/server > debug file.

Running Instruction
-First, run the server executable file from the server/debug folder.
-Next, run the client executable file from the client/debug folder using the command line, not by directly clicking it. You must provide the server’s IP address as an argument. Example: client 127.0.0.1
-Then, the client can enter any commands like login, newuser, send, and logout.
