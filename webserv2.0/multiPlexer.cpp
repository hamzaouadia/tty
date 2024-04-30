#include <signal.h>
#include "includes/multiPlex.hpp"

int read_size = 1024;

std::vector<Serv>    MultiPlexer::getServBySock( int sock, std::vector<Serv> &servers )
{
    std::vector<Serv>   v;
    for ( size_t i = 0 ; i < servers.size() ; i++ )
        if ( socknData[sock].sin_port == htons( servers[i].port ) )
            v.push_back( servers[i] );
    return v;
}

void    MultiPlexer::addSockToEpoll( int sockToAdd )
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = sockToAdd;
    if ( epoll_ctl( epollFd, EPOLL_CTL_ADD, sockToAdd, &ev ) == -1 )
        throw std::runtime_error( "Failed to add Socket to epoll" );
}

void    MultiPlexer::delSockFrEpoll( int sockToDel )
{
    struct epoll_event ev;
    (void)ev;
    ev.data.fd = sockToDel;
    if ( epoll_ctl( epollFd, EPOLL_CTL_DEL, sockToDel, NULL ) == -1 )
        throw std::runtime_error( "failed deleted from epoll" );
}

int MultiPlexer::existentSockForPort( int &nport )
{
    std::map<int, sockaddr_in>::iterator it = socknData.begin();
    uint16_t hport = htons( nport );
    for ( ; it != socknData.end() ; it++ )
        if ( it->second.sin_port == hport )
            return 1;
    return 0;
}

MultiPlexer::MultiPlexer( std::vector<Serv> &servers )
{
    epollFd = epoll_create( servers.size() );
    if ( epollFd == -1 )
        throw std::runtime_error( "Epoll creation failed");
    for ( std::vector<Serv>::iterator it = servers.begin() ; it != servers.end() ; it++ ) {
        if ( !existentSockForPort( it->port ) )
        {
            int sock = socket( AF_INET, SOCK_STREAM, 0 );
            if ( sock == -1 )
                throw std::runtime_error( "Socket creation failed");
            int f = fcntl(sock, F_GETFL, 0);
            if ( f == -1 )
                throw std::runtime_error( "F_GETFL in fcntl failed");
            if ( fcntl(sock, F_SETFL, f | O_NONBLOCK) == -1)
                throw std::runtime_error( "Failed to set socket to non block");
            int reuse = 1;
            if ( setsockopt( sock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse) ) == -1 )  // remove reuse port & add REUSEADDR
                throw std::runtime_error( "Failed to set socket option to REUSE");
            sockaddr_in servAdd;
            servAdd.sin_family = AF_INET;
            servAdd.sin_addr.s_addr = inet_addr( (it->host).c_str() );
            servAdd.sin_port = htons( it->port );
            if ( bind( sock, (struct sockaddr*)&servAdd, sizeof( servAdd ) ) == -1 )
                throw std::runtime_error( "Failed to bind socket" );
            if ( listen( sock, SOMAXCONN ) == -1 )
                throw std::runtime_error( "Failed to set socket to listen" );
            std::cout << "server : " << it->servName << " listen to port : " << it->port << std::endl;
            socknData[sock] = servAdd;
            addSockToEpoll( sock );
        }
    }
}

int MultiPlexer::isFdServer( int fd )
{
    std::map<int, sockaddr_in>::iterator it = socknData.find( fd );
    if ( it != socknData.end() )
        return 1;
    return 0;
}

void    MultiPlexer::acceptCli( int fd, std::vector<Serv> &servers, std::map<int, ReqHandler*> &reqMap )
{
    int cliSock = accept( fd, NULL, NULL);
    if ( cliSock == -1 )
    {
        std::cerr << "ERROR ACCEPT FUNCTIONNNN" << std::endl;
        return;
    }
    serv_cli[cliSock] = fd;
    std::vector<Serv> req_serv = getServBySock( fd, servers );
    ReqHandler *req = new ReqHandler( req_serv );
    req->clock_out = clock();
    reqMap[cliSock] = req;
    addSockToEpoll( cliSock );
}

int MultiPlexer::spotIn( int fd, ReqHandler* obj, std::map<int, ReqHandler*> &reqMap )
{
    char buff[read_size];
    memset(buff, 0, sizeof(buff) );
    size_t bytes = read( fd, buff, sizeof(buff) - 1 );
    obj->clock_out = clock();
    if ( (int)bytes == -1 )
    {
        std::cerr << "error read failed 1" << std::endl;
        close( fd );
        return 0;
    }
    else if ( !bytes )
    {
        std::cout << "ATTETION ! 0 BYTES" << std::endl;
        delSockFrEpoll( fd );
        serv_cli.erase( fd );
        reqMap.erase( fd );
        close( fd );
        return 0;
    }
    if ( !obj->passedOnce )
    {
        obj->checkBuff( buff, bytes );
        std::cerr << obj->request.uri << std::endl;
    }
    else
    {   
        obj->nextBuff( buff, bytes );
    }
    return 1;
}

int MultiPlexer::spotOut( int fd, ReqHandler* obj, std::map<int, Response*> &resMap, std::map<int, ReqHandler*> &reqMap )
{
    obj->clock_out = clock();
    read_size = 1024;
    std::map<int, Response*>::iterator itr = resMap.find( fd );
    if ( itr == resMap.end() )
    {
        Response *rs = new Response( obj, fd );
        resMap[fd] = rs;
    }
    else
    {
        ssize_t bytesSent;
        std::string resp;
        if (itr->second->endOfResp != 1)
        {
            if (itr->second->cgi_on == true)
                resp = itr->second->read_from_a_pipe();
            else
                resp = itr->second->folder == false ? itr->second->read_from_a_file() : itr->second->list_folder();
            bytesSent = send( fd, resp.c_str(), resp.size(), 0);
        }
        if ( itr->second->endOfResp || (int)bytesSent == -1 )
        {
            delSockFrEpoll( fd );
            delete( itr->second );
            delete( obj );
            resMap.erase( fd );
            reqMap.erase( fd );
            serv_cli.erase( fd );
            close( fd );
            return 0;
        }                        
    }
    return 1;
}

void    MultiPlexer::webServLoop( std::vector<Serv> &servers )
{
    struct epoll_event evs[1024];
    std::map<int, ReqHandler*> reqMap;
    std::map<int, Response*> resMap;
    std::map<int, clock_t>  timap;
    signal(SIGPIPE, SIG_IGN);
    while (1)
    {
        int evNum = epoll_wait( epollFd, evs, 1024, -1 );
        if ( evNum == -1 )
        {
            throw std::runtime_error( "Epoll wait failed");
        }
        for ( int i = 0 ; i < evNum ; i++ )
        {
            if ( isFdServer( evs[i].data.fd ) )
            {
                acceptCli( evs[i].data.fd, servers, reqMap );
                continue ;
            }
            std::map<int, ReqHandler*>::iterator it = reqMap.find( evs[i].data.fd );
            if ( evs[i].events & EPOLLIN && !it->second->endOfRead )
                if ( !spotIn( evs[i].data.fd, it->second, reqMap ) )
                    continue;
            if ( evs[i].events & EPOLLOUT && it->second->endOfRead )
                if ( !spotOut( evs[i].data.fd, it->second, resMap, reqMap ) )
                    continue;
            clock_t end = clock();
            float timeOut = static_cast<float>(end - it->second->clock_out) / CLOCKS_PER_SEC;
            if ( timeOut >= 10 )
            {
                it->second->deleteFile();
                it->second->uri_depon_cs( 408 );
            }
        }
    }
}

MultiPlexer::~MultiPlexer()
{
    close( epollFd );
    std::map<int, sockaddr_in>::iterator it = socknData.begin();
    for ( ; it != socknData.end() ; it++ )
        close( it->first );
}