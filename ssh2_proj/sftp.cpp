/*
该样例可实现以下功能：
（1）download：读取服务器上的图片文件并写入到本地
（2）mkdir：在服务器的home文件夹内新建文件夹
（3）upload：将本地文件上传至服务器
*/

#pragma comment(lib,"ws2_32.lib")

#include "libssh2_config.h"
#include "libssh2.h"
#include "libssh2_sftp.h"
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

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

int main(int argc, char *argv[])
{
	unsigned long hostaddr;
	int sock, i, auth_pw = 1;
	struct sockaddr_in sin;
	const char *fingerprint;
	LIBSSH2_SESSION *session;
	const char *username = "ldx";	//ssh用户名
	const char *password = "123";	//ssh密码
	const char *loclfile_for_upload = "D:\\lidongxuan\\1.jpg";	//需要被上传的本地文件
	const char *sftpfile_write = "1.jpg";	//上传后位于服务器的文件名
	const char *sftppath_new_dir = "test1";	//服务器新建文件夹的名称

	const char *sftpfile_for_download = "2.PNG";	//需要下载的文件名
	const char *loclfile_write = "D:\\1.jpg";	//下载后保存本地的文件名

	int rc;
	FILE *local;
	LIBSSH2_SFTP *sftp_session;
	LIBSSH2_SFTP_HANDLE *sftp_handle;
	char mem[1024 * 100];
	size_t nread;
	char *ptr;


#ifdef WIN32
	WSADATA wsadata;
	int err;

	err = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if (err != 0) 
	{
		fprintf(stderr, "WSAStartup failed with error: %d\n", err);
		return 1;
	}
#endif

	if (argc > 1) 
	{
		hostaddr = inet_addr(argv[1]);
	}
	else 
	{
		hostaddr = inet_addr("10.0.5.253");//目标ip，需要转为16进制
	}
	if (argc > 2) 
	{
		username = argv[2];
	}
	if (argc > 3) 
	{
		password = argv[3];
	}
	if (argc > 4) 
	{
		loclfile_for_upload = argv[4];
	}
	if (argc > 5) 
	{
		sftpfile_write = argv[5];
	}

	rc = libssh2_init(0);
	if (rc != 0) 
	{
		fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
		return -1;
	}

	/*创建socket并建立连接*/
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(22);
	sin.sin_addr.s_addr = hostaddr;
	if (connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != 0) 
	{
		fprintf(stderr, "failed to connect!\n");
		return -1;
	}

	/*建立一个会话实例*/
	session = libssh2_session_init();
	if (!session)
		return -1;

	/* 将libssh2的模式设置为blocking ，0为nonblocking*/
	libssh2_session_set_blocking(session, 1);

	/* 启动SSH会话. This will trade welcome banners, exchange keys,and setup crypto, compression, and MAC layers */
	rc = libssh2_session_handshake(session, sock);
	if (rc) 
	{
		fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
		return -1;
	}

	/* At this point we havn't yet authenticated.  The first thing to do
	* is check the hostkey's fingerprint against our known hosts Your app
	* may have it hard coded, may go to a file, may present it to the
	* user, that's your call*/
	fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
	//fprintf(stderr, "Fingerprint: ");
	//for (i = 0; i < 20; i++) {
	//	fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
	//}
	//fprintf(stderr, "\n");

	if (auth_pw) 
	{
		/* 通过密码授权 */
		if (libssh2_userauth_password(session, username, password)) 
		{
			fprintf(stderr, "Authentication by password failed.\n");
			goto shutdown;
		}
	}
	else 
	{
		/* 通过public密钥授权 */
		if (libssh2_userauth_publickey_fromfile(session, username,"/home/username/.ssh/id_rsa.pub","/home/username/.ssh/id_rsa",password)) 
		{
			fprintf(stderr, "\tAuthentication by public key failed\n");
			goto shutdown;
		}
	}

	/*sftp初始化*/
	sftp_session = libssh2_sftp_init(session);

	if (!sftp_session) 
	{
		fprintf(stderr, "Unable to init SFTP session\n");
		goto shutdown;
	}

	//下载文件
	/* Request a file via SFTP */
	sftp_handle = libssh2_sftp_open(sftp_session, sftpfile_for_download, LIBSSH2_FXF_READ, 0);

	if (!sftp_handle)
	{
		fprintf(stderr, "Unable to open file with SFTP: %ld\n",libssh2_sftp_last_error(sftp_session));
		goto shutdown;
	}
	local = fopen(loclfile_write, "wb+");
	if (!local) 
	{
		fprintf(stderr, "Can't create local file %s\n", loclfile_write);
		system("pause");
		return -1;
	}
	fprintf(stderr, "libssh2_sftp_open() is done, now receive data!\n");
	do {
		char mem[2048];
		/* loop until we fail */
		rc = libssh2_sftp_read(sftp_handle, mem, sizeof(mem));
		if (rc > 0) 
		{
			fwrite(mem, rc, 1, local);
		}
		else 
		{
			break;
		}
	} while (1);
	fclose(local);

	//新建文件夹
	/* Make a directory via SFTP */
	rc = libssh2_sftp_mkdir(sftp_session, sftppath_new_dir,
		LIBSSH2_SFTP_S_IRWXU |
		LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IXGRP |
		LIBSSH2_SFTP_S_IROTH | LIBSSH2_SFTP_S_IXOTH);
	if (rc)
		fprintf(stderr, "libssh2_sftp_mkdir failed or file already have: %d\n", rc);

	//上传文件
	local = fopen(loclfile_for_upload, "rb");
	if (!local) 
	{
		fprintf(stderr, "Can't open local uploading file %s\n", loclfile_for_upload);
		return -1;
	}

	/* Request a file via SFTP */
	sftp_handle =
		libssh2_sftp_open(sftp_session, sftpfile_write,
		LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
		LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
		LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);

	if (!sftp_handle) 
	{
		fprintf(stderr, "Unable to open file with SFTP\n");
		goto shutdown;
	}

	fprintf(stderr, "libssh2_sftp_open() is done, now send data!\n");
	do {
		nread = fread(mem, 1, sizeof(mem), local);
		if (nread <= 0) {
			/* end of file */
			break;
		}
		ptr = mem;
		
		do {
			/* write data in a loop until we block */
			rc = libssh2_sftp_write(sftp_handle, ptr, nread);
			if (rc < 0)
				break;
			ptr += rc;
			nread -= rc;
		} while (nread);

	} while (rc > 0);

	libssh2_sftp_close(sftp_handle);
	libssh2_sftp_shutdown(sftp_session);

shutdown:
	libssh2_session_disconnect(session,"Normal Shutdown, Thank you for playing");
	libssh2_session_free(session);

#ifdef WIN32
	closesocket(sock);
#else
	close(sock);
#endif
	if (local)
		fclose(local);
	fprintf(stderr, "all done\n");

	libssh2_exit();
	system("pause");
	return 0;
}

