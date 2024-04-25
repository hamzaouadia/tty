
#ifndef REQHANDLER_HPP
#define REQHANDLER_HPP

#include "fconf.hpp"
#include <sys/socket.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>
// #include <cstdio> // for remove

extern int read_size;
std::string decodeUri( std::string str );
int checkUrirPath( std::string &str );
int checkUri( std::string &str );
int find_me( char c );
int hexaToDeci( std::string &s );

typedef struct reqFinalForm {
    int         status;
    std::string method;
    std::string uri;
} s_reqFinalForm;

class ReqHandler {
    
    public:
    /*tests*/
        // std::map<int, sockaddr_in> socknData;
        // std::vector<Serv> servers;
        std::vector<std::string>  reqHds;
        std::string body_string;
        // char *po;
        std::string rest;
        std::string non_body_str;
        std::string value;
        std::string cType;
        std::ofstream   pFile;
        std::string     fName;
        // std::string srvName;
        // std::string hostName;
        // int fdServ;
        int g;
        bool    passedOnce;
        int end_of_chunk;
        // long long fsf_num;
        long long chunk_size;
        long long size_counter;
        unsigned long bigScounter;
    /*end tests*/
        int                                 endOfRead;
        unsigned long                       content_lenght;
        unsigned long                       bytes_red;
        bool                                bodyStartFound;
        // int                                 client_sock;
        int                                 loc_idx;
        
        Serv                                myServ;
        s_reqFinalForm                      request;
        std::vector<std::string>            req;
        std::map<std::string, std::string>  reqHeaders;

        // methods
        ReqHandler();
        // ReqHandler( std::map<int, sockaddr_in> &_socknData, int _fdServ, std::vector<Serv> &_servers );
        ReqHandler( Serv &_myServ );
        ~ReqHandler();
        void        pFileOpener();
        void        deleteFile();
        void        cLenght_post( std::string &str );
        void        tChunked_post( std::string &str );
        std::string fNameGenerator();
        void        checkBuff( char *buff, size_t bytes );
        void        nextBuff( char *buff, size_t bytes );
        void        reqStrToVec( std::string &line );
        void        fillReqHeaders();
        // void    getRequestFinalForm();
        void        countBodyBytes( std::string &str );
        void        parse_request();
        void        parseHeaders();
        int         getHeaderVal( std::string key, std::string &val );
        void        uri_depon_cs( int code );
        void        getFinalUri( std::string str );
        void        checkRetIdx();
        void        isAllowed( s_location &loc, std::vector<std::string> &splited_uri, int i );
        int         isLocation( std::string &str );
        s_location  getLocationByName( std::string &str );
        std::string getFullUri( std::vector<std::string> &spl_uri, std::string &root, int i );
        // Serv    getServer();
};

#endif