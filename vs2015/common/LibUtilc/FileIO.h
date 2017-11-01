#ifndef __FILEIO_H__
#define __FILEIO_H__

/* @Detail File write & read
 * Xml read & write
 * Txt read & write
 * JSon read & write
 * InI read & write
*/

/*实时解决文件过大的问题.
  通过文件大小来控制文件打啊小效率过低.
  通过文件文本行号控制文件大小不是很准确.（实际应用足够）
*/
namespace plusFCL_BTL
{
	class DLLEXPORT CFileIO
	{
	public:
		CFileIO();
		~CFileIO();

		void openLog(const std::string &filepath,int fileFlage = OPEN_ALWAYS);
		void openMedia(const std::string &filepath,int fileFlage = CREATE_ALWAYS);
		void openReadFile(const std::string &filePath);
		void close();
		int write(std::string bufferStr);
		int read(char *bufferOut,int bufferLen);
		std::string readLine();
		std::string read();

	public:
		static bool generatorFile(const std::string &path);

	protected:
		int write(char *bufferIn, int bufferLen);

	private:
		HANDLE fileHandle_;
		int filelimitLine_;
		bool isLog_;
	};

	class DLLEXPORT CFileIni
	{
	public:
		CFileIni();
		CFileIni(const std::string &filePath);
		~CFileIni();

		bool openFile(const std::string &IniFile);
		bool write(const std::string &section, const std::string &key, const std::string &Value);
		std::string read(const std::string &section,const std::string &key);
		bool getSectionName(std::vector<std::string> &vecSection);
		bool getSection(const std::string &section,std::map<std::string,std::string> &mapKeyValue);

	private:
		std::string iniFile_;
		bool isValid_;
	};

	class DLLEXPORT CIniBase
	{
	public:
		CIniBase(const std::string &filePath);
		virtual ~CIniBase();

#define __DECLARE_INICONFIG_FUN(CMDID)\
		std::string get##CMDID();\
		int set##CMDID(const std::string &v);

#define __IMPLEMENT_INICONFIG_FUN(CLASSNAME,CMDID,FIELD,KEY)\
		std::string CLASSNAME::get##CMDID()	{\
		return pIniInstance_->read(FIELD,KEY);}\
		int CLASSNAME::set##CMDID(const std::string &v)	{\
		return pIniInstance_->write(FIELD,KEY,v);}

#define __DECLARE_INICONFIG_SIMILAR_FUN(CMDID)\
		std::string get##CMDID(const std::string &filed);\
		int set##CMDID(const std::string &filed,const std::string &v);

#define __IMPLEMENT_INICONFIG_SIMILAR_FUN(CLASSNAME,CMDID,KEY)\
		std::string CLASSNAME::get##CMDID(const std::string &filed){\
		return pIniInstance_->read(filed,KEY);}\
		int CLASSNAME::set##CMDID(const std::string &filed,const std::string &v){\
		return pIniInstance_->write(filed,KEY,v);}

	protected:
		CFileIni *pIniInstance_;
	};

	class DLLEXPORT CFileXml
	{
	public:
		CFileXml();
		CFileXml(const std::string &xmlPath);
		~CFileXml();

	private:
		std::string xmlFile_;
	};

	class DLLEXPORT CFileJson
	{
	public:
		CFileJson();
		CFileJson(const std::string &jsonPath);
		~CFileJson();

	private:
		std::string jsonFile_;
	};
}

#endif
