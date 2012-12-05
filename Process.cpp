#include "Process.h"
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <stdexcept>
#include <algorithm>

using namespace std;

int e;
Process::Process(const std::vector<std::string> & args) 
{char *dumy[] = { NULL };
    try 
	{
	m_name = args[0];
	pipe(readpipe);
	m_pread = fdopen(readpipe[0], "r");
        pipe(writepipe);
        std::cout << "Parent ["<<getpid()<<"] Process Constructor"<<endl;
  	
        std::vector<const char *> cargs;
        std::transform(args.begin(), args.end(), std::back_inserter(cargs),[]( const std::string s){ return s.c_str();} );
        cargs.push_back(NULL);
	
	if(!(m_pid = fork())) 
	{	
	    close (readpipe[0]);
            close (writepipe[1]);

            e = dup2 (readpipe[1], 1);
            	if (e < 0)
                	std::cerr << strerror(errno);

	    e = dup2 (writepipe[0],0);
            	if (e < 0)
                	std::cerr << strerror(errno);

            e = execve(m_name.c_str(), const_cast<char**>(&cargs[0]), dumy);
            	if (e < 0)
          	      std::cerr << strerror(errno);
        }

        else if(m_pid > 0) 
	{
            close(readpipe[1]);
            close(writepipe[0]);
        }

	else
            throw std::runtime_error(strerror(errno));
    }
    catch (const char *e)
	{
        	std::cerr << e;
	}
}

Process::~Process()
{   
    fclose(m_pread);
    close (readpipe[0]);
    close (writepipe[1]);
    
    kill(m_pid, SIGTERM);
    waitpid(m_pid, NULL, 0);
}

void Process::write(const std::string& line) 
{
    ::write(writepipe[1], line.c_str(), strlen(line.c_str()));
}

std::string Process::readline() 
{
    char *line = NULL;
    size_t len[100];
    getline(&line, len, m_pread);
    return line;
}
