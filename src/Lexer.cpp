#include "Lexer.hpp"

Lexer::Lexer(string fName)
{
    this->sourceFile.open(fName);
    this->closingQuoteSymbol = false;

    if(!this->sourceFile.is_open())
    {
        cout << "Cannot open file" << endl;
        exit(1);
    }
}


Lexer::~Lexer()
{
    if(this->sourceFile.is_open())
        this->sourceFile.close();
}


string Lexer::nextToken()
{
    char c = (char)this->sourceFile.get();
    char nextC;
    string buffer;

    if(c == EOF)
        return "";
    if(this->lastSymbol == '\'' || this->lastSymbol == '\"')
    {
        if(!this->closingQuoteSymbol) //if it's closing quote don't call getQuotedString
        {
            string ret = getQuotedString(c);
            if (ret.length() != 0)
                return ret;
        }
    }
    if(this->lastSymbol == '-')
    {
        string ret = getComment(c);
        if(ret.length() != 0)
            return ret;
    }
    if(this->lastSymbol == '>')
    {
        while(isspace(c)) //omit all whitespaces
            c = (char)this->sourceFile.get();

        string ret = getContent(c);
        if(ret.length() != 0)
            return ret;
    }

    while(isspace(c)) //omit all whitespaces
        c = (char)this->sourceFile.get();

    if(c == '=') // single character tokens
        return string(1, c);
    else if(c == '>')
    {
        this->lastSymbol = '>';
        return string(1, c);
    }
    else if(c == '\'')
    {
        if(this->closingQuoteSymbol)
            this->closingQuoteSymbol = false;
        else
            this->lastSymbol = c;

        return string(1, c);

    }
    else if(c == '\"')
    {
        if(this->closingQuoteSymbol)
            this->closingQuoteSymbol = false;
        else
            this->lastSymbol = c;

        return string(1, c);
    }
    else if(c == '<') //tokens starting with "<"
    {
        nextC = (char)this->sourceFile.peek();

        if(nextC == '!')
        {
          /*  this->sourceFile.get();
            if(this->sourceFile.peek() == '-')
            {
                this->sourceFile.seekg(1, ios_base::cur);
                if(this->sourceFile.peek() == '-')
                {
                    this->sourceFile.seekg(-1, ios_base::cur);
                    this->sourceFile.get();
                    this->sourceFile.get();
                    this->usedQuoteOrCommentSymbol = '-';
                    return "<!--";
                }
                else
                {
                    this->sourceFile.seekg(-1, ios_base::cur);
                }
            } */
            this->sourceFile.get();
            return "<!";
        }
        else if(nextC == '/')
        {
            this->sourceFile.get();
            return "</";
        }
        else
            return "<";
    }
    else if(c == '/')
    {
        nextC = (char)this->sourceFile.peek();
        if(nextC == '>')
        {
            this->sourceFile.get();
            this->lastSymbol = '>';
            return "/>";
        }
    }
    else //collect all symbols to whitespace, <, >, /, space, =
    {
        string buffer;
        buffer += c;
        char peekedChar = (char)this->sourceFile.peek();
        while(peekedChar != '<' && peekedChar != '>' && peekedChar != ' ' && peekedChar != '/' &&
              peekedChar != '=' && peekedChar != EOF)
        {
            buffer += this->sourceFile.get();
            peekedChar = (char)this->sourceFile.peek();
        }

        return buffer;
    }
}

//Collect all chars until next quote symbol
string Lexer::getQuotedString(char alreadyTakenChar)
{
    string stringToReturn;
    stringToReturn += alreadyTakenChar;
    while(this->sourceFile.peek() != this->lastSymbol && this->sourceFile.peek() != EOF)
            stringToReturn += this->sourceFile.get();

    this->lastSymbol = ' ';
    this->closingQuoteSymbol = true;
    return stringToReturn;
}

string Lexer::getContent(char alreadyTakenChar)
{
    string stringToReturn;
    if(alreadyTakenChar == '<')
    {
        this->lastSymbol = ' ';
        return "";
    }

    stringToReturn += alreadyTakenChar;
    while(this->sourceFile.peek() != '<' && this->sourceFile.peek() != EOF)
        stringToReturn += this->sourceFile.get();

    this->lastSymbol = ' ';
    return stringToReturn;
}

string Lexer::getComment(char alreadyTakenChar)
{
    string stringToReturn;
    stringToReturn += alreadyTakenChar;
    while (this->sourceFile.peek() != EOF)
    {
        char c = (char) this->sourceFile.peek();
        if (c == '-')
        {
            this->sourceFile.seekg(1, ios_base::cur);
            if (this->sourceFile.peek() == '-')
            {
                this->sourceFile.seekg(1, ios_base::cur);
                if(this->sourceFile.peek() == '>')
                {
                    this->sourceFile.seekg(-2, ios_base::cur);
                    break;
                }
                this->sourceFile.seekg(-2, ios_base::cur);
            }
            else
                this->sourceFile.seekg(-1, ios_base::cur);
        }

        c = (char) this->sourceFile.get();
        stringToReturn += c;
    }

    this->lastSymbol = ' ';
    return stringToReturn;
}