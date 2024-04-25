#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <string>
#include <vector>


// std::string generateRandomFileName(std::string cType) {
//     std::map<std::string, std::string> cType_ext;
//     cType_ext["text/plain"] = ".txt";
//     cType_ext["text/html"] = ".html";
//     cType_ext["text/css"] = ".css";
//     cType_ext["application/javascript"] = ".js";
//     cType_ext["application/json"] = ".json";
//     cType_ext["application/xml"] = ".xml";
//     cType_ext["application/pdf"] = ".pdf";
//     cType_ext["application/msword"] = ".doc";
//     cType_ext["application/vnd.ms-excel"] = ".xls";
//     cType_ext["application/vnd.ms-powerpoint"] = ".ppt";
//     cType_ext["application/zip"] = ".zip";
//     cType_ext["application/x-tar"] = ".tar";
//     cType_ext["application/gzip"] = ".gz";
//     cType_ext["image/jpeg"] = ".jpg";
//     cType_ext["image/png"] = ".png";
//     cType_ext["image/gif"] = ".gif";
//     cType_ext["image/bmp"] = ".bmp";
//     cType_ext["audio/wav"] = ".wav";
//     cType_ext["audio/mpeg"] = ".mp3";
//     cType_ext["video/mp4"] = ".mp4";
//     cType_ext["video/x-msvideo"] = ".avi";
    
//     std::string s = "abcdefghijklmnopqrstuvwxyz_ZYXWVUTSRQPONMLKJIHGFEDCBA0123456789";
//     int         sSize = s.size() - 1;
//     std::cout << "size : " << sSize << std::endl;
//     std::string ret;
//     srand( static_cast<unsigned int>(time(0)));
//     for ( int i = 0 ; i < 25 ; i++ )
//         ret.push_back( s[rand() % sSize]);
//     std::cout << RAND_MAX << std::endl;
//     std::map<std::string, std::string>::iterator it = cType_ext.find( cType );
//     if ( it != cType_ext.end() )
//         ret += it->second;
//     return ret;
// }

int main() {
    

    std::string s = "Hello world";
    std::string str = "my Name is : ";
    // s.erase(0, 7);
    s = str + s;
    std::cout << s << std::endl;
    // std::string fileName = generateRandomFileName(""); // Generate a random file name with 10 characters
    // std::cout << "Random file name: " << fileName << std::endl;
    // return 0;
}