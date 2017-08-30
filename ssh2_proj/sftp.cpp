/*
该样例可实现以下功能：
（1）download：读取服务器上的图片文件并写入到本地
（2）mkdir：在服务器的home文件夹内新建文件夹
（3）upload：将本地文件上传至服务器
*/

#include "sftp.h"

LIBSSH2_SESSION *session;
LIBSSH2_SFTP *sftp_session;
LIBSSH2_SFTP_HANDLE *sftp_handle;
int sock;

int sftp_init(std::string str_ip, std::string str_username, std::string str_password)
{
	struct sockaddr_in sin;

	WSADATA wsadata;
	int err;
	err = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if (err != 0)
	{
		fprintf(stderr, "WSAStartup failed with error: %d\n", err);
		return -1;
	}

	unsigned long hostaddr = inet_addr(str_ip.c_str());//目标ip，需要转为16进制
	err = libssh2_init(0);
	if (err != 0)
	{
		fprintf(stderr, "libssh2 initialization failed (%d)\n", err);
		return -1;
	}

	/*创建socket并建立连接*/
	sock = socket(AF_INET, SOCK_STREAM, 0);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(22);
	sin.sin_addr.s_addr = hostaddr;
	if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0)
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
	err = libssh2_session_handshake(session, sock);
	if (err)
	{
		fprintf(stderr, "Failure establishing SSH session: %d\n", err);
		return -1;
	}

	/*hostkey's fingerprintl*/
	const char *fingerprint;
	fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

	/* 通过密码授权 */
	if (libssh2_userauth_password(session, str_username.c_str(), str_password.c_str()))
	{
		fprintf(stderr, "Authentication by password failed.\n");
		
		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		closesocket(sock);
		libssh2_exit();

		return -1;
	}	

	/*sftp初始化*/
	sftp_session = libssh2_sftp_init(session);

	if (!sftp_session)
	{
		fprintf(stderr, "Unable to init SFTP session\n");
		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		closesocket(sock);
		libssh2_exit();
		return -1;
	}

	return 0;

}
int sftp_mkdir(std::string str_dir)
{
	//新建文件夹
	/* Make a directory via SFTP */
	int rc = libssh2_sftp_mkdir(sftp_session, str_dir.c_str(),
		LIBSSH2_SFTP_S_IRWXU |
		LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IXGRP |
		LIBSSH2_SFTP_S_IROTH | LIBSSH2_SFTP_S_IXOTH);
	if (rc)
	{
		fprintf(stderr, "libssh2_sftp_mkdir failed or file already have: %d\n", rc);
		return -1;
	}
	return 0;
}

int sftp_upload(std::string src_str_file_name, std::string dst_str_file_name)
{
	int rc;
	size_t nread;
	char mem[1024 * 100];
	char *ptr;
	//上传文件
	FILE *local = fopen(src_str_file_name.c_str(), "rb");
	if (!local)
	{
		fprintf(stderr, "Can't open local uploading file %s\n", src_str_file_name.c_str());
		return -1;
	}

	/* Request a file via SFTP */
	sftp_handle =
		libssh2_sftp_open(sftp_session, dst_str_file_name.c_str(),
		LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
		LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
		LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);

	if (!sftp_handle)
	{
		fprintf(stderr, "Unable to open file with SFTP\n");
		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		closesocket(sock);
		libssh2_exit();
		return -1;
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
	fclose(local);
	return 0;
}
int sftp_download(std::string src_str_file_name, std::string dst_str_file_name)
{
	int rc;
	//下载文件
	/* Request a file via SFTP */
	sftp_handle = libssh2_sftp_open(sftp_session, src_str_file_name.c_str(), LIBSSH2_FXF_READ, 0);

	if (!sftp_handle)
	{
		fprintf(stderr, "Unable to open file with SFTP: %ld\n", libssh2_sftp_last_error(sftp_session));
		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		closesocket(sock);
		libssh2_exit();
		return -1;
	}
	FILE * local = fopen(dst_str_file_name.c_str(), "wb+");
	if (!local)
	{
		fprintf(stderr, "Can't create local file %s\n", dst_str_file_name.c_str());
		
		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		closesocket(sock);
		libssh2_exit();
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
	return 0;
}

int sftp_close()
{
	libssh2_sftp_close(sftp_handle);
	libssh2_sftp_shutdown(sftp_session);
	libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
	libssh2_session_free(session);
	closesocket(sock);
	libssh2_exit();
	return 0;
}

