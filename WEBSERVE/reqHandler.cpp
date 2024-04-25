
#include "includes/reqHandler.hpp"
// to do 
/*
--- chmouk test
      -arabic char becomes  %65 needs to be decoded from hexadecimal to decimal 
-- redirectiom  folder without slash ---> redirect with location with slash
-- /../../../ postman test ---- > should stop in the root max previous




---- kamals  --- -// redirection fails // index // --fixed

    -- redirection must recheck the path from the beginning, getFinalUri
    -- same for index ---

    




    -- do a uri decoder
    -- '%20 %20' -- a file named %20.txt
    -- check and parse path -real path
    -- getServer by servname host ...
    -- post continuation
    -- time out checker


 after POST
 * have to set timeout logic
 * have to set boundarie to not implemented
 * other things than clenght and chunked too
 * server matching
 * fconf parsing host/ servname ---> host 127.0.0.1
 * set host in multiplexer first part
saad's tests
    time out setter nc printf ""
    time out POST meth with clenght less 
    code wait pid

 // TESTS 
 -- GET http://localhost:5555/ POSTMAN -- > with a body "Error: read/write ECONNRESET"
 -- Parse CGI_PATH into multiple CGI --- And also make sure the CGI_PATH parsing is good
 -- fixing the CHMOD of an ERROR PAGE to 000 --- 
 -- Checking permission of execution for CGI files
*/
// START OF POST FUNCTIONS 

// END OF POST FUNCTIONS 

// GETTERS START
int     ReqHandler::getHeaderVal( std::string key, std::string &val )
{
    std::map<std::string, std::string>::iterator it = reqHeaders.find( key );
    if ( it == reqHeaders.end() )
        return 0;
    else
        return (val = it->second, 1);
}

s_location  ReqHandler::getLocationByName( std::string &str )
{
    size_t i = 0;
    for (  ; i < myServ.locations.size() ; i++ )
    {
        if ( str == myServ.locations[i].name )
        {
            loc_idx = i;
            return myServ.locations[i];
        }
    }
    return myServ.locations[i];
}

std::string ReqHandler::getFullUri( std::vector<std::string> &spl_uri, std::string &root, int j )
{
    std::string ret = root;
    if ( ret.size() && ret[ret.size() - 1] == '/' )
        ret.erase(ret.size() - 1);
    for ( size_t i = j ; i < spl_uri.size() ; i++ )
    {
        ret = ret + "/";
        ret = ret + spl_uri[i];
    }
    return ret;
}

void ReqHandler::getFinalUri( std::string str )
{
    std::vector<std::string> splited_uri = split_uri( str );
    std::cout << "uri to split >>>>>>>> " << str << std::endl;
    if ( !splited_uri.size() )
    {
        //root is the requested directory go check listing auto index ....
        std::string loc_str = "/";
        if ( isLocation( loc_str ) )
        {
            s_location loc = getLocationByName( loc_str );
            isAllowed( loc, splited_uri, 1 );
        }
        else
        {
            request.uri = myServ.root;
            request.status = 200;
        }
    }
    else
    {
        std::string loc_str = "/" + splited_uri.front();
        if ( isLocation( loc_str ) )
        {
            std::string concat_uri;
            s_location  loc = getLocationByName( loc_str );
            isAllowed( loc, splited_uri, 0 );
        }
        else
        {
            std::string _loc_str = "/";
            if ( isLocation( _loc_str ) )
            {
                s_location loc = getLocationByName( _loc_str );
                isAllowed( loc, splited_uri, 2 );
            }
            else
            {
                request.uri = getFullUri( splited_uri, myServ.root, 0 );
                request.status = 200;
            }
        }
    }
}

// GETTERS END

// std::vector<std::string> lineSplit( std::string &line )
// {
//     std::istringstream fline( line );
//     std::string word;
//     std::vector<std::string> ret;
//     while( fline >> word )
//         ret.push_back( word );
//     return ret;
// }

void ReqHandler::countBodyBytes( std::string &str )
{
    std::string sequence = "\r\n\r\n";
    size_t pos = str.find(sequence);
    int     s = 0;
    if ( pos != std::string::npos )
    {
        s = pos;
        bodyStartFound = true;
    }
    else
    {
        non_body_str = str;
        return ;
    }
    std::string res = str.substr( s + 4 );
    non_body_str = str.substr( 0, pos + 4 );
    body_string = res;
}

void    ReqHandler::fillReqHeaders()
{
    for ( size_t i = 1 ; i < reqHds.size() ; i++ )
    {
        std::string first;
        std::string second;
        std::string word;
        std::istringstream src( reqHds[i] );
        src >> first;
        while ( src >> word )
        {
            second += word;
            second += " ";
        }
        if ( second.size() )
        {
            second.erase( second.end() - 1 );
            reqHeaders[first] = second;
        }
    }
    reqHds.clear();
}


void    ReqHandler::parseHeaders()
{
    // content-lenght // transfer-encoding // 
    // content-type //  host
    // std::string value;
    if ( !getHeaderVal( "Host:", value ) )
    {
        return ( uri_depon_cs( 400 ) );
    }
    if ( request.method == "POST" )
    {
        if ( !getHeaderVal( "Content-Type:", cType ) )
            return ( uri_depon_cs( 400 ) );
        if ( !getHeaderVal( "Transfer-Encoding:", value) )
        {
            if ( !getHeaderVal( "Content-Length:", value) )
            {
                uri_depon_cs( 411 );
            }
            else
            {
                std::istringstream iss( value );
                iss >> content_lenght;
                if ( content_lenght > myServ.limit )
                    return ( uri_depon_cs( 413 ) );
            }
        }
        else
        {
            if ( value != "chunked" )
                uri_depon_cs( 501 );
        }
    }
    std::cout << "parse Headers here" << std::endl;
}

// Serv    ReqHandler::getServer()
// {
//     std::vector<Serv> srvs;
//     for( size_t i = 0 ; i < servers.size() ; i++ )
//     {
//         if ( socknData[fdServ].sin_port == htons( servers[i].port ) )
//             srvs.push_back( servers[i] );
//     }
//     if( srvs.size() == 1 )
//         return srvs.front();
    
// }

void    ReqHandler::parse_request()
{
    std::cout << "REQUEST PARSING -------------" << std::endl;
    if ( !reqHds.size() )
        return( uri_depon_cs( 400 ) );
    reqStrToVec( reqHds.front() );
    if ( req.size() != 3 )
        return( uri_depon_cs( 400 ) );
    else if ( req.front() != "GET" &&  req.front() != "POST" &&  req.front() != "DELETE" )
        return( uri_depon_cs( 501 ) );
    else if ( !checkUri( req[1] ) )
        return ( uri_depon_cs( 400 ) );
    else if ( req.back() != "HTTP/1.1" )
        return( uri_depon_cs( 505 ) );

    fillReqHeaders();
    
    // myServ = getServer();
    // get the server depending on headers and port number

    request.method = req.front();
    getFinalUri( req[1] );
    if ( !checkUrirPath( request.uri ) )
        return ( uri_depon_cs( 403 ) );
    std::cout << "\033[31m============================" << "concat uri : " << request.uri << "\033[0m" << std::endl;
    if ( request.method == "GET" )
        checkRetIdx(); 
    parseHeaders();
    if ( request.method != "POST" )
        endOfRead = 1;
    else
    {
        if ( !endOfRead )
            pFileOpener();
    }
    std::cout << "-------- request URI -----------" <<  request.uri << std::endl;
}

void    ReqHandler::nextBuff( char *buff, size_t bytes )// take the size returned by read()
{
    (void)bytes;
    std::string myData( buff, bytes );
    if ( request.method == "POST" )
    {
        if ( bodyStartFound )
        {
            if ( pFile.is_open() )
            {
                if ( value == "chunked" )
                    tChunked_post( myData );
                else
                    cLenght_post( myData );
            }
        }
        else
        {
            countBodyBytes( myData );
            pFileOpener();
        }
    }
    else
    {
        std::cerr << " Request mehod != POST and still not responded need to check ???? " << std::endl;
    }
}

void    ReqHandler::checkBuff( char *buff, size_t bytes )
{
    std::string myData( buff , bytes );
    passedOnce = true;
    std::cout << bytes << " bytes popopo" << std::endl;
    std::cout << "in check buffer" << std::endl;
    std::cout << " >> -*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* << " << std::endl;
    std::cout << "\033[31m" << myData << "\033[0m" << std::endl;
    std::cout << " >> -*-*-*--*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* << " << std::endl;
    countBodyBytes( myData );
    std::istringstream src( non_body_str );
    std::string line;
    while ( std::getline( src, line ) )
        reqHds.push_back( line );
    parse_request();
}

ReqHandler::ReqHandler( Serv &_myServ )
{
    passedOnce = false;
    end_of_chunk = 0;
    size_counter = 0;
    bigScounter = 0;
    g = 0;
    myServ  = _myServ;
    loc_idx = -1;
    bytes_red = 0;
    content_lenght = -1;
    bodyStartFound = false;
    endOfRead = 0;
    std::cout << "---- IN reqhandler CONSTRUCTOR ----" << std::endl;
}

ReqHandler::~ReqHandler()
{ 
    std::cout << "destructor requesthandler is called" << std::endl;
    if ( pFile.is_open() )
        pFile.close();
}
// ep->events[i].events & EPOLLERR || ep->events[i].events & EPOLLHUP || ep->events[i].events & EPOLLRDHUP


// /path/hello/
// /path/hello//hello      -- slash clean
// /hello/file.txt
// /path/hello/location/