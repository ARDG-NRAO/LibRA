#include <string>
#include <iostream>
#include <cstdio>

using namespace std;

#include <alma/ASDMBinaries/Error.h>

Error::Error(){}

Error::Error(ErrorLevel errlev){
 acceptableErrorLevel=errlev;
}

Error::Error(ErrorLevel errlev, string message){
  errlev_  = errlev;
  errorMessage_ = message;
  printMessage(message);
  if(errlev>=acceptableErrorLevel)throw Error();
}

Error::Error(ErrorLevel errlev,   char *fmt, ... ){
  errlev_  = errlev;
  char        buffer[1024];
  va_list     args ;
  va_start( args, fmt ) ;
  vsprintf( buffer, fmt, args ) ;
  errorMessage_ = buffer;
  printMessage(errorMessage_);
  if(errlev>=acceptableErrorLevel)throw Error();
}

Error::Error(int errlev){
 acceptableErrorLevel= (ErrorLevel) errlev;
}

Error::Error(int errlev, string message){
  errlev_  = (ErrorLevel) errlev;
  errorMessage_ = message;
  printMessage(message);
  if(errlev>=acceptableErrorLevel)throw Error();
}

Error::Error(int errlev,   char *fmt, ... ){
  errlev_  =  (ErrorLevel) errlev;
  char        buffer[1024];
  va_list     args ;
  va_start( args, fmt ) ;
  vsprintf( buffer, fmt, args ) ;
  errorMessage_ = buffer;
  printMessage(errorMessage_);
  if(errlev>=acceptableErrorLevel)throw Error();
}



Error::~Error(){}

void Error::notify(ErrorLevel errlev, string message){
  errlev_  = errlev;
  errorMessage_ = message;
  printMessage(message);
  //if(errlev>=acceptableErrorLevel)exit(-1);
  if(errlev>=acceptableErrorLevel)throw Error();
}

void Error::notify(string message){
  errorMessage_ = message;
  cout << message <<endl;
}

void Error::setAcceptableLevel(ErrorLevel errlev){
  acceptableErrorLevel=errlev;
}

ErrorLevel Error::getLevel(){
  return errlev_;
}

string Error::getLevelToString(){
  if(errlev_==MINOR)return "MINOR";
  if(errlev_==WARNING)return "WARNING";
  if(errlev_==SERIOUS)return "SERIOUS";
  if(errlev_==FATAL)return "FATAL";
  return "";
}

string Error::getErrorMessage(){
  string errorMessage=errorMessage_;
  clearMessage();
  return errorMessage;
}

void Error::clearMessage(){
  errorMessage_="";
  clearErrLev();
}

void Error::clearErrLev(){
  errlev_=NOERROR;
}

ErrorLevel Error::getAcceptableLevel(){
  return acceptableErrorLevel;
}

string Error::getAcceptableLevelToString(){
  if(acceptableErrorLevel==MINOR)return "MINOR";
  if(acceptableErrorLevel==WARNING)return "WARNING";
  if(acceptableErrorLevel==SERIOUS)return "SERIOUS";
  if(acceptableErrorLevel==FATAL)return "FATAL";
  return "";
}


ErrorLevel Error::acceptableErrorLevel=SERIOUS;  // default level to stop a process
ErrorLevel Error::errlev_=NOERROR;
string     Error::errorMessage_="";

void Error::printMessage(string message){
  switch(errlev_)
    {
    case FATAL:      
      cout << "FATAL ERROR: " + message << endl;
      break;
    case SERIOUS:
      cout << "SERIOUS ERROR: " + message << endl;
      break;
    case MINOR:
      cout << "MINOR ERROR: " + message << endl;
      break;
    case WARNING:
      cout << "WARNING ERROR: " + message << endl;
      break;
    default:
      cout << "ERROR: " + message << endl;
    }
}
