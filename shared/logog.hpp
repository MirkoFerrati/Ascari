#ifdef AGENT_LIGHT


#ifndef FILELOGGER_HPP
#define FILELOGGER_HPP


#define ERR(...) {logog::myLogger::write(0,__VA_ARGS__);}
#define WARN(...) {logog::myLogger::write(1,__VA_ARGS__);}
#define LOGOG_INITIALIZE(...) {}
#define LOGOG_SHUTDOWN(...) {}
#define INFO(...) {logog::myLogger::write(2,__VA_ARGS__);}

#include <stdio.h>
#include <fstream>
#include <stdarg.h>
// Use the namespace you want
namespace logog {

class myLogger {
public:
        
    static void write(int loglevel, const char *fmt, ...)
    {
        
        //FILE* myFile=fopen("log.txt","w");
        
        va_list ap;
        va_start(ap, fmt);
        switch (loglevel)
        {
            case 0:
                printf("%s","ERR:");
//                 fprintf(myFile, "%s", "ERR:");
                break;
            case 1:
                printf("%s","WARN:");
//                 fprintf(myFile, "%s", "WARN:");
                break;
            case 2:
                printf("%s","INFO:");
//                 fprintf(myFile, "%s", "INFO:");
                break;
        }
        vprintf(fmt,ap);
//         vfprintf(myFile, fmt, ap);
        printf("%s","\n");
//         fprintf(myFile,"%s", "\n");
        va_end(ap);
        
//         fclose(myFile);
    }

}; // class end

}  // namespace


#endif // FILELOGGER_HPP

 namespace logog
 {
 typedef int Cout;
 
 }

#else
#include <../shared/logog/include/logog.hpp>


#endif

