#include "readerexception.h"

#include <string>
#include <sstream>
#include <QDebug>

ReaderException::ReaderException(void) :
    kipl::base::KiplException("Undeclared Exception","ReaderException")
{
}

ReaderException::~ReaderException(void)
{
}

ReaderException::ReaderException(std::string msg) : kipl::base::KiplException(msg)
{}

ReaderException::ReaderException(const std::string &msg, const std::string &filename, size_t line) :
kipl::base::KiplException(msg,filename,line,"ReaderException")
{
    qDebug() << "input" <<msg.c_str() << filename.c_str() << line;
    qDebug() << "members"<< sMessage.c_str() << sFileName.c_str() << nLineNumber;
    qDebug() << "what"<<what();
}

