#pragma comment(lib,"ws2_32.lib")

#include "libssh2_config.h"
#include "libssh2.h"
#include "libssh2_sftp.h"
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <iostream>

#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
# ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

int sftp_init(std::string str_ip, std::string str_username, std::string str_password);
int sftp_mkdir(std::string str_dir);
int sftp_upload(std::string str_file_name, std::string dst_str_file_name);
int sftp_download(std::string str_file_name, std::string dst_str_file_name);
int sftp_close();

