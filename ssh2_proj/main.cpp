#include "sftp.h"
#include <string>

int main()
{
	std::string ip, username, password;

	ip = "10.0.5.14";
	username = "ldx";
	password = "123";

	int rc = sftp_init(ip, username, password);
	if (rc!=0)
	{
		system("pause");
		return -1;
	}

	std::string file_name = "";

	////批量新建文件夹
	//for (int i = 1; i < 10;i++)
	//{
	//	file_name = std::to_string(i);
	//	rc = sftp_mkdir(file_name);
	//	if (rc != 0)
	//	{
	//		//system("pause");
	//		//return -1;
	//	}
	//}

	////上传大文件
	//rc = sftp_upload("D:/lidongxuan/somezip/cuda_8.0.61_windows.exe",  "2.exe");
	//if (rc != 0)
	//{
	//	system("pause");
	//	return -1;
	//}

	////下载大文件
	//rc = sftp_download("1.wmv", "D:/2.wmv");
	//if (rc != 0)
	//{
	//	system("pause");
	//	return -1;
	//}
	//rc = sftp_download("1.zip", "D:/2.zip");
	//if (rc != 0)
	//{
	//	system("pause");
	//	return -1;
	//}

	////批量上传
	//for (int i = 1; i < 100; i++)
	//{
	//	file_name = "D:/lidongxuan/test_image/TEST (" + std::to_string(i) + ").jpg";
	//	rc = sftp_upload(file_name, std::to_string(i) + ".jpg");
	//	if (rc != 0)
	//	{
	//		system("pause");
	//		return -1;
	//	}
	//}

	////批量下载
	//for (int i = 1; i < 100; i++)
	//{
	//	file_name = "D:/lidongxuan/test_image/" + std::to_string(i) + "_new.jpg";
	//	rc = sftp_download(std::to_string(i) + ".jpg", file_name);
	//	if (rc != 0)
	//	{
	//		system("pause");
	//		return -1;
	//	}
	//}

	sftp_close();

	system("pause");
	return 0;
}